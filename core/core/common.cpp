#include "core/common.hpp"
#include "core/utils.hpp"

namespace
{
  char* sErrorDetails = nullptr;
}


mCoreAPI char* coreGetErrorDetails()
{
  return marshalStrDup( sErrorDetails ? sErrorDetails : "<no-error>" );
}


void coreSetErrorDetails( const char* details )
{
  if( sErrorDetails )
    free( sErrorDetails );

  if( details )
  {
    mCoreLogError( "error details: %s\n", details );
    sErrorDetails = _strdup( details );
  }
  else
  {
    sErrorDetails = nullptr;
  }
}
