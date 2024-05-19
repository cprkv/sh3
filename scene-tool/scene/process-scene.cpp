#include "scene/process-scene.hpp"
#include "game/components/components.hpp"

using namespace intermediate;

namespace
{

}


core::data::ShSceneInfo intermediate::processScene( const SceneInfo& sceneInfo )
{
  auto outScene = core::data::ShSceneInfo();

  for( const auto& obj: sceneInfo.objects )
  {
    auto& objInfo = outScene.objects.emplace_back();
    objInfo.id    = StringId( obj.name );

    auto transform = core::logic::TransformComponentProps{
        .position = obj.transform.position,
        .rotation = obj.transform.rotation,
        .scale    = obj.transform.scale,
    };
    auto transformComponent = core::data::ShComponent{
        .type = core::logic::TransformComponent::getComponentId(),
        .data = Json( transform ),
    };
    objInfo.components.emplace_back( std::move( transformComponent ) );

    if( obj.mesh )
    {
      // TODO (this is common logic with render-chunk/image.cpp
      auto diffuseTexturePath = stdfs::path( obj.mesh->material_info.diffuse.path )
                                    .lexically_normal()
                                    .replace_extension( "" )
                                    .generic_string();
      auto diffuseTextureId = StringId( diffuseTexturePath );

      auto renderMesh = core::logic::RenderMeshComponentProps{
          .meshId           = objInfo.id,
          .textureDiffuseId = diffuseTextureId,
      };
      auto renderMeshComponent = core::data::ShComponent{
          .type = core::logic::RenderMeshComponent::getComponentId(),
          .data = Json( renderMesh ),
      };
      objInfo.components.emplace_back( std::move( renderMeshComponent ) );
    }
  }

  {
    auto& objInfo = outScene.objects.emplace_back();
    objInfo.id    = StringId( "demo-camera" );

    auto freeFlyCamera = game::FreeFlyCameraComponentProps{
        .position = { 20.0f, -39.0f, 1.4f },
        .right    = core::math::gGlobalRight,
        .forward  = core::math::gGlobalForward,
    };
    auto freeFlyCameraComponent = core::data::ShComponent{
        .type = game::FreeFlyCameraComponent::getComponentId(),
        .data = Json( freeFlyCamera ),
    };
    objInfo.components.emplace_back( std::move( freeFlyCameraComponent ) );
  }

  return outScene;
}
