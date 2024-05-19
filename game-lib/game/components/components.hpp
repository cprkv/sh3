#pragma once
#include "core/core.hpp"

namespace game
{
  struct FreeFlyCameraComponentProps
  {
    Vec3 position;
    Vec3 right;
    Vec3 forward;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( FreeFlyCameraComponentProps, position, right, forward );
  };

  // TODO: this camera not is not bound to scene. may be make active .. inactive?
  class FreeFlyCameraComponent : public core::Component
  {
    core::math::SphereStraightRotation rotation_;
    Vec3                               position_ = { 0, 0, 0 };

  public:
    mCoreComponent( FreeFlyCameraComponent, FreeFlyCameraComponentProps );

    void deserialize( FreeFlyCameraComponentProps props );
    void update( const core::system::DeltaTime& dt ) override;
  };


  struct RemoveSceneComponent : core::Component
  {
    struct Props
    {
      friend void to_json( Json&, const Props& ) {}
      friend void from_json( const Json&, Props& ) {}
    };

    mCoreComponent( RemoveSceneComponent, Props );

    void deserialize( Props ) {}
    void update( const core::system::DeltaTime& ) override;
  };


  void registerComponents();

} // namespace game
