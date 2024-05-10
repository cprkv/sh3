#pragma once
#include "core/common.hpp"
#include "core/render/data.hpp"
#include "core/render/shader-table.hpp"
#include "core/render/gapi/resources.hpp"
#include "core/render/gapi/device.hpp"

namespace core::render
{
  class RenderList
  {
  public:
    struct Drawable
    {
      Mesh*    mesh;
      Texture* diffuseTexture;
      Mat4     worldTransform;
    };

    std::vector<Drawable> drawables;

    Vec3 viewPosition;
    Mat4 worldToViewTransform;
    Mat4 viewToProjectionTransform;

    // TODO: camera
    void addMesh( Mesh* mesh, Texture* diffuseTexture, Mat4 worldTransform );
    void submit();
    void clear() { drawables.clear(); }
  };


  struct CommonRenderData
  {
    ShaderTable             shaderTable;
    gapi::VertexBuffer      fullscreenQuadVertexBuffer;
    gapi::DepthStencilState depthStencilStateEnabled;
    gapi::DepthStencilState depthStencilStateDisabled;

    gapi::ConstantBufferData<shaders::Texture2DVSConstantBuffer>    texture2DVSConstant;
    gapi::ConstantBufferData<shaders::OldFullVSConstantBuffer>      oldFullVSConstant;
    gapi::ConstantBufferData<shaders::OldFullVSConstantBufferModel> oldFullVSConstantModel;
  };


  extern CommonRenderData* gCommonRenderData;


  Status initialize( HWND windowHandle );
  void   destroy();

  RenderList& getRenderList();
  void        present();
} // namespace core::render
