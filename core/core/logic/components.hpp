#pragma once
#include "core/common.hpp"
#include "core/logic/entity-system.hpp"

namespace core::logic
{

  struct TransformComponentProps
  {
    Vec3 position;
    Quat rotation;
    Vec3 scale;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( TransformComponentProps, position, rotation, scale );
  };

  class TransformComponent : public core::Component, public TransformComponentProps
  {
  public:
    mCoreComponent( TransformComponent, TransformComponentProps );

    Mat4 getWorldTransform() const;
    void deserialize( TransformComponentProps props );
  };


  struct RenderMeshComponentProps
  {
    StringId meshId;
    StringId textureDiffuseId;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE( RenderMeshComponentProps, meshId, textureDiffuseId );
  };

  class RenderMeshComponent : public core::Component
  {
  public:
    mCoreComponent( RenderMeshComponent, RenderMeshComponentProps );

    core::render::Mesh*    mesh;
    core::render::Texture* textureDiffuse;
    TransformComponent*    transform;

    void deserialize( RenderMeshComponentProps props );
    void init() override;
    void update( const core::system::DeltaTime& ) override;
  };

  void registerComponents();
} // namespace core::logic
