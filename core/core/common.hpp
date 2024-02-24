#pragma once
#include "SDL.h"
#include "glm/glm.hpp"
#include "windows.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define mCoreAPI extern "C" __declspec( dllexport )
#define mCoreLog( ... ) fprintf( stdout, "[core] " __VA_ARGS__ )
#define mCoreLogError( ... ) fprintf( stderr, "[core][error] " __VA_ARGS__ )

typedef enum Status
{
  StatusOk,
  StatusSystemError,
} Status;

mCoreAPI char* coreGetErrorDetails();
void           coreSetErrorDetails( const char* details );
