#pragma once
#include "core/common.hpp"
#include "core/render/shaders/shader-types.hpp"
#include "core/render/gapi/resources.hpp"

namespace core::render
{
  struct ShaderPipeline
  {
    gapi::VertexShader vertexShader;
    gapi::PixelShader  pixelShader;
  };

  struct ShaderTable
  {
    std::string    directory;
    ShaderPipeline texture2D;
    ShaderPipeline texture2DMS;
    ShaderPipeline oldFull;
    ShaderPipeline line;

    Status init( std::string initDirectory );
    Status reload();
  };
} // namespace core::render
