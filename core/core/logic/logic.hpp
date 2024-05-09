#pragma once
#include "core/common.hpp"
#include "core/logic/entity-system.hpp"

namespace core::logic
{
  Status init();
  void   destroy();

  void sceneLoad( const char* name );
  void sceneUnload( const char* name );

  using ComponentFabric = std::function<Component*( Entity* )>;

  void       registerComponent( StringId componentId, ComponentFabric componentFabric );
  Component* instantiateComponent( StringId componentId, Entity* entity );

  template<typename TComponent>
  void registerComponent()
  {
    auto fabric = []( Entity* entity ) -> Component* { return new TComponent( entity ); };
    registerComponent( TComponent::getComponentId(), std::move( fabric ) );
  }
} // namespace core::logic
