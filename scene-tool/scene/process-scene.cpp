#include "scene/process-scene.hpp"
#include "scene/meta-components.hpp"

using namespace intermediate;

namespace
{
  void postprocessDefault( meta::Scene& scene )
  {
    if( !std::ranges::any_of( scene.entities, []( const auto& e ) {
          return e.hasComponent( meta::FreeFlyCameraComponent::id );
        } ) )
    {
      scene.addEntity( "debug-free-fly" )
          .addComponent( meta::FreeFlyCameraComponent() );
    }
  }

  void postprocessMREF( meta::Scene& scene )
  {
    scene.getEntity( "mref-test-camera" )
        .addComponent( meta::FreeFlyCameraComponent() );
  }

  void postprocess( const std::string& name, meta::Scene& scene )
  {
    if( name == "maps/mall-real/mall-real-split/mref" )
      postprocessMREF( scene );

    postprocessDefault( scene );
  }
} // namespace


core::data::ShSceneInfo intermediate::processScene( const SceneInfo& sceneInfo )
{
  auto scene = meta::Scene();

  for( const auto& obj: sceneInfo.objects )
  {
    auto& entity = scene.addEntity( &obj );
    entity.addComponent( meta::TransformComponent() );

    if( obj.mesh )
      entity.addComponent( meta::RenderMeshComponent() );
  }

  postprocess( sceneInfo.name, scene );

  return core::data::ShSceneInfo{
      .objects = scene.serialize(),
  };
}
