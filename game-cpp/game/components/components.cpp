#include "game/components/components.hpp"
#include "game/subsys/subsys.hpp"

using namespace game;


namespace
{
  std::array sScenes = {
      "maps/mall-real/mall-real-split/mr11",
      "maps/mall-real/mall-real-split/mr12",
      "maps/mall-real/mall-real-split/mr1e",
      "maps/mall-real/mall-real-split/mr1f",
      "maps/mall-real/mall-real-split/mr21",
      "maps/mall-real/mall-real-split/mr22",
      "maps/mall-real/mall-real-split/mr2e",
      "maps/mall-real/mall-real-split/mr2f",
      "maps/mall-real/mall-real-split/mrd1",
      "maps/mall-real/mall-real-split/mrdf",
      "maps/mall-real/mall-real-split/mre1",
      "maps/mall-real/mall-real-split/mree",
      "maps/mall-real/mall-real-split/mref",
      "maps/mall-real/mall-real-split/mrf1",
      "maps/mall-real/mall-real-split/mrf2",
      "maps/mall-real/mall-real-split/mrfd",
      "maps/mall-real/mall-real-split/mrfe",
      "maps/mall-real/mall-real-split/mrff",
  };

  int sCurrentScene = 0;
} // namespace


void FreeFlyCameraComponent::deserialize( const Json::object_t& obj )
{
  obj.at( "position" ).get_to( position_ );

  Vec3 forward;
  obj.at( "forward" ).get_to( forward );
  rotation_.setForward( forward );

  Vec3 right;
  obj.at( "right" ).get_to( right );
  rotation_.setRight( right );
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
  camera.focalLength = 28.0f;
  camera.aspectRatio = core::render::gapi::gDevice->viewport.fSize.x /
                       core::render::gapi::gDevice->viewport.fSize.y;
  camera.position  = position_;
  camera.direction = rotation_.getForward();

  auto& renderList                     = core::render::getRenderList();
  renderList.viewPosition              = camera.position;
  renderList.viewToProjectionTransform = camera.getViewToProjectionTransform();
  renderList.worldToViewTransform      = camera.getWorldToViewTransform();
}


void RemoveSceneComponent::update( const core::system::DeltaTime& )
{
  using namespace core::input;

  auto modulo = []( int a, int b ) -> int {
    const int result = a % b;
    return result >= 0 ? result : result + b;
  };

  if( isKeyDown( Key1 ) )
  {
    core::logic::sceneUnload( sScenes[static_cast<size_t>( sCurrentScene )] );
    sCurrentScene = modulo( sCurrentScene - 1, static_cast<int>( sScenes.size() ) );
    core::logic::sceneLoad( sScenes[static_cast<size_t>( sCurrentScene )] );
  }

  if( isKeyDown( Key0 ) )
  {
    core::logic::sceneUnload( sScenes[static_cast<size_t>( sCurrentScene )] );
    sCurrentScene = modulo( sCurrentScene + 1, static_cast<int>( sScenes.size() ) );
    core::logic::sceneLoad( sScenes[static_cast<size_t>( sCurrentScene )] );
  }
}


void game::registerComponents()
{
  core::logic::componentRegister<FreeFlyCameraComponent>();
  core::logic::componentRegister<RemoveSceneComponent>();
}
