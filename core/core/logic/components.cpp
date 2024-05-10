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


  class TransformComponent : public core::Component
  {
  public:
    mCoreComponent( TransformComponent );

    Vec3 position;
    Quat rotation;
    Vec3 scale;

    Mat4 getWorldTransform() const
    {
      return glm::translate( position ) *
             glm::toMat4( rotation ) *
             glm::scale( scale );
    }

    void deserialize( const Json::object_t& obj ) override
    {
      obj.at( "position" ).get_to( position );
      obj.at( "rotation" ).get_to( rotation );
      obj.at( "scale" ).get_to( scale );
    }
  };


  class RenderMeshComponent : public core::Component
  {
  public:
    mCoreComponent( RenderMeshComponent );

    core::render::Mesh*    mesh;
    core::render::Texture* textureDiffuse;
    TransformComponent*    transform;

    void deserialize( const Json::object_t& obj ) override
    {
      StringId meshId;
      StringId textureDiffuseId;

      obj.at( "mesh" ).get_to( meshId );
      obj.at( "textureDiffuse" ).get_to( textureDiffuseId );

      mesh           = findMesh( getEntity()->getScene()->getRenderChunks(), meshId );
      textureDiffuse = findTexture( getEntity()->getScene()->getRenderChunks(), textureDiffuseId );
    }

    void init() override
    {
      transform = getComponent<TransformComponent>();
      assert( transform );
    }

    void update( const core::system::DeltaTime& ) override
    {
      render::getRenderList().addMesh( mesh, textureDiffuse, transform->getWorldTransform() );
    }
  };
} // namespace


void core::logic::registerComponents()
{
  core::logic::componentRegister<TransformComponent>();
  core::logic::componentRegister<RenderMeshComponent>();
}
