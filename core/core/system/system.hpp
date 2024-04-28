#pragma once
#include "core/common.hpp"
#include "core/system/time.hpp"
#include "core/system/message-queue.hpp"

namespace core::system
{
  [[noreturn]] void fatalError( const char* fmt, ... );
} // namespace core::system
