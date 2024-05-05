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
    stdfs::path dataDirectory;
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
} // namespace


Status data::initialize()
{
  sData = new StaticData();
  mCoreCheckStatus( parseProjectConfig() );
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

std::string data::getDataPath( const stdfs::path& resourceName )
{
  return ( sData->dataDirectory / resourceName ).string();
}
