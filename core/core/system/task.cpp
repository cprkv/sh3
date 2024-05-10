#include "core/core.hpp"

using namespace core;
using namespace core::system;
using namespace core::system::task;

namespace
{
  constexpr u32 sDefferedTasksDeadlineMs = 14; // 16 ms per frame for 60 fps

  struct StaticData
  {
    system::MessageQueue<Task> defferedTasks;
    BS::thread_pool            threadPool{ 1 };
    std::list<PeriodicalTask>  periodicalTasks;
  };

  StaticData* sData = nullptr;
} // namespace


Status task::init()
{
  sData = new StaticData();
  return StatusOk;
}

void task::destroy()
{
  sData->threadPool.purge();
  delete sData;
}

void task::update()
{
  // do deffered tasks
  {
    auto stopwatch      = core::system::Stopwatch();
    u32  tasksCompleted = 0;

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
      if( timePassedMs >= sDefferedTasksDeadlineMs )
      {
#ifdef _DEBUG
        auto diff = timePassedMs - sDefferedTasksDeadlineMs;
        if( diff > 0 )
          mCoreLogDebug( "deffered tasks is " mFmtU64 "ms ahead of deadline\n", diff );
        mCoreLogDebug( "deffered tasks completed: " mFmtU32 "\n", tasksCompleted );
#endif
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
}

void task::runDeffered( Task task )
{
  sData->defferedTasks.push( std::move( task ) );
}

void task::runPeriodical( PeriodicalTask task )
{
  sData->periodicalTasks.emplace_back( std::move( task ) );
}

void task::runAsync( Task task )
{
  ( void ) sData->threadPool.detach_task( std::move( task ) );
}
