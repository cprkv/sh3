#pragma once
#include "core/core.hpp"

namespace game
{
  // TODO: this camera not is not bound to scene. may be make active .. inactive?
  class FreeFlyCameraComponent : public core::Component
  {
    core::math::SphereStraightRotation rotation_;
    Vec3                               position_ = { 0, 0, 0 };

  public:
    mCoreComponent( FreeFlyCameraComponent );

    void deserialize( const Json::object_t& obj ) override;
    void update( const core::system::DeltaTime& dt ) override;
  };


  struct RemoveSceneComponent : core::Component
  {
    mCoreComponent( RemoveSceneComponent );

    void update( const core::system::DeltaTime& ) override;
  };


  void registerComponents();

} // namespace game
