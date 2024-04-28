#include "core/common.hpp"
#include "core/utils.hpp"

namespace
{
  char* sErrorDetails = nullptr;
} // namespace


std::string core::formatMessage( const char* fmt, va_list args )
{
  int count = vsnprintf( nullptr, 0, fmt, args );
  if( count < 0 )
    return { "<format-error>" };

  auto   result     = std::string( count, '\0' );
  count             = vsnprintf( result.data(), count + 1, fmt, args );
  if( count < 0 )
    return { "<format-error>" };

  return result;
}


const char* core::getErrorDetails()
{
  return sErrorDetails ? sErrorDetails : "<no-error>";
}


void core::setErrorDetails( const char* fmt, ... )
{
  if( sErrorDetails )
  {
    free( sErrorDetails );
  }

  if( !fmt )
  {
    sErrorDetails = nullptr;
    return;
  }

  va_list args;
  va_start( args, fmt );
  {
    int count = vsnprintf( nullptr, 0, fmt, args );

    if( count < 0 )
    {
      sErrorDetails = _strdup( "<format-error>" );
    }
    else
    {
      size_t bufferSize = ( size_t ) count + 1;
      auto   result     = ( char* ) malloc( bufferSize );
      count             = vsnprintf( result, bufferSize, fmt, args );

      if( count < 0 )
      {
        sErrorDetails = _strdup( "<format-error>" );
        free( result );
      }
      else
      {
        sErrorDetails = result;
      }
    }
  }
  va_end( args );

  mCoreLogError( "error details: %s\n", sErrorDetails );
}
