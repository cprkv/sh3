#pragma once
#include "core/common.hpp"
#include "core/render/gapi/resources.hpp"
#include "core/render/render.hpp"
#include "core/system/time.hpp"

namespace core::render
{
  struct RenderPass3D
  {
    system::Stopwatch  stopwatch;
    gapi::RenderTarget renderTarget;
    gapi::DepthStencil depthStencil;

    Status init();
    void   render( RenderList& renderList );
  };
} // namespace core::render
