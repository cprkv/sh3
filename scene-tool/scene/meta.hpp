#pragma once
#include "scene-tool.hpp"
#include "schema.hpp"

// NOTE: this is some kind of builder interface for entity components setup

namespace intermediate::meta
{
  struct Scene;
  struct Entity;

  struct IComponent
  {
    virtual ~IComponent()                                         = default;
    virtual core::data::ShComponent build( Entity& entity ) const = 0;
  };

  struct Entity
  {
    std::string                          name;
    const intermediate::ObjectInfo*      objectInfo; // may be null if it is created by ourselves
    Scene*                               scene;
    std::vector<core::data::ShComponent> components;

    StringId getId() { return StringId( name ); }

    bool    hasComponent( StringId id ) const;
    Entity& addComponent( const IComponent& component );
    auto    serialize() const -> core::data::ShObjectInfo;
  };

  struct Scene
  {
    std::vector<Entity> entities;

    Entity& getEntity( std::string name );
    Entity& addEntity( const intermediate::ObjectInfo* objectInfo );
    Entity& addEntity( std::string name );
    auto    serialize() -> std::vector<core::data::ShObjectInfo>;
  };
} // namespace intermediate::meta
