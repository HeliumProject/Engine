//----------------------------------------------------------------------------------------------------------------------
// GameSystem.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/GameSystem.h"

#include "Foundation/AsyncLoader.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Reflect/Registry.h"
#include "Platform/Timer.h"
#include "Engine/Config.h"
#include "Engine/JobManager.h"
#include "Engine/CacheManager.h"
#include "Windowing/WindowManager.h"
#include "Rendering/Renderer.h"
#include "Rendering/RSurface.h"
#include "Graphics/DynamicDrawer.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/RenderResourceManager.h"
#include "Framework/CommandLineInitialization.h"
#include "Framework/ObjectTypeRegistration.h"
#include "Framework/MemoryHeapPreInitialization.h"
#include "Framework/ObjectLoaderInitialization.h"
#include "Framework/ConfigInitialization.h"
#include "Framework/WindowManagerInitialization.h"
#include "Framework/RendererInitialization.h"
#include "Framework/Layer.h"
#include "Framework/WorldManager.h"

using namespace Helium;

/// Constructor.
GameSystem::GameSystem()
: m_pObjectTypeRegistration( NULL )
, m_pObjectLoaderInitialization( NULL )
, m_pMainWindow( NULL )
{
}

/// Destructor.
GameSystem::~GameSystem()
{
}

/// Initialize this system.
///
/// @param[in] rCommandLineInitialization    Interface for initializing command-line parameters.
/// @param[in] rObjectTypeRegistration       Interface for registering GameObject-based types.  Note that this must
///                                          remain valid until Shutdown() is called on this system, as a reference
///                                          to it will be held by this system.
/// @param[in] rMemoryHeapPreInitialization  Interface for performing any necessary pre-initialization of dynamic
///                                          memory heaps.
/// @param[in] rObjectLoaderInitialization   Interface for creating and initializing the main GameObjectLoader instance.
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
    ObjectTypeRegistration& rObjectTypeRegistration,
    MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
    ObjectLoaderInitialization& rObjectLoaderInitialization,
    ConfigInitialization& rConfigInitialization,
    WindowManagerInitialization& rWindowManagerInitialization,
    RendererInitialization& rRendererInitialization,
    const GameObjectType* pWorldType )
{
    // Initialize the timer first of all, in case someone wants to use it.
    Timer::StaticInitialize();

    // Initialize command-line parameters.
    bool bCommandLineInitSuccess = rCommandLineInitialization.Initialize( m_moduleName, m_arguments );
    HELIUM_ASSERT( bCommandLineInitSuccess );
    if( !bCommandLineInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Command-line initialization failed.\n" ) );

        return false;
    }

#if HELIUM_ENABLE_TRACE
    HELIUM_TRACE( TRACE_INFO, TXT( "Module name: %s\n" ), *m_moduleName );
    HELIUM_TRACE( TRACE_INFO, TXT( "Command-line arguments:\n" ) );
    size_t argumentCount = m_arguments.GetSize();
    for( size_t argumentIndex = 0; argumentIndex < argumentCount; ++argumentIndex )
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "* %s\n" ), *m_arguments[ argumentIndex ] );
    }
