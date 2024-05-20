#include "core/render/gapi/resources.hpp"
#include "core/render/gapi/device.hpp"
#include "core/render/gapi/shader-compiler.hpp"

using namespace core::render;
using namespace core::render::gapi;


//#define mPvkFilterBilinear
#define mPvkFilterAnisotropic


namespace
{
  Status getWindowSize( HWND window, Vec2& outWindowSizeF, Vec2u& outWindowSizeU )
  {
    auto windowRect = RECT{};
    if( !GetClientRect( window, &windowRect ) )
      return StatusSystemError;

    UINT width  = static_cast<UINT>( windowRect.right - windowRect.left );
    UINT height = static_cast<UINT>( windowRect.bottom - windowRect.top );

    if( width == 0 ) width = 16;
    if( height == 0 ) height = 16;

    outWindowSizeF = { width, height };
    outWindowSizeU = outWindowSizeF;

    //    outViewport = {
    //        .TopLeftX = 0,
    //        .TopLeftY = 0,
    //        .Width    = outWindowSize.x,
    //        .Height   = outWindowSize.y,
    //        .MinDepth = 0,
    //        .MaxDepth = 1,
    //    };

    return StatusOk;
  }
} // namespace


#ifdef _DEBUG
#  define mSetDebugName( obj, name ) obj->SetPrivateData( WKPDID_D3DDebugObjectName, static_cast<UINT>( strlen( name ) ), name )
#else
#  define mSetDebugName( obj, name ) ( void ) name
#endif

// -----------------------------------------------------------------------------
// -- SamplerState
// -----------------------------------------------------------------------------

Status SamplerState::init( D3D11_TEXTURE_ADDRESS_MODE mode )
{
  auto samplerWrapDesc = D3D11_SAMPLER_DESC{
      .AddressU   = mode,
      .AddressV   = mode,
      .AddressW   = mode,
      .MipLODBias = 0.0f,
      .MinLOD     = 0.0f,
      .MaxLOD     = D3D11_FLOAT32_MAX,
  };

#if defined( mPvkFilterBilinear )
  samplerWrapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
#elif defined( mPvkFilterAnisotropic )
  samplerWrapDesc.Filter        = D3D11_FILTER_ANISOTROPIC;
  samplerWrapDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY; // TODO: depends on hardware limitation
#endif

  mCoreCheckHR( gDevice->device->CreateSamplerState( &samplerWrapDesc, &samplerState ) );
  return StatusOk;
}

void SamplerState::use( u32 slot )
{
  gDevice->context->PSSetSamplers( slot, 1u, samplerState.GetAddressOf() );
}


// -----------------------------------------------------------------------------
// -- ConstantBuffer
// -----------------------------------------------------------------------------

Status ConstantBuffer::init( ArrayBytesView bytes, ConstantBufferTarget initTarget )
{
  assert( !buffer );
  assert( bytes.getSize() == 1 );
  auto bufferDesc = D3D11_BUFFER_DESC{
      .ByteWidth           = static_cast<UINT>( bytes.getElementSize() * bytes.getSize() ),
      .Usage               = D3D11_USAGE_DYNAMIC,
      .BindFlags           = D3D11_BIND_CONSTANT_BUFFER,
      .CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
      .MiscFlags           = 0,
      .StructureByteStride = 0,
  };
  auto initialData = D3D11_SUBRESOURCE_DATA{
      .pSysMem          = bytes.getData(),
      .SysMemPitch      = 0,
      .SysMemSlicePitch = 0,
  };
  elementSize = bytes.getElementSize();
  target      = initTarget;
  mCoreCheckHR( gDevice->device->CreateBuffer( &bufferDesc, &initialData, &this->buffer ) );
  return StatusOk;
}

