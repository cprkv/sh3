#pragma once
#include "core/core.hpp"
#include "schema.hpp"

namespace intermediate
{
  inline u64 textureHash( const TextureInfo& textureInfo )
  {
    auto path = stdfs::path( textureInfo.path ).lexically_normal().replace_extension( "" ).generic_string();
    auto hash = StringId( path );
    printf( "texture: %s hash: " mFmtU64 "\n", path.c_str(), hash.getHash() );
    return hash;
  }

  struct ITextureCollection
  {
    virtual ~ITextureCollection() = default;

    // once per process
    virtual void addTexture( const TextureInfo& textureInfo, const std::string& imageUsage ) = 0;
    virtual void process()                                                                   = 0;

    // per scene
    virtual void resolve( const SceneInfo& sceneInfo, core::data::schema::Chunk& outputChunk ) const = 0;
  };

  std::unique_ptr<ITextureCollection> makeTextureCollection();
} // namespace intermediate
