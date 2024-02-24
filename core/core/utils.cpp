#include "core/utils.hpp"
#include <Objbase.h>


char* marshalStrDup( const char* string )
{
  if( !string )
    string = "";

  size_t size = strlen( string ) + 1;
  char*  data = static_cast<char*>( CoTaskMemAlloc( size ) );
  memcpy( data, string, size );
  return data;
}
