#pragma once
#include "scene-tool.hpp"
#include "schema.hpp"
#include "scene/meta.hpp"
#include "game/components/components.hpp"

namespace intermediate::meta
{
  struct TransformComponent : public IComponent
  {
    static constexpr StringId id = core::logic::TransformComponent::getComponentId();
    core::data::ShComponent   build( Entity& entity ) const override;
  };

  struct RenderMeshComponent : public IComponent
  {
    static constexpr StringId id = core::logic::RenderMeshComponent::getComponentId();
    core::data::ShComponent   build( Entity& entity ) const override;
  };

  struct FreeFlyCameraComponent : public IComponent
  {
    static constexpr StringId id = game::FreeFlyCameraComponent::getComponentId();
    core::data::ShComponent   build( Entity& entity ) const override;
  };

  struct ScenePortalComponent : public IComponent
  {
    StringId toSceneId;

    static constexpr StringId id = game::ScenePortalComponent::getComponentId();

    ScenePortalComponent( StringId toSceneId )
        : toSceneId( toSceneId )
    {}
    core::data::ShComponent build( Entity& entity ) const override;
  };
} // namespace intermediate::meta
