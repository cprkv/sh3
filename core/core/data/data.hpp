#pragma once
#include "core/common.hpp"

namespace core::render
{
  struct Material;
  struct Mesh;
  struct Texture;
} // namespace core::render

namespace core::data
{
  std::string        getDataPath( const stdfs::path& resourceName );
  inline std::string getDataPath( const char* resourceName ) { return getDataPath( stdfs::path( resourceName ) ); }

  Status initialize();
  void   destroy();

  struct RenderData
  {
    std::vector<StringHash> materials;
    std::vector<StringHash> meshes;
    std::vector<StringHash> textures;
  };

  struct LoadRenderChunkTask
  {
    Status     status = StatusSystemError;
    bool       done   = false;
    RenderData result;
  };

  // TODO: async api in 3 step way:
  //        1. send task to decode chunk in understandable format
  //        2. spawn bunch of deffered tasks and run them in loop step up to *deadline* constant
  //        3. when all tasks are ready, collect results and return back to callee
  // that means we should have:
  //   - promise based API interface (or may be simpler: return TaskId, and poll it in every loop step until it's ready)
  //   - tread pool (or just other thread for simplicity) for running tasks
  //   - main loop deffered tasks queue with deadline for uploading stuff to gpu or pushing results to API user
  void   loadRenderChunk( const char* name, LoadRenderChunkTask* task );
  Status unloadRenderChunk( const char* name );

  core::render::Material* findMaterial( StringId id );
  core::render::Mesh*     findMesh( StringId id );
  core::render::Texture*  findTexture( StringId id );
} // namespace core::data

// get-set props for materials/objects by id
