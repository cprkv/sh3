#include "core/data/render-chunk.hpp"
#include "core/data/ref-collection.hpp"
#include "core/render/data.hpp"
#include "core/core.hpp"

using namespace core;
using namespace core::data;


namespace
{
  template<typename T>
  struct RenderChunkPart
  {
    StringIdMap<T> data;

    T*   get( StringId id ) { return data.try_get( id ); }
    void add( StringId id, T value ) { data.emplace_unique( id, std::move( value ) ); }
  };
} // namespace

namespace core::data
{
  struct RenderChunkData
  {
    StringId id;
    bool     loading;

    std::list<std::move_only_function<void( Status )>> loadCallbacks;

    RenderChunkPart<core::render::Material> materials;
    RenderChunkPart<core::render::Mesh>     meshes;
    RenderChunkPart<core::render::Texture>  textures;
  };
} // namespace core::data

namespace
{
  struct StaticData
  {
    SimpleRefCollection<RenderChunkData> renderChunks;
  };

  StaticData* sData = nullptr;


  RenderChunkData* findRenderChunk( StringId id )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(),
                                 [=]( RenderChunkData& data ) { return data.id == id; } );
    return chunkIt != sData->renderChunks.end() ? &*chunkIt : nullptr;
  }


  cti::continuable<data::schema::Chunk> readChunkAsync( std::string chunkPath )
  {
    return system::task::ctiAsync( [chunkPath = std::move( chunkPath )]() -> std::expected<data::schema::Chunk, Status> {
      auto objHandle = msgpack::object_handle();
      auto obj       = msgpack::object();

      if( auto s = fs::readFileMsgpack( chunkPath.c_str(), obj, objHandle ); s != StatusOk )
        return std::unexpected( s );

      try
      {
        auto chunkImport = data::schema::Chunk();
        obj.convert( chunkImport );
        return { std::move( chunkImport ) };
      }
      catch( const std::exception& ex )
      {
        std::string exceptionMessage = ex.what();
        core::setErrorDetails( "exception while decoding render chunk: %s", exceptionMessage.c_str() );
        return std::unexpected( StatusSystemError );
      }
    } );
  }

  cti::continuable<None> uploadTextureToGPU( data::schema::Texture  texture,
                                             data::RenderChunkData* data )
  {
    return system::task::ctiDeffered( [texture = std::move( texture ), data]() -> std::expected<None, Status> {
      auto uploadTexture = core::render::Texture();
      uploadTexture.size = { texture.width, texture.height };

      if( auto s = uploadTexture.texture.init( texture ); s != StatusOk )
      {
        mCoreLogError( "error uploading texture\n" );
        return std::unexpected( s );
      }

      data->textures.add( texture.id, std::move( uploadTexture ) );
      return None();
    } );
  }

  cti::continuable<None> uploadMeshToGPU( data::schema::Mesh     mesh,
                                          data::RenderChunkData* data )
  {
    return system::task::ctiDeffered( [mesh = std::move( mesh ), data]() mutable -> std::expected<None, Status> {
      auto uploadMesh = core::render::Mesh();

      if( auto s = uploadMesh.indexBuffer.init( makeArrayBytesView( mesh.indexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading index buffer\n" );
        return std::unexpected( s );
      }

      if( auto s = uploadMesh.vertexBuffer.init( makeArrayBytesView( mesh.vertexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading vertex buffer\n" );
        return std::unexpected( s );
      }

      data->meshes.add( mesh.id, std::move( uploadMesh ) );
      return None();
    } );
  }

  RenderChunkData& addRenderChunk( StringId id, const char* name )
  {
    sData->renderChunks.add( { .id = id, .loading = true } );
    auto chunk     = findRenderChunk( id );
    auto chunkPath = data::getDataPath( name );

    readChunkAsync( chunkPath )
        .then( [chunk]( data::schema::Chunk chunkImport ) {
          auto subtasks = std::vector<cti::continuable<None>>();

          for( auto&& texture: chunkImport.textures )
            subtasks.emplace_back( uploadTextureToGPU( std::move( texture ), chunk ) );

          for( auto&& mesh: chunkImport.meshes )
            subtasks.emplace_back( uploadMeshToGPU( std::move( mesh ), chunk ) );

          return cti::when_all( std::move( subtasks ) );
        } )
        .then( [chunk, chunkPath]( std::vector<None> ) { // TODO: execution context?
          chunk->loading = false;

          for( auto&& func: chunk->loadCallbacks )
            func( StatusOk );
          chunk->loadCallbacks.clear();

          mCoreLog( "chunk '%s' is fully loaded and ready!\n", chunkPath.c_str() );
        } )
        .fail( [chunk, chunkPath]( Status s ) {
          chunk->loading = false;

          for( auto&& func: chunk->loadCallbacks )
            func( s );
          chunk->loadCallbacks.clear();

          mCoreLogError( "chunk '%s' is loaded with error: %d!\n", chunkPath.c_str(), static_cast<int>( s ) );
        } );

    return *chunk;
  }

  RenderChunkData& getOrAddRenderChunk( StringId id, const char* name )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(),
                                 [=]( RenderChunkData& data ) { return data.id == id; } );
    return chunkIt != sData->renderChunks.end()
               ? *chunkIt
               : addRenderChunk( id, name );
  }
} // namespace


Status data::initializeRenderChunk()
{
  sData = new StaticData();
  return StatusOk;
}

void data::destroyRenderChunk()
{
  delete sData;
}

void data::updateRenderChunk()
{
  std::vector<RefCounter> pinnedRefs;

  for( auto& chunk: sData->renderChunks )
  {
    if( chunk.loading )
    {
      pinnedRefs.push_back( sData->renderChunks.getRef( chunk ) );
    }
  }

  sData->renderChunks.cleanup( []( RenderChunkData& renderChunk ) {
    mCoreLog( "render chunk " mFmtStringHash " removed\n", renderChunk.id.getHash() );
  } );
}


void RenderChunk::load( const char* name )
{
  // load is like init, only once can happen
  assert( !data_ );
  assert( ref_.empty() );

  auto  id    = StringId( name );
  auto& chunk = getOrAddRenderChunk( id, name );

  ref_  = sData->renderChunks.getRef( chunk );
  data_ = &chunk;
}

cti::continuable<RenderChunk> RenderChunk::loadCti( const std::string& name )
{
  auto result = RenderChunk();
  result.load( name.c_str() );

  if( result.isLoaded() )
    return cti::make_ready_continuable<RenderChunk>( std::move( result ) );

  auto* data = result.data_;

  return cti::make_continuable<RenderChunk>( [data, result = std::move( result )]( auto&& promise ) {
    data->loadCallbacks.emplace_back( [result  = std::move( result ),
                                       promise = std::forward<decltype( promise )>( promise )](
                                          Status status ) mutable {
      if( status == StatusOk )
        promise.set_value( std::move( result ) );
      else
        promise.set_exception( status );
    } );
  } );
}

bool RenderChunk::isLoaded() const
{
  if( !data_ || ref_.empty() )
    return false;
  return !data_->loading;
}

core::render::Material* RenderChunk::getMaterial( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->materials.get( id );
}

core::render::Mesh* RenderChunk::getMesh( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->meshes.get( id );
}

core::render::Texture* RenderChunk::getTexture( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->textures.get( id );
}
