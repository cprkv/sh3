#include "core/system/system.hpp"
#include "core/common.hpp"
#include <windows.h>
#include <cstdlib>

using namespace core;


[[noreturn]] void system::fatalError( const char* fmt, ... )
{
  va_list args;
  va_start( args, fmt );
  auto message = formatMessage( fmt, args );
  va_end( args );

  mCoreLogError( "FATAL ERROR: %s", message.c_str() );
  MessageBoxA( nullptr, message.c_str(), "Fatal Error", MB_OK );
  std::abort();
}


Status system::getExeDirectory( stdfs::path& out )
{
  char exePathData[MAX_PATH];
  if( !GetModuleFileNameA( nullptr, exePathData, sizeof( exePathData ) ) )
  {
    core::setErrorDetails( "error getting process name" );
    return StatusSystemError;
  }

  out = stdfs::path( exePathData ).parent_path();
  return StatusOk;
}
