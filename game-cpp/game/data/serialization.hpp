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
    Quat            rotation;
    Vec3            scale;
  };

  struct ShComponentMaterial : public ShComponent
  {
    ~ShComponentMaterial() override = default;
    ShComponentType getType() const override { return ShComponentType_Material; }
    StringId        diffuse; // texture id
  };

  struct ShComponentMesh : public ShComponent
  {
    ~ShComponentMesh() override = default;
    ShComponentType getType() const override { return ShComponentType_Mesh; }
    StringId        id; // mesh id
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


  Status parseJsonFile( std::string path, SceneInfo& output );
} // namespace game
