#include "game/subsys/subsys.hpp"
#include "game/tasks/tasks.hpp"
#include "game/components/components.hpp"

using namespace game;

namespace
{
  struct SceneInfo
  {
    core::Scene                                scene;
    std::vector<core::data::RenderChunkHandle> renderChunkHandles;
  };


  struct SceneManagerImpl : public SceneManager
  {
    std::list<SceneInfo> scenes_;

    // TODO: may be load here some basic resources
    Status init() override
    {
      // TODO: this is very dirty hack....
      {
        auto& sceneInfo    = scenes_.emplace_back( SceneInfo{ .scene = core::Scene{ "camera"_sid } } );
        auto* cameraEntity = sceneInfo.scene.addEntity( "camera"_sid );
        cameraEntity->addComponent<FreeFlyCameraComponent>();
        sceneInfo.scene.init();
      }

      return StatusOk;
    }

    void update() override
    {
      for( auto& sceneInfo: scenes_ )
      {
        sceneInfo.scene.update( core::loopGetDeltaTime() );
      }
    }

    void shutdown() override
    {
      for( auto& sceneInfo: scenes_ )
      {
        sceneInfo.scene.shutdown();
      }
    }

    void loadScene( const char* name ) override
    {
      auto* scene = &scenes_.emplace_back( StringId( name ) );

      tasks::loadScene( name, [scene]( game::SceneInfo sceneInfo, core::data::RenderChunkHandle renderChunk ) {
        scene->renderChunkHandles = { std::move( renderChunk ) };

        for( const auto& object: sceneInfo.objects )
        {
          auto* entity = scene->scene.addEntity( StringId( object.name ) );
          instantiateComponents( *entity, object, scene->renderChunkHandles[0] );
        }

        scene->scene.init();
      } );
    }

    void unloadScene( const char* name ) override
    {
      auto sceneId = StringId( name );
      core::loopEnqueueDefferedTask( [this, sceneId]() {
        scenes_.remove_if( [=]( SceneInfo& scene ) {
          return scene.scene.getId() == sceneId;
        } );
      } );
    }

  private:
  };
} // namespace


SceneManager* gMakeSceneManager() { return new SceneManagerImpl(); }
