#pragma once

namespace math
{
  template<typename T>
  Vec4<T>::Vec4()
      : x( 0 )
      , y( 0 )
      , z( 0 )
      , w( 0 )
  {}

  template<typename T>
  Vec4<T>::Vec4( T value )
      : x( value )
      , y( value )
      , z( value )
      , w( value )
  {}

  template<typename T>
  Vec4<T>::Vec4( T x, T y )
      : x( x )
      , y( y )
      , z( 0 )
      , w( 0 )
  {}

  template<typename T>
  Vec4<T>::Vec4( T x, T y, T z )
      : x( x )
      , y( y )
      , z( z )
      , w( 0 )
  {}

  template<typename T>
  Vec4<T>::Vec4( T x, T y, T z, T w )
      : x( x )
      , y( y )
      , z( z )
      , w( w )
  {}

  template<typename T>
  Vec4<T>::Vec4( const Vec4& a )
      : x( a.x )
      , y( a.y )
      , z( a.z )
      , w( a.w )
  {}

  template<typename T>
  Vec4<T>& Vec4<T>::operator=( const Vec4& a )
  {
    x = a.x;
    y = a.y;
    z = a.z;
    w = a.w;
    return *this;
  }


  template<typename T>
  Vec4<T> Vec4<T>::operator-( const Vec4& a ) const
  {
    Vec4 ret;
    ret.x = x - a.x;
    ret.y = y - a.y;
    ret.z = z - a.z;
    ret.w = w - a.w;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator+( const Vec4& a ) const
  {
    Vec4 ret;
    ret.x = x + a.x;
    ret.y = y + a.y;
    ret.z = z + a.z;
    ret.w = w + a.w;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator*( const Vec4& a ) const
  {
    Vec4 ret;
    ret.x = x * a.x;
    ret.y = y * a.y;
    ret.z = z * a.z;
    ret.w = w * a.w;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator/( const Vec4& a ) const
  {
    Vec4 ret;
    ret.x = x / a.x;
    ret.y = y / a.y;
    ret.z = z / a.z;
    ret.w = w / a.w;
    return ret;
  }


  template<typename T>
  Vec4<T>& Vec4<T>::operator-=( const Vec4& a )
  {
    x = x - a.x;
    y = y - a.y;
    z = z - a.z;
    w = w - a.w;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator+=( const Vec4& a )
  {
    x = x + a.x;
    y = y + a.y;
    z = z + a.z;
    w = w + a.w;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator*=( const Vec4& a )
  {
    x = x * a.x;
    y = y * a.y;
    z = z * a.z;
    w = w * a.w;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator/=( const Vec4& a )
  {
    x = x / a.x;
    y = y / a.y;
    z = z / a.z;
    w = w / a.w;
    return *this;
  }


  template<typename T>
  Vec4<T>& Vec4<T>::operator-=( T a )
  {
    x = x - a;
    y = y - a;
    z = z - a;
    w = w - a;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator+=( T a )
  {
    x = x + a;
    y = y + a;
    z = z + a;
    w = w + a;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator*=( T a )
  {
    x = x * a;
    y = y * a;
    z = z * a;
    w = w * a;
    return *this;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::operator/=( T a )
  {
    x = x / a;
    y = y / a;
    z = z / a;
    w = w / a;
    return *this;
  }


  template<typename T>
  Vec4<T> Vec4<T>::operator-( T a ) const
  {
    Vec4 ret;
    ret.x = x - a;
    ret.y = y - a;
    ret.z = z - a;
    ret.w = w - a;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator+( T a ) const
  {
    Vec4 ret;
    ret.x = x + a;
    ret.y = y + a;
    ret.z = z + a;
    ret.w = w + a;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator*( T a ) const
  {
    Vec4 ret;
    ret.x = x * a;
    ret.y = y * a;
    ret.z = z * a;
    ret.w = w * a;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator/( T a ) const
  {
    Vec4 ret;
    ret.x = x / a;
    ret.y = y / a;
    ret.z = z / a;
    ret.w = w / a;
    return ret;
  }

  template<typename T>
  Vec4<T> Vec4<T>::operator-() const
  {
    Vec4 ret;
    ret.x = -x;
    ret.y = -y;
    ret.z = -z;
    ret.w = -w;
    return ret;
  }


  template<typename T>
  bool Vec4<T>::operator==( const Vec4& a ) const
  {
    return x == a.x && y == a.y && z == a.z && w == a.w;
  }

  template<typename T>
  bool Vec4<T>::operator!=( const Vec4& a ) const
  {
    return !( *this == a );
  }

  template<typename T>
  T Vec4<T>::length() const
    requires( std::is_floating_point_v<T> )
  {
    return std::sqrt( sqr() );
  }

  template<typename T>
  T Vec4<T>::sqr() const
    requires( std::is_floating_point_v<T> )
  {
    return x * x + y * y + z * z + w * w;
  }

  template<typename T>
  Vec4<T>& Vec4<T>::normalize()
    requires( std::is_floating_point_v<T> )
  {
    T l = length();
    if( l > 1e-08 )
    {
      T invLength = 1.0f / l;
      *this *= invLength;
    }
    return *this;
  }

  template<typename T>
  Vec4<T> Vec4<T>::normalized() const
    requires( std::is_floating_point_v<T> )
  {
    Vec4 ret = *this;
    ret.normalize();
    return ret;
  }

  template<typename T>
  T Vec4<T>::dot( const Vec4& a ) const
    requires( std::is_floating_point_v<T> )
  {
    return x * a.x +
           y * a.y +
           z * a.z +
           w * a.w;
  }

  template<typename T>
  Vec3<T> Vec4<T>::homogeneous() const
    requires( std::is_floating_point_v<T> )
  {
    T invW = 1.0f / w;
    return Vec3<T>( x * invW, y * invW, z * invW );
  }

  template<typename T>
  template<typename T2>
  Vec4<T> Vec4<T>::from( Vec4<T2> a )
  {
    return Vec4<T>( ( T ) a.x, ( T ) a.y, ( T ) a.z, ( T ) a.w );
  }
} // namespace math
