#pragma once
#include "core/common.hpp"
#include "core/data/data.hpp"
#include "core/fs/fs.hpp"
#include "core/system/system.hpp"
#include "core/render/render.hpp"
#include "core/input/input.hpp"
#include "core/math/math.hpp"
#include "core/logic/logic.hpp"
#include "core/utils.hpp"

namespace core
{
  enum LoopStatus
  {
    LoopStatusContinue,
    LoopStatusQuitRequested,
  };

  Status initialize();
  void   destroy();

  LoopStatus               loopStepBegin();
  void                     loopStepEnd();
  const system::DeltaTime& loopGetDeltaTime();
} // namespace core
