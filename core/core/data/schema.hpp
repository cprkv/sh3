#pragma once
#include "core/common.hpp"

namespace core::data::schema
{
  // TODO: intrusive Vec serialization as in scene-tool

  struct Vec2i
  {
    int x;
    int y;

    MSGPACK_DEFINE( x, y );
  };

  struct Vec2f
  {
    float x;
    float y;

    MSGPACK_DEFINE( x, y );
  };

  struct Vec3f
  {
    float x;
    float y;
    float z;

    MSGPACK_DEFINE( x, y, z );
  };

  struct Vec4f
  {
    float x;
    float y;
    float z;
    float w;

    MSGPACK_DEFINE( x, y, z, w );
  };

  struct VertexData
  {
    Vec3f position;
    Vec3f normal;
    Vec2f uv;

    MSGPACK_DEFINE( position, normal, uv );
  };

  static_assert( sizeof( VertexData ) == 8 * sizeof( float ) );

  struct Mesh
  {
    u64                     id;
    std::vector<u32>        indexBuffer;
    std::vector<VertexData> vertexBuffer;

    MSGPACK_DEFINE( id, vertexBuffer, indexBuffer );
  };

  struct TextureData
  {
    u32               memPitch;
    u32               memSlicePitch;
    std::vector<byte> mem;

    MSGPACK_DEFINE( memPitch, memSlicePitch, mem );
  };

  struct Texture
  {
    u64                      id;
    u32                      width;
    u32                      height;
    u32                      mipLevels;
    u32                      arraySize;
    u32                      format; // DXGI FORMAT
    std::vector<TextureData> data;

    MSGPACK_DEFINE( id,
                    width,
                    height,
                    mipLevels,
                    arraySize,
                    format,
                    data );
  };

  struct Chunk
  {
    std::vector<Mesh>    meshes;
    std::vector<Texture> textures;

    MSGPACK_DEFINE( meshes, textures );
  };
} // namespace core::data::schema
