#include "scene/meta-components.hpp"
#include "scene/meta.hpp"
#include "game/components/components.hpp"

using namespace intermediate;
using namespace intermediate::meta;


core::data::ShComponent TransformComponent::build( Entity& entity ) const
{
  mFailIf( !entity.objectInfo );
  auto& obj = *entity.objectInfo;

  auto props = core::logic::TransformComponentProps{
      .position = obj.transform.position,
      .rotation = obj.transform.rotation,
      .scale    = obj.transform.scale,
  };
  return core::data::ShComponent{
      .type = core::logic::TransformComponent::getComponentId(),
      .data = Json( props ),
  };
}


core::data::ShComponent RenderMeshComponent::build( Entity& entity ) const
{
  mFailIf( !entity.objectInfo );
  auto& obj = *entity.objectInfo;

  // TODO: this is common logic with render-chunk/image.cpp
  auto diffuseTexturePath = stdfs::path( obj.mesh->material_info.diffuse.path )
                                .lexically_normal()
                                .replace_extension( "" )
                                .generic_string();
  auto props = core::logic::RenderMeshComponentProps{
      .meshId           = entity.getId(),
      .textureDiffuseId = StringId( diffuseTexturePath ),
  };
  return core::data::ShComponent{
      .type = core::logic::RenderMeshComponent::getComponentId(),
      .data = Json( props ),
  };
}


core::data::ShComponent FreeFlyCameraComponent::build( Entity& entity ) const
{
  // default props
  auto props = game::FreeFlyCameraComponentProps{
      .position = Vec3( 0 ),
      .right    = core::math::gGlobalRight,
      .forward  = core::math::gGlobalForward,
  };

  if( entity.objectInfo )
  {
    mFailIf( entity.objectInfo->type != "CAMERA" );
    auto matrix    = glm::toMat3( entity.objectInfo->transform.rotation );
    props.position = entity.objectInfo->transform.position;
    props.right    = Vec3( matrix[0] );  //  x direction is right
    props.forward  = -Vec3( matrix[2] ); // -z direction is forward
  }

  return core::data::ShComponent{
      .type = game::FreeFlyCameraComponent::getComponentId(),
      .data = Json( props ),
  };
}


core::data::ShComponent ScenePortalComponent::build( Entity& entity ) const
{
  ( void ) entity;
  auto props = game::ScenePortalComponentProps{
      .toSceneId = toSceneId.getHash(),
  };
  return core::data::ShComponent{
      .type = game::ScenePortalComponent::getComponentId(),
      .data = Json( props ),
  };
}
