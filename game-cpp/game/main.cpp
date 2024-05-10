#include "core/core.hpp"
#include "core/system/system.hpp"
#include "game/subsys/subsys.hpp"
#include "game/components/components.hpp"


int main( int, char** )
{
  if( auto s = core::initialize(); s != StatusOk )
    core::system::fatalError( "Core initialization failed: %d %s", static_cast<int>( s ), core::getErrorDetails() );

  if( auto s = game::subsysInit(); s != StatusOk )
    core::system::fatalError( "Game initialization failed: %d %s", static_cast<int>( s ), core::getErrorDetails() );

  game::registerComponents();
  core::logic::sceneLoad( "maps/debug/cs-arrows" );
  core::logic::sceneLoad( "X0/MR1F-MFA/mr1f-pp" );

  // add new scene with camera
  {
    auto* scene = core::logic::sceneNew( "demo" );

    auto* cameraEntity = scene->addEntity( "camera"_sid );
    cameraEntity->addComponent<game::FreeFlyCameraComponent>();

    auto* removeSceneComponent = scene->addEntity( "removeScene"_sid );
    removeSceneComponent->addComponent<game::RemoveSceneComponent>();

    scene->init();
  }

  for( ;; )
  {
    if( auto loopStatus = core::loopStepBegin();
        loopStatus == core::LoopStatusQuitRequested )
      break;

    game::subsysUpdate();
    core::loopStepEnd();
  }

  game::subsysShutdown();
  core::destroy();

#if 0
  fprintf( stdout, "press any key to exit\n" );
  getchar();
#endif

  return 0;
}
