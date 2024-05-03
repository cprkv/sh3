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


void game::tasks::loadScene( const std::string& name, std::function<void( SceneInfo )> action )
{
  auto chunkPath     = core::data::getDataPath( ( name + ".chunk" ).c_str() );
  auto sceneJsonPath = core::data::getDataPath( ( name + ".scene.json" ).c_str() );

  auto loadTask = std::make_unique<core::data::LoadRenderChunkTask>();
  core::data::loadRenderChunk( chunkPath.c_str(), loadTask.get() );

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


  auto task = [loadTask = std::move( loadTask ),
               action   = std::move( action ),
               loadSceneInfoTask]() mutable -> core::PeriodicalStatus {
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

    // TODO: check everything in loadSceneInfoTask->result is actually loaded

    action( std::move( loadSceneInfoTask->result ) );

    mCoreLog( "loading scene task done!\n" );
    return core::PeriodicalStatusStop;
  };

  core::loopEnqueuePeriodicalTask( std::move( task ) );
}
