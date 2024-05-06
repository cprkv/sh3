#pragma once
#include "core/core.hpp"
#include "game/data/serialization.hpp"

namespace game::tasks
{
  using LoadSceneAction = std::function<void( game::SceneInfo, std::vector<core::data::RenderChunkHandle> )>;

  void loadScene( const std::string& name, LoadSceneAction action );
} // namespace game::tasks
