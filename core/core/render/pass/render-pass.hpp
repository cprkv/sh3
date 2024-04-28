#pragma once
#include "core/common.hpp"
#include "core/render/gapi/resources.hpp"
#include "core/render/render.hpp"

namespace core::render
{
  struct RenderPass3D
  {
    gapi::RenderTarget renderTarget;
    gapi::DepthStencil depthStencil;

    Status init();
    void   render( RenderList& renderList );
  };
} // namespace core::render
