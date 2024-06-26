#include "core/logic/logic.hpp"
#include "core/data/serialization.hpp"
#include "core/core.hpp"

using namespace core;
using namespace core::logic;


namespace
{
  struct SceneInfo
  {
    Scene             scene;
    bool              isLoading = false;
    system::Stopwatch loadingTime;
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

  void loadSceneAsync( StringId sceneId, SceneInfo* scene )
  {
    scene->isLoading   = true;
    auto sceneJsonPath = data::getDataPath( StringId( sceneId, ".scene.json" ) );

    system::task::ctiAsync( [sceneJsonPath]() -> std::expected<data::ShSceneInfo, Status> {
      return utils::turnIntoExpected( data::readJsonFile, sceneJsonPath );
    } )
        .then( []( data::ShSceneInfo sceneInfo ) {
          auto renderChunks = sceneInfo.render_chunks |
                              std::views::transform( []( StringHash h ) { return StringId( h ); } ) |
                              std::views::transform( data::RenderChunk::loadCti ) |
                              std::ranges::to<std::vector>();
          return cti::when_all( std::move( sceneInfo ), std::move( renderChunks ) );
        } )
        .then( [scene]( data::ShSceneInfo              sceneInfo,
                        std::vector<data::RenderChunk> renderChunks ) {
          scene->scene.setRenderChunks( std::move( renderChunks ) );

          for( const auto& object: sceneInfo.objects )
          {
            auto* entity = scene->scene.addEntity( object.id );
            instantiateComponents( *entity, object );
          }

          scene->scene.init();
          scene->isLoading = false;
          mCoreLog( "scene " mFmtStringHash " loaded and initialized. it took " mFmtU64 "ms\n",
                    scene->scene.getId().getHash(), scene->loadingTime.getMs() );
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
  bool hasActiveScene = false;

  for( auto& sceneInfo: sData->scenes )
  {
    if( !sceneInfo.isLoading )
    {
      sceneInfo.scene.update( loopGetDeltaTime() );
      hasActiveScene = true;
    }
  }

  if( !hasActiveScene )
  {
    render::getRenderList().loadingScreen = true;
  }
}

void logic::sceneLoad( StringId sceneId )
{
  mCoreLog( "loading scene " mFmtStringHash "\n", sceneId.getHash() );

  auto it = std::ranges::find_if( sData->scenes, [=]( SceneInfo& scene ) {
    return scene.scene.getId() == sceneId;
  } );
  if( it != sData->scenes.end() )
  {
    mCoreLogError( "trying to load scene " mFmtStringHash " which is already loaded\n", sceneId.getHash() );
    return;
  }

  mCoreLog( "loading scene " mFmtStringHash "...\n", sceneId.getHash() );
  auto* scene = &sData->scenes.emplace_back( Scene( sceneId ) );

  loadSceneAsync( sceneId, scene );
}

void logic::sceneUnload( StringId sceneId )
{
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
