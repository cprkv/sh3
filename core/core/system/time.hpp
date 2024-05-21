#pragma once
#include "core/common.hpp"

namespace core::system
{
  class Stopwatch
  {
    using Clock = std::chrono::high_resolution_clock;

    Clock::time_point begin_;

  public:
    Stopwatch() { reset(); }

    void reset();

    u64 getMs();   // milliseconds
    f32 getMsF();  // milliseconds
    u64 getUs();   // microseconds
    f32 getSecF(); // seconds
  };


  class DeltaTime
  {
    Stopwatch sw_;
    u64       us_  = 16'666u;
    u32       ms_  = 17u;
    f32       msf_ = 16.66666f;

  public:
    void onLoopStart();
    void onLoopEnd();

    u64 getUs() const { return us_; }   // microseconds
    u32 getMs() const { return ms_; }   // milliseconds
    f32 getMsF() const { return msf_; } // milliseconds
  };
} // namespace core::system
