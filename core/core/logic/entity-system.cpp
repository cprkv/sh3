#include "core/logic/entity-system.hpp"

using namespace core;


Entity::Entity( StringId id, Scene* scene )
    : id_( id )
    , scene_( scene )
{
}

Entity::~Entity()
{
  for( auto& it: components_ )
  {
    delete it.ptr;
  }
}

void Entity::init()
{
  for( u32 i = 0; i < components_.size(); ++i )
  {
    components_[i].ptr->init();
  }

#ifdef _DEBUG
  initialized_ = true;
#endif
}

void Entity::shutdown()
{
  for( auto& it: components_ )
  {
    it.ptr->shutdown();
  }
}

void Entity::update( const system::DeltaTime& dt )
{
  for( auto& it: components_ )
  {
    it.ptr->update( dt );
  }
}

Component* Entity::tryGetComponent( StringId componentId )
{
  for( auto& it: components_ )
    if( it.id == componentId )
      return it.ptr;

  return nullptr;
}

Component* Entity::addComponent( StringId componentId, Component* component )
{
  assert( !initialized_ ); // can't add components after init() finished
  components_.push_back( ComponentInfo{ .id = componentId, .ptr = component } );
  return component;
}


Component::Component( Entity* entity )
    : entity_( entity )
{
}

Component::~Component()
{
  // TODO: unregister from entity???
}

void Component::deserialize( const Json& obj ) { ( void ) obj; }
void Component::init() {}
void Component::shutdown() {}
void Component::update( const system::DeltaTime& dt ) { ( void ) dt; }


Scene::Scene( StringId id )
    : id_( id )
{
}

void Scene::init()
{
  for( auto it = entities_.begin(); it != entities_.end(); ++it )
  {
    it->init();
  }

#ifdef _DEBUG
  initialized_ = true;
#endif
}

void Scene::shutdown()
{
  for( auto& it: entities_ )
  {
    it.shutdown();
  }
}

void Scene::update( const system::DeltaTime& dt )
{
  for( auto& it: entities_ )
  {
    it.update( dt );
  }
}

Entity* Scene::addEntity( StringId id )
{
  assert( !initialized_ ); // can't add entities after init() started
  entities_.emplace_back( id, this );
  return &*entities_.rbegin();
}
