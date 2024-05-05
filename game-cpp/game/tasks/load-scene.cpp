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


void game::tasks::loadScene( const std::string& name, LoadSceneAction action )
{
  auto chunkPath     = core::data::getDataPath( ( name + ".chunk" ).c_str() );
  auto sceneJsonPath = core::data::getDataPath( ( name + ".scene.json" ).c_str() );

  //auto loadTask = std::make_unique<core::data::LoadRenderChunkTask>();
  //core::data::loadRenderChunk( chunkPath.c_str(), loadTask.get() );
  auto renderChunkHandle = core::data::RenderChunkHandle();
  renderChunkHandle.load( chunkPath.c_str() );

  auto loadSceneInfoTask = std::make_shared<LoadSceneJsonTask>();


  core::loopEnqueueTask( [sceneJsonPath, loadSceneInfoTask]() mutable {
    auto sceneInfo = SceneInfo();
    if( auto s = parseJsonFile( sceneJsonPath, sceneInfo ); s != StatusOk )
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


  auto task = [renderChunkHandle = std::move( renderChunkHandle ),
               action            = std::move( action ),
               loadSceneInfoTask]() mutable -> core::PeriodicalStatus {
    if( !renderChunkHandle.isLoaded() || !loadSceneInfoTask->status.has_value() )
    {
      return core::PeriodicalStatusContinue;
    }

    if( loadSceneInfoTask->status != StatusOk )
    {
      mCoreLogError( "error loading scene json: %d %s\n", ( int ) *loadSceneInfoTask->status,
                     core::getErrorDetails() );
      return core::PeriodicalStatusStop;
    }

    action( std::move( loadSceneInfoTask->result ), std::move( renderChunkHandle ) );

    mCoreLog( "loading scene task done!\n" );
    return core::PeriodicalStatusStop;
  };

  core::loopEnqueuePeriodicalTask( std::move( task ) );
}