Status ConstantBuffer::update( ArrayBytesView bytes ) const
{
  assert( buffer );
  assert( elementSize == bytes.getElementSize() );
  assert( bytes.getSize() == 1 );
  UINT subresource       = 0u;
  UINT mapFlags          = 0u;
  auto mappedSubresource = D3D11_MAPPED_SUBRESOURCE{};
  mCoreCheckHR( gDevice->context->Map(
      buffer.Get(), subresource, D3D11_MAP_WRITE_DISCARD, mapFlags, &mappedSubresource ) );
  memcpy( mappedSubresource.pData, bytes.getData(), elementSize );
  gDevice->context->Unmap( buffer.Get(), subresource );
  return StatusOk;
}

void ConstantBuffer::use()
{
  if( ( target & ConstantBufferTargetVertex ) != 0 )
    gDevice->context->VSSetConstantBuffers( 0, 1, buffer.GetAddressOf() );

  if( ( target & ConstantBufferTargetPixel ) != 0 )
    gDevice->context->PSSetConstantBuffers( 0, 1, buffer.GetAddressOf() );
}


// -----------------------------------------------------------------------------
// -- DepthStencil
// -----------------------------------------------------------------------------

Status DepthStencil::init( Vec2u size, std::optional<MSAA> msaa )
{
  auto dsTex2DDesc = D3D11_TEXTURE2D_DESC{
      .Width      = size.x,
      .Height     = size.y,
      .MipLevels  = 1u,
      .ArraySize  = 1u,
      .Format     = DXGI_FORMAT_R32_TYPELESS,
      .SampleDesc = {
          .Count   = msaa ? msaa->sampleCount : 1u,
          .Quality = 0,
      },
      .Usage          = D3D11_USAGE_DEFAULT,
      .BindFlags      = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = 0,
      .MiscFlags      = 0,
  };
  auto depthStencilTexture = ComPtr<ID3D11Texture2D>{};
  mCoreCheckHR( gDevice->device->CreateTexture2D(
      &dsTex2DDesc, nullptr, &depthStencilTexture ) );

  auto dsViewDesc = D3D11_DEPTH_STENCIL_VIEW_DESC{
      .Format        = DXGI_FORMAT_D32_FLOAT,
      .ViewDimension = msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D,
      .Flags         = 0,
      .Texture2D     = { .MipSlice = 0u },
  };
  mCoreCheckHR( gDevice->device->CreateDepthStencilView(
      depthStencilTexture.Get(), &dsViewDesc, &depthStencilView ) );

  auto shViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
      .Format        = DXGI_FORMAT_R32_FLOAT,
      .ViewDimension = msaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = { .MipLevels = msaa ? 0u : 1u }, // TODO: this is as i understand it
  };
  mCoreCheckHR( gDevice->device->CreateShaderResourceView(
      depthStencilTexture.Get(), &shViewDesc, &shaderResourceView ) );

  return StatusOk;
}

void DepthStencil::clear() const
{
  gDevice->context->ClearDepthStencilView( depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0 );
}


// -----------------------------------------------------------------------------
// -- DepthStencilState
// -----------------------------------------------------------------------------

Status DepthStencilState::init( bool enabled )
{
  auto desc = D3D11_DEPTH_STENCIL_DESC{
      .DepthEnable      = TRUE,
      .DepthWriteMask   = enabled ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO,
      .DepthFunc        = enabled ? D3D11_COMPARISON_LESS : D3D11_COMPARISON_ALWAYS, // z-less, it is closer to camera, value should be chosen
      .StencilEnable    = FALSE,
      .StencilReadMask  = 0,
      .StencilWriteMask = 0,
      .FrontFace        = {}, // stencil stuff
      .BackFace         = {}, // stencil stuff
  };
  mCoreCheckHR( gDevice->device->CreateDepthStencilState( &desc, &state ) );
  return StatusOk;
}


// -----------------------------------------------------------------------------
// -- IndexBuffer
// -----------------------------------------------------------------------------

