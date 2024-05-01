#pragma once
#include "core/common.hpp"
#include "core/render/data.hpp"
#include "core/render/shader-table.hpp"
#include "core/render/gapi/resources.hpp"
#include "core/render/gapi/device.hpp"
#include "core/data/string-id.hpp"

namespace core::render
{
  class RenderList
  {
  public:
    struct Drawable
    {
      Mesh*    mesh;
      Texture* diffuseTexture;
    };

    std::vector<Drawable> drawables;

    Vec3 viewPosition;
    Mat4 worldToViewTransform;
    Mat4 viewToProjectionTransform;

    // TODO: camera
    void addMesh( data::StringId meshId, data::StringId diffuseTextureId );
    void submit();
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
  void   present();
} // namespace core::render
