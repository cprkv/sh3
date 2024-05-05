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
    bool                                       isLoading = false;
  };


  struct RemoveSceneComponent : core::Component
  {
    mCoreComponent( RemoveSceneComponent );

    void update( const core::system::DeltaTime& ) override
    {
      using namespace core::input;

      if( isKeyDown( Key1 ) )
      {
        SceneManager::i->loadScene( "X0/MR1F-MFA/mr1f-pp" );
      }

      if( isKeyDown( Key0 ) )
      {
        SceneManager::i->unloadScene( "X0/MR1F-MFA/mr1f-pp" );
      }
    }
  };


  struct SceneManagerImpl : public SceneManager
  {
    std::list<SceneInfo> scenes_;

    // TODO: may be load here some basic resources
    Status init() override
    {
      // TODO: this is very dirty hack....
      {
        auto& sceneInfo = scenes_.emplace_back( SceneInfo{ .scene = core::Scene{ "camera"_sid } } );

        auto* cameraEntity = sceneInfo.scene.addEntity( "camera"_sid );
        cameraEntity->addComponent<FreeFlyCameraComponent>();

        auto* removeSceneComponent = sceneInfo.scene.addEntity( "removeScene"_sid );
        removeSceneComponent->addComponent<RemoveSceneComponent>();

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
      auto sceneId = StringId( name );

      auto it = std::find_if( scenes_.begin(), scenes_.end(), [=]( SceneInfo& scene ) {
        return scene.scene.getId() == sceneId;
      } );
      if( it != scenes_.end() )
      {
        mCoreLogError( "trying to load scene " mFmtU64 " which is already loaded\n", sceneId );
        return;
      }

      mCoreLog( "loading scene " mFmtU64 "...\n", sceneId.getHash() );
      auto* scene      = &scenes_.emplace_back( sceneId );
      scene->isLoading = true;

      tasks::loadScene( name, [scene]( game::SceneInfo sceneInfo, core::data::RenderChunkHandle renderChunk ) {
        scene->renderChunkHandles = { std::move( renderChunk ) };

        for( const auto& object: sceneInfo.objects )
        {
          auto* entity = scene->scene.addEntity( StringId( object.name ) );
          instantiateComponents( *entity, object, scene->renderChunkHandles[0] );
        }

        scene->scene.init();
        scene->isLoading = false;
      } );
    }

    void unloadScene( const char* name ) override
    {
      auto sceneId = StringId( name );

      core::loopEnqueueDefferedTask( [this, sceneId]() {
        auto it = std::find_if( scenes_.begin(), scenes_.end(), [=]( SceneInfo& scene ) {
          return scene.scene.getId() == sceneId;
        } );

        if( it == scenes_.end() )
        {
          mCoreLogError( "trying to unload scene " mFmtU64 " which is not loaded\n", sceneId.getHash() );
          return;
        }

        if( it->isLoading )
        {
          mCoreLogError( "trying to unload scene " mFmtU64 " which is loading\n", sceneId.getHash() );
          return;
        }

        mCoreLog( "unloading scene " mFmtU64 "...\n", sceneId.getHash() );
        it->scene.shutdown();
        scenes_.erase( it );
      } );
    }

  private:
  };
} // namespace


SceneManager* gMakeSceneManager() { return new SceneManagerImpl(); }
