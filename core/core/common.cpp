#include "core/common.hpp"
#include "core/utils.hpp"

namespace
{
  struct StaticData
  {
    char       errorDetails[4096] = { 0 };
    std::mutex errorDetailsMutex;
  };

  StaticData* sData = nullptr;
} // namespace


void core::commonInit() { sData = new StaticData(); }
void core::commonDestroy() { delete sData; }

std::string core::formatMessage( const char* fmt, va_list args )
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

  int count = vsnprintf( nullptr, 0, fmt, args );
  if( count < 0 )
    return { "<format-error>" };

  auto result = std::string( static_cast<size_t>( count ), '\0' );
  count       = vsnprintf( result.data(), static_cast<size_t>( count + 1 ), fmt, args );
  if( count < 0 )
    return { "<format-error>" };

#pragma GCC diagnostic pop

  return result;
}


const char* core::getErrorDetails()
{
  return sData->errorDetails;
}


void core::setErrorDetails( const char* fmt, ... )
{
  if( !fmt )
    return;

  va_list args;
  va_start( args, fmt );
  {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

    int  count = vsnprintf( nullptr, 0, fmt, args );
    auto lock  = std::scoped_lock( sData->errorDetailsMutex );

    if( count < 0 )
    {
      strcpy( sData->errorDetails, "<format-error>" );
    }
    else if( static_cast<size_t>( count ) >= std::size( sData->errorDetails ) )
    {
      strcpy( sData->errorDetails, "<error-too-big>" );
    }
    else
    {
      count = vsnprintf( sData->errorDetails, std::size( sData->errorDetails ), fmt, args );

      if( count < 0 )
      {
        strcpy( sData->errorDetails, "<format-error>" );
      }
    }

#pragma GCC diagnostic pop

    mCoreLogError( "error details: %s\n", sData->errorDetails );
  }
  va_end( args );
}
