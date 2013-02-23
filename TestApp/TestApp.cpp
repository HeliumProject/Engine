#include "TestAppPch.h"
#include "TestApp.h"

#include "Reflect/Registry.h"
#include "Reflect/Data/DataDeduction.h"
#include "Framework/FrameworkDataDeduction.h"

#include "Math/Color4.h"

#include "Engine/Asset.h"

#include "Reflect/ArchiveXML.h"
#include "Reflect/ArchiveBinary.h"

#include "PcSupport/ArchiveObjectLoader.h"
#include "PcSupport/ArchivePackageLoader.h"

#include "gtest.h"
#include "TestAsset.h"
#include "WindowProc.h"

#include <cfloat>
#include <ctime>

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"
#include "Framework/World.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"

using namespace Helium;

extern void RegisterEngineTypes();
extern void RegisterGraphicsTypes();
extern void RegisterFrameworkTypes();
extern void RegisterPcSupportTypes();
extern void RegisterComponentTypes();

extern void UnregisterEngineTypes();
extern void UnregisterGraphicsTypes();
extern void UnregisterFrameworkTypes();
extern void UnregisterPcSupportTypes();
extern void UnregisterComponentTypes();

#if HELIUM_TOOLS
extern void RegisterEditorSupportTypes();
extern void UnregisterEditorSupportTypes();
#endif

extern void RegisterTestAppTypes();
extern void UnregisterTestAppTypes();

#include "Engine/Components.h"


int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
    HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );

    Timer::StaticInitialize();

    AsyncLoader::GetStaticInstance().Initialize();

    FilePath baseDirectory;
    if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
    {
        HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
        return -1;
    }

    HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );

    Reflect::Initialize();

    Helium::Components::Initialize();

    RegisterEngineTypes();
    RegisterGraphicsTypes();
    RegisterFrameworkTypes();
    RegisterPcSupportTypes();
    RegisterComponentTypes();
#if HELIUM_TOOLS
    RegisterEditorSupportTypes();
#endif
    RegisterTestAppTypes();

    InitEngineJobsDefaultHeap();
    InitGraphicsJobsDefaultHeap();
    InitTestJobsDefaultHeap();

#if HELIUM_TOOLS
    FontResourceHandler::InitializeStaticLibrary();
#endif

#if HELIUM_TOOLS
    //HELIUM_VERIFY( EditorObjectLoader::InitializeStaticInstance() );
    HELIUM_VERIFY( ArchiveObjectLoader::InitializeStaticInstance() );

    ObjectPreprocessor* pObjectPreprocessor = ObjectPreprocessor::CreateStaticInstance();
    HELIUM_ASSERT( pObjectPreprocessor );
    PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
    HELIUM_ASSERT( pPlatformPreprocessor );
    pObjectPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
    HELIUM_VERIFY( PcCacheObjectLoader::InitializeStaticInstance() );