#endif


    // Initialize the async loading thread.
    bool bAsyncLoaderInitSuccess = AsyncLoader::GetStaticInstance().Initialize();
    HELIUM_ASSERT( bAsyncLoaderInitSuccess );
    if( !bAsyncLoaderInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Async loader initialization failed.\n" ) );

        return false;
    }

    //pmd - Initialize the cache manager
    Path baseDirectory;
    if ( !File::GetBaseDirectory( baseDirectory ) )
    {
      HELIUM_TRACE( TRACE_ERROR, TXT( "Could not get base directory." ) );
      return false;
    }

    HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );

    // Initialize the reflection type registry and register GameObject-based types.
    Reflect::Initialize();

    rObjectTypeRegistration.Register();
    m_pObjectTypeRegistration = &rObjectTypeRegistration;

    // Perform dynamic memory heap pre-initialization.
    rMemoryHeapPreInitialization.PreInitialize();

    // Create and initialize the main GameObjectLoader instance.
    GameObjectLoader* pObjectLoader = rObjectLoaderInitialization.Initialize();
    HELIUM_ASSERT( pObjectLoader );
    if( !pObjectLoader )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): GameObject loader initialization failed.\n" ) );

        return false;
    }

    m_pObjectLoaderInitialization = &rObjectLoaderInitialization;

    // Initialize system configuration.
    bool bConfigInitSuccess = rConfigInitialization.Initialize();
    HELIUM_ASSERT( bConfigInitSuccess );
    if( !bConfigInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Failed to initialize configuration settings.\n" ) );

        return false;
    }

    // Initialize the job manager.
    bool bJobManagerInitSuccess = JobManager::GetStaticInstance().Initialize();
    HELIUM_ASSERT( bJobManagerInitSuccess );
    if( !bJobManagerInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Job manager initialization failed.\n" ) );

        return false;
    }

    // Create and initialize the window manager (note that we need a window manager for message loop processing, so
    // the instance cannot be left null).
    bool bWindowManagerInitSuccess = rWindowManagerInitialization.Initialize();
    HELIUM_ASSERT( bWindowManagerInitSuccess );
    if( !bWindowManagerInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Window manager initialization failed.\n" ) );

        return false;
    }

    WindowManager* pWindowManager = WindowManager::GetStaticInstance();
    if( !pWindowManager )
    {
        HELIUM_TRACE(
            TRACE_INFO,
            ( TXT( "GameSystem::Initialize(): No window manager created.  A window manager is necessary for " )
            TXT( "GameSystem execution.\n" ) ) );

        return false;
    }

    // Create and initialize the renderer.
    bool bRendererInitSuccess = rRendererInitialization.Initialize();
    HELIUM_ASSERT( bRendererInitSuccess );
    if( !bRendererInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "GameSystem::Initialize(): Renderer initialization failed.\n" ) );

        return false;
    }

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "GameSystem::Initialize(): Using null renderer.\n" ) );
    }
    else
    {
        // Create the main application window.
        Config& rConfig = Config::GetStaticInstance();
        StrongPtr< GraphicsConfig > spGraphicsConfig(
            rConfig.GetConfigObject< GraphicsConfig >( Name( TXT( "GraphicsConfig" ) ) ) );
        HELIUM_ASSERT( spGraphicsConfig );

        uint32_t displayWidth = spGraphicsConfig->GetWidth();
        uint32_t displayHeight = spGraphicsConfig->GetHeight();
        bool bFullscreen = spGraphicsConfig->GetFullscreen();
        bool bVsync = spGraphicsConfig->GetVsync();

        Window::Parameters windowParameters;
        windowParameters.pTitle = TXT( "Helium" );
        windowParameters.width = displayWidth;
        windowParameters.height = displayHeight;
        windowParameters.bFullscreen = bFullscreen;

        m_pMainWindow = pWindowManager->Create( windowParameters );
        HELIUM_ASSERT( m_pMainWindow );
        if( !m_pMainWindow )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to create main application window.\n" ) );

            return false;
        }

        m_pMainWindow->SetOnDestroyed( Delegate<Window*>( this, &GameSystem::OnMainWindowDestroyed ) );

        Renderer::ContextInitParameters contextInitParams;
        contextInitParams.pWindow = m_pMainWindow->GetHandle();
        contextInitParams.displayWidth = displayWidth;
        contextInitParams.displayHeight = displayHeight;
        contextInitParams.bFullscreen = bFullscreen;
        contextInitParams.bVsync = bVsync;

        bool bContextCreateResult = pRenderer->CreateMainContext( contextInitParams );
        HELIUM_ASSERT( bContextCreateResult );
        if( !bContextCreateResult )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to create main renderer context.\n" ) );

            return false;
        }

        // Create and initialize the render resource manager.
        RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
        rRenderResourceManager.Initialize();

        // Create and initialize the dynamic drawing interface.
        DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
        if( !rDynamicDrawer.Initialize() )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to initialize dynamic drawing support.\n" ) );

            return false;
        }
    }

    // Initialize the world manager and main game world.
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    bool bWorldManagerInitSuccess = rWorldManager.Initialize();
    HELIUM_ASSERT( bWorldManagerInitSuccess );
    if( !bWorldManagerInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "World manager initialization failed.\n" ) );

        return false;
    }

    if( !pWorldType )
    {
        pWorldType = World::GetStaticType();
        HELIUM_ASSERT( pWorldType );
    }

    WorldPtr spDefaultWorld( rWorldManager.CreateDefaultWorld( pWorldType ) );
    HELIUM_ASSERT( spDefaultWorld );
    if( !spDefaultWorld )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to create the default world.\n" ) );

        return false;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "Created default world \"%s\".\n" ), *spDefaultWorld->GetPath().ToString() );

    bool bWorldInitSuccess = spDefaultWorld->Initialize();
    HELIUM_ASSERT( bWorldInitSuccess );
    if( !bWorldInitSuccess )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to initialize default world.\n" ) );

        return false;
    }

    PackagePtr spLayerPackage;
    HELIUM_VERIFY( GameObject::Create< Package >( spLayerPackage, Name( TXT( "DefaultLayerPackage" ) ), NULL ) );
    HELIUM_ASSERT( spLayerPackage );

    LayerPtr spLayer;
    HELIUM_VERIFY( GameObject::Create< Layer >( spLayer, Name( TXT( "Layer" ) ), spLayerPackage ) );
    HELIUM_ASSERT( spLayer );
    spLayer->BindPackage( spLayerPackage );

    HELIUM_VERIFY( spDefaultWorld->AddLayer( spLayer ) );

    // Initialization complete.
    return true;
}

