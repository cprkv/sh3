#include "core/common.hpp"
#include "core/utils.hpp"
#include "SDL_main.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD   ul_reason_for_call,
                       LPVOID  lpReserved )
{
  ( void ) lpReserved;
  ( void ) hModule;
  switch( ul_reason_for_call )
  {
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_ATTACH:
    case DLL_PROCESS_DETACH:
      break;
  }
  return TRUE;
}


mCoreAPI Status coreInitialize()
{
  Uint32 sdlInitSubsystems = SDL_INIT_AUDIO |
                             SDL_INIT_VIDEO |
                             SDL_INIT_JOYSTICK |
                             SDL_INIT_GAMECONTROLLER |
                             SDL_INIT_EVENTS;

  if( SDL_InitSubSystem( sdlInitSubsystems ) )
  {
    mCoreLogError( "core initialization error\n" );
    coreSetErrorDetails( SDL_GetError() );
    return StatusSystemError;
  }
  mCoreLog( "SDL2 initialized\n" );

  mCoreLog( "core initialize succeeded\n" );
  return StatusSystemError;
}


mCoreAPI void coreDestroy()
{
  mCoreLog( "core destroy started\n" );
  coreSetErrorDetails( nullptr ); // free error details
}