#endif
    gObjectLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( gObjectLoader );


    Config& rConfig = Config::GetStaticInstance();
    rConfig.BeginLoad();
    while( !rConfig.TryFinishLoad() )
    {
        gObjectLoader->Tick();
    }

    ConfigPc::SaveUserConfig();

    uint32_t displayWidth;
    uint32_t displayHeight;
    //bool bFullscreen;
    bool bVsync;
    
    {
        StrongPtr< GraphicsConfig > spGraphicsConfig(
            rConfig.GetConfigObject< GraphicsConfig >( Name( TXT( "GraphicsConfig" ) ) ) );
        HELIUM_ASSERT( spGraphicsConfig );
        displayWidth = spGraphicsConfig->GetWidth();
        displayHeight = spGraphicsConfig->GetHeight();
        //bFullscreen = spGraphicsConfig->GetFullscreen();
        bVsync = spGraphicsConfig->GetVsync();
    }

    WNDCLASSEXW windowClass;
    windowClass.cbSize = sizeof( windowClass );
    windowClass.style = 0;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = NULL;
    windowClass.hCursor = NULL;
    windowClass.hbrBackground = NULL;
    windowClass.lpszMenuName = NULL;
    windowClass.lpszClassName = L"HeliumTestAppClass";
    windowClass.hIconSm = NULL;
    HELIUM_VERIFY( RegisterClassEx( &windowClass ) );

    WindowData windowData;
    windowData.hMainWnd = NULL;
    windowData.hSubWnd = NULL;
    windowData.bProcessMessages = true;
    windowData.bShutdownRendering = false;
    windowData.resultCode = 0;

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
    RECT windowRect;

    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = static_cast< LONG >( displayWidth );
    windowRect.bottom = static_cast< LONG >( displayHeight );
    HELIUM_VERIFY( AdjustWindowRect( &windowRect, dwStyle, FALSE ) );

    HWND hMainWnd = ::CreateWindowW(
        L"HeliumTestAppClass",
        L"Helium TestApp",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL );
    HELIUM_ASSERT( hMainWnd );

    windowRect.left = 0;
    windowRect.top = 0;
    windowRect.right = static_cast< LONG >( displayWidth );
    windowRect.bottom = static_cast< LONG >( displayHeight );
    HELIUM_VERIFY( AdjustWindowRect( &windowRect, dwStyle, FALSE ) );

    HWND hSubWnd = ::CreateWindowW(
        L"HeliumTestAppClass",
        L"Helium TestApp (second view)",
        dwStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        NULL,
        NULL,
        hInstance,
        NULL );
    HELIUM_ASSERT( hSubWnd );

    windowData.hMainWnd = hMainWnd;
    windowData.hSubWnd = hSubWnd;

    SetWindowLongPtr( hMainWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( &windowData ) );
    SetWindowLongPtr( hSubWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( &windowData ) );
    ShowWindow( hMainWnd, nCmdShow );
    ShowWindow( hSubWnd, nCmdShow );
    UpdateWindow( hMainWnd );
    UpdateWindow( hSubWnd );

    HELIUM_VERIFY( D3D9Renderer::CreateStaticInstance() );

    Renderer* pRenderer = Renderer::GetStaticInstance();
    HELIUM_ASSERT( pRenderer );
    pRenderer->Initialize();

    Renderer::ContextInitParameters contextInitParams;

    contextInitParams.pWindow = hMainWnd;
    contextInitParams.displayWidth = displayWidth;
    contextInitParams.displayHeight = displayHeight;
    contextInitParams.bVsync = bVsync;
    HELIUM_VERIFY( pRenderer->CreateMainContext( contextInitParams ) );

    contextInitParams.pWindow = hSubWnd;
    RRenderContextPtr spSubRenderContext = pRenderer->CreateSubContext( contextInitParams );
    HELIUM_ASSERT( spSubRenderContext );

    RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
    rRenderResourceManager.Initialize();
    rRenderResourceManager.UpdateMaxViewportSize( displayWidth, displayHeight );

    DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
    HELIUM_VERIFY( rDynamicDrawer.Initialize() );

    RRenderContextPtr spMainRenderContext = pRenderer->GetMainContext();
    HELIUM_ASSERT( spMainRenderContext );
    
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    HELIUM_VERIFY( rWorldManager.Initialize() );

    // Create a scene definition
    SceneDefinitionPtr spSceneDefinition;
    Asset::Create<SceneDefinition>(spSceneDefinition, Name(TXT("SceneDefinition")), 0);

    EntityDefinitionPtr spEntityDefinition;
    Asset::Create<EntityDefinition>(spEntityDefinition, Name(TXT("EntityDefinition")), 0);

    TransformComponentDefinitionPtr spTransformComponentDefinition;
    Asset::Create(spTransformComponentDefinition, Name(TXT("TransformComponent")), 0);

    MeshComponentDefinitionPtr spMeshComponentDefinition;
    Asset::Create(spMeshComponentDefinition, Name(TXT("MeshComponent")), 0);

    spEntityDefinition->AddComponentDefinition(Name(TXT("Mesh")), spMeshComponentDefinition);
    spEntityDefinition->AddComponentDefinition(Name(TXT("Transform")), spTransformComponentDefinition);

    spMeshComponentDefinition.Release();
    spTransformComponentDefinition.Release();

    // Create a world
    WorldPtr spWorld( rWorldManager.CreateWorld(spSceneDefinition) );
    HELIUM_ASSERT( spWorld );
    HELIUM_TRACE( TraceLevels::Info, TXT( "Created world \"%s\".\n" ), *spSceneDefinition->GetPath().ToString() );

    Slice *pRootSlice = spWorld->GetRootSlice();
    Entity *entity = pRootSlice->CreateEntity(spEntityDefinition);
    	    
    GraphicsScene* pGraphicsScene = spWorld->GetGraphicsScene();
    HELIUM_ASSERT( pGraphicsScene );
    if( pGraphicsScene )
    {
        uint32_t mainSceneViewId = pGraphicsScene->AllocateSceneView();
        if( IsValid( mainSceneViewId ) )
        {
            float32_t aspectRatio =
                static_cast< float32_t >( displayWidth ) / static_cast< float32_t >( displayHeight );

            RSurface* pDepthStencilSurface = rRenderResourceManager.GetDepthStencilSurface();
            HELIUM_ASSERT( pDepthStencilSurface );

            GraphicsSceneView* pMainSceneView = pGraphicsScene->GetSceneView( mainSceneViewId );
            HELIUM_ASSERT( pMainSceneView );
            pMainSceneView->SetRenderContext( spMainRenderContext );
            pMainSceneView->SetDepthStencilSurface( pDepthStencilSurface );
            pMainSceneView->SetAspectRatio( aspectRatio );
            pMainSceneView->SetViewport( 0, 0, displayWidth, displayHeight );
            pMainSceneView->SetClearColor( Color( 0x00202020 ) );

            //spMainCamera->SetSceneViewId( mainSceneViewId );

            uint32_t subSceneViewId = pGraphicsScene->AllocateSceneView();
            if( IsValid( subSceneViewId ) )
            {
                GraphicsSceneView* pSubSceneView = pGraphicsScene->GetSceneView( subSceneViewId );
                HELIUM_ASSERT( pSubSceneView );
                pSubSceneView->SetRenderContext( spSubRenderContext );
                pSubSceneView->SetDepthStencilSurface( pDepthStencilSurface );
                pSubSceneView->SetAspectRatio( aspectRatio );
                pSubSceneView->SetViewport( 0, 0, displayWidth, displayHeight );
                pSubSceneView->SetClearColor( Color( 0x00202020 ) );

                //spSubCamera->SetSceneViewId( subSceneViewId );
            }
        }
    
#if !HELIUM_RELEASE && !HELIUM_PROFILE
        BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
        rSceneDrawer.DrawScreenText(
            20,
            20,
            String( TXT( "CACHING" ) ),
            Color( 0xff00ff00 ),
            RenderResourceManager::DEBUG_FONT_SIZE_LARGE );
        rSceneDrawer.DrawScreenText(
            21,
            20,
            String( TXT( "CACHING" ) ),
            Color( 0xff00ff00 ),
            RenderResourceManager::DEBUG_FONT_SIZE_LARGE );
#endif
    }

    rWorldManager.Update();

