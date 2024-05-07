#include "game/data/serialization.hpp"
#include <nlohmann/json.hpp>

using namespace game;

namespace game
{
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShComponent, type, data );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShObjectInfo, name, components );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( SceneInfo, objects, render_chunks );
} // namespace game


Status game::parseJsonFile( std::string path, SceneInfo& output )
{
  auto json = Json();
  mCoreCheckStatus( core::fs::readFileJson( path, json ) );

  try
  {
    output = json.get<SceneInfo>();
  }
  catch( const nlohmann::json::exception& ex )
  {
    mCoreLogError( "can't deserialize scene json: %s\n", ex.what() );
    core::setErrorDetails( ex.what() );
    return StatusSystemError;
  }

  return StatusOk;
}
