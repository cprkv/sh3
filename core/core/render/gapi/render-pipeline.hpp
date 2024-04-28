#pragma once
#include "core/common.hpp"
#include "core/render/gapi/resources.hpp"

namespace core::render::gapi
{
  struct RenderPipelineState
  {
    using RenderTargetViews = StaticVector<ID3D11RenderTargetView*, 6>;
    using SamplerStates     = StaticVector<ID3D11SamplerState*, 2>;
    using ConstantBuffers   = StaticVector<ID3D11Buffer*, 4>;
    using TextureBuffers    = StaticVector<ID3D11ShaderResourceView*, 6>;

    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    ID3D11DepthStencilView*  depthStencil      = nullptr;
    ID3D11DepthStencilState* depthStencilState = nullptr;
    VertexShader*            vertexShader      = nullptr;
    PixelShader*             pixelShader       = nullptr;
    VertexBuffer*            vertexBuffer      = nullptr;
    IndexBuffer*             indexBuffer       = nullptr;

    RenderTargetViews renderTargetViews;
    SamplerStates     samplerStates;
    ConstantBuffers   vertexConstants;
    ConstantBuffers   pixelConstants;
    TextureBuffers    textureBuffers;

    //bool dirty              = false;
    bool reloadOutputMergerTargets = false;
    bool alphaBlendingEnabled      = false;
    u32  numTexturesFreed          = 0;
  };


  // TODO: maybe rename to RenderState + RenderStateContext/RenderStateFrame?
  // TODO: maybe rename to DrawState + DrawStateContext/DrawStateFrame?
  class RenderPipeline
  {
    using CleanupFunc = void ( * )( RenderPipelineState* );

    RenderPipelineState*          state_;
    StaticVector<CleanupFunc, 24> cleanup_;

  public:
    explicit RenderPipeline( RenderPipelineState& renderPipelineState )
        : state_{ &renderPipelineState }
    {}

    ~RenderPipeline() noexcept;

    RenderPipeline extend() { return RenderPipeline{ *state_ }; }

    RenderPipeline( const RenderPipeline& )            = delete;
    RenderPipeline& operator=( const RenderPipeline& ) = delete;

    RenderPipeline( RenderPipeline&& ) noexcept            = default;
    RenderPipeline& operator=( RenderPipeline&& ) noexcept = default;

    RenderPipeline&& useTopology( D3D11_PRIMITIVE_TOPOLOGY topology ) &&;
    RenderPipeline&& useAlphaBlending() &&;
    RenderPipeline&& bind( VertexShader& vertexShader ) &&;
    RenderPipeline&& bind( PixelShader& pixelShader ) &&;
    RenderPipeline&& bind( VertexBuffer& vertexBuffer ) &&;
    RenderPipeline&& bind( IndexBuffer& indexBuffer ) &&;
    RenderPipeline&& bind( ConstantBuffer& constantBuffer, ConstantBufferTarget target = ConstantBufferTargetAll ) &&;
    RenderPipeline&& bind( SamplerState& samplerState ) &&;
    RenderPipeline&& bind( Texture& texture ) &&;
    RenderPipeline&& bind( RenderTarget& renderTarget ) &&;
    RenderPipeline&& bind( DepthStencil& depthStencil ) &&;
    RenderPipeline&& bind( DepthStencilState& depthStencilState ) &&;

    RenderPipeline&& addTarget( RenderTarget& renderTarget ) &&;
    RenderPipeline&& addTarget( RenderTargetDefault& renderTargetDefault ) &&;

    void draw();
  };
} // namespace core::render::gapi
