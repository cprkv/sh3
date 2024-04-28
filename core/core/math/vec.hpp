#pragma once
#include "core/common.hpp"

#define mFmtVec2f "(%.4f, %.4f)"
#define mFmtVec2fValue( a ) ( a ).x, ( a ).y

#define mFmtVec3f "(%.4f, %.4f, %.4f)"
#define mFmtVec3fValue( a ) ( a ).x, ( a ).y, ( a ).z

#define mFmtVec4f "(%.4f, %.4f, %.4f, %.4f)"
#define mFmtVec4fValue( a ) ( a ).x, ( a ).y, ( a ).z, ( a ).w

#pragma warning( push )
#pragma warning( disable : 4201 ) // nonstandard extension used: nameless struct/union

namespace math
{
  template<typename T>
  struct Vec2
  {
    union
    {
      struct
      {
        T x, y;
      };
      T v[2];
    };

    Vec2();
    Vec2( T value );
    Vec2( T x, T y );
    Vec2( const Vec2& a );

    Vec2& operator=( const Vec2& a );

    Vec2 operator-( const Vec2& a ) const;
    Vec2 operator+( const Vec2& a ) const;
    Vec2 operator*( const Vec2& a ) const;
    Vec2 operator/( const Vec2& a ) const;
    Vec2 operator-( T a ) const;
    Vec2 operator+( T a ) const;
    Vec2 operator*( T a ) const;
    Vec2 operator/( T a ) const;
    Vec2 operator-() const;

    Vec2& operator-=( const Vec2& a );
    Vec2& operator+=( const Vec2& a );
    Vec2& operator*=( const Vec2& a );
    Vec2& operator/=( const Vec2& a );
    Vec2& operator-=( T a );
    Vec2& operator+=( T a );
    Vec2& operator*=( T a );
    Vec2& operator/=( T a );

    bool operator==( const Vec2& a ) const;
    bool operator!=( const Vec2& a ) const;

    T length() const
      requires( std::is_floating_point_v<T> );

    T sqr() const
      requires( std::is_floating_point_v<T> );

    Vec2& normalize()
      requires( std::is_floating_point_v<T> );

    Vec2 normalized() const
      requires( std::is_floating_point_v<T> );

    template<typename T2>
    static Vec2<T> from( Vec2<T2> a );

    static const Vec2 gZero;
  };


  template<typename T>
  struct Vec3
  {
    union
    {
      struct
      {
        T x, y, z;
      };
      T v[3];
    };

    Vec3();
    Vec3( T value );
    Vec3( T x, T y );
    Vec3( T x, T y, T z );
    Vec3( const Vec3& a );

    Vec3& operator=( const Vec3& a );

    Vec3 operator-( const Vec3& a ) const;
    Vec3 operator+( const Vec3& a ) const;
    Vec3 operator*( const Vec3& a ) const;
    Vec3 operator/( const Vec3& a ) const;
    Vec3 operator-( T a ) const;
    Vec3 operator+( T a ) const;
    Vec3 operator*( T a ) const;
    Vec3 operator/( T a ) const;
    Vec3 operator-() const;

    Vec3& operator-=( const Vec3& a );
    Vec3& operator+=( const Vec3& a );
    Vec3& operator*=( const Vec3& a );
    Vec3& operator/=( const Vec3& a );
    Vec3& operator-=( T a );
    Vec3& operator+=( T a );
    Vec3& operator*=( T a );
    Vec3& operator/=( T a );

    bool operator==( const Vec3& a ) const;
    bool operator!=( const Vec3& a ) const;

    T length() const
      requires( std::is_floating_point_v<T> );

    T sqr() const
      requires( std::is_floating_point_v<T> );

    Vec3& normalize()
      requires( std::is_floating_point_v<T> );

    Vec3 normalized() const
      requires( std::is_floating_point_v<T> );

    T dot( const Vec3& a ) const
      requires( std::is_floating_point_v<T> );

    Vec3 cross( const Vec3& a ) const
      requires( std::is_floating_point_v<T> );

    template<typename T2>
    static Vec3<T> from( Vec3<T2> a );

    static const Vec3 gZero;
    static const Vec3 gRight;
    static const Vec3 gUp;
    static const Vec3 gForward;
  };


  template<typename T>
  struct Vec4
  {
    union
    {
      struct
      {
        T x, y, z, w;
      };
      T v[4];
    };

    Vec4();
    Vec4( T value );
    Vec4( T x, T y );
    Vec4( T x, T y, T z );
    Vec4( T x, T y, T z, T w );
    Vec4( const Vec4& a );

    Vec4& operator=( const Vec4& a );

    Vec4 operator-( const Vec4& a ) const;
    Vec4 operator+( const Vec4& a ) const;
    Vec4 operator*( const Vec4& a ) const;
    Vec4 operator/( const Vec4& a ) const;
    Vec4 operator-( T a ) const;
    Vec4 operator+( T a ) const;
    Vec4 operator*( T a ) const;
    Vec4 operator/( T a ) const;
    Vec4 operator-() const;

    Vec4& operator-=( const Vec4& a );
    Vec4& operator+=( const Vec4& a );
    Vec4& operator*=( const Vec4& a );
    Vec4& operator/=( const Vec4& a );
    Vec4& operator-=( T a );
    Vec4& operator+=( T a );
    Vec4& operator*=( T a );
    Vec4& operator/=( T a );

    bool operator==( const Vec4& a ) const;
    bool operator!=( const Vec4& a ) const;

    T length() const
      requires( std::is_floating_point_v<T> );

    T sqr() const
      requires( std::is_floating_point_v<T> );

    Vec4& normalize()
      requires( std::is_floating_point_v<T> );

    Vec4 normalized() const
      requires( std::is_floating_point_v<T> );

    T dot( const Vec4& a ) const
      requires( std::is_floating_point_v<T> );

    Vec3<T> homogeneous() const
      requires( std::is_floating_point_v<T> );

    template<typename T2>
    static Vec4<T> from( Vec4<T2> a );

    static const Vec4 gZero;
  };


  using Vec2f = Vec2<f32>;
  using Vec3f = Vec3<f32>;
  using Vec4f = Vec4<f32>;

  using Vec2u = Vec2<u32>;
  using Vec3u = Vec3<u32>;
  using Vec4u = Vec4<u32>;

  using Vec2s = Vec2<s32>;
  using Vec3s = Vec3<s32>;
  using Vec4s = Vec4<s32>;
} // namespace math

#include "core/math/vec2-inl.hpp"
#include "core/math/vec3-inl.hpp"
#include "core/math/vec4-inl.hpp"

#pragma warning( pop )
