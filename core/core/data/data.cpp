#include "core/data/data.hpp"
#include "core/data/schema.hpp"
#include "core/render/data.hpp"
#include "core/system/message-queue.hpp"
#include "core/system/time.hpp"
#include "core/fs/file.hpp"
#include "core/core.hpp"

using namespace core;
using namespace core::data;

namespace
{
  template<typename T>
  struct ChunkResource
  {
    const char*                resourceTypeName;
    StringIdMap<T>             resourceById;
    StringIdMultiMap<StringId> resourceIdByChunkName;

    ChunkResource( const char* resourceTypeName )
        : resourceTypeName( resourceTypeName )
    {}

    void add( StringId chunkName, StringId id, T resource )
    {
      resourceById.emplace_unique( id, std::move( resource ) );
      resourceIdByChunkName.emplace( chunkName, id );
    }

    void removeAllByName( StringId name )
    {
      auto range = resourceIdByChunkName.equal_range( name );

      for( auto i = range.first; i != range.second; ++i )
      {
        auto id = i->second;
        if( !resourceById.erase( id ) ) [[unlikely]]
        {
          mCoreLog( "warning: resource %s with id " mFmtU64 " should be deleted, but was not found\n",
                    resourceTypeName, id.getHash() );
        }
      }

      resourceIdByChunkName.erase( range.first, range.second );
    }
  };

  struct StaticData
  {
    stdfs::path dataDirectory;

    ChunkResource<core::render::Material> materials = { "materials" };
    ChunkResource<core::render::Mesh>     meshes    = { "meshes" };
    ChunkResource<core::render::Texture>  textures  = { "textures" };
  };

  StaticData* sData = nullptr;


  enum FileDetectionStatus
  {
    FileDetectionStatusFound,
    FileDetectionStatusNotFound,
    FileDetectionStatusError,
  };

  enum FsEntryType
  {
    FsEntryTypeFile,
    FsEntryTypeDirectory,
  };


  FileDetectionStatus hasFsEntry( const stdfs::path& filePath, FsEntryType entryType )
  {
    auto ec = std::error_code();

    auto entry = stdfs::directory_entry( filePath, ec );
    if( ec )
      return FileDetectionStatusError;

    auto exists = entry.exists( ec );
    if( ec )
    {
      if( ec == std::errc::no_such_file_or_directory )
        return FileDetectionStatusNotFound;

      core::setErrorDetails( "error directory entry info error: %s", ec.message().c_str() );
      return FileDetectionStatusError;
    }

    if( !exists )
      return FileDetectionStatusNotFound;

    bool isRequiredEntryType = entryType == FsEntryTypeFile ? entry.is_regular_file( ec )
                                                            : entry.is_directory( ec );
    if( ec )
    {
      core::setErrorDetails( "error directory entry info error: %s", ec.message().c_str() );
      return FileDetectionStatusError;
    }

    return isRequiredEntryType ? FileDetectionStatusFound
                               : FileDetectionStatusNotFound;
  }


  Status findProjectConfig( stdfs::path& out )
  {
    char exePathData[MAX_PATH];
    if( !GetModuleFileNameA( nullptr, exePathData, sizeof( exePathData ) ) )
    {
      core::setErrorDetails( "error getting process name" );
      return StatusSystemError;
    }

    const auto projectConfigName = stdfs::path( "project-config.json" );

    auto currentDir = stdfs::path( exePathData ).parent_path();
    int  depth      = 8;

    for( ;; )
    {
      auto currentFilePath = currentDir / projectConfigName;
      auto fileStatus      = hasFsEntry( currentFilePath, FsEntryTypeFile );

      if( fileStatus == FileDetectionStatusFound )
      {
        out = currentFilePath;
        return StatusOk;
      }

      if( fileStatus == FileDetectionStatusError )
      {
        return StatusSystemError;
      }

      currentDir = currentDir.parent_path();
      depth--;
      if( depth == 0 )
      {
        core::setErrorDetails( "project config not found: depth exceeded" );
        return StatusSystemError;
      }
    }

    return StatusNotFound;
  }


  Status parseProjectConfig()
  {
    auto projectConfigPath = stdfs::path();
    mCoreCheckStatus( findProjectConfig( projectConfigPath ) );

    auto projectConfigData = std::vector<byte>();
    mCoreCheckStatus( fs::readFile( projectConfigPath.generic_string().c_str(), projectConfigData ) );

    auto projectConfigStr = std::string_view(
        reinterpret_cast<const char*>( projectConfigData.data() ),
        projectConfigData.size() );

    const bool allowExceptions = false;
    const bool ignoreComments  = true;

    auto object = nlohmann::json::parse( projectConfigStr, nullptr,
                                         allowExceptions, ignoreComments );
    if( object.type() == nlohmann::json::value_t::discarded )
    {
      core::setErrorDetails( "can't parse project config json file" );
      return StatusBadFile;
    }

    // TODO: may be separate object and parse methods?
    std::string gameData;
    object.at( "gameData" ).get_to( gameData );
    sData->dataDirectory = projectConfigPath.parent_path() / gameData;

    if( auto fileStatus = hasFsEntry( sData->dataDirectory, FsEntryTypeDirectory );
        fileStatus != FileDetectionStatusFound )
    {
      if( fileStatus == FileDetectionStatusNotFound )
        core::setErrorDetails( "path to data directory is invalid" );
      return StatusNotFound;
    }

    return StatusOk;
  }


