#include "scene/meta-components.hpp"
#include "scene/meta.hpp"
#include "render-chunk/texture.hpp"
#include "game/components/components.hpp"

using namespace intermediate;
using namespace intermediate::meta;


namespace
{
  template<typename TComponent>
  core::data::ShComponent fromProps( typename TComponent::Props props = {} )
  {
    return core::data::ShComponent{
        .type = TComponent::getComponentId(),
        .data = Json( props ),
    };
  }
} // namespace


core::data::ShComponent automatic::transform( Entity& entity )
{
  return fromProps<core::logic::TransformComponent>( {
      .position = entity.objectInfo->transform.position,
      .rotation = entity.objectInfo->transform.rotation,
      .scale    = entity.objectInfo->transform.scale,
  } );
}

core::data::ShComponent automatic::material( Entity& entity )
{
  auto& material = entity.objectInfo->mesh->material_info;
  return fromProps<core::logic::MaterialComponent>( {
      .textureDiffuseId = textureHash( material.diffuse ),
      .blendMode        = parseBlendMode( material.blend_mode ),
  } );
}

core::data::ShComponent automatic::renderMesh( Entity& entity )
{
  return fromProps<core::logic::RenderMeshComponent>( {
      .meshId = entity.getId(),
  } );
}

core::data::ShComponent automatic::pointLight( Entity& entity )
{
  ( void ) entity;
  // TODO: point light is not present in serialized data from blender
  return fromProps<core::logic::PointLightComponent>( {
      .color     = core::math::decodeColorHex( 0xE7'8C'FF ),
      .intensity = 40,
  } );
}

core::data::ShComponent automatic::freeFlyCamera( Entity& entity )
{
  mFailIf( entity.objectInfo->type != "CAMERA" );

  auto matrix  = glm::toMat3( entity.objectInfo->transform.rotation );
  auto right   = Vec3( matrix[0] );  //  x direction is right
  auto forward = -Vec3( matrix[2] ); // -z direction is forward

  return fromProps<game::FreeFlyCameraComponent>( {
      .right   = right,
      .forward = forward,
  } );
}
