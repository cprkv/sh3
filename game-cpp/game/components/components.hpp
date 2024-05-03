#pragma once
#include "core/core.hpp"
#include "game/data/serialization.hpp"

namespace game
{
  // TODO: this camera not is not bound to scene. may be make active .. inactive?
  class FreeFlyCameraComponent : public core::Component
  {
    core::math::SphereStraightRotation rotation_;
    Vec3                               position_ = { 0, 0, 0 };

  public:
    mCoreComponent( FreeFlyCameraComponent );

    void update( const core::system::DeltaTime& dt ) override;
  };

  void instantiateComponents( core::Entity& entity, const ShObjectInfo& objectInfo );

} // namespace game
