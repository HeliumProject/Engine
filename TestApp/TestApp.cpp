#include "TestAppPch.h"
#include "TestApp.h"

#include "Components/ComponentsPch.h"

#include "Reflect/Registry.h"
#include "Reflect/TranslatorDeduction.h"

#include "Math/Color4.h"

#include "Engine/Asset.h"

#include "Persist/ArchiveJson.h"
#include "Persist/ArchiveMessagePack.h"

#include "PcSupport/LooseAssetLoader.h"
#include "PcSupport/LoosePackageLoader.h"

#include "gtest.h"
#include "TestAsset.h"
#include "WindowProc.h"

#include <cfloat>
#include <ctime>

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"
#include "Framework/World.h"
#include "Framework/WorldDefinition.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"
#include "Components/ComponentJobs.h"

#include "Engine/Components.h"
#include "Engine/TaskScheduler.h"

#include "Bullet/BulletEngine.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"
#include "Bullet/BulletBody.h"
#include "Bullet/BulletWorldComponent.h"

#include "Ois/OisSystem.h"

using namespace Helium;

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
	HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );

	Timer::StaticInitialize();
	
#if !HELIUM_RELEASE && !HELIUM_PROFILE
	Helium::InitializeSymbols();
#endif

	AsyncLoader::GetStaticInstance().Initialize();

	FilePath baseDirectory;
	if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
		return -1;
	}

	HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );
	Helium::Bullet::Initialize();

	int resultCode = -1;

	{
	Reflect::Initialize();

	Helium::Components::Initialize();
	
	Helium::TaskScheduler::CalculateSchedule();

#if HELIUM_TOOLS
#endif

	InitEngineJobsDefaultHeap();
	InitGraphicsJobsDefaultHeap();
	InitTestJobsDefaultHeap();

#if HELIUM_TOOLS
	FontResourceHandler::InitializeStaticLibrary();
#endif
	
	ForceLoadComponentsDll();

#if HELIUM_TOOLS
	//HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );
	HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );

	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
	HELIUM_ASSERT( pAssetPreprocessor );
	PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
	HELIUM_ASSERT( pPlatformPreprocessor );
	pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
	HELIUM_VERIFY( PcCacheAssetLoader::InitializeStaticInstance() );
#endif

	gAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( gAssetLoader );
	
	Config& rConfig = Config::GetStaticInstance();
	rConfig.BeginLoad();
	while( !rConfig.TryFinishLoad() )
	{
		gAssetLoader->Tick();
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

	Input::Initialize(&hMainWnd, false);

	RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
	rRenderResourceManager.Initialize();
	rRenderResourceManager.UpdateMaxViewportSize( displayWidth, displayHeight );

	//// Create a scene definition
	SceneDefinitionPtr spSceneDefinition;
	gAssetLoader->LoadObject( AssetPath( TXT( "/ExampleGame/Scenes/TestScene:SceneDefinition" ) ), spSceneDefinition );

	EntityDefinitionPtr spEntityDefinition;
	gAssetLoader->LoadObject( AssetPath( TXT( "/ExampleGame/Scenes/TestScene:TestBull_Entity" ) ), spEntityDefinition );

	DynamicDrawer& rDynamicDrawer = DynamicDrawer::GetStaticInstance();
	HELIUM_VERIFY( rDynamicDrawer.Initialize() );

	RRenderContextPtr spMainRenderContext = pRenderer->GetMainContext();
	HELIUM_ASSERT( spMainRenderContext );
	
	WorldManager& rWorldManager = WorldManager::GetStaticInstance();
	HELIUM_VERIFY( rWorldManager.Initialize() );

	// Create a world
	WorldPtr spWorld( rWorldManager.CreateWorld( spSceneDefinition ) );
	HELIUM_ASSERT( spWorld );
	HELIUM_TRACE( TraceLevels::Info, TXT( "Created world \"%s\".\n" ), *spSceneDefinition->GetPath().ToString() );

	//Slice *pRootSlice = spWorld->GetRootSlice();
	//Entity *pEntity = pRootSlice->CreateEntity(spEntityDefinition);
			
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
	
	float32_t meshRotation = 0.0f;

	spSubRenderContext.Release();
	spMainRenderContext.Release();

	while( windowData.bProcessMessages )
	{
		
		BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
		rSceneDrawer.DrawScreenText(
			20,
			20,
			String( TXT( "RUNNING" ) ),
			Color( 0xff00ff00 ),
			RenderResourceManager::DEBUG_FONT_SIZE_LARGE );
		rSceneDrawer.DrawScreenText(
			21,
			20,
			String( TXT( "RUNNING" ) ),
			Color( 0xff00ff00 ),
			RenderResourceManager::DEBUG_FONT_SIZE_LARGE );

		if (Input::IsKeyDown(Input::KeyCodes::KC_A))
		{
			HELIUM_TRACE( TraceLevels::Info, TXT( "A is down" ) );
		}

		if (Input::IsKeyDown(Input::KeyCodes::KC_ESCAPE))
		{
			HELIUM_TRACE( TraceLevels::Info, TXT( "Exiting" ) );
			break;
		}

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

				break;
			}

			if( message.message == WM_QUIT )
			{
				windowData.bProcessMessages = false;
				windowData.resultCode = static_cast< int >( message.wParam );
				resultCode = static_cast< int >( message.wParam );

				break;
			}
		}
			
		Helium::TaskScheduler::ExecuteSchedule();
		rWorldManager.Update();

		Helium::Components::ProcessPendingDeletes();

#if GRAPHICS_SCENE_BUFFERED_DRAWER
		if( pGraphicsScene )
		{
			BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
			rSceneDrawer.DrawScreenText(
				20,
				20,
				String( TXT( "Debug text test!" ) ),
				Color( 0xffffffff ) );
		}
#endif
	}

	if( spWorld )
	{
		spWorld->Shutdown();
	}

	spWorld.Release();
	}
	WorldManager::DestroyStaticInstance();
	

	DynamicDrawer::DestroyStaticInstance();
	RenderResourceManager::DestroyStaticInstance();

	Helium::Input::Cleanup();

	Renderer::DestroyStaticInstance();
	

	JobManager::DestroyStaticInstance();

	Config::DestroyStaticInstance();

#if HELIUM_TOOLS
	AssetPreprocessor::DestroyStaticInstance();
#endif
	AssetLoader::DestroyStaticInstance();
	CacheManager::DestroyStaticInstance();

#if HELIUM_TOOLS
	FontResourceHandler::DestroyStaticLibrary();
#endif

	Helium::Components::Cleanup();
	

	Reflect::Cleanup();
	AssetType::Shutdown();
	Asset::Shutdown();


	Reflect::ObjectRefCountSupport::Shutdown();
	Helium::Bullet::Cleanup();

	AssetPath::Shutdown();
	Name::Shutdown();

	FileLocations::Shutdown();

	ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
	DynamicMemoryHeap::LogMemoryStats();
	ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

	return resultCode;
}
