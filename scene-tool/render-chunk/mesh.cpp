#include "render-chunk/mesh.hpp"
#include "scene-tool.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <meshoptimizer.h>
#pragma GCC diagnostic pop

using namespace intermediate;


namespace
{
  // changes order of vertices (from opengl to directx)
  std::vector<core::data::schema::VertexData> openGlVerticesToDirectX(
      const std::vector<core::data::schema::VertexData>& vertices )
  {
    auto result = std::vector<core::data::schema::VertexData>( vertices.size() );
    for( size_t i = 0; i < vertices.size(); i += 3 )
    {
      result[0 + i] = vertices[0 + i];
      result[1 + i] = vertices[2 + i];
      result[2 + i] = vertices[1 + i];
    }
    return result;
  }
} // namespace


void intermediate::processMesh( const std::string&            name,
                                const intermediate::MeshInfo& meshInfo,
                                core::data::schema::Chunk&    chunk )
{
  printf( "processing mesh %s\n", name.c_str() );

  bool verticesIsCountOf3 = ( meshInfo.vertex_data.size() % 3 ) == 0;
  mFailIf( !verticesIsCountOf3 );

  auto   unindexedVertices    = openGlVerticesToDirectX( meshInfo.vertex_data );
  size_t indexCount           = unindexedVertices.size();
  size_t unindexedVertexCount = unindexedVertices.size();

  auto   remap       = std::vector<u32>( indexCount );
  size_t vertexCount = meshopt_generateVertexRemap(
      remap.data(),
      nullptr,
      indexCount,
      unindexedVertices.data(),
      unindexedVertexCount,
      sizeof( unindexedVertices[0] ) );

  auto indices  = std::vector<u32>( indexCount );
  auto vertices = std::vector<core::data::schema::VertexData>( vertexCount );

  meshopt_remapIndexBuffer( indices.data(),
                            nullptr,
                            indexCount,
                            remap.data() );

  meshopt_remapVertexBuffer( vertices.data(),
                             unindexedVertices.data(),
                             unindexedVertexCount,
                             sizeof( unindexedVertices[0] ),
                             remap.data() );

  meshopt_optimizeVertexCache( indices.data(), indices.data(),
                               indexCount, vertexCount );

  auto outputMesh = core::data::schema::Mesh{
      .id           = StringId( name ), // TODO: incorrect if obj has more than 1 meshes
      .indexBuffer  = std::move( indices ),
      .vertexBuffer = std::move( vertices ),
  };
  chunk.meshes.emplace_back( std::move( outputMesh ) );
}
