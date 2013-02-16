#include "TestAppPch.h"
#include "TestApp.h"

#include "Reflect/Registry.h"
#include "Reflect/Data/DataDeduction.h"
#include "Framework/FrameworkDataDeduction.h"

#include "Math/Color4.h"

#include "Engine/GameObject.h"

#include "Reflect/ArchiveXML.h"
#include "Reflect/ArchiveBinary.h"

#include "PcSupport/ArchiveObjectLoader.h"
#include "PcSupport/ArchivePackageLoader.h"

#include "gtest.h"
#include "TestGameObject.h"
#include "WindowProc.h"

#include <cfloat>
#include <ctime>

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"

using namespace Helium;

extern void RegisterEngineTypes();
extern void RegisterGraphicsTypes();
extern void RegisterFrameworkTypes();
extern void RegisterPcSupportTypes();

extern void UnregisterEngineTypes();
extern void UnregisterGraphicsTypes();
extern void UnregisterFrameworkTypes();
extern void UnregisterPcSupportTypes();

#if HELIUM_TOOLS
extern void RegisterEditorSupportTypes();
extern void UnregisterEditorSupportTypes();
#endif

extern void RegisterTestAppTypes();
extern void UnregisterTestAppTypes();

#include "Engine/Components.h"


class TestComponentFour : public Helium::Components::Component
{
public:
    TestComponentFour()
    {
        //static int32_t next_id = 100;
        //m_Id = next_id++;
    }

    //int32_t m_Id;
    Helium::Color4 m_Color;

    OBJECT_DECLARE_COMPONENT( TestComponentFour, Components::Component );

    static void PopulateComposite( Reflect::Composite& comp );
};

OBJECT_DEFINE_COMPONENT(TestComponentFour);

void TestComponentFour::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &TestComponentFour::m_Color, TXT( "m_Color" ) );
}







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

    RegisterEngineTypes();
    RegisterGraphicsTypes();
    RegisterFrameworkTypes();
    RegisterPcSupportTypes();
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
    gObjectLoader = GameObjectLoader::GetStaticInstance();
    HELIUM_ASSERT( gObjectLoader );


    Config& rConfig = Config::GetStaticInstance();
    rConfig.BeginLoad();
    while( !rConfig.TryFinishLoad() )
    {
        gObjectLoader->Tick();
    }

    ConfigPc::SaveUserConfig();



    Helium::Components::Initialize();
    ColorComponent::RegisterComponentType(64);

    struct TestObj : public Helium::Components::HasComponents
    {

    };

    //GameObject::Create(

    //TestObj.Allocate<ColorComponent>();

    Helium::StrongPtr<Helium::ComponentDescriptor_ColorComponent> color_descriptor1;
    ComponentDescriptor_ColorComponent::Create(color_descriptor1, Name(TXT("ColorComponent1")), NULL);
    
    Helium::StrongPtr<Helium::ComponentDescriptor_ColorComponent> color_descriptor2;
    ComponentDescriptor_ColorComponent::Create(color_descriptor2, Name(TXT("ColorComponent2")), NULL);

    Log::Print("ColorComponent1: %x\n", color_descriptor1.Get());
    Log::Print("ColorComponent2: %x\n", color_descriptor2.Get());

    color_descriptor1->m_Color = Color4(255, 0, 0, 255);

    Helium::StrongPtr<ComponentDefinitionSet> component_set;
    ComponentDefinitionSet::Create(component_set, Name(TXT("MyComponentSet")), NULL);

    component_set->AddDescriptor(Name(TXT("ColorComponent1")), color_descriptor1);
    component_set->AddDescriptor(Name(TXT("ColorComponent2")), color_descriptor2);
    component_set->AddParameter(Name(TXT("ColorComponent2")), Name(TXT("ColorComponent1")), Name(TXT("m_Pointer")));
    component_set->AddParameter(Name(TXT("Color")), Name(TXT("ColorComponent1")), Name(TXT("m_Color")));

    Helium::Components::ComponentSet instantiated_components;

    ParameterSet param_set;
    param_set.SetParameter(Name(TXT("Color")), Color4(0, 0, 255, 255));

    Helium::Components::DeployComponents(*component_set, param_set, instantiated_components);


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

    WorldPtr spWorld( rWorldManager.CreateDefaultWorld() );
    HELIUM_ASSERT( spWorld );
    HELIUM_VERIFY( spWorld->Initialize() );
    HELIUM_TRACE( TraceLevels::Info, TXT( "Created world \"%s\".\n" ), *spWorld->GetPath().ToString() );

    PackagePtr spSlicePackage;
    HELIUM_VERIFY( GameObject::Create< Package >( spSlicePackage, Name( TXT( "DefaultSlicePackage" ) ), NULL ) );
    HELIUM_ASSERT( spSlicePackage );

    SlicePtr spSlice;
    HELIUM_VERIFY( GameObject::Create< Slice >( spSlice, Name( TXT( "Slice" ) ), spSlicePackage ) );
    HELIUM_ASSERT( spSlice );
    spSlice->BindPackage( spSlicePackage );

    HELIUM_VERIFY( spWorld->AddSlice( spSlice ) );