  // runs on main loop
  struct UploadMeshToGpuTask
  {
    core::data::schema::Mesh mesh_;
    LoadRenderChunkTask*     task_;
    StringId                 chunkName_;

    void operator()()
    {
      auto uploadMesh = core::render::Mesh();

      if( auto s = uploadMesh.indexBuffer.init( makeArrayBytesView( mesh_.indexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading index buffer\n" );
        return;
      }

      if( auto s = uploadMesh.vertexBuffer.init( makeArrayBytesView( mesh_.vertexBuffer ) );
          s != StatusOk )
      {
        mCoreLogError( "error uploading vertex buffer\n" );
        return;
      }

      sData->meshes.add( chunkName_, mesh_.id, std::move( uploadMesh ) );
      task_->result.meshes.push_back( mesh_.id );
    }
  };


  // runs on main loop
  struct UploadTextureToGpuTask
  {
    core::data::schema::Texture texture_;
    LoadRenderChunkTask*        task_;
    StringId                    chunkName_;

    void operator()()
    {
      auto uploadTexture = core::render::Texture();
      uploadTexture.size = { texture_.width, texture_.height };

      if( auto s = uploadTexture.texture.init( texture_ );
          s != StatusOk )
      {
        mCoreLogError( "error uploading texture\n" );
        return;
      }

      sData->textures.add( chunkName_, texture_.id, std::move( uploadTexture ) );
      task_->result.textures.push_back( texture_.id );
    }
  };
} // namespace


Status data::initialize()
{
  sData = new StaticData();
  mCoreCheckStatus( parseProjectConfig() );
  return StatusOk;
}


void data::destroy()
{
  delete sData;
}


void data::loadRenderChunk( const char* name, LoadRenderChunkTask* loadRenderChunkTask )
{
  auto chunkPath = getDataPath( name );
  auto chunkName = StringId( name );

  core::loopEnqueueTask( [=]() {
    auto chunk = data::schema::Chunk();

    // 1. read file and decode chunk in understandable format
    {
      auto inputData = std::vector<byte>();
      if( auto s = fs::readFile( chunkPath.c_str(), inputData );
          s != StatusOk )
      {
        core::loopEnqueueDefferedTask( [=]() {
          loadRenderChunkTask->status = s;
          loadRenderChunkTask->done   = true;
        } );
        return;
      }

      try
      {
        msgpack::object_handle objectHandle = msgpack::unpack(
            reinterpret_cast<const char*>( inputData.data() ),
            inputData.size() );

        msgpack::object obj = objectHandle.get();
        obj.convert( chunk );
      }
      catch( const std::exception& ex )
      {
        std::string exceptionMessage = ex.what();
        core::loopEnqueueDefferedTask( [=, exceptionMessage = std::move( exceptionMessage )]() {
          core::setErrorDetails( "exception while decoding render chunk: %s",
                                 exceptionMessage.c_str() );
          loadRenderChunkTask->status = StatusSystemError;
          loadRenderChunkTask->done   = true;
        } );
        return;
      }
    }

    // 2. spawn bunch of deffered tasks and run them in loop step up to *deadline* constant
    {
      for( auto&& texture: chunk.textures )
      {
        core::loopEnqueueDefferedTask( UploadTextureToGpuTask{
            .texture_   = std::move( texture ),
            .task_      = loadRenderChunkTask,
            .chunkName_ = chunkName,
        } );
      }

      for( auto&& mesh: chunk.meshes )
      {
        core::loopEnqueueDefferedTask( UploadMeshToGpuTask{
            .mesh_      = std::move( mesh ),
            .task_      = loadRenderChunkTask,
            .chunkName_ = chunkName,
        } );
      }

      core::loopEnqueueDefferedTask( [=]() {
        loadRenderChunkTask->status = StatusOk;
        loadRenderChunkTask->done   = true;
      } );
    }
  } );
}


Status data::unloadRenderChunk( const char* name )
{
  auto chunkName = StringId( name );
  sData->materials.removeAllByName( chunkName );
  sData->meshes.removeAllByName( chunkName );
  sData->textures.removeAllByName( chunkName );
  return StatusOk;
}


std::string data::getDataPath( const char* resourceName )
{
  return ( resourceName == nullptr
               ? sData->dataDirectory
               : sData->dataDirectory / resourceName )
      .generic_string()
      .c_str();
}


core::render::Material* data::findMaterial( StringId id )
{
  return sData->materials.resourceById.try_get( id );
}

core::render::Mesh* data::findMesh( StringId id )
{
  return sData->meshes.resourceById.try_get( id );
}

core::render::Texture* data::findTexture( StringId id )
{
  return sData->textures.resourceById.try_get( id );
}
