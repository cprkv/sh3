#pragma once

#if __has_include("shader-types-pre.hpp")
  #include "shader-types-pre.hpp"
#endif

namespace core::render::shaders {

    #pragma pack(push)
    #pragma pack(16)

    #ifndef DIRECTX_TYPE_INTRO
    #define DIRECTX_TYPE_INTRO
      using dword = unsigned int;
      struct float2 { float x; float y; };
      struct float3 { float x; float y; float z; };
      struct float4 { float x; float y; float z; float w; };

      struct float2x2 { float2 x; float2 _pad; float2 y; };
      struct float3x3 { float3 x; float _pad1; float3 y; float _pad2; float3 z; };
      struct float4x4 { float m[4][4]; };
    #endif

    #ifndef DIRECTX_TYPE_CHECKS
    #define DIRECTX_TYPE_CHECKS
      static_assert(sizeof(dword) == 4, "directx structure size should match");
      static_assert(sizeof(float2) == 8, "directx structure size should match");
      static_assert(sizeof(float3) == 12, "directx structure size should match");
      static_assert(sizeof(float4) == 16, "directx structure size should match");

      static_assert(sizeof(float2x2) == 24, "directx structure size should match");
      static_assert(sizeof(float3x3) == 44, "directx structure size should match");
      static_assert(sizeof(float4x4) == 64, "directx structure size should match");
    #endif

      
  struct LineVSConstantBuffer {
    float4x4 gWorldToProjection;
  };
  
  struct OldFullPSConstantBuffer {
    dword gAlphaHash;
    float3 _pad_1;
  };
  
  struct OldPointLight {
    float3 position;
    float _pad_1;
    float3 color;
    float _pad_2;
    float intensity;
    float3 _pad_3;
  };
  
  struct OldSceneLights {
    OldPointLight lights[32];
    dword count;
    float3 ambient;
  };
  
  struct OldFullVSConstantBuffer {
    OldSceneLights gSceneLights;
    float3 gViewPos;
    float _pad_1;
    float4x4 gViewToProjection;
    float4x4 gWorldToView;
  };
  
  struct OldFullVSConstantBufferModel {
    float4x4 gModelToWorld;
    float4x4 gWorldInvTranspose;
  };
  
  
  struct Texture2DVSConstantBuffer {
    float2 gPosition;
    float2 gSize;
    float2 gScreenSize;
    float2 _pad_1;
  };
  
  
    #pragma pack(pop)
  
}
