#include "scene/process-scene.hpp"
#include "scene/meta-components.hpp"

using namespace intermediate;

namespace
{
  void postprocessDefault( meta::Scene& scene )
  {
    auto freeFlyComponentFilter = []( const meta::Entity& e ) {
      return e.hasComponent( game::FreeFlyCameraComponent::getComponentId() );
    };

    if( !std::ranges::any_of( scene.entities, freeFlyComponentFilter ) )
    {
      scene.addEntity( "debug-free-fly" )
          .addComponent<core::logic::TransformComponent>()
          .addComponent<game::FreeFlyCameraComponent>();
    }

    std::ranges::find_if( scene.entities, freeFlyComponentFilter )
        ->addComponent<core::logic::PointLightComponent>( {
            .color     = Vec3( 1, 1, 1 ),
            .intensity = 50,
        } );
  }

  void postprocess( const std::string& name, meta::Scene& scene )
  {
    if( name == "maps/mall-real/mall-real-split/mref" )
    {
      scene.getEntity( "mref-camera" )
          .addComponent( meta::automatic::freeFlyCamera );

      scene.getEntity( "mref-portal-mrff" )
          .addComponent<game::ScenePortalComponent>( {
              .toSceneId = StringId( "maps/mall-real/mall-real-split/mrff" ),
          } );
    }
    else if( name == "maps/mall-real/mall-real-split/mrff" )
    {
      scene.getEntity( "mrff-camera" )
          .addComponent( meta::automatic::freeFlyCamera );

      scene.getEntity( "mrff-portal-mree" )
          .addComponent<game::ScenePortalComponent>( {
              .toSceneId = StringId( "maps/mall-real/mall-real-split/mree" ),
          } );
    }
    else if( name == "maps/mall-real/mall-real-split/mree" )
    {
      scene.getEntity( "mree-camera" )
          .addComponent( meta::automatic::freeFlyCamera );
    }


    postprocessDefault( scene );
  }
} // namespace


core::data::ShSceneInfo intermediate::processScene( const SceneInfo& sceneInfo )
{
  auto scene = meta::Scene();

  for( const auto& obj: sceneInfo.objects )
  {
    auto& entity = scene.addEntity( &obj );
    entity.addComponent( meta::automatic::transform );

    if( obj.mesh )
    {
      entity.addComponent( meta::automatic::material );
      entity.addComponent( meta::automatic::renderMesh );
    }
  }

  postprocess( sceneInfo.name, scene );

  return core::data::ShSceneInfo{
      .objects = scene.serialize(),
  };
}