#if 0
    CameraPtr spMainCamera( Reflect::AssertCast< Camera >( spWorld->CreateEntity(
        spSlice,
        Camera::GetStaticType(),
        Simd::Vector3( 0.0f, 200.0f, 750.0f ),
        Simd::Quat( 0.0f, static_cast< float32_t >( HELIUM_PI ), 0.0f ),
        Simd::Vector3( 1.0f ),
        NULL,
        NULL_NAME,
        true ) ) );
    HELIUM_ASSERT( spMainCamera );

    CameraPtr spSubCamera( Reflect::AssertCast< Camera >( spWorld->CreateEntity(
        spSlice,
        Camera::GetStaticType(),
        Simd::Vector3( 750.0f, 200.0f, 0.0f ),
        Simd::Quat( 0.0f, static_cast< float32_t >( -HELIUM_PI_2 ), 0.0f ),
        Simd::Vector3( 1.0f ),
        NULL,
        NULL_NAME,
        true ) ) );
    HELIUM_ASSERT( spSubCamera );

#endif

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
        GameObjectPath meshPath;
        HELIUM_VERIFY( meshPath.Set(
            HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Meshes" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "TestBull.fbx" ) ) );

        GameObjectPtr spMeshObject;
        HELIUM_VERIFY( gObjectLoader->LoadObject( meshPath, spMeshObject ) );
        HELIUM_ASSERT( spMeshObject );
        HELIUM_ASSERT( spMeshObject->IsClass( Mesh::GetStaticType()->GetClass() ) );

        spMeshEntity->SetMesh( Reflect::AssertCast< Mesh >( spMeshObject.Get() ) );

        GameObjectPath animationPath;
        HELIUM_VERIFY( animationPath.Set(
            HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Animations" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "TestBull_anim.fbx" ) ) );

        GameObjectPtr spAnimationObject;
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
                //spMeshEntity.Release();
                //spSubCamera.Release();
                //spMainCamera.Release();

                if( spWorld )
                {
                    spWorld->Shutdown();
                }

                if( spSlice )
                {
                    spSlice->BindPackage( NULL );
                }

                spSlicePackage.Release();
                spSlice.Release();
                spWorld.Release();
                WorldManager::DestroyStaticInstance();

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

    if( spSlice )
    {
        spSlice->BindPackage( NULL );
    }

    spSlicePackage.Release();
    spSlice.Release();
    spWorld.Release();
    WorldManager::DestroyStaticInstance();

    DynamicDrawer::DestroyStaticInstance();
    RenderResourceManager::DestroyStaticInstance();

    Renderer::DestroyStaticInstance();

    int defaultTaskThreadCount = tbb::task_scheduler_init::default_num_threads();
    HELIUM_TRACE( TraceLevels::Info, TXT( "TBB default task thread count: %d\n" ), defaultTaskThreadCount );
    HELIUM_UNREF( defaultTaskThreadCount );

    LARGE_INTEGER perfFrequency, startCounter, endCounter;
    HELIUM_VERIFY( QueryPerformanceFrequency( &perfFrequency ) );

    // 600 seems to be a good grain size during testing (see benchmark results below for more information about the test
    // system).  Different values make work better for different platforms and CPU types.
    const size_t sortJobGrainSize = 600;

    float32_t SAMPLE_FLOAT32_SET[ 1024 ];
    String floatSetString, formatString;
    for( size_t floatIndex = 0; floatIndex < HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ); ++floatIndex )
    {
        SAMPLE_FLOAT32_SET[ floatIndex ] =
            ( static_cast< float32_t >( rand() ) / static_cast< float32_t >( RAND_MAX ) ) * 2.0f - 1.0f;

        formatString.Format( TXT( " %f" ), SAMPLE_FLOAT32_SET[ floatIndex ] );
        floatSetString += formatString;
    }

    HELIUM_TRACE( TraceLevels::Debug, TXT( "Float set:%s.\n" ), *floatSetString );

    HELIUM_SIMD_ALIGN_PRE float32_t sortedFloat32Set[ HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ) ] HELIUM_SIMD_ALIGN_POST;

    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );
        floatSetString.Clear();

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        std::sort( &sortedFloat32Set[ 0 ], &sortedFloat32Set[ HELIUM_ARRAY_COUNT( sortedFloat32Set ) ] );
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        float32_t sortTime = static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
        HELIUM_UNREF( sortTime );

        for( size_t floatIndex = 0; floatIndex < HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ); ++floatIndex )
        {
            formatString.Format( TXT( " %f" ), sortedFloat32Set[ floatIndex ] );
            floatSetString += formatString;
        }

        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sorted float set (std::sort()):%s\n" ), *floatSetString );
        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sort time (std::sort()): %f msec\n" ), sortTime );
    }

    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );
        floatSetString.Clear();

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        qsort( sortedFloat32Set, HELIUM_ARRAY_COUNT( sortedFloat32Set ), sizeof( sortedFloat32Set[ 0 ] ), FloatCompare );
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        float32_t sortTime = static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
        HELIUM_UNREF( sortTime );

        for( size_t floatIndex = 0; floatIndex < HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ); ++floatIndex )
        {
            formatString.Format( TXT( " %f" ), sortedFloat32Set[ floatIndex ] );
            floatSetString += formatString;
        }

        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sorted float set (qsort()):%s\n" ), *floatSetString );
        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sort time (qsort()): %f msec\n" ), sortTime );
    }

    {
        // Lazy method of making sure we use the time from a run-through after the job cache has already been prepared
        // (first couple run-throughs or so will create a bunch of job objects for the first time, which is inherently
        // slow, while the subsequent runs will reuse the pooled objects).
        float sortTime = 0.0f;
        for( size_t i = 0; i < 5; ++i )
        {
            MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );
            floatSetString.Clear();

            HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
            {
                // Yes, I do want to time job preparation as well...
                JobContext::Spawner< 1 > rootSpawner;

                JobContext* pContext = rootSpawner.Allocate();
                HELIUM_ASSERT( pContext );
                SortJob< float32_t >* pJob = pContext->Create< SortJob< float32_t > >();
                HELIUM_ASSERT( pJob );

                SortJob< float32_t >::Parameters& rParameters = pJob->GetParameters();
                rParameters.pBase = sortedFloat32Set;
                rParameters.count = HELIUM_ARRAY_COUNT( sortedFloat32Set );
                rParameters.singleJobCount = sortJobGrainSize;
            }
            HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
            sortTime = static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
                static_cast< float32_t >( perfFrequency.QuadPart );
        }

        for( size_t floatIndex = 0; floatIndex < HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ); ++floatIndex )
        {
            formatString.Format( TXT( " %f" ), sortedFloat32Set[ floatIndex ] );
            floatSetString += formatString;
        }

        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sorted float set (SortJob):%s\n" ), *floatSetString );
        HELIUM_TRACE( TraceLevels::Debug, TXT( "Sort time (SortJob): %f msec\n" ), sortTime );
    }

