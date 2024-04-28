#pragma once

namespace math
{
  template<typename T>
  Vec2<T>::Vec2()
      : x( 0 )
      , y( 0 )
  {}

  template<typename T>
  Vec2<T>::Vec2( T value )
      : x( value )
      , y( value )
  {}

  template<typename T>
  Vec2<T>::Vec2( T x, T y )
      : x( x )
      , y( y )
  {}

  template<typename T>
  Vec2<T>::Vec2( const Vec2& a )
      : x( a.x )
      , y( a.y )
  {}

  template<typename T>
  Vec2<T>& Vec2<T>::operator=( const Vec2& a )
  {
    x = a.x;
    y = a.y;
    return *this;
  }


  template<typename T>
  Vec2<T> Vec2<T>::operator-( const Vec2& a ) const
  {
    Vec2 ret;
    ret.x = x - a.x;
    ret.y = y - a.y;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator+( const Vec2& a ) const
  {
    Vec2 ret;
    ret.x = x + a.x;
    ret.y = y + a.y;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator*( const Vec2& a ) const
  {
    Vec2 ret;
    ret.x = x * a.x;
    ret.y = y * a.y;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator/( const Vec2& a ) const
  {
    Vec2 ret;
    ret.x = x / a.x;
    ret.y = y / a.y;
    return ret;
  }


  template<typename T>
  Vec2<T>& Vec2<T>::operator-=( const Vec2& a )
  {
    x = x - a.x;
    y = y - a.y;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator+=( const Vec2& a )
  {
    x = x + a.x;
    y = y + a.y;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator*=( const Vec2& a )
  {
    x = x * a.x;
    y = y * a.y;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator/=( const Vec2& a )
  {
    x = x / a.x;
    y = y / a.y;
    return *this;
  }


  template<typename T>
  Vec2<T> Vec2<T>::operator-( T a ) const
  {
    Vec2 ret;
    ret.x = x - a;
    ret.y = y - a;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator+( T a ) const
  {
    Vec2 ret;
    ret.x = x + a;
    ret.y = y + a;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator*( T a ) const
  {
    Vec2 ret;
    ret.x = x * a;
    ret.y = y * a;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator/( T a ) const
  {
    Vec2 ret;
    ret.x = x / a;
    ret.y = y / a;
    return ret;
  }

  template<typename T>
  Vec2<T> Vec2<T>::operator-() const
  {
    Vec2 ret;
    ret.x = -x;
    ret.y = -y;
    return ret;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator-=( T a )
  {
    x = x - a;
    y = y - a;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator+=( T a )
  {
    x = x + a;
    y = y + a;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator*=( T a )
  {
    x = x * a;
    y = y * a;
    return *this;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::operator/=( T a )
  {
    x = x / a;
    y = y / a;
    return *this;
  }


  template<typename T>
  bool Vec2<T>::operator==( const Vec2& a ) const
  {
    return x == a.x && y == a.y;
  }

  template<typename T>
  bool Vec2<T>::operator!=( const Vec2& a ) const
  {
    return !( *this == a );
  }

  template<typename T>
  T Vec2<T>::length() const
    requires( std::is_floating_point_v<T> )
  {
    return std::sqrt( sqr() );
  }

  template<typename T>
  T Vec2<T>::sqr() const
    requires( std::is_floating_point_v<T> )
  {
    return x * x + y * y;
  }

  template<typename T>
  Vec2<T>& Vec2<T>::normalize()
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
  Vec2<T> Vec2<T>::normalized() const
    requires( std::is_floating_point_v<T> )
  {
    Vec2 ret = *this;
    ret.normalize();
    return ret;
  }

  template<typename T>
  template<typename T2>
  Vec2<T> Vec2<T>::from( Vec2<T2> a )
  {
    return Vec2<T>( ( T ) a.x, ( T ) a.y );
  }
} // namespace math