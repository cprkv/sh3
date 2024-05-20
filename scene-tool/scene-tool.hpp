#pragma once
#include "core/core.hpp"

#define mFailIfHr( ... )                                             \
  if( HRESULT hr{ __VA_ARGS__ }; FAILED( hr ) )                      \
  {                                                                  \
    printf( "Expression = " #__VA_ARGS__ " failed:\nHRESULT %08X\n", \
            static_cast<unsigned int>( hr ) );                       \
    abort();                                                         \
  }


#define mNotImplementedIf( ... )                                       \
  if( __VA_ARGS__ )                                                    \
  {                                                                    \
    printf( "feature for " #__VA_ARGS__ " is not yet implemented\n" ); \
    abort();                                                           \
  }


#define mFailIf( ... )                                                 \
  if( __VA_ARGS__ )                                                    \
  {                                                                    \
    printf( "fatal error: expression (" #__VA_ARGS__ ") is true!\n" ); \
    abort();                                                           \
  }


#define mFailIfWinApi( ... )                                           \
  if( __VA_ARGS__ )                                                    \
  {                                                                    \
    printf( "fatal error: expression (" #__VA_ARGS__ ") is true!\n" ); \
    printf( "GetLastError() = %lu\n", GetLastError() );                \
    abort();                                                           \
  }


inline stdfs::path getResourcePath( const stdfs::path& path )
{
  char* shResourcesDir = getenv( "SH3_RESOURCES" );
  mFailIf( !shResourcesDir );
  return stdfs::path( shResourcesDir ) / path;
}

inline core::render::BlendMode parseBlendMode( const std::string& bm )
{
  core::render::BlendMode blendMode;
  if( !core::render::fromString( bm, blendMode ) )
  {
    printf( "unknown blend mode: %s\n", bm.c_str() );
    abort();
  }
  return blendMode;
}
