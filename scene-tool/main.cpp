#include "core/core.hpp"
#include "scene-tool.hpp"
#include "schema.hpp"
#include "render-chunk/render-chunk.hpp"


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


  void writeRenderChunk( const intermediate::SceneInfo& sceneInfo, const core::data::schema::Chunk& renderChunk )
  {
    auto outputPath = stdfs::path( core::data::getDataPath( sceneInfo.name + ".chunk" ) );
    stdfs::create_directories( outputPath.parent_path() );
    auto outputFile = intermediate::FileWriter( outputPath.string().c_str() );
    mFailIf( outputFile.hasError );
    printf( "writing render chunk %s...\n", outputPath.string().c_str() );
    msgpack::pack( outputFile, renderChunk );
    printf( "render chunk written\n" );
  }


  void runSceneTool( const char* path )
  {
    auto scenesInfo = readSceneInput( path );
    printf( "mesh tool parsed input file...\n" );

    for( const auto& sceneInfo: scenesInfo.scenes )
    {
      auto chunk = intermediate::processRenderChunk( sceneInfo );
      writeRenderChunk( sceneInfo, chunk );
    }
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

  if( argc != 2 )
  {
    printf( "usage: scene-tool.exe [path-to-temp-scene]\n" );
    return 1;
  }

  runSceneTool( argv[1] );

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
