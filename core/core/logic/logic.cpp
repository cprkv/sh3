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

  //---------------------------------------------------------------
  // TODO: this is serialization heavy-dependent stuff, should be in data?
  //---------------------------------------------------------------

  struct LoadSceneJsonTask
  {
    std::optional<Status> status = std::nullopt;
    data::ShSceneInfo     result;
  };

  using LoadSceneAction = std::function<void( data::ShSceneInfo, data::RenderChunks )>;

  void loadSceneTask( const std::string& name, LoadSceneAction action )
  {
    auto sceneJsonPath     = data::getDataPath( ( name + ".scene.json" ).c_str() );
    auto loadSceneInfoTask = std::make_shared<LoadSceneJsonTask>();

    // TODO: parse smth is very common task to do, so it probably should be something simpler
    loopEnqueueTask( [sceneJsonPath, loadSceneInfoTask]() mutable {
      auto sceneInfo = data::ShSceneInfo();
      auto status    = data::parseJsonFile( sceneJsonPath, sceneInfo );

      loopEnqueueDefferedTask( [loadSceneInfoTask = std::move( loadSceneInfoTask ),
                                sceneInfo         = std::move( sceneInfo ),
                                status]() mutable {
        loadSceneInfoTask->result = std::move( sceneInfo );
        loadSceneInfoTask->status = status;
      } );
    } );


    auto task = [renderChunks = data::RenderChunks(),
                 action       = std::move( action ),
                 loadSceneInfoTask]() mutable -> PeriodicalStatus {
      // wait json is loaded
      if( !loadSceneInfoTask->status.has_value() )
        return PeriodicalStatusContinue;

      if( loadSceneInfoTask->status != StatusOk )
      {
        mCoreLogError( "error loading scene json: %d %s\n",
                       static_cast<int>( *loadSceneInfoTask->status ),
                       getErrorDetails() );
        return PeriodicalStatusStop;
      }

      // queue load render chunks
      if( renderChunks.size() != loadSceneInfoTask->result.render_chunks.size() )
      {
        renderChunks.resize( loadSceneInfoTask->result.render_chunks.size() );
        for( size_t i = 0; i < renderChunks.size(); ++i )
          renderChunks[i].load( loadSceneInfoTask->result.render_chunks[i].c_str() );
      }

      // wait until render chunks ready
      if( renderChunks.size() )
      {
        auto allLoaded = std::ranges::all_of( renderChunks, []( const auto& chunk ) { return chunk.isLoaded(); } );
        if( !allLoaded )
          return PeriodicalStatusContinue;
      }

      action( std::move( loadSceneInfoTask->result ),
              std::move( renderChunks ) );

      mCoreLog( "loading scene task done!\n" );
      return PeriodicalStatusStop;
    };

    loopEnqueuePeriodicalTask( std::move( task ) );
  }

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

  loadSceneTask( name, [scene]( data::ShSceneInfo  sceneInfo,
                                data::RenderChunks renderChunks ) {
    scene->scene.setRenderChunks( std::move( renderChunks ) );

    for( const auto& object: sceneInfo.objects )
    {
      auto* entity = scene->scene.addEntity( StringId( object.name ) );
      instantiateComponents( *entity, object );
    }

    scene->scene.init();
    scene->isLoading = false;
  } );
}

void logic::sceneUnload( const char* name )
{
  auto sceneId = StringId( name );

  core::loopEnqueueDefferedTask( [sceneId]() {
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
