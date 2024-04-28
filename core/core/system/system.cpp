#include "core/system/system.hpp"
#include "core/common.hpp"
#include <Windows.h>
#include <cstdlib>


[[noreturn]] void core::system::fatalError( const char* fmt, ... )
{
  va_list args;
  va_start( args, fmt );
  auto message = core::formatMessage( fmt, args );
  va_end( args );

  mCoreLogError( "FATAL ERROR: %s", message.c_str() );
  MessageBoxA( nullptr, message.c_str(), "Fatal Error", MB_OK );
  std::abort();
}
