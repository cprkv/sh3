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
  struct StaticData
  {
    stdfs::path              dataDirectory;
    StringIdMap<std::string> idsToFiles;
  };

  StaticData* sData = nullptr;


  Status parseProjectConfig()
  {
    auto exeDirectory = stdfs::path();
    mCoreCheckStatus( system::getExeDirectory( exeDirectory ) );

    auto projectConfigPath = stdfs::path();
    mCoreCheckStatus( fs::findFileUp( exeDirectory, "project-config.json", projectConfigPath ) );
    mCoreLog( "projectConfigPath: %s\n", projectConfigPath.string().c_str() );

    auto object = Json();
    mCoreCheckStatus( fs::readFileJson( projectConfigPath.string(), object ) );

    // TODO: may be separate object and parse methods?
    std::string gameData;
    object.at( "gameData" ).get_to( gameData );
    sData->dataDirectory = projectConfigPath.parent_path() / gameData;

    auto entryInfo = fs::EntryInfo();
    if( auto status = fs::getEntryInfo( sData->dataDirectory, entryInfo );
        status != StatusOk || entryInfo.type != fs::FsEntryTypeDirectory )
    {
      core::setErrorDetails( "path to data directory is invalid" );
      return StatusNotFound;
    }

    return StatusOk;
  }


  // is this FS related?
  Status makeIdsToFiles()
  {
    auto ec = std::error_code();
    auto it = stdfs::recursive_directory_iterator( sData->dataDirectory, ec );
    if( ec )
    {
      core::setErrorDetails( "makeIdsToFiles: recursive directory iterator: %s", ec.message().c_str() );
      return StatusSystemError;
    }

    do {
      if( it->path().filename().string().starts_with( "." ) )
      {
        it.disable_recursion_pending();
      }
      else
      {
        bool isRegularFile = it->is_regular_file( ec );
        if( ec )
        {
          core::setErrorDetails( "makeIdsToFiles: is regular file: %s", ec.message().c_str() );
          return StatusSystemError;
        }

        if( isRegularFile )
        {
          auto relative = stdfs::relative( it->path(), sData->dataDirectory, ec );
          if( ec )
          {
            core::setErrorDetails( "makeIdsToFiles: relative: %s", ec.message().c_str() );
            return StatusSystemError;
          }

          auto path = relative.string();
          std::ranges::replace( path, '\\', '/' );
          auto id = StringId( path );
          mCoreLog( "makeIdsToFiles: " mFmtStringHash " -> %s\n", id.getHash(), path.c_str() );

          if( sData->idsToFiles.contains( id ) )
          {
            core::setErrorDetails( "makeIdsToFiles: already contains same id: %s (" mFmtStringHash ")", path.c_str(), id.getHash() );
            return StatusSystemError;
          }

          sData->idsToFiles.emplace_unique( id, std::move( path ) );
        }
      }

      it.increment( ec );
      if( ec )
      {
        core::setErrorDetails( "makeIdsToFiles: increment directory iterator: %s", ec.message().c_str() );
        return StatusSystemError;
      }
    } while( it != stdfs::recursive_directory_iterator() );

    return StatusOk;
  }
} // namespace


Status data::initialize()
{
  sData = new StaticData();
  mCoreCheckStatus( parseProjectConfig() );
  mCoreCheckStatus( makeIdsToFiles() );
  mCoreCheckStatus( initializeRenderChunk() );
  return StatusOk;
}

void data::destroy()
{
  destroyRenderChunk();
  delete sData;
}

void data::update()
{
  updateRenderChunk();
}

std::string data::getDataPath( StringId id )
{
  assert( sData->idsToFiles.contains( id ) );
  auto relativePath = sData->idsToFiles.get_or_return_default( id );
  return ( sData->dataDirectory / relativePath ).string();
}

std::string data::getDataPath( const stdfs::path& resourceName )
{
  return ( sData->dataDirectory / resourceName ).string();
}
