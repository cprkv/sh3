#pragma once
#include "core/common.hpp"
#include "core/system/time.hpp"
#include "core/system/message-queue.hpp"
#include "core/system/task.hpp"

namespace core::system
{
  [[noreturn]] void fatalError( const char* fmt, ... );
  Status            getExeDirectory( stdfs::path& out );

} // namespace core::system