#if 0
    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );
        floatSetString.Clear();

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        for( size_t j = 0; j < 1000; ++j )
        {
            float32_t* pFloat = sortedFloat32Set;
            for( size_t i = 0; i < HELIUM_ARRAY_COUNT( sortedFloat32Set ); i += 4, pFloat += 4 )
            {
                __m128 vec = _mm_load_ps( pFloat );
#define MOVEHL_REP( Z, N, DATA ) vec = _mm_movehl_ps( vec, vec );
                BOOST_PP_REPEAT( 200, MOVEHL_REP, );
#undef MOVEHL_REP
                _mm_store_ps( pFloat, vec );
            }
        }
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        float32_t processTime = static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
        HELIUM_UNREF( processTime );

        HELIUM_TRACE( TraceLevels::Debug, TXT( "_mm_movhl_ps() work time: %f msec\n" ), processTime );
    }

    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );
        floatSetString.Clear();

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        for( size_t j = 0; j < 1000; ++j )
        {
            float32_t* pFloat = sortedFloat32Set;
            for( size_t i = 0; i < HELIUM_ARRAY_COUNT( sortedFloat32Set ); i += 4, pFloat += 4 )
            {
                __m128 vec = _mm_load_ps( pFloat );
#define SHUFFLE_REP( Z, N, DATA ) vec = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 0, 3, 2 ) );
                BOOST_PP_REPEAT( 200, SHUFFLE_REP, );
#undef SHUFFLE_REP
                _mm_store_ps( pFloat, vec );
            }
        }
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        float32_t processTime = static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
        HELIUM_UNREF( processTime );

        HELIUM_TRACE( TraceLevels::Debug, TXT( "_mm_shuffle_ps() work time: %f msec\n" ), processTime );
    }
