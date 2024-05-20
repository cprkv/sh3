#pragma once
#include "scene-tool.hpp"
#include "schema.hpp"

// NOTE: this is some kind of builder interface for entity components setup

namespace intermediate::meta
{
  struct Scene;
  struct Entity;

  struct Entity
  {
    std::string                          name;
    const intermediate::ObjectInfo*      objectInfo; // may be null if it is created by ourselves
    Scene*                               scene;
    std::vector<core::data::ShComponent> components;

    StringId getId() { return StringId( name ); }

    bool    hasComponent( StringId id ) const;
    Entity& addComponent( core::data::ShComponent component );
    Entity& addComponent( std::function<core::data::ShComponent( Entity& )> factory );

    template<typename TComponent>
    Entity& addComponent( typename TComponent::Props props = {} )
    {
      return addComponent( core::data::ShComponent{
          .type = TComponent::getComponentId(),
          .data = Json( props ),
      } );
    }

    auto serialize() const -> core::data::ShObjectInfo;
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
