#include "core/logic/logic.hpp"

using namespace core;


namespace
{
  struct StaticData
  {
    StringIdMap<logic::ComponentFabric> componentFabrics;
  };

  StaticData* sData = nullptr;
} // namespace


Entity::Entity( StringId id, Scene* scene )
    : id_( id )
    , scene_( scene )
{
}

Entity ::~Entity()
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

Component* Entity::getComponent( StringId componentId )
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

void Entity::deleteLater()
{
  // TODO: ??
}


Component::Component( Entity* entity )
    : entity_( entity )
{
}

Component::~Component()
{
  // TODO: unregister from entity???
}

void Component::deserialize( const Json::object_t& obj ) { ( void ) obj; }
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


Status logic::init()
{
  sData = new StaticData();
  return StatusOk;
}

void logic::destroy()
{
  delete sData;
}

void logic::registerComponent( StringId componentId, ComponentFabric componentFabric )
{
  mCoreLog( "register component " mFmtStringHash "\n", componentId.getHash() );
  auto [_, inserted] = sData->componentFabrics.try_emplace( componentId, std::move( componentFabric ) );
  assert( inserted );
  ( void ) inserted;
}

Component* logic::instantiateComponent( StringId componentId, Entity* entity )
{
  auto* componentFabric = sData->componentFabrics.try_get( componentId );
  assert( componentFabric );
  return ( *componentFabric )( entity );
}
