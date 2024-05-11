#pragma once
#include "core/common.hpp"
#include "core/render/gapi/common.hpp"
#include "core/data/schema.hpp"


namespace core::render::gapi
{
  struct MSAA
  {
    u32 sampleCount = 1;
  };


  struct SamplerState
  {
    ComPtr<ID3D11SamplerState> samplerState;

    Status init( D3D11_TEXTURE_ADDRESS_MODE mode );
    void   use( u32 slot );
  };


  enum ConstantBufferTarget : u8
  {
    ConstantBufferTargetVertex = 1u,
    ConstantBufferTargetPixel  = 2u,
    ConstantBufferTargetAll    = 3u,
  };

  struct ConstantBuffer
  {
    ComPtr<ID3D11Buffer> buffer;
    usize                elementSize = 0;
    ConstantBufferTarget target      = ConstantBufferTargetAll;

    Status init( ArrayBytesView bytes, ConstantBufferTarget target );
    Status update( ArrayBytesView bytes ) const;
    void   use();
  };

  template<typename TData>
  struct ConstantBufferData : public ConstantBuffer
  {
    struct AlignedData
    {
      alignas( 16 ) TData data{};
    };

    AlignedData data_{};

    Status init( ConstantBufferTarget aTarget ) { return ConstantBuffer::init( makeArrayBytesView( &data_, 1 ), aTarget ); }
    TData& data() { return data_.data; }
    Status update() { return ConstantBuffer::update( makeArrayBytesView( &data_, 1 ) ); }

    TData* operator->() { return &data_.data; }
    TData& operator*() { return data_.data; }
  };


  struct DepthStencil
  {
    ComPtr<ID3D11DepthStencilView>   depthStencilView;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    Status init( Vec2u size, std::optional<MSAA> msaa = std::nullopt );
    void   clear() const;
  };

  struct DepthStencilState
  {
    ComPtr<ID3D11DepthStencilState> state;

    Status init( bool enabled );
  };


  struct IndexBuffer
  {
    ComPtr<ID3D11Buffer> buffer;
    u32                  elementSize  = 0;
    u32                  elementCount = 0;
    DXGI_FORMAT          format;

    Status init( ArrayBytesView bytes );
    void   use() const;
  };


  struct VertexBuffer
  {
    ComPtr<ID3D11Buffer> buffer;
    u32                  elementSize  = 0;
    u32                  elementCount = 0;

    Status init( ArrayBytesView bytes );
    void   use();
  };


  struct VertexShaderItemLayout
  {
    const char* name;
    GPUFormat   format;
  };

  using VertexShaderLayout = ArrayView<const VertexShaderItemLayout>;

  struct VertexShader
  {
    ComPtr<ID3D11VertexShader> vertexShader;
    ComPtr<ID3D11InputLayout>  inputLayout;

    Status init( ArrayBytesView bytes, VertexShaderLayout layout );
    Status initFromSource( const char* directory, const char* name, VertexShaderLayout layout );
    void   use();
  };


  struct PixelShader
  {
    ComPtr<ID3D11PixelShader> pixelShader;

    Status init( ArrayBytesView bytes );
    Status initFromSource( const char* directory, const char* name );
    void   use();
  };


  struct Texture
  {
    ComPtr<ID3D11Texture2D>          texture;
    ComPtr<ID3D11ShaderResourceView> view;

    Status init( u32 width, u32 height, DXGI_FORMAT format );
    Status init( u32 width, u32 height, Vec4b* data );
    Status init( const data::schema::Texture& textureSchema );
    void   use( u32 slot );
  };


  struct RenderTarget
  {
    ComPtr<ID3D11Texture2D>          texture;
    ComPtr<ID3D11RenderTargetView>   renderTargetView;
    ComPtr<ID3D11ShaderResourceView> shaderResourceView;

    Status init( const char* name, Vec2u size, GPUFormat format, std::optional<MSAA> msaa = std::nullopt );
    void   clear( Vec4 clearColor ) const;
  };

  // TODO: this is the same as RenderTarget, except no SRV, and different create method
  struct RenderTargetDefault
  {
    ComPtr<ID3D11RenderTargetView> renderTargetView;
    ComPtr<ID3D11Texture2D>        backBuffer;
    u32                            width;
    u32                            height;
    DXGI_FORMAT                    format;

    Status init();
    Status reset();
    void   clear( Vec4 clearColor ) const;
    void   copyTo( const Texture& dst ) const;
  };


  struct Viewport
  {
    HWND                   window;
    Vec2                   fSize;
    Vec2u                  uSize;
    ComPtr<IDXGISwapChain> swapChain;
    RenderTargetDefault    renderTarget;
    bool                   vsync = true;

    Status init( HWND window );
    void   clear( Vec4 color = { 0.12, 0.12, 0.12, 1 } );
    Status present();
    Status reset();
  };
} // namespace core::render::gapi
