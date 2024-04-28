#pragma once
#include "core/common.hpp"

#ifndef DIRECTX_TYPE_INTRO
#  define DIRECTX_TYPE_INTRO
#endif

#pragma pack( push )
#pragma pack( 16 )

namespace core::render::shaders
{
  using dword = u32;

#if 1
  using float2   = Vec2;
  using float3   = Vec3;
  using float4   = Vec4;
  using float4x4 = Mat4;
#else
  struct float2
  {
    float x;
    float y;

    float2() = default;

    float2( const Vec2& a )
        : x( a.x )
        , y( a.y )
    {}
  };

  struct float3
  {
    float x;
    float y;
    float z;

    float3() = default;

    float3( const Vec3& a )
        : x( a.x )
        , y( a.y )
        , z( a.z )
    {}
  };

  struct float4
  {
    float x;
    float y;
    float z;
    float w;

    float4() = default;

    float4( const Vec4& a )
        : x( a.x )
        , y( a.y )
        , z( a.z )
        , w( a.w )
    {}
  };

  struct float4x4
  {
    float m[4][4];

    float4x4() = default;

    float4x4( const pvk::Mat4& a )
    {
      static_assert( sizeof( Mat4 ) == sizeof( float4x4 ), "Mat4 size should be same as float4x4" );
      *this = *reinterpret_cast<float4x4*>( &const_cast<Mat4&>( a ) );
    }
  };
#endif

  struct float2x2
  {
    float2 x;
    float2 _pad;
    float2 y;

    float2x2() = default;
  };

  struct float3x3
  {
    float3 x;
    float  _pad1;
    float3 y;
    float  _pad2;
    float3 z;

    float3x3() = default;
  };

  enum class GBufferFlags : dword
  {
    None         = 0x00000000,
    EnableNormal = 0x00000001,
  };
} // namespace core::render::shaders

#pragma pack( pop )
