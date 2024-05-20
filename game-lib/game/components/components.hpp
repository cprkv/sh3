#pragma once
#include "core/core.hpp"

namespace game
{
  // TODO: this camera not is not bound to scene. may be make active .. inactive?
  class FreeFlyCameraComponent : public core::Component
  {
  public:
    struct Props
    {
      Vec3 right;
      Vec3 forward;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, right, forward );
    };

    mCoreComponent( FreeFlyCameraComponent );

    core::math::SphereStraightRotation rotation;
    core::logic::TransformComponent*   transform = nullptr;

    void init() override;
    void update( const core::system::DeltaTime& dt ) override;
  };


  struct RemoveSceneComponent : public core::Component
  {
    struct Props
    {
      friend void to_json( Json&, const Props& ) {}
      friend void from_json( const Json&, Props& ) {}
    };

    mCoreComponent( RemoveSceneComponent );

    void update( const core::system::DeltaTime& ) override;
  };


  class ScenePortalComponent : public core::Component
  {
  public:
    struct Props
    {
      StringHash toSceneId;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, toSceneId );
    };

    mCoreComponent( ScenePortalComponent );

    core::math::BoundingBox bb;

    void init() override;
    void update( const core::system::DeltaTime& dt ) override;
  };


  void registerComponents();

} // namespace game
