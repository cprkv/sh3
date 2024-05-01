#pragma once
#include "core/core.hpp"

namespace game
{
  struct SceneEntity
  {
    core::data::StringId mesh;
    core::data::StringId textureDiffuse;
    Mat4                 transform; // TODO: make class Transform in core::math
  };


  struct SceneCollection
  {
    std::map<std::string, std::vector<SceneEntity>> scenes;

    void load( const std::string& name );
    void unload( const std::string& name );
    void unloadAll();
  };
} // namespace game
