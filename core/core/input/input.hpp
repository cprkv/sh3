#pragma once
#include "core/common.hpp"
#include "SDL_keycode.h"

#define mXKey( X )                  \
  X( KeyA, SDLK_a )                 \
  X( KeyB, SDLK_b )                 \
  X( KeyC, SDLK_c )                 \
  X( KeyD, SDLK_d )                 \
  X( KeyE, SDLK_e )                 \
  X( KeyF, SDLK_f )                 \
  X( KeyG, SDLK_g )                 \
  X( KeyH, SDLK_h )                 \
  X( KeyI, SDLK_i )                 \
  X( KeyJ, SDLK_j )                 \
  X( KeyK, SDLK_k )                 \
  X( KeyL, SDLK_l )                 \
  X( KeyM, SDLK_m )                 \
  X( KeyN, SDLK_n )                 \
  X( KeyO, SDLK_o )                 \
  X( KeyP, SDLK_p )                 \
  X( KeyQ, SDLK_q )                 \
  X( KeyR, SDLK_r )                 \
  X( KeyS, SDLK_s )                 \
  X( KeyT, SDLK_t )                 \
  X( KeyU, SDLK_u )                 \
  X( KeyV, SDLK_v )                 \
  X( KeyW, SDLK_w )                 \
  X( KeyX, SDLK_x )                 \
  X( KeyY, SDLK_y )                 \
  X( KeyZ, SDLK_z )                 \
  X( Key0, SDLK_0 )                 \
  X( Key1, SDLK_1 )                 \
  X( Key2, SDLK_2 )                 \
  X( Key3, SDLK_3 )                 \
  X( Key4, SDLK_4 )                 \
  X( Key5, SDLK_5 )                 \
  X( Key6, SDLK_6 )                 \
  X( Key7, SDLK_7 )                 \
  X( Key8, SDLK_8 )                 \
  X( Key9, SDLK_9 )                 \
  X( KeyF1, SDLK_F1 )               \
  X( KeyF2, SDLK_F2 )               \
  X( KeyF3, SDLK_F3 )               \
  X( KeyF4, SDLK_F4 )               \
  X( KeyF5, SDLK_F5 )               \
  X( KeyF6, SDLK_F6 )               \
  X( KeyF7, SDLK_F7 )               \
  X( KeyF8, SDLK_F8 )               \
  X( KeyF9, SDLK_F9 )               \
  X( KeyF10, SDLK_F10 )             \
  X( KeyF11, SDLK_F11 )             \
  X( KeyF12, SDLK_F12 )             \
  X( KeyReturn, SDLK_RETURN )       \
  X( KeyEscape, SDLK_ESCAPE )       \
  X( KeyBackspace, SDLK_BACKSPACE ) \
  X( KeyTab, SDLK_TAB )             \
  X( KeySpace, SDLK_SPACE )         \
  X( KeyRight, SDLK_RIGHT )         \
  X( KeyLeft, SDLK_LEFT )           \
  X( KeyDown, SDLK_DOWN )           \
  X( KeyUp, SDLK_UP )               \
  X( KeyLCtrl, SDLK_LCTRL )         \
  X( KeyLShift, SDLK_LSHIFT )       \
  X( KeyLAlt, SDLK_LALT )           \
  X( KeyRCtrl, SDLK_RCTRL )         \
  X( KeyRShift, SDLK_RSHIFT )       \
  X( KeyRAlt, SDLK_RALT )           \
  X( KeyCount, SDLK_UNKNOWN )

#define mXKeyMod( X )          \
  X( KeyModCtrl, KMOD_CTRL )   \
  X( KeyModShift, KMOD_SHIFT ) \
  X( KeyModAlt, KMOD_ALT )     \
  X( KeyModCount, KMOD_NONE )

#define mXEnumMember( name, sdlValue ) name,

namespace core::input
{
  enum Key
  {
    mXKey( mXEnumMember )
  };

  enum KeyMod
  {
    mXKeyMod( mXEnumMember )
  };

  void initialize();
  void destroy();
  void preUpdate();
  void handle( SDL_Event& e );

  bool isKeyPressed( Key k );
  bool isKeyDown( Key k );
  bool isKeyUp( Key k );
  bool isKeyMod( KeyMod m );
} // namespace core::input

#undef mXEnumMember
