#include "core/render/gapi/common.hpp"

using namespace core::render;
using namespace core::render::gapi;


DXGI_FORMAT gapi::convertGPUFormatToDXGIFormat( GPUFormat format )
{
  switch( format )
  {
    case GPUFormatR32F:
      return DXGI_FORMAT_R32_FLOAT;
    case GPUFormatRG32F:
      return DXGI_FORMAT_R32G32_FLOAT;
    case GPUFormatRGBA16F:
      return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case GPUFormatRGB32F:
      return DXGI_FORMAT_R32G32B32_FLOAT;
    case GPUFormatRGBA32F:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case GPUFormatRGB8_SRGB:
      return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    case GPUFormatR24G8Typeless:
      return DXGI_FORMAT_R24G8_TYPELESS;
    default:
      assert( false );
      return DXGI_FORMAT_UNKNOWN;
  }
}
