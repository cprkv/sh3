#pragma once
#include "core/common.hpp"

namespace core::data
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

  struct ShSceneInfo
  {
    std::vector<ShObjectInfo> objects;
    std::vector<std::string>  render_chunks;
  };

  Status parseJsonFile( std::string path, ShSceneInfo& output );
} // namespace game