/// Shut down this system.
///
/// @see Initialize()
void GameSystem::Shutdown()
{
    WorldManager::DestroyStaticInstance();
    DynamicDrawer::DestroyStaticInstance();
    RenderResourceManager::DestroyStaticInstance();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( pRenderer )
    {
        pRenderer->Shutdown();
        Renderer::DestroyStaticInstance();
    }

    WindowManager* pWindowManager = WindowManager::GetStaticInstance();
    if( pWindowManager )
    {
        if( m_pMainWindow )
        {
            m_pMainWindow->Destroy();
            while( m_pMainWindow )
            {
                pWindowManager->Update();
            }
        }

        pWindowManager->Shutdown();
        WindowManager::DestroyStaticInstance();
    }

    HELIUM_ASSERT( !m_pMainWindow );

    JobManager::DestroyStaticInstance();

    Config::DestroyStaticInstance();

    if( m_pObjectLoaderInitialization )
    {
        m_pObjectLoaderInitialization->Shutdown();
        m_pObjectLoaderInitialization = NULL;
    }

    if( m_pObjectTypeRegistration )
    {
        m_pObjectTypeRegistration->Unregister();
        m_pObjectTypeRegistration = NULL;
    }

    GameObjectType::Shutdown();
    GameObject::Shutdown();

    Reflect::Cleanup();

    AsyncLoader::GetStaticInstance().Shutdown();
    AsyncLoader::DestroyStaticInstance();

    Reflect::ObjectRefCountSupport::Shutdown();

    GameObjectPath::Shutdown();
    CharName::Shutdown();
    WideName::Shutdown();

    File::Shutdown();

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
    WindowManager* pWindowManager = WindowManager::GetStaticInstance();
    if( !pWindowManager )
    {
        return 0;
    }

    WorldManager& rWorldManager = WorldManager::GetStaticInstance();

    while( pWindowManager->Update() )
    {
        rWorldManager.Update();
    }

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

/// Callback executed when the main window is actually destroyed.
///
/// @param[in] pWindow  Pointer to the destroyed Window instance.
void GameSystem::OnMainWindowDestroyed( Window* pWindow )
{
    HELIUM_ASSERT( m_pMainWindow == pWindow );
    HELIUM_UNREF( pWindow );

    m_pMainWindow = NULL;

    WindowManager* pWindowManager = WindowManager::GetStaticInstance();
    HELIUM_ASSERT( pWindowManager );
    pWindowManager->RequestQuit();
}
