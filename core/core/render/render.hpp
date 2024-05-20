#pragma once
#include "core/common.hpp"
#include "core/render/data.hpp"
#include "core/render/shader-table.hpp"
#include "core/render/gapi/resources.hpp"
#include "core/render/gapi/device.hpp"

namespace core::render
{
#define xBlendModeEnum( X ) \
  X( BlendMode, Opaque )    \
  X( BlendMode, AlphaHash ) \
  X( BlendMode, AlphaBlend )

  mCoreDeclareEnum( BlendMode, xBlendModeEnum );


  class RenderList
  {
  public:
    struct Drawable
    {
      Mesh*     mesh;
      Texture*  diffuseTexture;
      BlendMode blendMode;
      Mat4      worldTransform;
    };

    std::vector<Drawable> drawables;

    struct Point
    {
      Vec3 position;
      Vec4 color;
    };

    std::vector<Point> lines; // must be pairs (line list)

    struct PointLight
    {
      Vec3 position;
      Vec3 color;
      f32  intensity;
    };
    std::vector<PointLight> lights;

    Vec3 viewPosition;
    Mat4 worldToViewTransform;
    Mat4 viewToProjectionTransform;

    void submit();

    void clear()
    {
      drawables.clear();
      lines.clear();
      lights.clear();
    }
  };


  struct CommonRenderData
  {
    ShaderTable             shaderTable;
    gapi::VertexBuffer      fullscreenQuadVertexBuffer;
    gapi::DepthStencilState depthStencilStateEnabled;
    gapi::DepthStencilState depthStencilStateDisabled;

    gapi::ConstantBufferData<shaders::Texture2DVSConstantBuffer>    texture2DVSConstant;
    gapi::ConstantBufferData<shaders::OldFullPSConstantBuffer>      oldFullPSConstant;
    gapi::ConstantBufferData<shaders::OldFullVSConstantBuffer>      oldFullVSConstant;
    gapi::ConstantBufferData<shaders::OldFullVSConstantBufferModel> oldFullVSConstantModel;
    gapi::ConstantBufferData<shaders::LineVSConstantBuffer>         lineVSConstant;
  };


  extern CommonRenderData* gCommonRenderData;


  Status initialize( HWND windowHandle );
  void   destroy();

  RenderList& getRenderList();
  void        present();
} // namespace core::render
