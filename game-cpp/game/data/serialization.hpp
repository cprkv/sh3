#pragma once

namespace game
{
  enum ShComponentType
  {
    ShComponentType_Transform,
    ShComponentType_Material,
    ShComponentType_Mesh,
  };

  struct ShComponent
  {
    virtual ~ShComponent()                  = default;
    virtual ShComponentType getType() const = 0;
  };

  struct ShComponentTransform : public ShComponent
  {
    ~ShComponentTransform() override = default;
    ShComponentType getType() const override { return ShComponentType_Transform; }
    Vec3            position;
    Vec4            rotation;
    Vec3            scale;
  };

  struct ShComponentMaterial : public ShComponent
  {
    ~ShComponentMaterial() override = default;
    ShComponentType getType() const override { return ShComponentType_Material; }
    core::data::StringId  diffuse; // texture id
  };

  struct ShComponentMesh : public ShComponent
  {
    ~ShComponentMesh() override = default;
    ShComponentType getType() const override { return ShComponentType_Mesh; }
    core::data::StringId id; // mesh id
  };

  struct ShObjectInfo
  {
    std::string                               name;
    std::vector<std::unique_ptr<ShComponent>> components;
  };

  struct SceneInfo
  {
    std::vector<ShObjectInfo> objects;
  };


  Status parseJson( std::string_view input, SceneInfo& output );
} // namespace game