#pragma once
#include "core/common.hpp"
#include "core/data/data.hpp"
#include "core/fs/fs.hpp"
#include "core/system/system.hpp"
#include "core/render/render.hpp"
#include "core/input/input.hpp"
#include "core/math/math.hpp"

namespace core
{
  enum LoopStatus
  {
    LoopStatusContinue,
    LoopStatusQuitRequested,
  };

  enum PeriodicalStatus
  {
    PeriodicalStatusContinue,
    PeriodicalStatusStop,
  };

  using Task           = std::move_only_function<void()>;
  using PeriodicalTask = std::move_only_function<PeriodicalStatus()>;

  Status initialize();
  void   destroy();

  LoopStatus               loopStepBegin();
  void                     loopStepEnd();
  void                     loopEnqueueDefferedTask( Task task );
  void                     loopEnqueueTask( Task task );
  void                     loopEnqueuePeriodicalTask( PeriodicalTask task );
  const system::DeltaTime& loopGetDeltaTime();
} // namespace core
