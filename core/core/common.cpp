#include "core/common.hpp"
#include "core/utils.hpp"

namespace
{
  char       sErrorDetails[4096] = { 0 };
  std::mutex sErrorDetailsMutex;
} // namespace


std::string core::formatMessage( const char* fmt, va_list args )
{
  int count = vsnprintf( nullptr, 0, fmt, args );
  if( count < 0 )
    return { "<format-error>" };

  auto result = std::string( count, '\0' );
  count       = vsnprintf( result.data(), count + 1, fmt, args );
  if( count < 0 )
    return { "<format-error>" };

  return result;
}


const char* core::getErrorDetails()
{
  return sErrorDetails;
}


void core::setErrorDetails( const char* fmt, ... )
{
  if( !fmt )
    return;

  va_list args;
  va_start( args, fmt );
  {
    int  count = vsnprintf( nullptr, 0, fmt, args );
    auto lock  = std::scoped_lock( sErrorDetailsMutex );

    if( count < 0 )
    {
      strcpy( sErrorDetails, "<format-error>" );
    }
    else if( count >= std::size( sErrorDetails ) )
    {
      strcpy( sErrorDetails, "<error-too-big>" );
    }
    else
    {
      count = vsnprintf( sErrorDetails, std::size( sErrorDetails ), fmt, args );

      if( count < 0 )
      {
        strcpy( sErrorDetails, "<format-error>" );
      }
    }

    mCoreLogError( "error details: %s\n", sErrorDetails );
  }
  va_end( args );
}
