#pragma once
#include "core/common.hpp"

namespace core::data
{
  struct ShComponent
  {
    StringHash type;
    Json       data;
  };

  struct ShObjectInfo
  {
    StringHash               id;
    std::vector<ShComponent> components;
  };

  struct ShSceneInfo
  {
    std::vector<ShObjectInfo> objects;
    std::vector<StringHash>   render_chunks;
  };

  Status readJsonFile( const std::string& path, ShSceneInfo& output );
  Status writeJsonFile( const std::string& path, const ShSceneInfo& data );
} // namespace core::data
