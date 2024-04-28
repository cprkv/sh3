#pragma once
#include "core/common.hpp"
#include "core/render/gapi/resources.hpp"

namespace core::render
{
  //static constexpr glm::vec3 gGlobalForward = { 0, 0, 1 };
  //static constexpr glm::vec3 gGlobalRight   = { 1, 0, 0 };
  //static constexpr glm::vec3 gGlobalUp      = { 0, 1, 0 };

  static constexpr glm::vec3 gGlobalForward = { 0, 1, 0 };
  static constexpr glm::vec3 gGlobalRight   = { 1, 0, 0 };
  static constexpr glm::vec3 gGlobalUp      = { 0, 0, 1 };

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