Status IndexBuffer::init( const char* name, ArrayBytesView bytes )
{
  assert( bytes.getElementSize() == sizeof( u32 ) ||
          bytes.getElementSize() == sizeof( u16 ) );
  auto bufferDesc = D3D11_BUFFER_DESC{
      .ByteWidth           = static_cast<UINT>( bytes.getElementSize() * bytes.getSize() ),
      .Usage               = D3D11_USAGE_IMMUTABLE,
      .BindFlags           = D3D11_BIND_INDEX_BUFFER,
      .CPUAccessFlags      = 0,
      .MiscFlags           = 0,
      .StructureByteStride = static_cast<UINT>( bytes.getElementSize() ),
  };
  auto initialData = D3D11_SUBRESOURCE_DATA{
      .pSysMem          = bytes.getData(),
      .SysMemPitch      = 0,
      .SysMemSlicePitch = 0,
  };
  elementSize  = static_cast<UINT>( bytes.getElementSize() );
  elementCount = static_cast<UINT>( bytes.getSize() );
  format       = bytes.getElementSize() == sizeof( u32 ) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT;
  mCoreCheckHR( gDevice->device->CreateBuffer( &bufferDesc, &initialData, &buffer ) );
  mSetDebugName( buffer, name );
  return StatusOk;
}

void IndexBuffer::use() const
{
  gDevice->context->IASetIndexBuffer( buffer.Get(), format, 0 );
}


// -----------------------------------------------------------------------------
// -- VertexBuffer
// -----------------------------------------------------------------------------

Status VertexBuffer::init( const char* name, ArrayBytesView bytes )
{
  auto bufferDesc = D3D11_BUFFER_DESC{
      .ByteWidth           = static_cast<UINT>( bytes.getElementSize() * bytes.getSize() ),
      .Usage               = D3D11_USAGE_IMMUTABLE,
      .BindFlags           = D3D11_BIND_VERTEX_BUFFER,
      .CPUAccessFlags      = 0,
      .MiscFlags           = 0,
      .StructureByteStride = static_cast<UINT>( bytes.getElementSize() ),
  };
  auto initialData = D3D11_SUBRESOURCE_DATA{
      .pSysMem          = bytes.getData(),
      .SysMemPitch      = 0,
      .SysMemSlicePitch = 0,
  };
  elementSize  = static_cast<u32>( bytes.getElementSize() );
  elementCount = static_cast<u32>( bytes.getSize() );
  mCoreCheckHR( gDevice->device->CreateBuffer( &bufferDesc, &initialData, &buffer ) );
  mSetDebugName( buffer, name );
  return StatusOk;
}

void VertexBuffer::use()
{
  UINT vertexStrides[] = { elementSize };
  UINT vertexOffsets[] = { 0 };
  gDevice->context->IASetVertexBuffers( 0, 1, buffer.GetAddressOf(), vertexStrides, vertexOffsets );
}


// -----------------------------------------------------------------------------
// -- VertexShader
// -----------------------------------------------------------------------------

Status VertexShader::init( ArrayBytesView bytes, VertexShaderLayout layout )
{
  constexpr size_t gDescCapacity = 16;

  if( layout.size() > gDescCapacity )
  {
    core::setErrorDetails( "VertexShaderLayout is too big" );
    return StatusBufferOverflow;
  }

  D3D11_INPUT_ELEMENT_DESC desc[gDescCapacity];
  u32                      descCount = 0;

  for( const auto& item: layout )
  {
    desc[descCount++] = D3D11_INPUT_ELEMENT_DESC{
        .SemanticName         = item.name,
        .SemanticIndex        = 0,
        .Format               = convertGPUFormatToDXGIFormat( item.format ),
        .InputSlot            = 0,
        .AlignedByteOffset    = D3D11_APPEND_ALIGNED_ELEMENT,
        .InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA,
        .InstanceDataStepRate = 0,
    };
  }

  mCoreCheckHR( gDevice->device->CreateVertexShader( bytes.getData(), static_cast<UINT>( bytes.getSize() ), nullptr,
                                                     &vertexShader ) );

  mCoreCheckHR( gDevice->device->CreateInputLayout( desc, static_cast<UINT>( descCount ),
                                                    bytes.getData(), static_cast<UINT>( bytes.getSize() ),
                                                    &inputLayout ) );

  return StatusOk;
}

