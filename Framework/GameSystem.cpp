#include "FrameworkPch.h"
#include "Framework/GameSystem.h"

#include "Engine/AsyncLoader.h"
#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Foundation/DirectoryIterator.h"
#include "Reflect/Registry.h"
#include "Platform/Timer.h"
#include "Platform/Process.h"
#include "Engine/Config.h"
#include "Engine/CacheManager.h"
#include "Framework/MemoryHeapPreInitialization.h"
#include "Framework/AssetLoaderInitialization.h"
#include "Framework/ConfigInitialization.h"
#include "Framework/WindowManagerInitialization.h"
#include "Framework/RendererInitialization.h"
#include "Framework/Slice.h"
#include "Framework/WorldManager.h"
#include "Framework/SceneDefinition.h"
#include "Framework/TaskScheduler.h"

#if !HELIUM_SHARED
namespace Helium
{
	void EnumerateDynamicTypes();
}
#endif

using namespace Helium;

static uint32_t g_InitCount = 0;
GameSystem* GameSystem::sm_pInstance = NULL;

/// Constructor.
GameSystem::GameSystem()
: m_pAssetLoaderInitialization( NULL )
, m_pRendererInitialization( NULL )
, m_pWindowManagerInitialization( NULL )
, m_bStopRunning( false )
{
}

/// Initialize this system.
///
/// @param[in] rMemoryHeapPreInitialization  Interface for performing any necessary pre-initialization of dynamic
///                                          memory heaps.
/// @param[in] rAssetLoaderInitialization   Interface for creating and initializing the main AssetLoader instance.
///                                          Note that this must remain valid until Shutdown() is called on this
///                                          system, as a reference to it will be held by this system.
/// @param[in] rConfigInitialization         Interface for initializing application configuration settings.
/// @param[in] rWindowManagerInitialization  Interface for creating and initializing the global window manager
///                                          instance.
/// @param[in] rRendererInitialization       Interface for creating and initializing the global renderer instance.
/// @param[in] pWorldType                    Type of World to create for the main world.  If this is null, the
///                                          actual World type will be used.
bool GameSystem::Initialize(
	MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
	AssetLoaderInitialization& rAssetLoaderInitialization,
	ConfigInitialization& rConfigInitialization,
	WindowManagerInitialization& rWindowManagerInitialization,
	RendererInitialization& rRendererInitialization,
	AssetPath &rSystemDefinitionPath)
{
	m_moduleName = Helium::GetProcessPath().c_str();

	HELIUM_TRACE( TraceLevels::Info, TXT( "Module name: %s\n" ), *m_moduleName );

#if HELIUM_SHARED
	// Initialize sibling dynamically loaded modules.
	FilePath path ( *m_moduleName );
	for ( DirectoryIterator itr ( FilePath( path.Directory() ) ); !itr.IsDone(); itr.Next() )
	{
		std::string ext = itr.GetItem().m_Path.Extension();
		if ( ext == HELIUM_MODULE_EXTENSION )
		{
			HELIUM_TRACE( TraceLevels::Info, TXT( "Loading module: %s\n" ), itr.GetItem().m_Path.Data() );
			ModuleHandle module = LoadModule( itr.GetItem().m_Path.Data() );
			HELIUM_ASSERT( module != HELIUM_INVALID_MODULE );
		}
	}
#else
	bool checkPreDestroy = Asset::s_CheckPreDestroy;
	Asset::s_CheckPreDestroy = false;
	EnumerateDynamicTypes();
	Asset::s_CheckPreDestroy = checkPreDestroy;
#endif

	AsyncLoader::Startup();
	CacheManager::Startup();
	Reflect::Startup();

	rMemoryHeapPreInitialization.Startup();
	rAssetLoaderInitialization.Startup();

	m_pAssetLoaderInitialization = &rAssetLoaderInitialization;

	rConfigInitialization.Startup();

	if ( !rSystemDefinitionPath.IsEmpty() )
	{
		AssetLoader* pAssetLoader = AssetLoader::GetInstance();
		HELIUM_ASSERT( pAssetLoader );
		if( !pAssetLoader )
		{
			HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Asset loader initialization failed.\n" ) );
			return false;
		}

		pAssetLoader->LoadObject<SystemDefinition>( rSystemDefinitionPath, m_spSystemDefinition );
		if ( !m_spSystemDefinition )
		{
			HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Could not find SystemDefinition. LoadObject on '%s' failed.\n" ), *rSystemDefinitionPath.ToString() );
		}
		else
		{
			m_spSystemDefinition->Initialize();
		}
	}

	Components::Startup( m_spSystemDefinition.Get() );

	TaskScheduler::CalculateSchedule( TickTypes::RenderingGame, m_Schedule );

	rWindowManagerInitialization.Startup();
	m_pWindowManagerInitialization = &rWindowManagerInitialization;
	
	// Create and initialize the renderer.
	bool bRendererInitSuccess = rRendererInitialization.Initialize();
	HELIUM_ASSERT( bRendererInitSuccess );
	if( !bRendererInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Renderer initialization failed.\n" ) );

		return false;
	}

	m_pRendererInitialization = &rRendererInitialization;
	
	WorldManager::Startup();

	// Initialization complete.
	return true;
}

