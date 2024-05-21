#include "core/core.hpp"
#include "scene-tool.hpp"
#include "schema.hpp"
#include "render-chunk/texture.hpp"
#include "render-chunk/mesh.hpp"
#include "scene/process-scene.hpp"
#include <execution>


namespace
{
  intermediate::ScenesInfo readSceneInput( const char* path )
  {
    auto objectHandle = msgpack::object_handle();
    auto object       = msgpack::object();
    mFailIf( core::fs::readFileMsgpack( path, object, objectHandle ) != StatusOk );
    auto scenesInfo = intermediate::ScenesInfo();
    object.convert( scenesInfo );
    return scenesInfo;
  }


  void writeRenderChunk( const intermediate::SceneInfo&   sceneInfo,
                         const core::data::schema::Chunk& renderChunk,
                         bool                             useCompression )
  {
    auto outputPath = stdfs::path( core::data::getDataPath( sceneInfo.name + ".chunk" ) );
    stdfs::create_directories( outputPath.parent_path() );

    auto memoryWriter = intermediate::VectorWriter();
    printf( "serializing render chunk...\n" );
    msgpack::pack( memoryWriter, renderChunk );

    int compressionLevel = useCompression ? 3 : 1;
    printf( "writing render chunk %s (compression: %d)...\n", outputPath.string().c_str(), compressionLevel );
    mFailIf( core::fs::writeFileCompressed( outputPath.string(), memoryWriter.bytes, compressionLevel ) != StatusOk );
    printf( "render chunk written\n" );
  }


  void writeScene( const intermediate::SceneInfo& intermediateSceneInfo,
                   const core::data::ShSceneInfo& outSceneInfo )
  {
    auto outputPath = stdfs::path( core::data::getDataPath( intermediateSceneInfo.name + ".scene.json" ) );
    stdfs::create_directories( outputPath.parent_path() );
    printf( "writing scene info %s...\n", outputPath.string().c_str() );
    mFailIf( core::data::writeJsonFile( outputPath.string(), outSceneInfo ) != StatusOk );
    printf( "scene info written\n" );
  }


  void runSceneTool( const char* path, bool useCompression )
  {
    auto scenesInfo = readSceneInput( path );
    printf( "mesh tool parsed input file...\n" );

    auto textures = intermediate::makeTextureCollection();

    for( const auto& sceneInfo: scenesInfo.scenes )
      for( const auto& obj: sceneInfo.objects )
        if( obj.mesh )
          textures->addTexture( obj.mesh->material_info.diffuse,
                                obj.mesh->material_info.blend_mode );

    textures->process();

    std::for_each(
        std::execution::par, scenesInfo.scenes.begin(), scenesInfo.scenes.end(),
        [&textures, useCompression]( const auto& sceneInfo ) {
          // handle render chunk
          {
            auto chunk = core::data::schema::Chunk();
            for( const auto& object: sceneInfo.objects )
            {
              if( !object.mesh.has_value() )
                continue;
              auto& mesh = object.mesh.value();
              intermediate::processMesh( object.name, mesh, chunk );
            }
            textures->resolve( sceneInfo, chunk );
            writeRenderChunk( sceneInfo, chunk, useCompression );
          }

          // handle scene
          {
            auto scene          = intermediate::processScene( sceneInfo );
            scene.render_chunks = { StringId( sceneInfo.name + ".chunk" ) };
            writeScene( sceneInfo, scene );
          }
        } );
  }
} // namespace


int main( int argc, char** argv )
try
{
  printf( "scene tool started\n" );

  core::commonInit();

  if( auto s = core::data::initialize(); s != StatusOk )
  {
    printf( "data initialize failed\n" );
    return 1;
  }

  if( argc != 2 && argc != 3 )
  {
    printf( "usage: scene-tool.exe [path-to-temp-scene]\n" );
    return 1;
  }

  bool useCompression = false;
  if( argc == 3 && argv[2] == std::string_view( "-compress" ) )
    useCompression = true;

  runSceneTool( argv[1], useCompression );

  core::data::destroy();
  printf( "scene tool ended\n" );
  return 0;
}
catch( const msgpack::type_error& ex )
{
  printf( "scene tool ended with msgpack type error: %s\n", ex.what() );
  return 1;
}
catch( std::exception& ex )
{
  printf( "scene tool ended with exception: %s\n", ex.what() );
  return 1;
}
