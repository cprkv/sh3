#pragma once

namespace math
{
  template<typename T>
  Vec3<T>::Vec3()
      : x( 0 )
      , y( 0 )
      , z( 0 )
  {}

  template<typename T>
  Vec3<T>::Vec3( T value )
      : x( value )
      , y( value )
      , z( value )
  {}

  template<typename T>
  Vec3<T>::Vec3( T x, T y )
      : x( x )
      , y( y )
      , z( 0 )
  {}

  template<typename T>
  Vec3<T>::Vec3( T x, T y, T z )
      : x( x )
      , y( y )
      , z( z )
  {}

  template<typename T>
  Vec3<T>::Vec3( const Vec3& a )
      : x( a.x )
      , y( a.y )
      , z( a.z )
  {}

  template<typename T>
  Vec3<T>& Vec3<T>::operator=( const Vec3& a )
  {
    x = a.x;
    y = a.y;
    z = a.z;
    return *this;
  }


  template<typename T>
  Vec3<T> Vec3<T>::operator-( const Vec3& a ) const
  {
    Vec3 ret;
    ret.x = x - a.x;
    ret.y = y - a.y;
    ret.z = z - a.z;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator+( const Vec3& a ) const
  {
    Vec3 ret;
    ret.x = x + a.x;
    ret.y = y + a.y;
    ret.z = z + a.z;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator*( const Vec3& a ) const
  {
    Vec3 ret;
    ret.x = x * a.x;
    ret.y = y * a.y;
    ret.z = z * a.z;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator/( const Vec3& a ) const
  {
    Vec3 ret;
    ret.x = x / a.x;
    ret.y = y / a.y;
    ret.z = z / a.z;
    return ret;
  }


  template<typename T>
  Vec3<T>& Vec3<T>::operator-=( const Vec3& a )
  {
    x = x - a.x;
    y = y - a.y;
    z = z - a.z;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator+=( const Vec3& a )
  {
    x = x + a.x;
    y = y + a.y;
    z = z + a.z;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator*=( const Vec3& a )
  {
    x = x * a.x;
    y = y * a.y;
    z = z * a.z;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator/=( const Vec3& a )
  {
    x = x / a.x;
    y = y / a.y;
    z = z / a.z;
    return *this;
  }


  template<typename T>
  Vec3<T>& Vec3<T>::operator-=( T a )
  {
    x = x - a;
    y = y - a;
    z = z - a;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator+=( T a )
  {
    x = x + a;
    y = y + a;
    z = z + a;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator*=( T a )
  {
    x = x * a;
    y = y * a;
    z = z * a;
    return *this;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::operator/=( T a )
  {
    x = x / a;
    y = y / a;
    z = z / a;
    return *this;
  }


  template<typename T>
  Vec3<T> Vec3<T>::operator-( T a ) const
  {
    Vec3 ret;
    ret.x = x - a;
    ret.y = y - a;
    ret.z = z - a;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator+( T a ) const
  {
    Vec3 ret;
    ret.x = x + a;
    ret.y = y + a;
    ret.z = z + a;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator*( T a ) const
  {
    Vec3 ret;
    ret.x = x * a;
    ret.y = y * a;
    ret.z = z * a;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator/( T a ) const
  {
    Vec3 ret;
    ret.x = x / a;
    ret.y = y / a;
    ret.z = z / a;
    return ret;
  }

  template<typename T>
  Vec3<T> Vec3<T>::operator-() const
  {
    Vec3 ret;
    ret.x = -x;
    ret.y = -y;
    ret.z = -z;
    return ret;
  }

  template<typename T>
  bool Vec3<T>::operator==( const Vec3& a ) const
  {
    return x == a.x && y == a.y && z == a.z;
  }

  template<typename T>
  bool Vec3<T>::operator!=( const Vec3& a ) const
  {
    return !( *this == a );
  }


  template<typename T>
  T Vec3<T>::length() const
    requires( std::is_floating_point_v<T> )
  {
    return std::sqrt( sqr() );
  }

  template<typename T>
  T Vec3<T>::sqr() const
    requires( std::is_floating_point_v<T> )
  {
    return x * x + y * y + z * z;
  }

  template<typename T>
  Vec3<T>& Vec3<T>::normalize()
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
  Vec3<T> Vec3<T>::normalized() const
    requires( std::is_floating_point_v<T> )
  {
    Vec3 ret = *this;
    ret.normalize();
    return ret;
  }

  template<typename T>
  T Vec3<T>::dot( const Vec3& a ) const
    requires( std::is_floating_point_v<T> )
  {
    return x * a.x +
           y * a.y +
           z * a.z;
  }

  template<typename T>
  Vec3<T> Vec3<T>::cross( const Vec3& a ) const
    requires( std::is_floating_point_v<T> )
  {
    return Vec3( y * a.z - z * a.y,
                 z * a.x - x * a.z,
                 x * a.y - y * a.x );
  }

  template<typename T>
  template<typename T2>
  Vec3<T> Vec3<T>::from( Vec3<T2> a )
  {
    return Vec3<T>( ( T ) a.x, ( T ) a.y, ( T ) a.z );
  }
} // namespace math