#pragma once
#include "core/common.hpp"
#include "core/logic/entity-system.hpp"
#include "core/render/render.hpp"

namespace core::logic
{
  class TransformComponent : public core::Component
  {
  public:
    struct Props
    {
      Vec3 position = Vec3( 0, 0, 0 );
      Quat rotation = Quat( 1, 0, 0, 0 );
      Vec3 scale    = Vec3( 1, 1, 1 );

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, position, rotation, scale );
    };

    mCoreComponent( TransformComponent );

    Mat4 getWorldTransform() const;
  };


  class MaterialComponent : public core::Component
  {
  public:
    struct Props
    {
      StringId                textureDiffuseId;
      core::render::BlendMode blendMode;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, textureDiffuseId, blendMode );
    };

    mCoreComponent( MaterialComponent );

    core::render::Texture* textureDiffuse;

    void init() override;
  };


  class RenderMeshComponent : public core::Component
  {
  public:
    struct Props
    {
      StringId meshId;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, meshId );
    };

    mCoreComponent( RenderMeshComponent );

    core::render::Mesh* mesh;
    MaterialComponent*  material;
    TransformComponent* transform;

    void init() override;
    void update( const core::system::DeltaTime& ) override;
  };


  class PointLightComponent : public core::Component
  {
  public:
    struct Props
    {
      Vec3 color     = { 1, 0, 0 };
      f32  intensity = 40;

      NLOHMANN_DEFINE_TYPE_INTRUSIVE( Props, color, intensity );
    };

    mCoreComponent( PointLightComponent );

    TransformComponent* transform;

    void init() override;
    void update( const core::system::DeltaTime& ) override;
  };


  void registerComponents();
} // namespace core::logic
