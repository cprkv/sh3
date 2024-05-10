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
} // namespace


static auto* gMakeCommandManager() { return new CommandManagerImpl(); }
static auto* gMakeDebugConsole() { return new DebugConsoleImpl(); }
static auto* gMakeUiManager() { return new UiManagerImpl(); }


#define xSubSystems( X ) \
  X( CommandManager )    \
  X( DebugConsole )      \
  X( UiManager )


#define mInitSubsystem( Name )                              \
  mCoreLog( "initializing game subsystem " #Name "...\n" ); \
  Name::i = gMake##Name();                                  \
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
