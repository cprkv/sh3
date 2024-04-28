#pragma once
#include "core/math/vec.hpp"

/*
#define mFmtMat4 "[%.4f, %.4f, %.4f, %.4f\n" \
                 " %.4f, %.4f, %.4f, %.4f\n" \
                 " %.4f, %.4f, %.4f, %.4f\n" \
                 " %.4f, %.4f, %.4f, %.4f]"

#define mFmtMat4Value( a ) ( a ).m[0][0], ( a ).m[0][1], ( a ).m[0][2], ( a ).m[0][3], \
                           ( a ).m[1][0], ( a ).m[1][1], ( a ).m[1][2], ( a ).m[1][3], \
                           ( a ).m[2][0], ( a ).m[2][1], ( a ).m[2][2], ( a ).m[2][3], \
                           ( a ).m[3][0], ( a ).m[3][1], ( a ).m[3][2], ( a ).m[3][3]
                           */

#pragma warning( push )
#pragma warning( disable : 4201 ) // nonstandard extension used: nameless struct/union

namespace math
{
  // Row major storage with column notation.
  // Uses columns-vectors, so multiplication is M*v.
  // Format is [row][col] (row-major as any c array).
  // Needs to be transposed for use in any graphics api!
  struct Mat4
  {
    union
    {
      f32 m[4][4];
      f32 v[16];
    };

    Mat4();
    Mat4( f32 a00, f32 a01, f32 a02, f32 a03,
          f32 a10, f32 a11, f32 a12, f32 a13,
          f32 a20, f32 a21, f32 a22, f32 a23,
          f32 a30, f32 a31, f32 a32, f32 a33 );

    bool operator==( const Mat4& a ) const;
    bool operator!=( const Mat4& a ) const;

    Mat4  operator*( const Mat4& a ) const;
    Vec4f operator*( const Vec4f& a ) const;

    Vec4f getRow( u32 i ) const;
    Mat4& setRow( u32 i, const Vec4f& a );

    Vec4f getColumn( u32 i ) const;
    Mat4& setColumn( u32 i, const Vec4f& a );

    Mat4& transpose();
    Mat4  transposed() const;

    std::string toString() const;

    static Mat4 translation( Vec3f offset );
    static Mat4 scale( Vec3f scale );

    static const Mat4 gZero;
    static const Mat4 gIdentity;
  };
} // namespace math

#pragma warning( pop )
