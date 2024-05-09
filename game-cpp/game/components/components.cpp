#include "game/components/components.hpp"
#include "game/subsys/subsys.hpp"

using namespace game;

namespace
{
  // temporary all components live here...

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
      RenderManager::i->renderList.addMesh( mesh, textureDiffuse, transform->getWorldTransform() );
    }
  };
} // namespace


void FreeFlyCameraComponent::update( const core::system::DeltaTime& )
{
  {
    using namespace core::input;

    // rotation
    {
      auto delta = Vec2( 0 );

      if( isKeyPressed( KeyUp ) && !isKeyPressed( KeyDown ) )
        delta.y = 1;
      else if( !isKeyPressed( KeyUp ) && isKeyPressed( KeyDown ) )
        delta.y = -1;

      if( isKeyPressed( KeyLeft ) && !isKeyPressed( KeyRight ) )
        delta.x = -1;
      else if( !isKeyPressed( KeyLeft ) && isKeyPressed( KeyRight ) )
        delta.x = 1;

      f32 rotateFactor = core::loopGetDeltaTime().getMsF() * 0.002f;
      delta *= rotateFactor;

      // TODO: this is messed up: delta.x should be positive...
      rotation_.rotate( -delta.x, delta.y );
    }

    // translation
    {
      f32 deltaForward = 0;
      f32 deltaRight   = 0;

      if( isKeyPressed( KeyA ) && !isKeyPressed( KeyD ) )
        deltaRight = -1;
      else if( !isKeyPressed( KeyA ) && isKeyPressed( KeyD ) )
        deltaRight = 1;

      if( isKeyPressed( KeyW ) && !isKeyPressed( KeyS ) )
        deltaForward = 1;
      else if( !isKeyPressed( KeyW ) && isKeyPressed( KeyS ) )
        deltaForward = -1;

      f32 moveFactor = core::loopGetDeltaTime().getMsF() * 0.01f;
      position_ += deltaForward * moveFactor * rotation_.getForward();

      // TODO: this is messed up: KeyA should add positive to deltaRight but it moves left...
      position_ += deltaRight * moveFactor * rotation_.getRight();
    }
  }

  core::math::Camera camera;
  camera.aspectRatio = core::render::gapi::gDevice->viewport.fSize.x /
                       core::render::gapi::gDevice->viewport.fSize.y;
  camera.position  = position_;
  camera.direction = rotation_.getForward();

  auto& renderList                     = RenderManager::i->renderList;
  renderList.viewPosition              = camera.position;
  renderList.viewToProjectionTransform = camera.getViewToProjectionTransform();
  renderList.worldToViewTransform      = camera.getWorldToViewTransform();
}


void game::instantiateComponents( core::Entity& entity, const ShObjectInfo& objectInfo )
{
  // TODO: this is developer's try-catch
  try
  {
    for( auto& component: objectInfo.components )
    {
      auto* componentInstance = core::logic::instantiateComponent( component.type, &entity );
      componentInstance->deserialize( component.data );
      entity.addComponent( component.type, componentInstance );
    }
  }
  catch( std::exception& ex )
  {
    mCoreLogError( "instantiate components on entity " mFmtStringHash " failed: %s\n",
                   entity.getId().getHash(), ex.what() );
    assert( false );
  }
}


void game::registerComponents()
{
  core::logic::registerComponent<TransformComponent>();
  core::logic::registerComponent<RenderMeshComponent>();
  core::logic::registerComponent<FreeFlyCameraComponent>();
}
