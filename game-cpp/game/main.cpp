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
  //core::logic::sceneLoad( "X0/MR1F-MFA/mr1f-pp" );
  //core::logic::sceneLoad( "maps/mall-real/mall-real-split" );
  core::logic::sceneLoad( "maps/mall-real/mall-real-split/mr11" );

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
