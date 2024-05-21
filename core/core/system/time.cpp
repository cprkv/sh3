#include "core/system/time.hpp"

using namespace core::system;


void Stopwatch::reset()
{
  begin_ = Clock::now();
}

u64 Stopwatch::getMs()
{
  auto timeDiff = Clock::now() - begin_;
  return static_cast<u64>( std::chrono::duration_cast<std::chrono::milliseconds>( timeDiff ).count() );
}

f32 Stopwatch::getMsF()
{
  return static_cast<f32>( getUs() ) / 1000.f;
}

u64 Stopwatch::getUs()
{
  auto timeDiff = Clock::now() - begin_;
  return static_cast<u64>( std::chrono::duration_cast<std::chrono::microseconds>( timeDiff ).count() );
}

f32 Stopwatch::getSecF()
{
  return static_cast<f32>( getMs() ) / 1000.f;
}


void DeltaTime::onLoopStart()
{
  sw_.reset();
}

void DeltaTime::onLoopEnd()
{
  us_  = sw_.getUs();
  msf_ = static_cast<f32>( us_ ) / 1000.f;
  ms_  = static_cast<u32>( ceilf( msf_ ) );
}
