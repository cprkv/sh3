#include "core/math/mat.hpp"

namespace math
{
  Mat4::Mat4()
      : Mat4( 1, 0, 0, 0,
              0, 1, 0, 0,
              0, 0, 1, 0,
              0, 0, 0, 1 )
  {
  }

  Mat4::Mat4( f32 a00, f32 a01, f32 a02, f32 a03,
              f32 a10, f32 a11, f32 a12, f32 a13,
              f32 a20, f32 a21, f32 a22, f32 a23,
              f32 a30, f32 a31, f32 a32, f32 a33 )
  {
    // row0
    m[0][0] = a00;
    m[0][1] = a01;
    m[0][2] = a02;
    m[0][3] = a03;

    // row1
    m[1][0] = a10;
    m[1][1] = a11;
    m[1][2] = a12;
    m[1][3] = a13;

    // row2
    m[2][0] = a20;
    m[2][1] = a21;
    m[2][2] = a22;
    m[2][3] = a23;

    // row3
    m[3][0] = a30;
    m[3][1] = a31;
    m[3][2] = a32;
    m[3][3] = a33;
  }

  bool Mat4::operator==( const Mat4& a ) const
  {
    return std::equal( std::begin( v ), std::end( v ),
                       std::begin( a.v ) );
  }

  bool Mat4::operator!=( const Mat4& a ) const
  {
    return !( *this == a );
  }

  Mat4 Mat4::operator*( const Mat4& a ) const
  {
    return Mat4(
        m[0][0] * a.m[0][0] + m[0][1] * a.m[1][0] + m[0][2] * a.m[2][0] + m[0][3] * a.m[3][0],
        m[0][0] * a.m[0][1] + m[0][1] * a.m[1][1] + m[0][2] * a.m[2][1] + m[0][3] * a.m[3][1],
        m[0][0] * a.m[0][2] + m[0][1] * a.m[1][2] + m[0][2] * a.m[2][2] + m[0][3] * a.m[3][2],
        m[0][0] * a.m[0][3] + m[0][1] * a.m[1][3] + m[0][2] * a.m[2][3] + m[0][3] * a.m[3][3],

        m[1][0] * a.m[0][0] + m[1][1] * a.m[1][0] + m[1][2] * a.m[2][0] + m[1][3] * a.m[3][0],
        m[1][0] * a.m[0][1] + m[1][1] * a.m[1][1] + m[1][2] * a.m[2][1] + m[1][3] * a.m[3][1],
        m[1][0] * a.m[0][2] + m[1][1] * a.m[1][2] + m[1][2] * a.m[2][2] + m[1][3] * a.m[3][2],
        m[1][0] * a.m[0][3] + m[1][1] * a.m[1][3] + m[1][2] * a.m[2][3] + m[1][3] * a.m[3][3],

        m[2][0] * a.m[0][0] + m[2][1] * a.m[1][0] + m[2][2] * a.m[2][0] + m[2][3] * a.m[3][0],
        m[2][0] * a.m[0][1] + m[2][1] * a.m[1][1] + m[2][2] * a.m[2][1] + m[2][3] * a.m[3][1],
        m[2][0] * a.m[0][2] + m[2][1] * a.m[1][2] + m[2][2] * a.m[2][2] + m[2][3] * a.m[3][2],
        m[2][0] * a.m[0][3] + m[2][1] * a.m[1][3] + m[2][2] * a.m[2][3] + m[2][3] * a.m[3][3],

        m[3][0] * a.m[0][0] + m[3][1] * a.m[1][0] + m[3][2] * a.m[2][0] + m[3][3] * a.m[3][0],
        m[3][0] * a.m[0][1] + m[3][1] * a.m[1][1] + m[3][2] * a.m[2][1] + m[3][3] * a.m[3][1],
        m[3][0] * a.m[0][2] + m[3][1] * a.m[1][2] + m[3][2] * a.m[2][2] + m[3][3] * a.m[3][2],
        m[3][0] * a.m[0][3] + m[3][1] * a.m[1][3] + m[3][2] * a.m[2][3] + m[3][3] * a.m[3][3] );
  }

  Vec4f Mat4::operator*( const Vec4f& a ) const
  {
    return Vec4f(
        m[0][0] * a.x + m[0][1] * a.y + m[0][2] * a.z + m[0][3] * a.w,
        m[1][0] * a.x + m[1][1] * a.y + m[1][2] * a.z + m[1][3] * a.w,
        m[2][0] * a.x + m[2][1] * a.y + m[2][2] * a.z + m[2][3] * a.w,
        m[3][0] * a.x + m[3][1] * a.y + m[3][2] * a.z + m[3][3] * a.w );
  }

  Vec4f Mat4::getRow( u32 i ) const
  {
    assert( i <= 3 );
    return Vec4f( m[i][0], m[i][1], m[i][2], m[i][3] );
  }

  Mat4& Mat4::setRow( u32 i, const Vec4f& a )
  {
    assert( i <= 3 );
    m[i][0] = a.v[0];
    m[i][1] = a.v[1];
    m[i][2] = a.v[2];
    m[i][3] = a.v[3];
    return *this;
  }

  Vec4f Mat4::getColumn( u32 i ) const
  {
    assert( i <= 3 );
    return Vec4f( m[0][i], m[1][i], m[2][i], m[3][i] );
  }

  Mat4& Mat4::setColumn( u32 i, const Vec4f& a )
  {
    assert( i <= 3 );
    m[0][i] = a.v[0];
    m[1][i] = a.v[1];
    m[2][i] = a.v[2];
    m[3][i] = a.v[3];
    return *this;
  }

  Mat4& Mat4::transpose()
  {
    std::swap( m[0][1], m[1][0] );
    std::swap( m[0][2], m[2][0] );
    std::swap( m[0][3], m[3][0] );
    std::swap( m[1][2], m[2][1] );
    std::swap( m[1][3], m[3][1] );
    std::swap( m[2][3], m[3][2] );
    return *this;
  }

  Mat4 Mat4::transposed() const
  {
    return Mat4( m[0][0], m[1][0], m[2][0], m[3][0],
                 m[0][1], m[1][1], m[2][1], m[3][1],
                 m[0][2], m[1][2], m[2][2], m[3][2],
                 m[0][3], m[1][3], m[2][3], m[3][3] );
  }

  std::string Mat4::toString() const
  {
    char str[256] = { 0 };
    sprintf( str, mFmtMat4, mFmtMat4Value( *this ) );
    return std::string( str );
  }

  Mat4 Mat4::translation( Vec3f offset )
  {
    return Mat4( 1, 0, 0, offset.x,
                 0, 1, 0, offset.y,
                 0, 0, 1, offset.z,
                 0, 0, 0, 1 );
  }

  Mat4 Mat4::scale( Vec3f scale )
  {
    return Mat4( scale.x, 0, 0, 0,
                 0, scale.y, 0, 0,
                 0, 0, scale.z, 0,
                 0, 0, 0, 1 );
  }

  const Mat4 Mat4::gZero = Mat4( 0, 0, 0, 0,
                                 0, 0, 0, 0,
                                 0, 0, 0, 0,
                                 0, 0, 0, 0 );

  const Mat4 Mat4::gIdentity = Mat4();
} // namespace math
