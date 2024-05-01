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

    core::math::SphereStraightRotation rotation_;
    Vec3                               position_         = { 0, 0, 0 };
    Mat4                               cubeTransform_    = Mat4( 1 );
    Vec3                               cubeRotationAxis_ = Vec3( 1, 0, 0 );

    // TODO: may be load here some basic resources
    Status init() override { return StatusOk; }

    void update() override
    {
      {
        using namespace core::input;

        // cube rotation
        {
          if( isKeyPressed( KeyX ) )
          {
            cubeRotationAxis_ = Vec3( 1, 0, 0 );
            cubeTransform_    = Mat4( 1 );
          }
          else if( isKeyPressed( KeyY ) )
          {
            cubeRotationAxis_ = Vec3( 0, 1, 0 );
            cubeTransform_    = Mat4( 1 );
          }
          else if( isKeyPressed( KeyZ ) )
          {
            cubeRotationAxis_ = Vec3( 0, 0, 1 );
            cubeTransform_    = Mat4( 1 );
          }

          f32 rotateFactor = core::loopGetDeltaTime().getMsF() * 0.002f;
          cubeTransform_ *= glm::rotate( rotateFactor, cubeRotationAxis_ );
        }

        // rotation
        {
          auto delta = Vec2( 0 );

          if( isKeyPressed( KeyUp ) && !isKeyPressed( KeyDown ) )
            delta.y = 1;
          else if( !isKeyPressed( KeyUp ) && isKeyPressed( KeyDown ) )
            delta.y = -1;

          if( isKeyPressed( KeyLeft ) && !isKeyPressed( KeyRight ) )
            delta.x = -1;
          else if( !isKeyPressed( KeyLeft ) && isKeyPressed( KeyRight ) )
            delta.x = 1;

          f32 rotateFactor = core::loopGetDeltaTime().getMsF() * 0.002f;
          delta *= rotateFactor;

          // TODO: this is messed up: delta.x should be positive...
          rotation_.rotate( -delta.x, delta.y );
        }

        // translation
        {
          f32 deltaForward = 0;
          f32 deltaRight   = 0;

          if( isKeyPressed( KeyA ) && !isKeyPressed( KeyD ) )
            deltaRight = -1;
          else if( !isKeyPressed( KeyA ) && isKeyPressed( KeyD ) )
            deltaRight = 1;

          if( isKeyPressed( KeyW ) && !isKeyPressed( KeyS ) )
            deltaForward = 1;
          else if( !isKeyPressed( KeyW ) && isKeyPressed( KeyS ) )
            deltaForward = -1;

          f32 moveFactor = core::loopGetDeltaTime().getMsF() * 0.01f;
          position_ += deltaForward * moveFactor * rotation_.getForward();

          // TODO: this is messed up: KeyA should add positive to deltaRight but it moves left...
          position_ += deltaRight * moveFactor * rotation_.getRight();
        }
      }

      core::render::RenderList renderList;

      core::math::Camera camera;
      camera.aspectRatio = core::render::gapi::gDevice->viewport.fSize.x /
                           core::render::gapi::gDevice->viewport.fSize.y;
      camera.position  = position_;
      camera.direction = rotation_.getForward();

      renderList.viewPosition              = camera.position;
      renderList.viewToProjectionTransform = camera.getViewToProjectionTransform();
      renderList.worldToViewTransform      = camera.getWorldToViewTransform();

      for( const auto& [name, scene]: sceneCollection_.scenes )
      {
        for( const auto& entity: scene )
        {
          if( entity.mesh == core::data::StringId( "Cube" ) )
            renderList.addMesh( entity.mesh, entity.textureDiffuse, entity.transform * cubeTransform_ );
          else
            renderList.addMesh( entity.mesh, entity.textureDiffuse, entity.transform );
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
