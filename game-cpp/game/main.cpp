#include "core/core.hpp"
#include "core/system/system.hpp"
#include "game/subsys/subsys.hpp"


int main( int, char** )
{
  if( auto s = core::initialize(); s != StatusOk )
    core::system::fatalError( "Core initialization failed: %d %s", ( int ) s, core::getErrorDetails() );

  if( auto s = game::subsysInit(); s != StatusOk )
    core::system::fatalError( "Game initialization failed: %d %s", ( int ) s, core::getErrorDetails() );

  game::SceneManager::i->loadScene( "maps/debug/cs-arrows" );
  game::SceneManager::i->loadScene( "X0/MR1F-MFA/mr1f-pp" );

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
