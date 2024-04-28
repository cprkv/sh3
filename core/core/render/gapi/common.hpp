#pragma once
#include "core/common.hpp"

#ifdef _DEBUG
#  define mCoreGAPIDeviceDebug
#endif

#define mCoreCheckHR( ... )                                  \
  if( HRESULT hr = __VA_ARGS__; FAILED( hr ) ) [[unlikely]]  \
  {                                                          \
    mCoreLogError( "expression " #__VA_ARGS__ " failed\n" ); \
    return StatusSystemError;                                \
  }

namespace core::render::gapi
{
  template<typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

  enum GPUFormat
  {
    GPUFormatR32F,
    GPUFormatRG32F,
    GPUFormatRGB32F,
    GPUFormatRGBA16F,
    GPUFormatRGBA32F,
    GPUFormatRGB8_SRGB,
    GPUFormatR24G8Typeless,
  };

  DXGI_FORMAT convertGPUFormatToDXGIFormat( GPUFormat format );
} // namespace core::render::gapi
