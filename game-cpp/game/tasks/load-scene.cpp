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
  auto sceneJsonPath     = core::data::getDataPath( ( name + ".scene.json" ).c_str() );
  auto loadSceneInfoTask = std::make_shared<LoadSceneJsonTask>();

  core::loopEnqueueTask( [sceneJsonPath, loadSceneInfoTask]() mutable {
    auto sceneInfo = SceneInfo();
    auto status    = parseJsonFile( sceneJsonPath, sceneInfo );

    core::loopEnqueueDefferedTask( [loadSceneInfoTask = std::move( loadSceneInfoTask ),
                                    sceneInfo         = std::move( sceneInfo ),
                                    status]() mutable {
      loadSceneInfoTask->result = std::move( sceneInfo );
      loadSceneInfoTask->status = status;
    } );
  } );


  auto task = [renderChunks = std::vector<core::data::RenderChunkHandle>(),
               action       = std::move( action ),
               loadSceneInfoTask]() mutable -> core::PeriodicalStatus {
    // wait json is loaded
    if( !loadSceneInfoTask->status.has_value() )
      return core::PeriodicalStatusContinue;

    if( loadSceneInfoTask->status != StatusOk )
    {
      mCoreLogError( "error loading scene json: %d %s\n", ( int ) *loadSceneInfoTask->status, core::getErrorDetails() );
      return core::PeriodicalStatusStop;
    }

    // queue load render chunks
    if( renderChunks.size() != loadSceneInfoTask->result.render_chunks.size() )
    {
      renderChunks.resize( loadSceneInfoTask->result.render_chunks.size() );
      for( size_t i = 0; i < renderChunks.size(); ++i )
        renderChunks[i].load( loadSceneInfoTask->result.render_chunks[i].c_str() );
    }

    // wait until render chunks ready
    if( renderChunks.size() )
    {
      auto allLoaded = std::all_of( renderChunks.begin(), renderChunks.end(),
                                    []( const auto& chunk ) { return chunk.isLoaded(); } );
      if( !allLoaded )
        return core::PeriodicalStatusContinue;
    }

    action( std::move( loadSceneInfoTask->result ),
            std::move( renderChunks ) );

    mCoreLog( "loading scene task done!\n" );
    return core::PeriodicalStatusStop;
  };

  core::loopEnqueuePeriodicalTask( std::move( task ) );
}
