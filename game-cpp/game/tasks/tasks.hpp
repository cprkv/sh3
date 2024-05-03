#pragma once
#include "core/core.hpp"
#include "game/data/serialization.hpp"

namespace game::tasks
{
  void loadScene( const std::string& name, std::function<void( SceneInfo )> action );
} // namespace game::tasks
