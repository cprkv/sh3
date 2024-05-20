#include "scene/meta.hpp"

using namespace intermediate;
using namespace intermediate::meta;


bool Entity::hasComponent( StringId id ) const
{
  return std::ranges::any_of( components, [type = id.getHash()]( const auto& c ) {
    return c.type == type;
  } );
}

Entity& Entity::addComponent( core::data::ShComponent component )
{
  components.emplace_back( std::move( component ) );
  return *this;
}

Entity& Entity::addComponent( std::function<core::data::ShComponent( Entity& )> factory )
{
  assert( objectInfo );
  return addComponent( factory( *this ) );
}

core::data::ShObjectInfo Entity::serialize() const
{
  return core::data::ShObjectInfo{
      .id         = StringId( name ),
      .components = components,
  };
}


Entity& Scene::getEntity( std::string name )
{
  auto it = std::ranges::find_if( entities, [name = std::move( name )]( const auto& entity ) {
    return entity.name == name;
  } );
  mFailIf( it == std::end( entities ) );
  return *it;
}

Entity& Scene::addEntity( const intermediate::ObjectInfo* objectInfo )
{
  mFailIf( !objectInfo );
  return entities.emplace_back( Entity{
      .name       = objectInfo->name,
      .objectInfo = objectInfo,
      .scene      = this,
  } );
}

Entity& Scene::addEntity( std::string name )
{
  bool alreadyContainsEntityWithSameName = std::ranges::any_of(
      entities, [name]( const auto& e ) { return e.name == name; } );
  mFailIf( alreadyContainsEntityWithSameName );
  return entities.emplace_back( Entity{
      .name       = std::move( name ),
      .objectInfo = nullptr,
      .scene      = this,
  } );
}

std::vector<core::data::ShObjectInfo> Scene::serialize()
{
  return entities |
         std::views::transform( []( Entity e ) { return e.serialize(); } ) |
         std::ranges::to<std::vector>();
}
