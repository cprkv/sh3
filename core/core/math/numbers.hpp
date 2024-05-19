#pragma once
#include "core/common.hpp"

namespace core::math
{
  inline constexpr f32 gEpsilon3 = 1e-03f;
  inline constexpr f32 gEpsilon5 = 1e-05f;
  inline constexpr f32 gEpsilon6 = 1e-06f;
  inline constexpr f32 gEpsilon7 = 1e-07f;

  inline constexpr f32 gPi     = 3.1415926535897932384626433832795f;
  inline constexpr f32 gPiMul2 = 6.2831853071795864769252867665590f;
  inline constexpr f32 gPiMul4 = 12.566370614359172953850573533118f;
  inline constexpr f32 gPiDiv2 = 1.5707963267948966192313216916398f;
  inline constexpr f32 gPiDiv4 = 0.7853981633974483096156608458199f;

  inline f32 softEquals( f32 a, f32 b, f32 eps = gEpsilon5 )
  {
    return fabs( a - b ) < eps;
  }

  inline constexpr f32 toRadians( f32 a ) { return a * ( gPi / 180.f ); }
  inline constexpr f32 toDegrees( f32 a ) { return a * ( 180.f / gPi ); }

  template<typename T>
  inline constexpr T lerp( T a, T b, T t )
  {
    return a + ( b - a ) * t;
  }

  inline f32 clamp( f32 a, f32 min, f32 max )
  {
    if( a < min )
      return min;
    if( a > max )
      return max;
    return a;
  }


  struct Angle
  {
    f32 radians = 0.f;

    static Angle fromRadians( f32 radians ) { return { .radians = radians }; }
    static Angle fromDegrees( f32 degrees ) { return fromRadians( math::toRadians( degrees ) ); }

    // TODO: may be method `Vec2f toXY() const;`
    // TODO: may be operators +/-, with clumping by 2pi
  };
} // namespace math
