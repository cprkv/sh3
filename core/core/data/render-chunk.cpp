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


  // runs on main loop
  struct UploadMeshToGpuTask
  {
    core::data::schema::Mesh mesh;
    RenderChunkData*         data;

    void operator()()
    {
      auto uploadMesh = core::render::Mesh();

      if( auto s = uploadMesh.indexBuffer.init( makeArrayBytesView( mesh.indexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading index buffer\n" );
        return;
      }

      if( auto s = uploadMesh.vertexBuffer.init( makeArrayBytesView( mesh.vertexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading vertex buffer\n" );
        return;
      }

      data->meshes.add( mesh.id, std::move( uploadMesh ) );
    }
  };


  // runs on main loop
  struct UploadTextureToGpuTask
  {
    core::data::schema::Texture texture;
    RenderChunkData*            data;

    void operator()()
    {
      auto uploadTexture = core::render::Texture();
      uploadTexture.size = { texture.width, texture.height };

      if( auto s = uploadTexture.texture.init( texture ); s != StatusOk )
      {
        mCoreLogError( "error uploading texture\n" );
        return;
      }

      data->textures.add( texture.id, std::move( uploadTexture ) );
    }
  };


  RenderChunkData* findRenderChunk( StringId id )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(), [=]( RenderChunkData& data ) { return data.id == id; } );
    return chunkIt != sData->renderChunks.end() ? &*chunkIt : nullptr;
  }

  RenderChunkData& addRenderChunk( StringId id, const char* name )
  {
    sData->renderChunks.add( { .id = id, .loading = true } );
    auto chunk     = findRenderChunk( id );
    auto chunkPath = data::getDataPath( name );

    core::loopEnqueueTask( [=]() {
      auto chunkImport = data::schema::Chunk();

      // 1. read file and decode chunk in understandable format
      {
        msgpack::object_handle objHandle;
        msgpack::object        obj;
        if( auto s = fs::readFileMsgpack( chunkPath.c_str(), obj, objHandle ); s != StatusOk )
        {
          core::loopEnqueueDefferedTask( [=]() { chunk->loading = false; } );
          return;
        }

        try
        {
          obj.convert( chunkImport );
        }
        catch( const std::exception& ex )
        {
          std::string exceptionMessage = ex.what();
          core::setErrorDetails( "exception while decoding render chunk: %s", exceptionMessage.c_str() );
          core::loopEnqueueDefferedTask( [=]() { chunk->loading = false; } );
          return;
        }
      }

      // 2. spawn bunch of deffered tasks and run them in loop step up to *deadline* constant
      {
        for( auto&& texture: chunkImport.textures )
          core::loopEnqueueDefferedTask( UploadTextureToGpuTask{
              .texture = std::move( texture ),
              .data    = chunk } );

        for( auto&& mesh: chunkImport.meshes )
          core::loopEnqueueDefferedTask( UploadMeshToGpuTask{
              .mesh = std::move( mesh ),
              .data = chunk } );

        core::loopEnqueueDefferedTask( [=]() { chunk->loading = false; } );
      }
    } );

    return *chunk;
  }

  RenderChunkData& getOrAddRenderChunk( StringId id, const char* name )
  {
    auto chunkIt = std::find_if( sData->renderChunks.begin(), sData->renderChunks.end(), [=]( RenderChunkData& data ) { return data.id == id; } );
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
    mCoreLog( "render chunk " mFmtU64 " removed\n", renderChunk.id.getHash() );
  } );
}


void RenderChunkHandle::load( const char* name )
{
  // load is like init, only once can happen
  assert( !data_ );
  assert( ref_.empty() );

  auto  id    = StringId( name );
  auto& chunk = getOrAddRenderChunk( id, name );

  ref_  = sData->renderChunks.getRef( chunk );
  data_ = &chunk;
}

bool RenderChunkHandle::isLoaded() const
{
  if( !data_ || ref_.empty() )
    return false;
  return !data_->loading;
}

core::render::Material* RenderChunkHandle::getMaterial( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->materials.get( id );
}

core::render::Mesh* RenderChunkHandle::getMesh( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->meshes.get( id );
}

core::render::Texture* RenderChunkHandle::getTexture( StringId id )
{
  if( !isLoaded() )
    return nullptr;
  return data_->textures.get( id );
}