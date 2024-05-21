#include "game/components/components.hpp"

using namespace game;


void FreeFlyCameraComponent::init()
{
  transform = getComponent<core::logic::TransformComponent>();
  rotation.setForward( props.forward );
  rotation.setRight( props.right );
}


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
      rotation.rotate( -delta.x, delta.y );
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

      f32 moveFactorPerMs = 0.002f;

      if( isKeyMod( KeyModShift ) )
        moveFactorPerMs *= 4;

      f32 moveFactor = core::loopGetDeltaTime().getMsF() * moveFactorPerMs;
      transform->props.position += deltaForward * moveFactor * rotation.getForward();

      // TODO: this is messed up: KeyA should add positive to deltaRight but it moves left...
      transform->props.position += deltaRight * moveFactor * rotation.getRight();
    }
  }

  core::math::Camera camera;
  camera.focalLength = 28.0f;
  camera.aspectRatio = core::render::gapi::gDevice->viewport.fSize.x /
                       core::render::gapi::gDevice->viewport.fSize.y;
  camera.position  = transform->props.position;
  camera.direction = rotation.getForward();


  auto& renderList                     = core::render::getRenderList();
  renderList.viewPosition              = camera.position;
  renderList.viewToProjectionTransform = camera.getViewToProjectionTransform();
  renderList.worldToViewTransform      = camera.getWorldToViewTransform();
}


void ScenePortalComponent::init()
{
  // NOTE: assuming that this component doesn't move
  // NOTE: scale is half size of bounding box, and position is it's center
  // TODO: take rotation into account

  auto* transform = getComponent<core::logic::TransformComponent>();

  bb.center = transform->props.position - transform->props.scale;
  bb.bx     = Vec3( transform->props.scale.x, 0, 0 ) * 2;
  bb.by     = Vec3( 0, transform->props.scale.y, 0 ) * 2;
  bb.bz     = Vec3( 0, 0, transform->props.scale.z ) * 2;
}

void ScenePortalComponent::update( const core::system::DeltaTime& dt )
{
  ( void ) dt;

  bb.debugDraw();

  for( auto it = getEntity()->getScene()->entitiesIteratorBegin();
       it != getEntity()->getScene()->entitiesIteratorEnd();
       ++it )
  {
    if( auto* camera = it->tryGetComponent<FreeFlyCameraComponent>() )
    {
      if( bb.isInside( camera->transform->props.position ) )
      {
        core::logic::sceneUnload( getEntity()->getScene()->getId() );
        core::logic::sceneLoad( props.toSceneId );
      }
    }
  }
}


void game::registerComponents()
{
  core::logic::componentRegister<FreeFlyCameraComponent>();
  core::logic::componentRegister<ScenePortalComponent>();
}
