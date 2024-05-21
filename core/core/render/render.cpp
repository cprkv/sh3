#include "core/render/render.hpp"
#include "core/render/gapi/device.hpp"
#include "core/render/shader-table.hpp"
#include "core/render/pass/render-pass.hpp"
#include "core/data/data.hpp"
#include "core/math/math.hpp"
#include "core/input/input.hpp"

using namespace core;
using namespace core::render;
using namespace core::render::gapi;

core::render::CommonRenderData* core::render::gCommonRenderData = nullptr;

namespace
{
  struct StaticData
  {
    RenderPass3D renderPass3d;
    RenderList   renderList;
  };

  StaticData* sData = nullptr;


  struct Vertex2DInput
  {
    Vec2 position;
    Vec2 uv;
  };

  Vertex2DInput sFullscreenQuadData[] = {
      { .position = { -1, 1 }, .uv = { 0, 0 } },  // top-left      first triangle
      { .position = { 1, -1 }, .uv = { 1, 1 } },  // bottom-right  first triangle
      { .position = { -1, -1 }, .uv = { 0, 1 } }, // bottom-left   first triangle
      { .position = { -1, 1 }, .uv = { 0, 0 } },  // top-left      second triangle
      { .position = { 1, 1 }, .uv = { 1, 0 } },   // top-right     second triangle
      { .position = { 1, -1 }, .uv = { 1, 1 } },  // bottom-left   second triangle
  };
} // namespace


// TODO: allow multiple submit to draw different parts... with different perspective, or different type like RenderList2D...
void RenderList::submit()
{
  gDevice->viewport.renderTarget.clear( Vec4( 1, 0, 0, 0 ) );
  sData->renderPass3d.render( *this );
}


Status core::render::initialize( HWND windowHandle )
{
  sData = new StaticData();

  // device
  {
    gDevice = new Device();
    if( auto s = gDevice->init( windowHandle ); s != StatusOk )
    {
      delete gDevice;
      gDevice = nullptr;
      mCoreLogError( "render device init failed\n" );
      return s;
    }
  }

  // common data
  {
    gCommonRenderData = new CommonRenderData();
    mCoreCheckStatus( gCommonRenderData->shaderTable.init( data::getDataPath( "shaders" ) ) );

    mCoreCheckStatus( gCommonRenderData->fullscreenQuadVertexBuffer.init( "fullscreen-quad", makeArrayBytesView( sFullscreenQuadData ) ) );
    mCoreCheckStatus( gCommonRenderData->depthStencilStateEnabled.init( true ) );
    mCoreCheckStatus( gCommonRenderData->depthStencilStateDisabled.init( false ) );

    mCoreCheckStatus( gCommonRenderData->texture2DVSConstant.init( ConstantBufferTargetVertex ) );
    mCoreCheckStatus( gCommonRenderData->loadingConstant.init( ConstantBufferTargetAll ) );
    mCoreCheckStatus( gCommonRenderData->oldFullPSConstant.init( ConstantBufferTargetPixel ) );
    mCoreCheckStatus( gCommonRenderData->oldFullVSConstant.init( ConstantBufferTargetVertex ) );
    mCoreCheckStatus( gCommonRenderData->oldFullVSConstantModel.init( ConstantBufferTargetVertex ) );
    mCoreCheckStatus( gCommonRenderData->lineVSConstant.init( ConstantBufferTargetVertex ) );
  }

  // static data
  {
    mCoreCheckStatus( sData->renderPass3d.init() );
  }

  return StatusOk;
}


void render::destroy()
{
  delete sData;
  delete gCommonRenderData;
  delete gDevice;
}


void render::update()
{
#ifdef _DEBUG
  if( input::isKeyMod( input::KeyModCtrl ) && input::isKeyDown( input::KeyR ) )
  {
    auto s = gCommonRenderData->shaderTable.reload();
    if( s != StatusOk )
      mCoreLogError( "error reload shader table: %d\n", static_cast<int>( s ) );
    else
      mCoreLog( "shader table reloaded!\n" );
  }
#endif
}


RenderList& render::getRenderList()
{
  return sData->renderList;
}


void render::present()
{
  sData->renderList.submit();
  sData->renderList.clear();
  gDevice->viewport.present();
  gDevice->logMessages();
}