Status VertexShader::initFromSource( const char* directory, const char* name, VertexShaderLayout layout )
{
  auto shaderBytes = std::vector<byte>{};
  mCoreCheckStatus( compileShader( directory, name, ShaderTypeVertex, shaderBytes ) );
  return init( ArrayBytesView::fromContainer( shaderBytes ), layout );
}

void VertexShader::use()
{
  gDevice->context->IASetInputLayout( inputLayout.Get() );
  gDevice->context->VSSetShader( vertexShader.Get(), nullptr, 0 );
}


// -----------------------------------------------------------------------------
// -- PixelShader
// -----------------------------------------------------------------------------

Status PixelShader::init( ArrayBytesView bytes )
{
  mCoreCheckHR( gDevice->device->CreatePixelShader( bytes.getData(), bytes.getSize(), nullptr, &pixelShader ) );
  return StatusOk;
}

Status PixelShader::initFromSource( const char* directory, const char* name )
{
  auto shaderBytes = std::vector<byte>{};
  mCoreCheckStatus( compileShader( directory, name, ShaderTypePixel, shaderBytes ) );
  return init( ArrayBytesView::fromContainer( shaderBytes ) );
}

void PixelShader::use()
{
  gDevice->context->PSSetShader( pixelShader.Get(), nullptr, 0 );
}


// -----------------------------------------------------------------------------
// -- Texture
// -----------------------------------------------------------------------------

Status Texture::init( u32 width, u32 height, DXGI_FORMAT format )
{
  auto textureDesc = D3D11_TEXTURE2D_DESC{
      .Width          = width,
      .Height         = height,
      .MipLevels      = 1u,
      .ArraySize      = 1u,
      .Format         = format,
      .SampleDesc     = { .Count = 1, .Quality = 0 },
      .Usage          = D3D11_USAGE_DYNAMIC,
      .BindFlags      = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
      .MiscFlags      = 0u,
  };
  mCoreCheckHR( gDevice->device->CreateTexture2D( &textureDesc, nullptr, &texture ) );
  auto viewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
      .Format        = textureDesc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = { .MostDetailedMip = 0u, .MipLevels = 1u },
  };
  mCoreCheckHR( gDevice->device->CreateShaderResourceView( texture.Get(), &viewDesc, &view ) );
  return StatusOk;
}

Status Texture::init( u32 width, u32 height, Vec4b* data )
{
  auto textureDesc = D3D11_TEXTURE2D_DESC{
      .Width          = width,
      .Height         = height,
      .MipLevels      = 1u,
      .ArraySize      = 1u,
      .Format         = DXGI_FORMAT_R8G8B8A8_UNORM,
      .SampleDesc     = { .Count = 1, .Quality = 0 },
      .Usage          = D3D11_USAGE_DEFAULT,
      .BindFlags      = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = 0u,
      .MiscFlags      = 0u,
  };
  static_assert( sizeof( Vec4b ) == sizeof( u32 ) );
  auto subresourceData = D3D11_SUBRESOURCE_DATA{
      .pSysMem          = data,
      .SysMemPitch      = static_cast<UINT>( width * sizeof( Vec4b ) ), // R8G8B8A8 = 32-bit images = Vec4b
      .SysMemSlicePitch = 0u,
  };
  mCoreCheckHR( gDevice->device->CreateTexture2D( &textureDesc, &subresourceData, &texture ) );
  auto viewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
      .Format        = textureDesc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = { .MostDetailedMip = 0u, .MipLevels = 1u },
  };
  mCoreCheckHR( gDevice->device->CreateShaderResourceView( texture.Get(), &viewDesc, &view ) );
  return StatusOk;
}

