#include "core/core.hpp"
#include "core/utils.hpp"
#include "core/render/render.hpp"
#include "core/data/data.hpp"
#include "core/system/time.hpp"
#include "core/system/message-queue.hpp"
#include <SDL_syswm.h>

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
    SDL_Window*                window = nullptr;
    system::MessageQueue<Task> defferedTasks;
    BS::thread_pool            threadPool{ 1 };
    system::DeltaTime          deltaTime;
    std::list<PeriodicalTask>  periodicalTasks;
  };

  StaticData* sData = nullptr;


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
    int windowSizeX = 1024;
    int windowSizeY = 768;

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
  sData = new StaticData();
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

  // static data shutdown
  {
    core::setErrorDetails( nullptr ); // free error details
    sData->threadPool.purge();
    delete sData;
  }
}


LoopStatus core::loopStepBegin()
{
  sData->deltaTime.onLoopStart();
  auto stopwatch = core::system::Stopwatch();

  data::update();
  input::preUpdate();

  SDL_Event event;
  while( SDL_PollEvent( &event ) )
  {
    if( event.type == SDL_QUIT )
      return LoopStatusQuitRequested;

    input::handle( event );
  }

  // do deffered tasks
  {
    constexpr u32 deadlineMs     = 14; // 16 ms per frame for 60 fps
    u32           tasksCompleted = 0;

    for( ;; )
    {
      auto message = sData->defferedTasks.tryPop();
      if( !message )
      {
        if( tasksCompleted > 0 )
          mCoreLogDebug( "deffered tasks completed: " mFmtU32 "\n", tasksCompleted );
        break;
      }

      ( *message )();
      tasksCompleted++;

      u64 timePassedMs = stopwatch.getMs();
      if( timePassedMs >= deadlineMs )
      {
        auto diff = timePassedMs - deadlineMs;
        if( diff > 0 )
          mCoreLogDebug( "deffered tasks is " mFmtU64 "ms ahead of deadline\n", diff );
        mCoreLogDebug( "deffered tasks completed: " mFmtU32 "\n", tasksCompleted );
        break;
      }
    }
  }

  // do periodical tasks
  {
    auto it = sData->periodicalTasks.begin();

    while( it != sData->periodicalTasks.end() )
    {
      auto cur = it;
      ++it;

      auto periodicalStatus = ( *cur )();
      if( periodicalStatus == PeriodicalStatusStop )
        sData->periodicalTasks.erase( cur );
    }
  }

  return LoopStatusContinue;
}


void core::loopStepEnd()
{
  // run all subsystems up to end of frame (and vsync)

  render::present();
  sData->deltaTime.onLoopEnd();
}


void core::loopEnqueueDefferedTask( Task task )
{
  sData->defferedTasks.push( std::move( task ) );
}


void core::loopEnqueuePeriodicalTask( PeriodicalTask task )
{
  sData->periodicalTasks.emplace_back( std::move( task ) );
}


void core::loopEnqueueTask( Task task )
{
  ( void ) sData->threadPool.detach_task( std::move( task ) );
}


const system::DeltaTime& core::loopGetDeltaTime()
{
  return sData->deltaTime;
}
