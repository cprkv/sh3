#pragma once
#include "core/core.hpp"

namespace game
{
  struct ShComponent
  {
    StringHash     type;
    Json::object_t data;
  };

  struct ShObjectInfo
  {
    std::string              name;
    std::vector<ShComponent> components;
  };

  struct SceneInfo
  {
    std::vector<ShObjectInfo> objects;
    std::vector<std::string>  render_chunks;
  };

  Status parseJsonFile( std::string path, SceneInfo& output );
} // namespace game
