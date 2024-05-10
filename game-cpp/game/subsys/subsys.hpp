#pragma once
#include "core/core.hpp"

namespace game
{
  struct SubSystem
  {
    virtual ~SubSystem() {}
    virtual Status init() { return StatusOk; }
    virtual void   update() {}
    virtual void   shutdown() {}
  };


  struct CommandManager : public SubSystem // virtual?
  {
    static inline CommandManager* i = nullptr;

    // "load-scene ..." -> SceneSubSystem::instance->loadScene("...")
    virtual void runCommand( const char* cmd ) = 0;
  };


  struct DebugConsole : public SubSystem
  {
    static inline DebugConsole* i = nullptr;

    virtual void setShown( bool shown ) = 0;
  };


  struct UiManager : public SubSystem
  {
    static inline UiManager* i = nullptr;
  };


  Status subsysInit();
  void   subsysUpdate();
  void   subsysShutdown();

} // namespace game
