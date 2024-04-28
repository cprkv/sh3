#pragma once
#include "core/core.hpp"
#include "game/data/data.hpp"

namespace game::tasks
{
  void loadScene( const std::string& name, std::vector<SceneEntity>& entities );
}