/// Shut down this system.
///
/// @see Initialize()
void GameSystem::Cleanup()
{
	WorldManager::Shutdown();

	if( m_pRendererInitialization )
	{
		m_pRendererInitialization->Shutdown();
		m_pRendererInitialization = NULL;
	}

	if( m_pWindowManagerInitialization )
	{
		m_pWindowManagerInitialization->Shutdown();
		m_pWindowManagerInitialization = NULL;
	}

	Components::Shutdown();

	if ( m_spSystemDefinition )
	{
		m_spSystemDefinition->Cleanup();
		m_spSystemDefinition = NULL;
	}

	Config::Shutdown();

	if( m_pAssetLoaderInitialization )
	{
		m_pAssetLoaderInitialization->Shutdown();
		m_pAssetLoaderInitialization = NULL;
	}

	Reflect::Shutdown();
	AssetType::Shutdown();
	Asset::Shutdown();
	AsyncLoader::Shutdown();

	Reflect::ObjectRefCountSupport::Shutdown();

	AssetPath::Shutdown();
	Name::Shutdown();

	FileLocations::Shutdown();
}

/// Run the application loop.
///
/// This will not return until the application is ready to shut down and terminate.
///
/// @return  Result code of application execution.
int32_t GameSystem::Run()
{
	while ( !m_bStopRunning )
	{
		AssetLoader::GetInstance()->Tick();
		m_AssetSyncUtility.Sync();

		WorldManager* pWorldManager = WorldManager::GetInstance();
		HELIUM_ASSERT( pWorldManager );
		pWorldManager->Update( m_Schedule );
	}

	m_bStopRunning = false;

	return 0;
}

/// Get the singleton GameSystem instance.
///
/// @return  Pointer to the GameSystem instance.
///
/// @see Startup(), Shutdown()
GameSystem* GameSystem::GetInstance()
{
	return sm_pInstance;
}

/// Create a GameSystem instance as the singleton System instance.
///
/// @see Shutdown(), GetInstance()
void GameSystem::Startup()
{
	if ( ++g_InitCount == 1 )
	{
		HELIUM_ASSERT( !sm_pInstance );
		sm_pInstance = new GameSystem;
		HELIUM_ASSERT( sm_pInstance );
	}
}

/// Delete the GameSystem singleton.
///
/// @see Startup(), GetInstance()
void GameSystem::Shutdown()
{
	if ( --g_InitCount == 0 )
	{
		HELIUM_ASSERT( sm_pInstance );
		delete sm_pInstance;
		sm_pInstance = NULL;
	}
}

World *GameSystem::LoadScene( SceneDefinition *pSceneDefinition )
{
	WorldManager* pWorldManager = WorldManager::GetInstance();
	HELIUM_ASSERT( pWorldManager );
	return pWorldManager->CreateWorld( pSceneDefinition );
}

void GameSystem::StopRunning()
{
	m_bStopRunning = true;
}