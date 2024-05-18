#include "render-chunk/render-chunk.hpp"
#include "render-chunk/image.hpp"
#include "render-chunk/mesh.hpp"

using namespace intermediate;


core::data::schema::Chunk intermediate::processRenderChunk( const SceneInfo& sceneInfo )
{
  auto chunk = core::data::schema::Chunk();

  for( const auto& object: sceneInfo.objects )
  {
    if( !object.mesh.has_value() )
      continue;

    auto& mesh = object.mesh.value();
    intermediate::processMesh( object.name, mesh, chunk );
  }

  intermediate::processMaterials( sceneInfo, chunk );

  return chunk;
}
