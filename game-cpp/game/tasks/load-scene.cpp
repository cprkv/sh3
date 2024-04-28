#include "game/tasks/tasks.hpp"
#include "game/data/serialization.hpp"

using namespace game;


namespace
{
  struct LoadSceneJsonTask
  {
    std::optional<Status> status = std::nullopt;
    SceneInfo             result;
  };
} // namespace


void game::tasks::loadScene( const std::string& name, std::vector<SceneEntity>& entities )
{
  // TODO: more logic about what scene looks like: is it just bunch of stuff like weapons collection or is it real 3D scene..

  auto chunkPath     = core::data::getDataPath( ( name + ".chunk" ).c_str() );
  auto sceneJsonPath = core::data::getDataPath( ( name + ".scene.json" ).c_str() );

  auto loadTask = std::make_unique<core::data::LoadRenderChunkTask>();
  core::data::loadRenderChunk( chunkPath.c_str(), loadTask.get() );

  auto loadSceneInfoTask = std::make_shared<LoadSceneJsonTask>();


  core::loopEnqueueTask( [sceneJsonPath, loadSceneInfoTask]() mutable {
    std::vector<u8> sceneJsonBytes;
    if( auto s = core::fs::readFile( sceneJsonPath, sceneJsonBytes ); s != StatusOk )
    {
      auto currentPath = stdfs::current_path().string();
      mCoreLogError( "error loading scene: can't read scene json file\ncurrent path was: %s\n", currentPath.c_str() );
      core::loopEnqueueDefferedTask( [loadSceneInfoTask = std::move( loadSceneInfoTask )]() {
        loadSceneInfoTask->status = StatusSystemError;
      } );
      return;
    }

    auto sceneJsonString = std::string_view{
        reinterpret_cast<const char*>( sceneJsonBytes.data() ),
        reinterpret_cast<const char*>( sceneJsonBytes.data() ) + sceneJsonBytes.size() };

    SceneInfo sceneInfo;
    if( auto s = parseJson( sceneJsonString, sceneInfo ); s != StatusOk )
    {
      core::loopEnqueueDefferedTask( [loadSceneInfoTask = std::move( loadSceneInfoTask )]() {
        loadSceneInfoTask->status = StatusSystemError;
      } );
      return;
    }

    core::loopEnqueueDefferedTask( [loadSceneInfoTask = std::move( loadSceneInfoTask ),
                                    sceneInfo         = std::move( sceneInfo )]() mutable {
      loadSceneInfoTask->result = std::move( sceneInfo );
      loadSceneInfoTask->status = StatusOk;
    } );
  } );


  auto task = [loadTask = std::move( loadTask ), loadSceneInfoTask, &entities]() mutable -> core::PeriodicalStatus {
    if( !loadTask->done || !loadSceneInfoTask->status.has_value() )
      return core::PeriodicalStatusContinue;

    if( loadTask->status != StatusOk )
    {
      mCoreLogError( "error loading chunk: %d %s\n", ( int ) loadTask->status,
                     core::getErrorDetails() );
      return core::PeriodicalStatusStop;
    }

    if( loadSceneInfoTask->status != StatusOk )
    {
      mCoreLogError( "error loading scene json: %d %s\n", ( int ) *loadSceneInfoTask->status,
                     core::getErrorDetails() );
      return core::PeriodicalStatusStop;
    }

    for( const auto& obj: loadSceneInfoTask->result.objects )
    {
      SceneEntity entity;

      for( const auto& component: obj.components )
      {
        if( component->getType() == ShComponentType_Mesh )
        {
          const auto* typedComponent = reinterpret_cast<const ShComponentMesh*>( component.get() );
          entity.mesh                = typedComponent->id;
        }
        else if( component->getType() == ShComponentType_Material )
        {
          const auto* typedComponent = reinterpret_cast<const ShComponentMaterial*>( component.get() );
          entity.textureDiffuse      = typedComponent->diffuse;
        }
      }

      if( entity.mesh && entity.textureDiffuse )
      {
        entities.push_back( entity );
      }
      else
      {
        mCoreLog( "warn: found entity with no mesh and texture" );
      }
    }

    mCoreLog( "loading scene done! entities count: %d\n", ( int ) entities.size() );
    return core::PeriodicalStatusStop;
  };

  core::loopEnqueuePeriodicalTask( std::move( task ) );
}