Status Texture::init( const data::schema::Texture& textureSchema )
{
  auto textureDesc = D3D11_TEXTURE2D_DESC{
      .Width          = textureSchema.width,
      .Height         = textureSchema.height,
      .MipLevels      = textureSchema.mipLevels,
      .ArraySize      = 1u,
      .Format         = static_cast<DXGI_FORMAT>( textureSchema.format ),
      .SampleDesc     = { .Count = 1, .Quality = 0 },
      .Usage          = D3D11_USAGE_DEFAULT,
      .BindFlags      = D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = 0u,
      .MiscFlags      = 0u,
  };
  auto subresourceData = std::vector<D3D11_SUBRESOURCE_DATA>( textureSchema.data.size() );
  for( size_t i = 0; i < subresourceData.size(); ++i )
  {
    subresourceData[i] = D3D11_SUBRESOURCE_DATA{
        .pSysMem          = textureSchema.data[i].mem.data(),
        .SysMemPitch      = textureSchema.data[i].memPitch,
        .SysMemSlicePitch = textureSchema.data[i].memSlicePitch,
    };
  }
  mCoreCheckHR( gDevice->device->CreateTexture2D( &textureDesc, subresourceData.data(), &texture ) );
  auto viewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
      .Format        = textureDesc.Format,
      .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = { .MostDetailedMip = 0u, .MipLevels = textureDesc.MipLevels },
  };
  mCoreCheckHR( gDevice->device->CreateShaderResourceView( texture.Get(), &viewDesc, &view ) );

#ifdef _DEBUG
  char name[128] = { 0 };
  sprintf( name, "tex-" mFmtStringHash, textureSchema.id );
  mSetDebugName( texture, name );
  mSetDebugName( view, name );
#endif

  return StatusOk;
}

void Texture::use( u32 slot ) // TODO: many
{
  gDevice->context->PSSetShaderResources( slot, 1u, view.GetAddressOf() );
}


// -----------------------------------------------------------------------------
// -- RenderTarget
// -----------------------------------------------------------------------------

Status RenderTarget::init( const char* name, Vec2u size, GPUFormat format, std::optional<MSAA> msaa )
{
  assert( size.x > 0 && size.y > 0 );

  auto textureDesc = D3D11_TEXTURE2D_DESC{
      .Width      = size.x,
      .Height     = size.y,
      .MipLevels  = 1u,
      .ArraySize  = 1u,
      .Format     = convertGPUFormatToDXGIFormat( format ),
      .SampleDesc = {
          .Count   = msaa ? msaa->sampleCount : 1u,
          .Quality = 0,
      },
      .Usage          = D3D11_USAGE_DEFAULT,
      .BindFlags      = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
      .CPUAccessFlags = 0u,
      .MiscFlags      = 0u,
  };
  mCoreCheckHR( gDevice->device->CreateTexture2D( &textureDesc, nullptr, &texture ) );

  auto renderTargetViewDesc = D3D11_RENDER_TARGET_VIEW_DESC{
      .Format        = textureDesc.Format,
      .ViewDimension = msaa ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D,
  };
  mCoreCheckHR( gDevice->device->CreateRenderTargetView(
      texture.Get(), &renderTargetViewDesc, &renderTargetView ) );

  auto shaderResourceViewDesc = D3D11_SHADER_RESOURCE_VIEW_DESC{
      .Format        = textureDesc.Format,
      .ViewDimension = msaa ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D,
      .Texture2D     = { .MipLevels = msaa ? 0u : 1u }, // TODO: this is as i understand it
  };
  mCoreCheckHR( gDevice->device->CreateShaderResourceView(
      texture.Get(), &shaderResourceViewDesc, &shaderResourceView ) );

#ifdef _DEBUG
  UINT nameLen = static_cast<UINT>( strlen( name ) );
  texture->SetPrivateData( WKPDID_D3DDebugObjectName, nameLen, name );
  shaderResourceView->SetPrivateData( WKPDID_D3DDebugObjectName, nameLen, name );
  renderTargetView->SetPrivateData( WKPDID_D3DDebugObjectName, nameLen, name );
#else
  ( void ) name;
#endif

  return StatusOk;
}

void RenderTarget::clear( Vec4 clearColor ) const
{
  gDevice->context->ClearRenderTargetView( renderTargetView.Get(), &clearColor[0] );
}


// -----------------------------------------------------------------------------
// -- RenderTargetDefault
// -----------------------------------------------------------------------------

