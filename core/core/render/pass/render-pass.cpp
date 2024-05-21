#include "core/render/pass/render-pass.hpp"
#include "core/render/gapi/device.hpp"
#include "core/render/gapi/render-pipeline.hpp"
#include "core/math/math.hpp"

using namespace core::render;
using namespace core::render::gapi;

namespace
{
  template<typename T>
  void renderBindableOnViewport( ShaderPipeline& pipeline, T& what )
  {
    auto& vertexBuffer = gCommonRenderData->fullscreenQuadVertexBuffer;
    auto& depthStencil = gCommonRenderData->depthStencilStateDisabled;
    auto& vsConstant   = gCommonRenderData->texture2DVSConstant;
    auto& samplerState = gDevice->samplerStateClamp;
    auto& renderTarget = gDevice->viewport.renderTarget;

    vsConstant.data() = {
        .gPosition   = Vec2( 0, 0 ),
        .gSize       = gDevice->viewport.fSize,
        .gScreenSize = gDevice->viewport.fSize,
    };

    if( vsConstant.update() != StatusOk )
    {
      mCoreLog( "error updating constant buffer" );
      return;
    }

    auto rpState = gapi::RenderPipelineState();
    gapi::RenderPipeline( rpState )
        .useTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
        .useAlphaBlending()
        .bind( pipeline.vertexShader )
        .bind( pipeline.pixelShader )
        .bind( vertexBuffer )
        .bind( vsConstant )
        .bind( depthStencil )
        .bind( samplerState )
        .bind( what )
        .addTarget( renderTarget )
        .draw();
  }

  void renderFullScreenOnViewportMS( gapi::RenderTarget& renderTarget )
  {
    return renderBindableOnViewport( gCommonRenderData->shaderTable.texture2DMS, renderTarget );
  }
} // namespace


Status RenderPass3D::init()
{
  auto msaa = std::make_optional( MSAA{ .sampleCount = 4 } );
  mCoreCheckStatus( renderTarget.init( "RenderScene3D", gDevice->viewport.uSize, GPUFormatRGBA32F, msaa ) );
  mCoreCheckStatus( depthStencil.init( gDevice->viewport.uSize, msaa ) );
  return StatusOk;
}


void RenderPass3D::render( RenderList& renderList )
{
  depthStencil.clear();
  renderTarget.clear( Vec4( 0, 1, 0, 1 ) );

  // loading screen
  if( renderList.loadingScreen )
  {
    auto& vsConstant  = gCommonRenderData->loadingConstant;
    vsConstant.data() = {
        .iResolution = gDevice->viewport.fSize,
        .iTime       = stopwatch.getSecF(),
    };
    if( vsConstant.update() != StatusOk ) // TODO
      abort();

    auto rpState = gapi::RenderPipelineState();
    gapi::RenderPipeline( rpState )
        .useTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
        .useAlphaBlending()
        .bind( gCommonRenderData->shaderTable.loading.vertexShader )
        .bind( gCommonRenderData->shaderTable.loading.pixelShader )
        .bind( gCommonRenderData->fullscreenQuadVertexBuffer )
        .bind( vsConstant )
        .bind( gCommonRenderData->depthStencilStateDisabled )
        .addTarget( renderTarget )
        .draw();
  }

  // 3d geometry
  {
    auto& vsConstant                 = gCommonRenderData->oldFullVSConstant;
    auto& psConstant                 = gCommonRenderData->oldFullPSConstant;
    vsConstant->gSceneLights.ambient = Vec3( 0.1f );

    size_t lightsCount             = std::min( std::size( vsConstant->gSceneLights.lights ), renderList.lights.size() );
    vsConstant->gSceneLights.count = static_cast<decltype( vsConstant->gSceneLights.count )>( lightsCount );

    for( size_t i = 0; i < lightsCount; ++i )
    {
      vsConstant->gSceneLights.lights[i].position  = renderList.lights[i].position;
      vsConstant->gSceneLights.lights[i].color     = renderList.lights[i].color;
      vsConstant->gSceneLights.lights[i].intensity = renderList.lights[i].intensity;
    }

    vsConstant->gViewPos          = renderList.viewPosition;
    vsConstant->gWorldToView      = renderList.worldToViewTransform;
    vsConstant->gViewToProjection = renderList.viewToProjectionTransform;
    if( vsConstant.update() != StatusOk ) // TODO
      abort();

    auto& vsConstantModel = gCommonRenderData->oldFullVSConstantModel;

    auto rpState        = RenderPipelineState();
    auto renderPipeline = RenderPipeline( rpState );

    renderPipeline
        .useTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
        .bind( gCommonRenderData->shaderTable.oldFull.vertexShader )
        .bind( gCommonRenderData->shaderTable.oldFull.pixelShader )
        .bind( vsConstant, ConstantBufferTargetVertex )
        .bind( vsConstantModel, ConstantBufferTargetVertex )
        .bind( psConstant, ConstantBufferTargetPixel )
        .bind( depthStencil )
        .bind( gCommonRenderData->depthStencilStateEnabled )
        .bind( gDevice->samplerStateClamp )
        .addTarget( renderTarget );

    std::ranges::sort( renderList.drawables, []( const RenderList::Drawable& a, const RenderList::Drawable& b ) {
      return a.blendMode < b.blendMode;
    } );

    for( auto& drawable: renderList.drawables )
    {
      vsConstantModel->gModelToWorld      = drawable.worldTransform;
      vsConstantModel->gWorldInvTranspose = glm::inverseTranspose( vsConstantModel->gModelToWorld );
      if( vsConstantModel.update() != StatusOk ) // TODO
        abort();

      psConstant->gAlphaHash = static_cast<u32>( drawable.blendMode == BlendMode_AlphaHash );
      if( psConstant.update() != StatusOk ) // TODO
        abort();

      auto currentRp = renderPipeline.extend();

      if( drawable.blendMode == BlendMode_AlphaBlend )
        currentRp.useAlphaBlending();

      currentRp
          .bind( drawable.mesh->indexBuffer )
          .bind( drawable.mesh->vertexBuffer )
          .bind( drawable.diffuseTexture->texture )
          .draw();
    }
  }

  // lines
  if( !renderList.lines.empty() )
  {
    auto vertexBuffer = gapi::VertexBuffer();
    auto abv          = makeArrayBytesView( renderList.lines );
    if( vertexBuffer.init( "line-buffer", abv ) != StatusOk ) // TODO
      abort();

    gCommonRenderData->lineVSConstant->gWorldToProjection =
        renderList.viewToProjectionTransform *
        renderList.worldToViewTransform;
    if( gCommonRenderData->lineVSConstant.update() != StatusOk ) // TODO
      abort();

    auto rpState = RenderPipelineState();
    RenderPipeline( rpState )
        .useTopology( D3D11_PRIMITIVE_TOPOLOGY_LINELIST )
        .useAlphaBlending()
        .bind( gCommonRenderData->shaderTable.line.vertexShader )
        .bind( gCommonRenderData->shaderTable.line.pixelShader )
        .bind( gCommonRenderData->lineVSConstant )
        .bind( gCommonRenderData->depthStencilStateDisabled )
        .bind( vertexBuffer )
        .addTarget( renderTarget )
        .draw();
  }

  renderFullScreenOnViewportMS( renderTarget );
}
