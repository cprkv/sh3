#include "game/data/data.hpp"
#include "game/tasks/tasks.hpp"

using namespace game;


void SceneCollection::load( const std::string& name )
{
  auto hint = scenes.lower_bound( name );
  if( hint != scenes.end() && hint->first == name )
  {
    mCoreLog( "trying to load scene that already exists\n" );
    return;
  }

  auto  it       = scenes.emplace_hint( hint, name, std::vector<SceneEntity>{} );
  auto& entities = it->second;
  tasks::loadScene( name, entities );
}

void SceneCollection::unload( const std::string& name )
{
  auto it = scenes.find( name );
  if( it != scenes.end() )
  {
    core::data::unloadRenderChunk( name.c_str() );
    scenes.erase( it );
  }
}

void SceneCollection::unloadAll()
{
  for( auto it = scenes.begin(); it != scenes.end(); )
  {
    auto cur = it;
    ++it;

    core::data::unloadRenderChunk( cur->first.c_str() );
    scenes.erase( cur );
  }
}

