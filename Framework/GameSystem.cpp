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
#include "Framework/CommandLineInitialization.h"
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

/// Constructor.
GameSystem::GameSystem()
: m_pAssetLoaderInitialization( NULL )
, m_bStopRunning( false )
{
}

/// Destructor.
GameSystem::~GameSystem()
{
}

/// Initialize this system.
///
/// @param[in] rCommandLineInitialization    Interface for initializing command-line parameters.
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
	CommandLineInitialization& rCommandLineInitialization,
	MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
	AssetLoaderInitialization& rAssetLoaderInitialization,
	ConfigInitialization& rConfigInitialization,
	WindowManagerInitialization& rWindowManagerInitialization,
	RendererInitialization& rRendererInitialization,
	AssetPath &rSystemDefinitionPath)
{
	// Initialize command-line parameters.
	bool bCommandLineInitSuccess = rCommandLineInitialization.Initialize( m_moduleName, m_arguments );
	HELIUM_ASSERT( bCommandLineInitSuccess );
	if( !bCommandLineInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Command-line initialization failed.\n" ) );

		return false;
	}

#if HELIUM_ENABLE_TRACE
	HELIUM_TRACE( TraceLevels::Info, TXT( "Module name: %s\n" ), *m_moduleName );
	HELIUM_TRACE( TraceLevels::Info, TXT( "Command-line arguments:\n" ) );
	size_t argumentCount = m_arguments.GetSize();
	for( size_t argumentIndex = 0; argumentIndex < argumentCount; ++argumentIndex )
	{
		HELIUM_TRACE( TraceLevels::Info, TXT( "* %s\n" ), *m_arguments[ argumentIndex ] );
	}
#endif

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

	//pmd - Initialize the cache manager
	FilePath baseDirectory;
	if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
		return false;
	}

	CacheManager::Startup();

	// Initialize the reflection type registry and register Asset-based types.
	Reflect::Startup();

	// Perform dynamic memory heap pre-initialization.
	rMemoryHeapPreInitialization.PreInitialize();

	// Create and initialize the main AssetLoader instance.
	rAssetLoaderInitialization.Startup();

	AssetLoader* pAssetLoader = AssetLoader::GetInstance();
	HELIUM_ASSERT( pAssetLoader );
	if( !pAssetLoader )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Asset loader initialization failed.\n" ) );

		return false;
	}

	m_pAssetLoaderInitialization = &rAssetLoaderInitialization;

	// Initialize system configuration.
	bool bConfigInitSuccess = rConfigInitialization.Initialize();
	HELIUM_ASSERT( bConfigInitSuccess );
	if( !bConfigInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Failed to initialize configuration settings.\n" ) );

		return false;
	}

	if ( !rSystemDefinitionPath.IsEmpty() )
	{
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

	// Create and initialize the window manager (note that we need a window manager for message loop processing, so
	// the instance cannot be left null).
	bool bWindowManagerInitSuccess = rWindowManagerInitialization.Initialize();
	HELIUM_ASSERT( bWindowManagerInitSuccess );
	if( !bWindowManagerInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Window manager initialization failed.\n" ) );

		return false;
	}
	
	// Create and initialize the renderer.
	bool bRendererInitSuccess = rRendererInitialization.Initialize();
	HELIUM_ASSERT( bRendererInitSuccess );
	if( !bRendererInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Renderer initialization failed.\n" ) );

		return false;
	}

	m_pRendererInitialization = &rRendererInitialization;
	
	// Initialize the world manager and main game world.
	WorldManager* pWorldManager = WorldManager::GetInstance();
	HELIUM_ASSERT( pWorldManager );
	bool bWorldManagerInitSuccess = pWorldManager->Initialize();
	HELIUM_ASSERT( bWorldManagerInitSuccess );
	if( !bWorldManagerInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "World manager initialization failed.\n" ) );
		return false;
	}

	// Initialization complete.
	return true;
}

/// Shut down this system.
///
/// @see Initialize()
void GameSystem::Shutdown()
{
	WorldManager::DestroyStaticInstance();

	if( m_pRendererInitialization )
	{
		m_pRendererInitialization->Shutdown();
		m_pRendererInitialization = NULL;
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

	// Perform base System shutdown last.
	System::Shutdown();
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

/// Create a GameSystem instance as the singleton System instance if one does not already exist.
///
/// @return  Pointer to a newly allocated GameSystem instance if no singleton System instance exists and one was
///          created successfully, null if creation failed or a System instance already exists.
///
/// @see GetInstance(), DestroyStaticInstance()
GameSystem* GameSystem::CreateStaticInstance()
{
	if( sm_pInstance )
	{
		return NULL;
	}

	GameSystem* pSystem = new GameSystem;
	HELIUM_ASSERT( pSystem );
	sm_pInstance = pSystem;

	return pSystem;
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