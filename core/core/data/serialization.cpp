#include "core/data/serialization.hpp"
#include "core/fs/fs.hpp"
#include <nlohmann/json.hpp>

using namespace core;

namespace core::data
{
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShComponent, type, data );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShObjectInfo, name, components );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShSceneInfo, objects, render_chunks );
} // namespace game


Status data::parseJsonFile( std::string path, ShSceneInfo& output )
{
  auto json = Json();
  mCoreCheckStatus( core::fs::readFileJson( path, json ) );

  try
  {
    output = json.get<ShSceneInfo>();
  }
  catch( const nlohmann::json::exception& ex )
  {
    mCoreLogError( "can't deserialize scene json: %s\n", ex.what() );
    core::setErrorDetails( ex.what() );
    return StatusSystemError;
  }

  return StatusOk;
}
