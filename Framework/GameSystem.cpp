#include "FrameworkPch.h"
#include "Framework/GameSystem.h"

#include "Engine/AsyncLoader.h"
#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Reflect/Registry.h"
#include "Platform/Timer.h"
#include "Engine/Config.h"
#include "Engine/JobManager.h"
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
	RendererInitialization& rRendererInitialization)
{
	// Initialize the timer first of all, in case someone wants to use it.
	Timer::StaticInitialize();

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


	// Initialize the async loading thread.
	bool bAsyncLoaderInitSuccess = AsyncLoader::GetStaticInstance().Initialize();
	HELIUM_ASSERT( bAsyncLoaderInitSuccess );
	if( !bAsyncLoaderInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Async loader initialization failed.\n" ) );

		return false;
	}

	//pmd - Initialize the cache manager
	FilePath baseDirectory;
	if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
	{
	  HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
	  return false;
	}

	HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );

	// Initialize the reflection type registry and register Asset-based types.
	Reflect::Initialize();
	
	Components::Initialize();
	
	TaskScheduler::CalculateSchedule();

	// Perform dynamic memory heap pre-initialization.
	rMemoryHeapPreInitialization.PreInitialize();

	// Create and initialize the main AssetLoader instance.
	AssetLoader* pAssetLoader = rAssetLoaderInitialization.Initialize();
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

	// Initialize the job manager.
	bool bJobManagerInitSuccess = JobManager::GetStaticInstance().Initialize();
	HELIUM_ASSERT( bJobManagerInitSuccess );
	if( !bJobManagerInitSuccess )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "GameSystem::Initialize(): Job manager initialization failed.\n" ) );

		return false;
	}

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
	WorldManager& rWorldManager = WorldManager::GetStaticInstance();
	bool bWorldManagerInitSuccess = rWorldManager.Initialize();
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

	JobManager::DestroyStaticInstance();

	Config::DestroyStaticInstance();

	if( m_pAssetLoaderInitialization )
	{
		m_pAssetLoaderInitialization->Shutdown();
		m_pAssetLoaderInitialization = NULL;
	}
	
	Components::Cleanup();

	Reflect::Cleanup();
	AssetType::Shutdown();
	Asset::Shutdown();

	AsyncLoader::DestroyStaticInstance();

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
		WorldManager& rWorldManager = WorldManager::GetStaticInstance();
		rWorldManager.Update();
	}

	m_bStopRunning = false;

	return 0;
}

/// Create a GameSystem instance as the singleton System instance if one does not already exist.
///
/// @return  Pointer to a newly allocated GameSystem instance if no singleton System instance exists and one was
///          created successfully, null if creation failed or a System instance already exists.
///
/// @see GetStaticInstance(), DestroyStaticInstance()
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
	WorldManager &rWorldManager = WorldManager::GetStaticInstance();

	return rWorldManager.CreateWorld( pSceneDefinition );
}

void GameSystem::StopRunning()
{
	m_bStopRunning = true;
}