#include "core/data/serialization.hpp"
#include "core/fs/fs.hpp"
#include <nlohmann/json.hpp>

using namespace core;

namespace core::data
{
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShComponent, type, data );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShObjectInfo, id, components );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShSceneInfo, objects, render_chunks );
} // namespace core::data


Status data::readJsonFile( const std::string& path, ShSceneInfo& output )
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


Status data::writeJsonFile( const std::string& path, const ShSceneInfo& data )
{
  Json json;

  try
  {
    json = Json( data );
  }
  catch( const nlohmann::json::exception& ex )
  {
    mCoreLogError( "can't serialize scene json: %s\n", ex.what() );
    core::setErrorDetails( ex.what() );
    return StatusSystemError;
  }

  return fs::writeFileJson( path, json );
}
