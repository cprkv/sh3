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

    RenderChunkPart<core::render::Mesh>    meshes;
    RenderChunkPart<core::render::Texture> textures;
  };
} // namespace core::data

namespace
{
  struct StaticData
  {
    SimpleRefCollection<RenderChunkData> renderChunks;
  };

  StaticData* sData = nullptr;


  cti::continuable<data::schema::Chunk> readChunkAsync( StringId id )
  {
    return system::task::ctiAsync( [id]() -> std::expected<data::schema::Chunk, Status> {
      auto path      = data::getDataPath( id );
      auto objHandle = msgpack::object_handle();
      auto obj       = msgpack::object();

      if( auto s = fs::readFileMsgpackCompressed( path.c_str(), obj, objHandle ); s != StatusOk )
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

#ifdef _DEBUG
      char ibName[128] = { 0 };
      char vbName[128] = { 0 };
      sprintf( ibName, "ib-" mFmtStringHash, mesh.id );
      sprintf( vbName, "ib-" mFmtStringHash, mesh.id );
#else
      const char* vbName = "";
      const char* vbName = "";
#endif

      if( auto s = uploadMesh.indexBuffer.init( ibName, makeArrayBytesView( mesh.indexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading index buffer\n" );
        return std::unexpected( s );
      }

      if( auto s = uploadMesh.vertexBuffer.init( vbName, makeArrayBytesView( mesh.vertexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading vertex buffer\n" );
        return std::unexpected( s );
      }

      data->meshes.add( mesh.id, std::move( uploadMesh ) );
      return None();
    } );
  }


  RenderChunkData* findRenderChunk( StringId id )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(),
                                 [=]( RenderChunkData& data ) { return data.id == id; } );
    return chunkIt != sData->renderChunks.end() ? &*chunkIt : nullptr;
  }

  RenderChunkData& addRenderChunk( StringId id )
  {
    sData->renderChunks.add( { .id = id, .loading = true } );
    auto chunk = findRenderChunk( id );

    readChunkAsync( id )
        .then( [chunk]( data::schema::Chunk chunkImport ) {
          auto subtasks = std::vector<cti::continuable<None>>();

          for( auto&& texture: chunkImport.textures )
            subtasks.emplace_back( uploadTextureToGPU( std::move( texture ), chunk ) );

          for( auto&& mesh: chunkImport.meshes )
            subtasks.emplace_back( uploadMeshToGPU( std::move( mesh ), chunk ) );

          return cti::when_all( std::move( subtasks ) );
        } )
        .then( [chunk]( std::vector<None> ) { // TODO: execution context?
          chunk->loading = false;

          for( auto&& func: chunk->loadCallbacks )
            func( StatusOk );
          chunk->loadCallbacks.clear();

          mCoreLog( "chunk " mFmtStringHash " is fully loaded and ready!\n", chunk->id.getHash() );
        } )
        .fail( [chunk]( Status s ) {
          chunk->loading = false;

          for( auto&& func: chunk->loadCallbacks )
            func( s );
          chunk->loadCallbacks.clear();

          mCoreLogError( "chunk " mFmtStringHash " is loaded with error: %d!\n",
                         chunk->id.getHash(), static_cast<int>( s ) );
        } );

    return *chunk;
  }

  RenderChunkData& getOrAddRenderChunk( StringId id )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(),
                                 [=]( RenderChunkData& data ) { return data.id == id; } );
    return chunkIt != sData->renderChunks.end()
               ? *chunkIt
               : addRenderChunk( id );
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


void RenderChunk::load( StringId id )
{
  // load is like init, only once can happen
  assert( !data_ );
  assert( ref_.empty() );

  auto& chunk = getOrAddRenderChunk( id );

  ref_  = sData->renderChunks.getRef( chunk );
  data_ = &chunk;
}

cti::continuable<RenderChunk> RenderChunk::loadCti( StringId id )
{
  auto result = RenderChunk();
  result.load( id );

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
