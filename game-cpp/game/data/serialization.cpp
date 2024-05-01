#include "game/data/serialization.hpp"
#include <nlohmann/json.hpp>

using namespace game;

namespace glm
{
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( vec3, x, y, z );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( vec4, x, y, z, w );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( fquat, x, y, z, w );
} // namespace glm

namespace nlohmann
{
  template<>
  struct adl_serializer<std::unique_ptr<ShComponent>>
  {
    static void to_json( json& j, const std::unique_ptr<ShComponent>& p )
    {
      ( void ) j;
      ( void ) p;
      throw std::logic_error( "not implemented" );
    }

    static void from_json( const json& j, std::unique_ptr<ShComponent>& p )
    {
      auto type = j.at( "$type" ).get<std::string>();

      if( type == "Transform" )
      {
        auto c = std::make_unique<ShComponentTransform>();
        j.at( "position" ).get_to( c->position );
        j.at( "rotation" ).get_to( c->rotation );
        j.at( "scale" ).get_to( c->scale );
        p = std::move( c );
      }
      else if( type == "Material" )
      {
        auto c     = std::make_unique<ShComponentMaterial>();
        c->diffuse = core::data::StringId( j.at( "diffuse" ).get<std::string>() );
        p          = std::move( c );
      }
      else if( type == "Mesh" )
      {
        auto c = std::make_unique<ShComponentMesh>();
        c->id  = core::data::StringId( j.at( "name" ).get<std::string>() );
        p      = std::move( c );
      }
      else
      {
        throw std::logic_error( "not implemented" );
      }
    }
  };
} // namespace nlohmann

namespace game
{
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( ShObjectInfo, name, components );
  NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE( SceneInfo, objects );
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
