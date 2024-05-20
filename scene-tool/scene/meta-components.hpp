#pragma once
#include "scene-tool.hpp"
#include "schema.hpp"
#include "scene/meta.hpp"
#include "game/components/components.hpp"

namespace intermediate::meta::automatic
{
  core::data::ShComponent transform( Entity& entity );
  core::data::ShComponent material( Entity& entity );
  core::data::ShComponent renderMesh( Entity& entity );
  core::data::ShComponent pointLight( Entity& entity );
  core::data::ShComponent freeFlyCamera( Entity& entity );
} // namespace intermediate::meta::automatic
