#include "core/render/gapi/render-pipeline.hpp"
#include "core/render/gapi/device.hpp"

using namespace core::render;
using namespace core::render::gapi;


RenderPipeline::~RenderPipeline() noexcept
{
  for( auto& cleanup_func: cleanup_ )
  {
    cleanup_func( state_ );
  }

  if( state_->reloadOutputMergerTargets )
  {
    gDevice->context->OMSetRenderTargets( static_cast<UINT>( state_->renderTargetViews.size() ),
                                          state_->renderTargetViews.data(),
                                          state_->depthStencil );
    state_->reloadOutputMergerTargets = false;
  }

  if( state_->numTexturesFreed )
  {
    auto srv = decltype( state_->textureBuffers )( state_->numTexturesFreed, nullptr );
    gDevice->context->PSSetShaderResources( static_cast<UINT>( state_->textureBuffers.size() ),
                                            static_cast<UINT>( srv.size() ),
                                            srv.data() );
    state_->numTexturesFreed = 0;
  }

  if( state_->alphaBlendingEnabled )
  {
    state_->alphaBlendingEnabled = false;
    gDevice->enableAlphaBlending( false );
  }

  ( void ) gDevice->logMessages();
}


RenderPipeline&& RenderPipeline::useTopology( D3D11_PRIMITIVE_TOPOLOGY topology ) &&
{
  state_->primitiveTopology = topology;
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::useAlphaBlending() &&
{
  state_->alphaBlendingEnabled = true;
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( VertexShader& vertexShader ) &&
{
  assert( !state_->vertexShader );
  state_->vertexShader = &vertexShader;
  cleanup_.push_back( []( RenderPipelineState* s ) {
    gDevice->context->VSSetShader( nullptr, nullptr, 0 );
    s->vertexShader = nullptr;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( PixelShader& pixelShader ) &&
{
  assert( !state_->pixelShader );
  state_->pixelShader = &pixelShader;
  cleanup_.push_back( []( RenderPipelineState* s ) {
    gDevice->context->PSSetShader( nullptr, nullptr, 0 );
    s->pixelShader = nullptr;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( VertexBuffer& vertexBuffer ) &&
{
  assert( !state_->vertexBuffer );
  state_->vertexBuffer = &vertexBuffer;
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->vertexBuffer = nullptr;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( IndexBuffer& indexBuffer ) &&
{
  assert( !state_->indexBuffer );
  state_->indexBuffer = &indexBuffer;
  cleanup_.push_back( []( RenderPipelineState* s ) {
    gDevice->context->IASetIndexBuffer( nullptr, DXGI_FORMAT_UNKNOWN, 0 );
    s->indexBuffer = nullptr;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( ConstantBuffer& constantBuffer, ConstantBufferTarget target ) &&
{
  if( ( target & ConstantBufferTargetVertex ) != 0 )
  {
    state_->vertexConstants.push_back( constantBuffer.buffer.Get() );
    cleanup_.push_back( []( RenderPipelineState* s ) {
      s->vertexConstants.pop_back();
    } );
  }

  if( ( target & ConstantBufferTargetPixel ) != 0 )
  {
    state_->pixelConstants.push_back( constantBuffer.buffer.Get() );
    cleanup_.push_back( []( RenderPipelineState* s ) {
      s->pixelConstants.pop_back();
    } );
  }

  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( SamplerState& samplerState ) &&
{
  state_->samplerStates.push_back( samplerState.samplerState.Get() );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->samplerStates.pop_back();
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( Texture& texture ) &&
{
  state_->textureBuffers.push_back( texture.view.Get() );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->textureBuffers.pop_back();
    ++s->numTexturesFreed;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( RenderTarget& renderTarget ) &&
{
  state_->textureBuffers.push_back( renderTarget.shaderResourceView.Get() );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->textureBuffers.pop_back();
    ++s->numTexturesFreed;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( DepthStencil& depthStencil ) &&
{
  assert( !state_->depthStencil );
  state_->depthStencil = depthStencil.depthStencilView.Get();
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->depthStencil              = nullptr;
    s->reloadOutputMergerTargets = true;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::bind( DepthStencilState& depthStencilState ) &&
{
  assert( !state_->depthStencilState );
  state_->depthStencilState = depthStencilState.state.Get();
  assert( state_->depthStencilState );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->depthStencil = nullptr;
    gDevice->context->OMSetDepthStencilState( nullptr, 0u );
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::addTarget( RenderTarget& renderTarget ) &&
{
  state_->renderTargetViews.push_back( renderTarget.renderTargetView.Get() );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->renderTargetViews.pop_back();
    s->reloadOutputMergerTargets = true;
  } );
  return std::move( *this );
}


RenderPipeline&& RenderPipeline::addTarget( RenderTargetDefault& renderTargetDefault ) &&
{
  assert( state_->renderTargetViews.empty() );
  state_->renderTargetViews.push_back( renderTargetDefault.renderTargetView.Get() );
  cleanup_.push_back( []( RenderPipelineState* s ) {
    s->renderTargetViews.pop_back();
    s->reloadOutputMergerTargets = true;
  } );
  return std::move( *this );
}


void RenderPipeline::draw()
{
  assert( state_->vertexShader );
  assert( state_->pixelShader );
  assert( state_->vertexBuffer );
  assert( !state_->renderTargetViews.empty() );

  state_->vertexShader->use();
  state_->pixelShader->use();
  state_->vertexBuffer->use();

  if( state_->alphaBlendingEnabled )
    gDevice->enableAlphaBlending( true );

  if( state_->indexBuffer )
    state_->indexBuffer->use();

  gDevice->context->IASetPrimitiveTopology( state_->primitiveTopology );

  if( !state_->vertexConstants.empty() )
    gDevice->context->VSSetConstantBuffers( 0, static_cast<UINT>( state_->vertexConstants.size() ),
                                            state_->vertexConstants.data() );

  if( !state_->pixelConstants.empty() )
    gDevice->context->PSSetConstantBuffers( 0, static_cast<UINT>( state_->pixelConstants.size() ),
                                            state_->pixelConstants.data() );

  if( !state_->textureBuffers.empty() )
    gDevice->context->PSSetShaderResources( 0, static_cast<UINT>( state_->textureBuffers.size() ),
                                            state_->textureBuffers.data() );

  if( !state_->samplerStates.empty() )
    gDevice->context->PSSetSamplers( 0, static_cast<UINT>( state_->samplerStates.size() ),
                                     state_->samplerStates.data() );

  if( state_->depthStencilState )
    gDevice->context->OMSetDepthStencilState( state_->depthStencilState, 0u );

  gDevice->context->OMSetRenderTargets( static_cast<UINT>( state_->renderTargetViews.size() ),
                                        state_->renderTargetViews.data(), state_->depthStencil );

  auto dxViewport = D3D11_VIEWPORT{
      .TopLeftX = 0,
      .TopLeftY = 0,
      .Width    = gDevice->viewport.fSize.x,
      .Height   = gDevice->viewport.fSize.y,
      .MinDepth = 0,
      .MaxDepth = 1,
  };
  gDevice->context->RSSetViewports( 1, &dxViewport );

  if( state_->indexBuffer )
    gDevice->context->DrawIndexed( state_->indexBuffer->elementCount, 0, 0 );
  else
    gDevice->context->Draw( state_->vertexBuffer->elementCount, 0 );

  ( void ) gDevice->logMessages();
}
