#include "core/common.hpp"
#include "core/fs/file.hpp"
#include "core/data/data.hpp"
#include "schema.hpp"
#include "image.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include <meshoptimizer.h>
#pragma GCC diagnostic pop

#include <iostream>


namespace
{
  void processMesh( const intermediate::MeshInfo& meshInfo,
                    core::data::schema::Chunk&    chunk )
  {
#if 1
    size_t indexCount           = meshInfo.vertex_data.size();
    size_t unindexedVertexCount = meshInfo.vertex_data.size();

#  if 0 
    const auto& unindexedVertices    = meshInfo.vertex_data;
#  else
    // change order of vertices (from opengl to directx)
    assert( meshInfo.vertex_data.size() % 3 == 0 );
    auto unindexedVertices = std::vector<core::data::schema::VertexData>( meshInfo.vertex_data.size() );
    for( size_t i = 0; i < meshInfo.vertex_data.size(); i += 3 )
    {
      unindexedVertices[0 + i] = meshInfo.vertex_data[0 + i];
      unindexedVertices[1 + i] = meshInfo.vertex_data[2 + i];
      unindexedVertices[2 + i] = meshInfo.vertex_data[1 + i];
    }
#  endif

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
#else
    auto vertices = meshInfo.vertex_data;
    auto indices  = std::vector<u32>( vertices.size() );
    for( size_t i = 0; i < indices.size(); i++ )
      indices[i] = ( u32 ) i;
#endif

    auto outputMesh = core::data::schema::Mesh{
        .id           = StringId( meshInfo.name ),
        .indexBuffer  = std::move( indices ),
        .vertexBuffer = std::move( vertices ),
    };
    chunk.meshes.emplace_back( std::move( outputMesh ) );
  }


  struct FileWriter
  {
    core::fs::File file;
    bool           hasError = false;

    FileWriter( const char* path )
        : file( path, "wb" )
    {
      hasError = !file.isOpen();
    }

    void write( const char* data, size_t size )
    {
      if( hasError )
        return;

      auto s = file.write( data, size );
      if( s != StatusOk )
      {
        hasError = true;
      }
    }
  };


  FileWriter openSceneOuputFile( const intermediate::SceneInfo& sceneInfo )
  {
    auto relativeScenePath = stdfs::path( sceneInfo.path.path );
    stdfs::create_directories( core::data::getDataPath(
        relativeScenePath.parent_path().generic_string().c_str() ) );

    auto outputPath     = relativeScenePath.replace_extension( ".chunk" ).generic_string();
    auto outputFullPath = core::data::getDataPath( outputPath.c_str() );

    auto outputFile = FileWriter( outputFullPath.c_str() );
    if( outputFile.hasError )
    {
      printf( "error: bad output file: %s\n", outputFullPath.c_str() );
      abort();
    }

    printf( "writing file %s...\n", outputFullPath.c_str() );
    return outputFile;
  }
} // namespace


int main( int argc, char** argv )
try
{
  printf( "mesh tool started\n" );

// TODO: write tests
#if 0
  printf( "string id for \"\" = " mFmtStringHash "\n",
          core::data::StringId( "" ).getHash() );
  printf( "string id for \"hallo wrodl!\" = " mFmtStringHash "\n",
          core::data::StringId( "hallo wrodl!" ).getHash() );
  printf( "string id for \"mr1f_8_0_0_0_0x00071df0_f0\" = " mFmtStringHash "\n",
          core::data::StringId( "mr1f_8_0_0_0_0x00071df0_f0" ).getHash() );
  printf( "string id for \"X0/MR1F-MFA/mr1f.map-i2\" = " mFmtStringHash "\n",
          core::data::StringId( "X0/MR1F-MFA/mr1f.map-i2" ).getHash() );
  printf( "string id for \"X0/MR1F-MFA/mr1f-pp\" = " mFmtStringHash "\n",
          core::data::StringId( "X0/MR1F-MFA/mr1f-pp" ).getHash() );
  printf( "string id for \"X0/MR1F-MFA/mr2f-pp\" = " mFmtStringHash "\n",
          core::data::StringId( "X0/MR1F-MFA/mr2f-pp" ).getHash() );
  printf( "string id for \"X0/MR1F-MFA/mr2e-pp\" = " mFmtStringHash "\n",
          core::data::StringId( "X0/MR1F-MFA/mr2e-pp" ).getHash() );
#endif

  if( auto s = core::data::initialize(); s != StatusOk )
  {
    printf( "data initialize failed\n" );
    return 1;
  }

  if( argc != 2 )
  {
    printf( "usage: mesh-tool.exe [path-to-temp-mesh]\n" );
    return 1;
  }

  auto inputData = std::vector<byte>();
  if( core::fs::readFile( argv[1], inputData ) != StatusOk )
  {
    printf( "error: bad file\n" );
    return 1;
  }

  msgpack::object_handle objectHandle = msgpack::unpack(
      reinterpret_cast<char*>( inputData.data() ),
      inputData.size() );

  msgpack::object obj = objectHandle.get();

  auto sceneInfo = intermediate::SceneInfo();
  obj.convert( sceneInfo );
  printf( "mesh tool parsed input file...\n" );

  auto chunk = core::data::schema::Chunk();

  for( const auto& mesh: sceneInfo.meshes )
  {
    printf( "processing mesh %s\n", mesh.name.c_str() );
    processMesh( mesh, chunk );
  }

  intermediate::processMaterials( sceneInfo, chunk );

  {
    auto outputFile = openSceneOuputFile( sceneInfo );
    msgpack::pack( outputFile, chunk );
  }

  printf( "file written\n" );

  core::data::destroy();
  printf( "mesh tool ended\n" );
  return 0;
}
catch( const msgpack::type_error& ex )
{
  printf( "mesh tool ended with msgpack type error: %s\n", ex.what() );
  return 1;
}
catch( std::exception& ex )
{
  printf( "mesh tool ended with exception: %s\n", ex.what() );
  return 1;
}
