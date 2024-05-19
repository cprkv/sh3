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
    return nullptr;
  }

  core::render::Texture* findTexture( core::data::RenderChunksView renderChunks, StringId id )
  {
    for( auto& chunk: renderChunks )
      if( auto* mesh = chunk.getTexture( id ) )
        return mesh;

    assert( false ); // texture not found
    return nullptr;
  }
} // namespace


Mat4 TransformComponent::getWorldTransform() const
{
  return glm::translate( position ) *
         glm::toMat4( rotation ) *
         glm::scale( scale );
}

void TransformComponent::deserialize( TransformComponentProps props )
{
  position = props.position;
  rotation = props.rotation;
  scale    = props.scale;
}


void RenderMeshComponent::deserialize( RenderMeshComponentProps props )
{
  mesh           = findMesh( getEntity()->getScene()->getRenderChunks(), props.meshId );
  textureDiffuse = findTexture( getEntity()->getScene()->getRenderChunks(), props.textureDiffuseId );
}

void RenderMeshComponent::init()
{
  transform = getComponent<TransformComponent>();
  assert( transform );
}

void RenderMeshComponent::update( const core::system::DeltaTime& )
{
  render::getRenderList().addMesh( mesh, textureDiffuse, transform->getWorldTransform() );
}


void core::logic::registerComponents()
{
  core::logic::componentRegister<TransformComponent>();
  core::logic::componentRegister<RenderMeshComponent>();
}
