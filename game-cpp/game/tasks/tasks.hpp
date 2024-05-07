#pragma once
#include "core/core.hpp"
#include "game/data/serialization.hpp"

namespace game::tasks
{
  using LoadSceneAction = std::function<void( game::SceneInfo, core::data::RenderChunks )>;

  void loadScene( const std::string& name, LoadSceneAction action );
} // namespace game::tasks
