#pragma once
#include "core/common.hpp"
#include "core/render/gapi/common.hpp"

namespace core::render::gapi
{
  enum ShaderType
  {
    ShaderTypeVertex,
    ShaderTypePixel,
  };

  Status compileShader( const char*      directory,
                        const char*      name,
                        ShaderType       shaderType,
                        std::vector<u8>& out );

} // namespace core::render::gapi