#endif

    floatSetString.Clear();
    formatString.Clear();

    // Average timing, comparing qsort(), std::sort(), TBB parallel_sort() (as compact as you will likely get using TBB
    // directly), and our SortJob implementation (runs through additional abstraction layers provided by the engine, but
    // provides a decent starting optimization target for the job system as a whole).
    //
    // Note that non-release builds can have additional overhead due to logging and debug assertions.  It may be
    // preferable to create a release build with logging explicitly enabled (see Platform/Trace.h).
    //
    // Benchmark results on 2010-08-25 from 50,000 iterations of sorting of a pseudo-random list of 32-bit floats
    // (average times recorded):
    // - Windows 64-bit release build (w/ logging enabled)
    // - Intel Core i7-860 (4 cores w/ hyper-threading @ 2.8 GHz each, 256 KB L2 cache per core, 8 MB shared L3 cache)
    // - 8 GB RAM
    // - 64-bit Vista
    // - A whole bunch of other processes in the background (I didn't bother to shut anything down...)
    // Grain size for SortJob was tuned to 600 (that is, a job receiving 600 or less values will sort the entire chunk
    // serially instead of spawning additional children in order to reduce job scheduling overhead).  This seemed like a
    // reasonable sweet-spot for performance on the aforementioned benchmark system (both 32-bit and 64-bit builds
    // tested).
    //
    // - 100 floats:
    //   - qsort(): 0.007408 msec
    //   - std::sort(): 0.003120 msec
    //   - tbb::parallel_sort(): 0.003329 msec
    //   - SortJob: 0.001976 msec
    // - 500 floats:
    //   - qsort(): 0.044920 msec
    //   - std::sort(): 0.024747 msec
    //   - tbb::parallel_sort(): 0.023207 msec
    //   - SortJob: 0.019501 msec
    // - 1000 floats:
    //   - qsort(): 0.098956 msec
    //   - std::sort() (C++ STL): 0.054019 msec
    //   - tbb::parallel_sort(): 0.037225 msec
    //   - SortJob: 0.042649 msec
    // - 5000 floats:
    //   - qsort(): 0.577524 msec
    //   - std::sort(): 0.350688 msec
    //   - tbb::parallel_sort(): 0.123013 msec
    //   - SortJob: 0.128522 msec
    // - 10000 floats:
    //   - qsort(): 1.240391 msec
    //   - std::sort(): 0.754110 msec
    //   - tbb::parallel_sort(): 0.221948 msec
    //   - SortJob: 0.222561 msec
    // - 20000 floats:
    //   - qsort(): 2.669877 msec
    //   - std::sort(): 1.611734 msec
    //   - tbb::parallel_sort(): 0.431200 msec
    //   - SortJob: 0.402075 msec
    float32_t qsortAvg = 0.0f;
    float32_t stdSortAvg = 0.0f;
    float32_t tbbParallelAvg = 0.0f;
    float32_t jobParallelAvg = 0.0f;

    const size_t iterationCount = 500;

    for( size_t i = 0; i < iterationCount; ++i )
    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        qsort( sortedFloat32Set, HELIUM_ARRAY_COUNT( sortedFloat32Set ), sizeof( sortedFloat32Set[ 0 ] ), FloatCompare );
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        qsortAvg += static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
    }

    for( size_t i = 0; i < iterationCount; ++i )
    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        std::sort( &sortedFloat32Set[ 0 ], &sortedFloat32Set[ HELIUM_ARRAY_COUNT( sortedFloat32Set ) ] );
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        stdSortAvg += static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
    }

    for( size_t i = 0; i < iterationCount; ++i )
    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        tbb::parallel_sort( &sortedFloat32Set[ 0 ], &sortedFloat32Set[ HELIUM_ARRAY_COUNT( sortedFloat32Set ) ] );
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        tbbParallelAvg += static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
    }

    for( size_t i = 0; i < iterationCount; ++i )
    {
        MemoryCopy( sortedFloat32Set, SAMPLE_FLOAT32_SET, sizeof( SAMPLE_FLOAT32_SET ) );

        HELIUM_VERIFY( QueryPerformanceCounter( &startCounter ) );
        {
            // Yes, I do want to time job preparation as well...
            JobContext::Spawner< 1 > rootSpawner;

            JobContext* pContext = rootSpawner.Allocate();
            HELIUM_ASSERT( pContext );
            SortJob< float32_t >* pJob = pContext->Create< SortJob< float32_t > >();
            HELIUM_ASSERT( pJob );

            SortJob< float32_t >::Parameters& rParameters = pJob->GetParameters();
            rParameters.pBase = sortedFloat32Set;
            rParameters.count = HELIUM_ARRAY_COUNT( sortedFloat32Set );
            rParameters.singleJobCount = sortJobGrainSize;
        }
        HELIUM_VERIFY( QueryPerformanceCounter( &endCounter ) );
        jobParallelAvg += static_cast< float32_t >( endCounter.QuadPart - startCounter.QuadPart ) * 1000.0f /
            static_cast< float32_t >( perfFrequency.QuadPart );
    }

    qsortAvg /= static_cast< float32_t >( iterationCount );
    stdSortAvg /= static_cast< float32_t >( iterationCount );
    tbbParallelAvg /= static_cast< float32_t >( iterationCount );
    jobParallelAvg /= static_cast< float32_t >( iterationCount );

    HELIUM_TRACE(
        TraceLevels::Debug,
        TXT( "Sorting benchmark (%" ) TPRIuSZ TXT( " 32-bit floats, average over %" ) TPRIuSZ TXT( " iterations):\n" ),
        HELIUM_ARRAY_COUNT( SAMPLE_FLOAT32_SET ),
        iterationCount );
    HELIUM_TRACE( TraceLevels::Debug, TXT( "- qsort() (C-standard library): %f msec\n" ), qsortAvg );
    HELIUM_TRACE( TraceLevels::Debug, TXT( "- std::sort() (C++ STL): %f msec\n" ), stdSortAvg );
    HELIUM_TRACE( TraceLevels::Debug, TXT( "- TBB parallel_sort(): %f msec\n" ), tbbParallelAvg );
    HELIUM_TRACE( TraceLevels::Debug, TXT( "- Helium SortJob: %f msec\n" ), jobParallelAvg );

    JobManager::DestroyStaticInstance();

    Config::DestroyStaticInstance();

#if HELIUM_TOOLS
    ObjectPreprocessor::DestroyStaticInstance();
#endif
    GameObjectLoader::DestroyStaticInstance();
    CacheManager::DestroyStaticInstance();

#if HELIUM_TOOLS
    FontResourceHandler::DestroyStaticLibrary();
#endif

	UnregisterTestAppTypes();
#if HELIUM_TOOLS
    UnregisterEditorSupportTypes();
#endif
    UnregisterPcSupportTypes();
    UnregisterFrameworkTypes();
    UnregisterGraphicsTypes();
    UnregisterEngineTypes();

    GameObjectType::Shutdown();
    GameObject::Shutdown();

    AsyncLoader::DestroyStaticInstance();

    Reflect::Cleanup();

    Reflect::ObjectRefCountSupport::Shutdown();

    GameObjectPath::Shutdown();
    Name::Shutdown();

    FileLocations::Shutdown();

    ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
    DynamicMemoryHeap::LogMemoryStats();
    ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

    return windowData.resultCode;
}
