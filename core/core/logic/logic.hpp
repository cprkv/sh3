#pragma once
#include "core/common.hpp"
#include "core/system/time.hpp"

namespace core
{
  class Component;

  class Entity
  {
    StringId id_;

  public:
    StringId getId() const { return id_; }

    void* getComponent( StringId componentId );
    void  deleteLater();

    template<typename T>
    T* getComponent()
    {
      return static_cast<T*>( getComponent( T::getComponentId() ) );
    }
  };


  class Component
  {
  public:
    // TODO (macro mCoreComponent(name);
    // inline static StringId getComponentId() { return "..."_sid; }

    Component();
    virtual ~Component();

    virtual void init();
    virtual void shutdown();
    virtual void update( system::DeltaTime& dt );

    Entity* getEntity();

    template<typename T>
    T* getComponent()
    {
      return getEntity()->getComponent<T>();
    }
  };
} // namespace core
