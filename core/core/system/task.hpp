#pragma once
#include "core/common.hpp"
#include "core/system/message-queue.hpp"
#include "core/system/task-result.hpp"

namespace core::system::task
{
  Status init();
  void   destroy();
  void   update();

  enum PeriodicalStatus
  {
    PeriodicalStatusContinue,
    PeriodicalStatusStop,
  };

  using Task           = std::move_only_function<void()>;
  using PeriodicalTask = std::move_only_function<PeriodicalStatus()>;

  void runDeffered( Task task );
  void runAsync( Task task );
  void runPeriodical( PeriodicalTask task );

  template<typename F>
  auto runDeffered1( F&& task ) -> DefferedResult<std::invoke_result_t<F>>
  {
    auto result = DefferedResult<std::invoke_result_t<F>>();
    runDeffered( [task = std::move( task ), result]() mutable {
      auto value = task();
      result.setResult( value );
    } );
    return result;
  }

  template<typename F>
  auto runAsync1( F&& task ) -> AsyncResult<std::invoke_result_t<F>>
  {
    auto result = AsyncResult<std::invoke_result_t<F>>();
    runAsync( [task = std::move( task ), result]() mutable {
      auto value = task();
      result.setResult( value );
    } );
    return result;
  }
} // namespace core::system::task
