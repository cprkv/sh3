#include "game/subsys/subsys.hpp"
#include "game/tasks/tasks.hpp"
#include "game/components/components.hpp"

using namespace game;

namespace
{
  struct SceneManagerImpl : public SceneManager
  {
    std::list<core::Scene> scenes_;

    // TODO: may be load here some basic resources
    Status init() override
    {
      // TODO: this is very dirty hack....
      {
        scenes_.emplace_back( "camera"_sid );
        auto* cameraEntity = scenes_.rbegin()->addEntity( "camera"_sid );
        cameraEntity->addComponent<FreeFlyCameraComponent>();
        scenes_.rbegin()->init();
      }

      return StatusOk;
    }

    void update() override
    {
      for( auto& scene: scenes_ )
      {
        scene.update( core::loopGetDeltaTime() );
      }
    }

    void shutdown() override
    {
      for( auto& scene: scenes_ )
      {
        scene.shutdown();
      }
    }

    void loadScene( const char* name ) override
    {
      scenes_.emplace_back( StringId( name ) );
      auto* scene = &*scenes_.rbegin();

      tasks::loadScene( name, [scene]( SceneInfo sceneInfo ) {
        for( const auto& object: sceneInfo.objects )
        {
          auto* entity = scene->addEntity( StringId( object.name ) );
          instantiateComponents( *entity, object );
        }
        scene->init();
      } );
    }

  private:
  };
} // namespace


SceneManager* gMakeSceneManager() { return new SceneManagerImpl(); }