Status RenderTargetDefault::init()
{
  mCoreCheckStatus( reset() );

#ifdef _DEBUG
  auto name = std::string{ "RenderTargetDefault" };
  backBuffer->SetPrivateData( WKPDID_D3DDebugObjectName, static_cast<UINT>( name.size() ), name.c_str() );
  renderTargetView->SetPrivateData( WKPDID_D3DDebugObjectName, static_cast<UINT>( name.size() ), name.c_str() );
#endif

  return StatusOk;
}

Status RenderTargetDefault::reset()
{
  renderTargetView.Reset();
  backBuffer.Reset();

  // TODO: this not works, because sciter holds internal buffer somehow?
  mCoreCheckHR( gDevice->viewport.swapChain->ResizeBuffers( 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0 ) );
  mCoreCheckHR( gDevice->viewport.swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), static_cast<void**>( &backBuffer ) ) );
  mCoreCheckHR( gDevice->device->CreateRenderTargetView( backBuffer.Get(), nullptr, &renderTargetView ) );

  auto desc = D3D11_TEXTURE2D_DESC{};
  backBuffer->GetDesc( &desc );
  width  = desc.Width;
  height = desc.Height;
  format = desc.Format;

  return StatusOk;
}

void RenderTargetDefault::clear( Vec4 clearColor ) const
{
  gDevice->context->ClearRenderTargetView( renderTargetView.Get(), &clearColor[0] );
}

void RenderTargetDefault::copyTo( const Texture& dst ) const
{
  gDevice->context->CopyResource( dst.texture.Get(), backBuffer.Get() );
}


// -----------------------------------------------------------------------------
// -- Viewport
// -----------------------------------------------------------------------------

Status Viewport::init( HWND initWindow )
{
  window = initWindow;
  mCoreCheckStatus( getWindowSize( window, fSize, uSize ) );

  // TODO: before creating swapchain we need to do device_->CheckMultisampleQualityLevels() and
  //       only then set DXGI_SAMPLE_DESC to be correct (this structure passes into CreateSwapChain)
  //       see https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-createswapchain
  //       see https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11device-checkmultisamplequalitylevels

  auto bufferDesc = DXGI_MODE_DESC{
      .Width            = 0, // this means: dx, look into .OutputWindow and figure it out
      .Height           = 0,
      .RefreshRate      = { .Numerator = 0, .Denominator = 0 },
      .Format           = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, // DXGI_FORMAT_B8G8R8A8_UNORM, // DXGI_FORMAT_R8G8B8A8_UNORM ?? TODD: check supported
      .ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED,
      .Scaling          = DXGI_MODE_SCALING_UNSPECIFIED,
  };
  auto swapChainDesc = DXGI_SWAP_CHAIN_DESC{
      .BufferDesc   = bufferDesc,
      .SampleDesc   = { .Count = 1, .Quality = 0 }, // antialiasing
      .BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT,
      .BufferCount  = 1, // double-buffering?
      .OutputWindow = window,
      .Windowed     = TRUE, // TODO: check is really windowed
      .SwapEffect   = DXGI_SWAP_EFFECT_DISCARD,
      .Flags        = 0,
  };
  mCoreCheckHR( gDevice->dxgiFactory->CreateSwapChain( gDevice->device.Get(), &swapChainDesc, &swapChain ) );

  ComPtr<IDXGIOutput> output;
  mCoreCheckHR( swapChain->GetContainingOutput( &output ) );

  //  auto gammaControl = DXGI_GAMMA_CONTROL{  };
  //  mPvkFailIfHRESULT( output->SetGammaControl( &gammaControl ) );

  mCoreCheckStatus( renderTarget.init() );
  return StatusOk;
}

void Viewport::clear( Vec4 color )
{
  renderTarget.clear( color );
}

Status Viewport::present()
{
  UINT syncInterval = vsync ? 1u : 0u;
  UINT flags        = 0u; // DXGI_PRESENT_...
  mCoreCheckHR( swapChain->Present( syncInterval, flags ) );
  return StatusOk;
}

Status Viewport::reset()
{
  mCoreCheckStatus( getWindowSize( window, fSize, uSize ) );
  mCoreCheckStatus( renderTarget.reset() );
  return StatusOk;
}
