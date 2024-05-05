#pragma once
#include "core/common.hpp"
#include "core/system/time.hpp"

namespace core
{
  class Component;
  class Scene;


  class Entity
  {
    struct ComponentInfo
    {
      StringId   id;
      Component* ptr;
    };

    StringId                       id_;
    Scene*                         scene_;
    StaticVector<ComponentInfo, 8> components_;

#ifdef _DEBUG
    bool initialized_ = false;
#endif

  public:
    Entity( StringId id, Scene* scene );

    StringId getId() const { return id_; }
    Scene*   getScene() const { return scene_; }

    void       init();
    void       shutdown();
    void       update( const system::DeltaTime& dt );
    Component* getComponent( StringId componentId );
    Component* addComponent( StringId componentId, Component* component );
    void       deleteLater();

    template<typename T>
    T* getComponent()
    {
      return static_cast<T*>( getComponent( T::getComponentId() ) );
    }

    template<typename T>
    T* addComponent()
    {
      return static_cast<T*>( addComponent( T::getComponentId(), new T( this ) ) );
    }
  };


  class Component
  {
    Entity* entity_;

  public:
    Component( Entity* entity );
    virtual ~Component();

    Entity* getEntity() const { return entity_; }

    virtual void init();
    virtual void shutdown();
    virtual void update( const system::DeltaTime& dt );

    template<typename T>
    T* getComponent()
    {
      return getEntity()->getComponent<T>();
    }

    template<typename T>
    T* addComponent()
    {
      return getEntity()->addComponent<T>();
    }
  };


  // TODO: make it possible to add entities withing init() call (smth like deffered entities collection?)
  // TODO: make it possible to delete entities
  //
  // TODO!: scene logic class, which handles updates as any component
  // TODO!: world logic which persistent..
  class Scene
  {
    StringId          id_;
    std::list<Entity> entities_;

#ifdef _DEBUG
    bool initialized_ = false;
#endif

  public:
    Scene( StringId id );

    StringId getId() const { return id_; }

    void    init();
    void    shutdown();
    void    update( const system::DeltaTime& dt );
    Entity* addEntity( StringId id );

    // TODO: it looks weird...
    auto entitiesIteratorBegin() { return entities_.begin(); }
    auto entitiesIteratorEnd() { return entities_.end(); }
  };
} // namespace core


#define mCoreComponent( name )                                    \
  inline static StringId getComponentId() { return #name##_sid; } \
                                                                  \
  name( core::Entity* entity )                                    \
      : core::Component( entity )                                 \
  {}                                                              \
                                                                  \
  ~name() override = default;
