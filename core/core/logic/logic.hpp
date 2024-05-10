#pragma once
#include "core/common.hpp"
#include "core/logic/entity-system.hpp"
#include "core/logic/components.hpp"


namespace core::logic
{
  Status init();
  void   destroy();
  void   update();

  void   sceneLoad( const char* name );
  void   sceneUnload( const char* name );
  Scene* sceneNew( const char* name );

  using ComponentFabric = std::function<Component*( Entity* )>;

  void       componentRegister( StringId componentId, ComponentFabric componentFabric );
  Component* componentInstantiate( StringId componentId, Entity* entity );

  template<typename TComponent>
  void componentRegister()
  {
    auto fabric = []( Entity* entity ) -> Component* { return new TComponent( entity ); };
    componentRegister( TComponent::getComponentId(), std::move( fabric ) );
  }
} // namespace core::logic
