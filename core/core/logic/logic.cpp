#include "core/logic/logic.hpp"
#include "core/data/serialization.hpp"
#include "core/core.hpp"

using namespace core;
using namespace core::logic;


namespace
{
  struct SceneInfo
  {
    Scene scene;
    bool  isLoading = false;
  };

  struct StaticData
  {
    StringIdMap<ComponentFabric> componentFabrics;
    std::list<SceneInfo>         scenes;
  };

  StaticData* sData = nullptr;


  void instantiateComponents( Entity& entity, const data::ShObjectInfo& objectInfo )
  {
    // TODO: this is developer's try-catch
    try
    {
      for( auto& component: objectInfo.components )
      {
        auto* componentInstance = componentInstantiate( component.type, &entity );
        componentInstance->deserialize( component.data );
        entity.addComponent( component.type, componentInstance );
      }
    }
    catch( std::exception& ex )
    {
      mCoreLogError( "instantiate components on entity " mFmtStringHash " failed: %s\n",
                     entity.getId().getHash(), ex.what() );
      assert( false );
    }
  }

  void loadSceneAsync( const std::string& name, SceneInfo* scene )
  {
    auto sceneJsonPath = data::getDataPath( ( name + ".scene.json" ).c_str() );

    system::task::ctiAsync( [sceneJsonPath]() -> std::expected<data::ShSceneInfo, Status> {
      return utils::turnIntoExpected( data::parseJsonFile, sceneJsonPath );
    } )
        .then( []( data::ShSceneInfo sceneInfo ) {
          auto renderChunks = sceneInfo.render_chunks |
                              std::views::transform( data::RenderChunk::loadCti ) |
                              std::ranges::to<std::vector>();
          return cti::when_all( std::move( sceneInfo ), std::move( renderChunks ) );
        } )
        .then( [scene]( data::ShSceneInfo              sceneInfo,
                        std::vector<data::RenderChunk> renderChunks ) {
          scene->scene.setRenderChunks( std::move( renderChunks ) );

          for( const auto& object: sceneInfo.objects )
          {
            auto* entity = scene->scene.addEntity( StringId( object.name ) );
            instantiateComponents( *entity, object );
          }

          scene->scene.init();
          scene->isLoading = false;
        } )
        .fail( [scene]( Status s ) {
          mCoreLogError( "scene load failed: %d\n", static_cast<int>( s ) );
          scene->isLoading = false;
        } );
  }
} // namespace


Status logic::init()
{
  sData = new StaticData();
  registerComponents();
  return StatusOk;
}

void logic::destroy()
{
  delete sData;
}

void logic::update()
{
  for( auto& sceneInfo: sData->scenes )
  {
    sceneInfo.scene.update( loopGetDeltaTime() );
  }
}

void logic::sceneLoad( const char* name )
{
  auto sceneId = StringId( name );

  auto it = std::ranges::find_if( sData->scenes, [=]( SceneInfo& scene ) {
    return scene.scene.getId() == sceneId;
  } );
  if( it != sData->scenes.end() )
  {
    mCoreLogError( "trying to load scene " mFmtStringHash " which is already loaded\n", sceneId.getHash() );
    return;
  }

  mCoreLog( "loading scene " mFmtStringHash "...\n", sceneId.getHash() );
  auto* scene      = &sData->scenes.emplace_back( sceneId );
  scene->isLoading = true;

  loadSceneAsync( name, scene );
}

void logic::sceneUnload( const char* name )
{
  auto sceneId = StringId( name );

  core::system::task::runDeffered( [sceneId]() {
    auto it = std::ranges::find_if( sData->scenes, [=]( SceneInfo& scene ) {
      return scene.scene.getId() == sceneId;
    } );

    if( it == sData->scenes.end() )
    {
      mCoreLogError( "trying to unload scene " mFmtStringHash " which is not loaded\n", sceneId.getHash() );
      return;
    }

    if( it->isLoading )
    {
      mCoreLogError( "trying to unload scene " mFmtStringHash " which is loading\n", sceneId.getHash() );
      return;
    }

    mCoreLog( "unloading scene " mFmtStringHash "...\n", sceneId.getHash() );
    it->scene.shutdown();
    sData->scenes.erase( it );
  } );
}

Scene* logic::sceneNew( const char* name )
{
  auto& sceneInfo = sData->scenes.emplace_back( SceneInfo{
      .scene     = core::Scene( StringId( name ) ),
      .isLoading = false,
  } );
  return &sceneInfo.scene;
}

void logic::componentRegister( StringId componentId, ComponentFabric componentFabric )
{
  mCoreLog( "register component " mFmtStringHash "\n", componentId.getHash() );
  auto [_, inserted] = sData->componentFabrics.try_emplace( componentId, std::move( componentFabric ) );
  assert( inserted );
  ( void ) inserted;
}

Component* logic::componentInstantiate( StringId componentId, Entity* entity )
{
  auto* componentFabric = sData->componentFabrics.try_get( componentId );
  assert( componentFabric );
  return ( *componentFabric )( entity );
}
