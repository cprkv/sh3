#pragma once
#include "core/common.hpp"
#include "core/data/schema.hpp"


namespace intermediate
{
  struct PathInfo
  {
    std::string base;
    std::string full;
    std::string path;

    MSGPACK_DEFINE_MAP( base, full, path );
  };

  struct TextureInfo
  {
    PathInfo                  path;
    core::data::schema::Vec2i size;

    MSGPACK_DEFINE_MAP( path, size );
  };

  struct MaterialInfo
  {
    std::string name;
    TextureInfo diffuse;

    MSGPACK_DEFINE_MAP( name, diffuse );
  };

  struct MeshInfo
  {
    std::string                                 name;
    MaterialInfo                                material_info;
    std::vector<core::data::schema::VertexData> vertex_data;

    MSGPACK_DEFINE_MAP( name, material_info, vertex_data );
  };

  struct SceneInfo
  {
    PathInfo              path;
    std::vector<MeshInfo> meshes;

    MSGPACK_DEFINE_MAP( path, meshes );
  };
} // namespace intermediate
