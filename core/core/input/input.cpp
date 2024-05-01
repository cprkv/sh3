#include "core/input/input.hpp"

using namespace core;
using namespace core::input;


namespace
{
  Key convertSdlKey( SDL_Keycode code )
  {
#define mXCheckKey( key, sdlKey ) \
  case sdlKey:                    \
    return key;

    switch( code )
    {
      mXKey( mXCheckKey );
      default:
        return KeyCount;
    }

#undef mXCheckKey
  }

  const char* keyModToString( KeyMod mod )
  {
#define mXSwitchKeyMod( keyMod, sdlKeyMod ) \
  case keyMod:                              \
    return #keyMod;

    switch( mod )
    {
      mXKeyMod( mXSwitchKeyMod );
      default:
        return "<unknown>";
    }

#undef mXSwitchKeyMod
  }


  struct StaticData
  {
    bool keyState[KeyCount]           = { false };
    bool keyModState[KeyModCount + 1] = { false };
  };

  StaticData* sData = nullptr;


  void changeKeyModState( KeyMod mod, bool newState )
  {
    bool oldState = sData->keyModState[mod];
    if( oldState != newState )
    {
      mCoreLogDebug( "key mod change state: %s (%d->%d)\n", keyModToString( mod ), ( int ) oldState, ( int ) newState );
      sData->keyModState[mod] = newState;
    }
  }

  void setSdlKeyMod( SDL_Keymod mod )
  {
#define mXResetKeyModState( keyMod, sdlKeyMod ) \
  changeKeyModState( keyMod, false );

#define mXChangeKeyModState( keyMod, sdlKeyMod ) \
  changeKeyModState( keyMod, ( mod & sdlKeyMod ) != 0 );

    if( mod == KMOD_NONE )
    {
      mXKeyMod( mXResetKeyModState );
    }
    else
    {
      mXKeyMod( mXChangeKeyModState );
    }

#undef mXResetKeyModState
#undef mXChangeKeyModState
  }
} // namespace


void input::initialize()
{
  sData = new StaticData();
}

void input::destroy()
{
  delete sData;
}

void input::handle( SDL_Event& e )
{
  if( e.type != SDL_KEYDOWN && e.type != SDL_KEYUP )
    return;

  if( Key k = convertSdlKey( e.key.keysym.sym );
      k != KeyCount )
  {
    bool oldState = sData->keyState[k];
    bool newState = ( e.type == SDL_KEYDOWN );
    if( newState != oldState )
    {
      mCoreLogDebug( "key change state: %s (%d->%d)\n", SDL_GetKeyName( e.key.keysym.sym ),
                     ( int ) oldState, ( int ) newState );
      sData->keyState[k] = newState;
    }
  }

  setSdlKeyMod( ( SDL_Keymod ) e.key.keysym.mod );
}

bool input::isKeyPressed( Key k )
{
  return sData->keyState[k];
}

bool input::isKeyMod( KeyMod m )
{
  return sData->keyModState[m];
}
