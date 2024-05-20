#pragma once
#include "core/common.hpp"
#include "core/data/schema.hpp"


namespace intermediate
{
  struct FileWriter
  {
    core::fs::File file;
    bool           hasError = false;

    FileWriter( const char* path );
    void write( const char* data, size_t size );
  };
} // namespace intermediate


// TODO: this is core serialization actually

#define mMsgPackDeclareSerializable( Type )                                                                                  \
  namespace msgpack                                                                                                          \
  {                                                                                                                          \
    MSGPACK_API_VERSION_NAMESPACE( MSGPACK_DEFAULT_API_NS )                                                                  \
    {                                                                                                                        \
      namespace adaptor                                                                                                      \
      {                                                                                                                      \
        template<>                                                                                                           \
        struct convert<Type>                                                                                                 \
        {                                                                                                                    \
          const msgpack::object& operator()( const msgpack::object& o, Type& v ) const;                                      \
        };                                                                                                                   \
                                                                                                                             \
        template<>                                                                                                           \
        struct pack<Type>                                                                                                    \
        {                                                                                                                    \
          packer<intermediate::FileWriter>& operator()( msgpack::packer<intermediate::FileWriter>& o, const Type& v ) const; \
        };                                                                                                                   \
      }                                                                                                                      \
    }                                                                                                                        \
  }


mMsgPackDeclareSerializable( Vec2s );
mMsgPackDeclareSerializable( Vec3 );
mMsgPackDeclareSerializable( Vec4 );
mMsgPackDeclareSerializable( Quat );


namespace intermediate
{
  struct TextureInfo
  {
    std::string path; // relative path
    Vec2s       size;

    MSGPACK_DEFINE_MAP( path, size );
  };

  struct MaterialInfo
  {
    std::string name;
    TextureInfo diffuse;
    std::string blend_mode;

    MSGPACK_DEFINE_MAP( name, diffuse, blend_mode );
  };

  struct MeshInfo
  {
    std::vector<core::data::schema::VertexData> vertex_data;
    MaterialInfo                                material_info;

    MSGPACK_DEFINE_MAP( material_info, vertex_data );
  };

  struct Transform
  {
    Vec3 position;
    Quat rotation;
    Vec3 scale;

    MSGPACK_DEFINE_MAP( position, rotation, scale );
  };

  struct ObjectInfo
  {
    std::string             name;
    std::string             type;
    Transform               transform;
    std::optional<MeshInfo> mesh;

    MSGPACK_DEFINE_MAP( name, type, transform, mesh );
  };

  struct SceneInfo
  {
    std::string             name; // relative path
    std::vector<ObjectInfo> objects;

    MSGPACK_DEFINE_MAP( name, objects );
  };

  struct ScenesInfo
  {
    std::vector<SceneInfo> scenes;

    MSGPACK_DEFINE_MAP( scenes );
  };
} // namespace intermediate
