#include "game/subsys/subsys.hpp"
#include "game/data/data.hpp"

using namespace game;


namespace
{
  struct CommandManagerImpl : public CommandManager
  {
    Status init() override { return StatusOk; }
    void   update() override {}
    void   shutdown() override {}

    void runCommand( const char* cmd ) override
    {
      ( void ) cmd;
    }
  };


  struct DebugConsoleImpl : public DebugConsole
  {
    Status init() override { return StatusOk; }
    void   update() override {}
    void   shutdown() override {}

    void setShown( bool shown ) override
    {
      ( void ) shown;
    }
  };


  struct UiManagerImpl : public UiManager
  {
    Status init() override { return StatusOk; }
    void   update() override {}
    void   shutdown() override {}
  };


  struct SceneManagerImpl : public SceneManager
  {
    SceneCollection sceneCollection_;

    Vec3 camDirection = { 0, 1, 0 };

    // TODO: may be load here some basic resources
    Status init() override { return StatusOk; }

    void update() override
    {
      {
        f32 diff = core::loopGetDeltaTime().getMsF() * 0.001f;

        using namespace core::input;

        auto changeOnKey = [diff]( f32& v, Key a, Key b ) {
          if( isKeyPressed( a ) )
            v += diff;
          else if( isKeyPressed( b ) )
            v -= diff;
        };

        changeOnKey( camDirection.x, KeyQ, KeyA );
        changeOnKey( camDirection.y, KeyW, KeyS );
        changeOnKey( camDirection.z, KeyE, KeyD );

        camDirection = glm::normalize( camDirection );
        mCoreLog( "camDirection: " mFmtVec3 "\n", mFmtVec3Value( camDirection ) );
      }

      core::render::RenderList renderList;

      core::render::Camera camera;
      camera.aspectRatio = core::render::gapi::gDevice->viewport.fSize.x /
                           core::render::gapi::gDevice->viewport.fSize.y;
      camera.position  = { 1, 0, -3 };
      camera.direction = camDirection;

      renderList.viewPosition              = camera.position;
      renderList.viewToProjectionTransform = camera.getViewToProjectionTransform();
      renderList.worldToViewTransform      = camera.getWorldToViewTransform();

      for( const auto& [name, scene]: sceneCollection_.scenes )
      {
        for( const auto& entity: scene )
        {
          renderList.addMesh( entity.mesh, entity.textureDiffuse );
        }
      }

      renderList.submit();
    }

    void shutdown() override
    {
      sceneCollection_.unloadAll();
    }

    void loadScene( const char* name ) override
    {
      sceneCollection_.load( name );
    }
  };
} // namespace


#define xSubSystems( X ) \
  X( CommandManager )    \
  X( DebugConsole )      \
  X( SceneManager )      \
  X( UiManager )


#define mInitSubsystem( Name )                              \
  mCoreLog( "initializing game subsystem " #Name "...\n" ); \
  Name::i = new Name##Impl();                               \
  mCoreCheckStatus( Name::i->init() );                      \
  mCoreLog( "  game subsystem " #Name " is ready\n" );


#define mUpdateSubsystem( Name ) \
  Name::i->update();


#define mShutdownSubsystem( Name ) \
  Name::i->shutdown();             \
  delete Name::i;


Status game::subsysInit()
{
  xSubSystems( mInitSubsystem );
  return StatusOk;
}


void game::subsysUpdate()
{
  xSubSystems( mUpdateSubsystem );
}


void game::subsysShutdown()
{
  // TODO (may be inverse order?)
  xSubSystems( mShutdownSubsystem );
}
