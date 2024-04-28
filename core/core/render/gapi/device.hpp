#pragma once
#include "core/common.hpp"
#include "core/render/gapi/common.hpp"
#include "core/render/gapi/resources.hpp"

namespace core::render::gapi
{
  struct Device
  {
    ComPtr<IDXGIFactory1>       dxgiFactory;
    ComPtr<ID3D11Device>        device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<ID3D11InfoQueue>     infoQueue;
    SamplerState                samplerStateWrap;
    SamplerState                samplerStateClamp;
    ComPtr<ID3D11BlendState>    blendStateDefault;
    ComPtr<ID3D11BlendState>    blendStateAlpha;
    Viewport                    viewport;

    Status init( HWND window );
    Status logMessages();
    void   enableAlphaBlending( bool enable = true );
  };

  extern Device* gDevice;
} // namespace core::render::gapi