#if 0
    

    //Quat meshEntityBaseRotation( Simd::Vector3( 1.0f, 0.0f, 0.0f ), static_cast< float32_t >( -HELIUM_PI_2 ) );
    Simd::Quat meshEntityBaseRotation = Simd::Quat::IDENTITY;
    SkeletalMeshEntityPtr spMeshEntity( Reflect::AssertCast< SkeletalMeshEntity >( spWorld->CreateEntity(
        spSlice,
        SkeletalMeshEntity::GetStaticType(),
        Simd::Vector3( 0.0f, -20.0f, 0.0f ),
        meshEntityBaseRotation ) ) );
    HELIUM_ASSERT( spMeshEntity );

    {
        AssetPath meshPath;
        HELIUM_VERIFY( meshPath.Set(
            HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Meshes" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "TestBull.fbx" ) ) );

        AssetPtr spMeshObject;
        HELIUM_VERIFY( gObjectLoader->LoadObject( meshPath, spMeshObject ) );
        HELIUM_ASSERT( spMeshObject );
        HELIUM_ASSERT( spMeshObject->IsClass( Mesh::GetStaticType()->GetClass() ) );

        spMeshEntity->SetMesh( Reflect::AssertCast< Mesh >( spMeshObject.Get() ) );

        AssetPath animationPath;
        HELIUM_VERIFY( animationPath.Set(
            HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Animations" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "TestBull_anim.fbx" ) ) );

        AssetPtr spAnimationObject;
        HELIUM_VERIFY( gObjectLoader->LoadObject( animationPath, spAnimationObject ) );
        HELIUM_ASSERT( spAnimationObject );
        HELIUM_ASSERT( spAnimationObject->IsClass( Animation::GetStaticType()->GetClass() ) );

        spMeshEntity->SetAnimation( Reflect::AssertCast< Animation >( spAnimationObject.Get() ) );
    }
#endif
    
    float32_t meshRotation = 0.0f;

    spSubRenderContext.Release();
    spMainRenderContext.Release();

    while( windowData.bProcessMessages )
    {
        MSG message;
        if( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &message );
            DispatchMessage( &message );

            if( windowData.bShutdownRendering )
            {
                if( spWorld )
                {
                    spWorld->Shutdown();
                }

                spWorld.Release();
                WorldManager::DestroyStaticInstance();

                spSceneDefinition.Release();
                spEntityDefinition.Release();

                DynamicDrawer::DestroyStaticInstance();
                RenderResourceManager::DestroyStaticInstance();

                Renderer::DestroyStaticInstance();
            }

            if( message.message == WM_QUIT )
            {
                windowData.bProcessMessages = false;
                windowData.resultCode = static_cast< int >( message.wParam );

                break;
            }
        }
        
        //if( spMeshEntity )
        {
            //Simd::Quat rotation( 0.0f, meshRotation, 0.0f );
            //Simd::Quat rotation( meshRotation * 0.438f, static_cast< float32_t >( HELIUM_PI_2 ), meshRotation );
            //spMeshEntity->SetRotation( meshEntityBaseRotation * rotation );

            //meshRotation += 0.01f;

#if 0 //!HELIUM_RELEASE && !HELIUM_PROFILE
            if( pGraphicsScene )
            {
                const SimpleVertex sceneVertices[] =
                {
                    SimpleVertex( -50.0f, -50.0f, -100.0f, 0xffff0000 ),
                    SimpleVertex( 50.0f, 50.0f, 100.0f, 0xffff0000 ),
                };

                const uint16_t sceneIndices[] =
                {
                    0,
                    1,
                };

                BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
                rSceneDrawer.DrawLines(
                    sceneVertices,
                    static_cast< uint32_t >( HELIUM_ARRAY_COUNT( sceneVertices ) ),
                    sceneIndices,
                    static_cast< uint32_t >( HELIUM_ARRAY_COUNT( sceneIndices ) / 2 ) );

                BufferedDrawer* pViewDrawer = pGraphicsScene->GetSceneViewBufferedDrawer( 0 );
                if( pViewDrawer )
                {
                    const SimpleVertex viewVertices[] =
                    {
                        SimpleVertex( 50.0f, -50.0f, -100.0f, 0xff0000ff ),
                        SimpleVertex( -50.0f, 50.0f, 100.0f, 0xff0000ff ),
                    };

                    const uint16_t viewIndices[] =
                    {
                        0,
                        1,
                    };

                    pViewDrawer->DrawLines(
                        viewVertices,
                        static_cast< uint32_t >( HELIUM_ARRAY_COUNT( viewVertices ) ),
                        viewIndices,
                        static_cast< uint32_t >( HELIUM_ARRAY_COUNT( viewIndices ) / 2 ) );
                }
            }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

#if 0 //!HELIUM_RELEASE && !HELIUM_PROFILE
            if( pGraphicsScene )
            {
                BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
                rSceneDrawer.DrawWorldText(
                    Simd::Matrix44( Simd::Matrix44::INIT_SCALING, 0.75f ),
                    String( TXT( "Debug text test!" ) ),
                    Color( 0xffffffff ),
                    RenderResourceManager::DEBUG_FONT_SIZE_LARGE );
            }
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

#if !HELIUM_RELEASE && !HELIUM_PROFILE
            if( pGraphicsScene )
            {
                BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
                rSceneDrawer.DrawScreenText(
                    20,
                    20,
                    String( TXT( "Debug text test!" ) ),
                    Color( 0xffffffff ) );
            }

            rWorldManager.Update();
        }
    }


#if 0
    spMeshEntity.Release();
#endif
    
#endif

    //spSubCamera.Release();
    //spMainCamera.Release();

    if( spWorld )
    {
        spWorld->Shutdown();
    }

    spWorld.Release();
    WorldManager::DestroyStaticInstance();

    DynamicDrawer::DestroyStaticInstance();
    RenderResourceManager::DestroyStaticInstance();

    Renderer::DestroyStaticInstance();

    
    Helium::Components::Cleanup();

    JobManager::DestroyStaticInstance();

    Config::DestroyStaticInstance();

#if HELIUM_TOOLS
    ObjectPreprocessor::DestroyStaticInstance();
#endif
    AssetLoader::DestroyStaticInstance();
    CacheManager::DestroyStaticInstance();

#if HELIUM_TOOLS
    FontResourceHandler::DestroyStaticLibrary();
#endif

	UnregisterTestAppTypes();
#if HELIUM_TOOLS
    UnregisterEditorSupportTypes();
#endif
    UnregisterPcSupportTypes();
    UnregisterComponentTypes();
    UnregisterFrameworkTypes();
    UnregisterGraphicsTypes();
    UnregisterEngineTypes();

    AssetType::Shutdown();
    Asset::Shutdown();

    AsyncLoader::DestroyStaticInstance();

    Reflect::Cleanup();

    Reflect::ObjectRefCountSupport::Shutdown();

    AssetPath::Shutdown();
    Name::Shutdown();

    FileLocations::Shutdown();

    ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
    DynamicMemoryHeap::LogMemoryStats();
    ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

    return windowData.resultCode;
}
