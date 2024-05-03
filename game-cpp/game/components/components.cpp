#include "game/components/components.hpp"
#include "game/subsys/subsys.hpp"

using namespace game;

namespace
{
  // temporary all components live here...

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
  };


  class RenderMeshComponent : public core::Component
  {
  public:
    mCoreComponent( RenderMeshComponent );

    StringId            mesh;
    StringId            textureDiffuse;
    TransformComponent* transform;

    void init() override
    {
      transform = getComponent<TransformComponent>();
      assert( transform );
    }

    void update( const core::system::DeltaTime& dt ) override
    {
      RenderManager::i->renderList.addMesh( mesh, textureDiffuse, transform->getWorldTransform() );
    }
  };


  template<typename T>
  const T* findShComponent( const std::vector<std::unique_ptr<ShComponent>>& components )
  {
    T test;

    for( const auto& component: components )
      if( component->getType() == test.getType() )
        return static_cast<T*>( component.get() );

    return nullptr;
  }
} // namespace


void FreeFlyCameraComponent::update( const core::system::DeltaTime& dt )
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
  auto* transform = findShComponent<ShComponentTransform>( objectInfo.components );
  auto* material  = findShComponent<ShComponentMaterial>( objectInfo.components );
  auto* mesh      = findShComponent<ShComponentMesh>( objectInfo.components );

  if( transform )
  {
    auto* c     = entity.addComponent<TransformComponent>();
    c->position = transform->position;
    c->rotation = transform->rotation;
    c->scale    = transform->scale;
  }

  if( material && mesh )
  {
    auto* c           = entity.addComponent<RenderMeshComponent>();
    c->mesh           = mesh->id;
    c->textureDiffuse = material->diffuse;
  }
}
