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


Status logic::init()
{
  sData = new StaticData();
  return StatusOk;
}

void logic::destroy()
{
  delete sData;
}

void logic::sceneLoad( const char* name )
{
  ( void ) name;
}

void logic::sceneUnload( const char* name )
{
  ( void ) name;
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
