#include "core/core.hpp"
#include "core/utils.hpp"
#include <SDL_syswm.h>
#include <dwmapi.h>

using namespace core;

namespace
{
  constexpr const Uint32 sSDLInitSubsystems = SDL_INIT_AUDIO |
                                              SDL_INIT_VIDEO |
                                              SDL_INIT_JOYSTICK |
                                              SDL_INIT_GAMECONTROLLER |
                                              SDL_INIT_EVENTS;

  constexpr const char* sWindowName = "sh3";

  struct StaticData
  {
    SDL_Window*       window = nullptr;
    system::DeltaTime deltaTime;
  };

  StaticData* sData = nullptr;


  Status enableDarkMode()
  {
    auto sysWmInfo = SDL_SysWMinfo{};
    SDL_VERSION( &sysWmInfo.version );
    if( !SDL_GetWindowWMInfo( sData->window, &sysWmInfo ) )
    {
      mCoreLogError( "error get window information\n" );
      core::setErrorDetails( SDL_GetError() );
      return StatusSystemError;
    }

    constexpr BOOL useImmersiveDarkMode = true;
    mCoreCheckHR( DwmSetWindowAttribute(
        sysWmInfo.info.win.window, DWMWA_USE_IMMERSIVE_DARK_MODE,
        &useImmersiveDarkMode, sizeof( useImmersiveDarkMode ) ) );

    return StatusOk;
  }


  Status initSDL()
  {
    if( SDL_InitSubSystem( sSDLInitSubsystems ) )
    {
      mCoreLogError( "sdl init subsystems error\n" );
      core::setErrorDetails( SDL_GetError() );
      return StatusSystemError;
    }

    int windowPosX  = SDL_WINDOWPOS_CENTERED;
    int windowPosY  = SDL_WINDOWPOS_CENTERED;
    int windowSizeX = 1920;
    int windowSizeY = 1080;

    sData->window = SDL_CreateWindow( sWindowName,
                                      windowPosX, windowPosY,
                                      windowSizeX, windowSizeY,
                                      SDL_WINDOW_ALLOW_HIGHDPI );
    if( !sData->window )
    {
      mCoreLogError( "create window error\n" );
      core::setErrorDetails( SDL_GetError() );
      return StatusSystemError;
    }

    if( auto s = enableDarkMode(); s != StatusOk )
    {
      mCoreLogError( "dark mode enable failed\n" );
    }

    mCoreLog( "SDL2 initialized\n" );
    return StatusOk;
  }


  Status initData()
  {
    if( auto s = data::initialize(); s != StatusOk )
    {
      mCoreLogError( "data initialize failed\n" );
      return s;
    }
    mCoreLog( "data initialized\n" );
    return StatusOk;
  }


  Status initRender()
  {
    auto sysWmInfo = SDL_SysWMinfo{};
    SDL_VERSION( &sysWmInfo.version );
    if( !SDL_GetWindowWMInfo( sData->window, &sysWmInfo ) )
    {
      mCoreLogError( "error get window information\n" );
      core::setErrorDetails( SDL_GetError() );
      return StatusSystemError;
    }

    if( auto s = render::initialize( sysWmInfo.info.win.window ); s != StatusOk )
    {
      mCoreLogError( "data initialize failed\n" );
      return s;
    }

    mCoreLog( "render initialized\n" );
    return StatusOk;
  }
} // namespace


Status core::initialize()
{
  commonInit();
  sData = new StaticData();
  mCoreCheckStatus( system::task::init() );
  mCoreCheckStatus( initSDL() );
  mCoreCheckStatus( initData() );
  mCoreCheckStatus( initRender() );
  mCoreCheckStatus( input::init() );
  mCoreCheckStatus( logic::init() );
  mCoreLog( "core initialize succeeded\n" );
  return StatusOk;
}


void core::destroy()
{
  mCoreLog( "core destroy started\n" );

  logic::destroy();
  input::destroy();
  render::destroy();
  data::destroy();

  // SDL2 shutdown
  {
    if( sData->window )
      SDL_DestroyWindow( sData->window );
    SDL_QuitSubSystem( sSDLInitSubsystems );
  }

  system::task::destroy();

  // static data shutdown
  delete sData;

  commonDestroy();
}


LoopStatus core::loopStepBegin()
{
  sData->deltaTime.onLoopStart();

  data::update();
  input::preUpdate();

  SDL_Event event;
  while( SDL_PollEvent( &event ) )
  {
    if( event.type == SDL_QUIT )
      return LoopStatusQuitRequested;

    input::handle( event );
  }

  system::task::update();
  logic::update();
  render::update();

  return LoopStatusContinue;
}


void core::loopStepEnd()
{
  render::present();
  sData->deltaTime.onLoopEnd();
}


const system::DeltaTime& core::loopGetDeltaTime()
{
  return sData->deltaTime;
}
