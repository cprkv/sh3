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

  {
    auto& vsConstant                             = gCommonRenderData->oldFullVSConstant;
    vsConstant->gSceneLights.ambient             = Vec3( 0.1f );
    vsConstant->gSceneLights.count               = 1;
    vsConstant->gSceneLights.lights[0].position  = Vec3( 0 );
    vsConstant->gSceneLights.lights[0].color     = core::math::decodeColorHex( 0xE7'8C'FF );
    vsConstant->gSceneLights.lights[0].intensity = 40;
    vsConstant->gViewPos                         = renderList.viewPosition;
    vsConstant->gWorldToView                     = renderList.worldToViewTransform;
    vsConstant->gViewToProjection                = renderList.viewToProjectionTransform;
    if( vsConstant.update() != StatusOk )
    {
      mCoreLog( "update constant failed" );
      return;
    }

    auto& vsConstantModel = gCommonRenderData->oldFullVSConstantModel;

    auto rpState = RenderPipelineState();

    auto rp = RenderPipeline( rpState )
                  .useTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST )
                  .bind( gCommonRenderData->shaderTable.oldFull.vertexShader )
                  .bind( gCommonRenderData->shaderTable.oldFull.pixelShader )
                  .bind( vsConstant, ConstantBufferTargetVertex )
                  .bind( vsConstantModel, ConstantBufferTargetVertex )
                  .bind( depthStencil )
                  .bind( gCommonRenderData->depthStencilStateEnabled )
                  .bind( gDevice->samplerStateClamp )
                  .addTarget( renderTarget );

    for( auto& drawable: renderList.drawables )
    {
      vsConstantModel->gModelToWorld      = drawable.worldTransform;
      vsConstantModel->gWorldInvTranspose = glm::inverseTranspose( vsConstantModel->gModelToWorld );
      if( vsConstantModel.update() != StatusOk )
      {
        mCoreLog( "update constant failed" );
        return;
      }

      rp.extend()
          .bind( drawable.mesh->indexBuffer )
          .bind( drawable.mesh->vertexBuffer )
          .bind( drawable.diffuseTexture->texture )
          .draw();
    }
  }

  renderFullScreenOnViewportMS( renderTarget );
}
