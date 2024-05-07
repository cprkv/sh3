#pragma once
#include "core/common.hpp"
#include "core/data/ref-collection.hpp"
#include "core/render/data.hpp"

namespace core::data
{
  Status initializeRenderChunk();
  void   destroyRenderChunk();
  void   updateRenderChunk();


  class RenderChunk
  {
    RefCounter              ref_;
    struct RenderChunkData* data_ = nullptr;

  public:
    bool isLoaded() const;
    void load( const char* name );

    core::render::Material* getMaterial( StringId id );
    core::render::Mesh*     getMesh( StringId id );
    core::render::Texture*  getTexture( StringId id );
  };

  using RenderChunks     = std::vector<data::RenderChunk>;
  using RenderChunksView = std::span<data::RenderChunk>;

} // namespace core::data
