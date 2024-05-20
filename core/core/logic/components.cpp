#include "core/logic/components.hpp"
#include "core/logic/logic.hpp"
#include "core/render/render.hpp"

using namespace core;
using namespace core::logic;

namespace
{
  core::render::Mesh* findMesh( core::data::RenderChunksView renderChunks, StringId id )
  {
    for( auto& chunk: renderChunks )
      if( auto* mesh = chunk.getMesh( id ) )
        return mesh;

    assert( false ); // mesh not found
    return nullptr;  // TODO: return default?
  }

  core::render::Texture* findTexture( core::data::RenderChunksView renderChunks, StringId id )
  {
    for( auto& chunk: renderChunks )
      if( auto* mesh = chunk.getTexture( id ) )
        return mesh;

    assert( false ); // texture not found
    return nullptr;  // TODO: return default?
  }
} // namespace


Mat4 TransformComponent::getWorldTransform() const
{
  return glm::translate( props.position ) *
         glm::toMat4( props.rotation ) *
         glm::scale( props.scale );
}


void MaterialComponent::init()
{
  textureDiffuse = findTexture( getEntity()->getScene()->getRenderChunks(), props.textureDiffuseId );
}


void RenderMeshComponent::init()
{
  mesh      = findMesh( getEntity()->getScene()->getRenderChunks(), props.meshId );
  transform = getComponent<TransformComponent>();
  material  = getComponent<MaterialComponent>();
}

void RenderMeshComponent::update( const core::system::DeltaTime& )
{
  auto drawable = render::RenderList::Drawable{
      .mesh           = mesh,
      .diffuseTexture = material->textureDiffuse,
      .blendMode      = material->props.blendMode,
      .worldTransform = transform->getWorldTransform(),
  };
  render::getRenderList().drawables.push_back( drawable );
}


void PointLightComponent::init()
{
  transform = getComponent<TransformComponent>();
}

void PointLightComponent::update( const core::system::DeltaTime& )
{
  auto pointLight = render::RenderList::PointLight{
      .position  = transform->props.position,
      .color     = props.color,
      .intensity = props.intensity,
  };
  render::getRenderList().lights.push_back( pointLight );
}


void core::logic::registerComponents()
{
  core::logic::componentRegister<TransformComponent>();
  core::logic::componentRegister<MaterialComponent>();
  core::logic::componentRegister<RenderMeshComponent>();
  core::logic::componentRegister<PointLightComponent>();
}
