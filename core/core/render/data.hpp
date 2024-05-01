#pragma once
#include "core/common.hpp"
#include "core/render/gapi/resources.hpp"

namespace core::render
{
  struct Texture
  {
    gapi::Texture texture;
    Vec2          size;
  };

  struct Mesh
  {
    gapi::VertexBuffer vertexBuffer;
    gapi::IndexBuffer  indexBuffer;
  };

  struct Material
  {
    Texture                diffuseTexture;
    std::optional<Texture> normalTexture;
  };

  // internal?? data related stuff...
  struct VertexData
  {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
  };
} // namespace core::render
