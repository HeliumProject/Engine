#include "TestAppPch.h"
#include "TestApp.h"

#include "Components/ComponentsPch.h"

#include "Reflect/Registry.h"
#include "Reflect/TranslatorDeduction.h"

#include "Math/Color4.h"

#include "Persist/ArchiveJson.h"
#include "Persist/ArchiveMessagePack.h"

#if HELIUM_TOOLS
# include "PcSupport/LooseAssetLoader.h"
# include "PcSupport/LoosePackageLoader.h"
#else
# include "Engine/CacheAssetLoader.h"
#endif

#include "TestAsset.h"
#include "WindowProc.h"

#include <cfloat>
#include <ctime>

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"
#include "Framework/World.h"
#include "Framework/WorldDefinition.h"
#include "Framework/Entity.h"
#include "Framework/EntityDefinition.h"
#include "Framework/SceneDefinition.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"
#include "Components/RotateComponent.h"
#include "Components/ComponentTasks.h"

#include "Engine/Asset.h"
#include "Framework/Components.h"
#include "Framework/TaskScheduler.h"

#include "Graphics/Texture2d.h"
#include "Graphics/GraphicsManagerComponent.h"

#include "EditorSupport/EditorSupportPch.h"

#include "Bullet/BulletEngine.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"
#include "Bullet/BulletBody.h"
#include "Bullet/BulletWorldComponent.h"

#include "Ois/OisSystem.h"

#define MULTI_WINDOW 0

using namespace Helium;

int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
	ForceLoadComponentsDll();

#if HELIUM_TOOLS
	ForceLoadEditorSupportDll();
#endif

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
	//HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );
	HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );

	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
	HELIUM_ASSERT( pAssetPreprocessor );
	PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
	HELIUM_ASSERT( pPlatformPreprocessor );
	pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );
#else
	HELIUM_VERIFY( CacheAssetLoader::InitializeStaticInstance() );
#endif

#if !GTEST
	AssetLoader* gAssetLoader = NULL;
#endif

	gAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( gAssetLoader );

	Config& rConfig = Config::GetStaticInstance();
	rConfig.BeginLoad();
	while( !rConfig.TryFinishLoad() )
	{
		gAssetLoader->Tick();
	}

#if HELIUM_TOOLS
	ConfigPc::SaveUserConfig();
#endif

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
#if MULTI_WINDOW
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
#endif

	windowData.hMainWnd = hMainWnd;
	SetWindowLongPtr( hMainWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( &windowData ) );
	ShowWindow( hMainWnd, nCmdShow );
	UpdateWindow( hMainWnd );
#if MULTI_WINDOW
	windowData.hSubWnd = hSubWnd;
	SetWindowLongPtr( hSubWnd, GWLP_USERDATA, reinterpret_cast< LONG_PTR >( &windowData ) );
	ShowWindow( hSubWnd, nCmdShow );
	UpdateWindow( hSubWnd );
#endif

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
#if MULTI_WINDOW
	contextInitParams.pWindow = hSubWnd;
	RRenderContextPtr spSubRenderContext = pRenderer->CreateSubContext( contextInitParams );
	HELIUM_ASSERT( spSubRenderContext );
#endif

	Input::Initialize(&hMainWnd, false);

	{
		AssetPath prePassShaderPath;
		HELIUM_VERIFY( prePassShaderPath.Set(
			HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Shaders" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "PrePass.hlsl" ) ) );

		AssetPtr spPrePassShader;
		HELIUM_VERIFY( AssetLoader::GetStaticInstance()->LoadObject( prePassShaderPath, spPrePassShader ) );

		HELIUM_ASSERT( spPrePassShader.Get() );
	}

	RenderResourceManager& rRenderResourceManager = RenderResourceManager::GetStaticInstance();
	rRenderResourceManager.Initialize();
	rRenderResourceManager.UpdateMaxViewportSize( displayWidth, displayHeight );

	//// Create a scene definition
	SceneDefinitionPtr spSceneDefinition;
	gAssetLoader->LoadObject( AssetPath( TXT( "/ExampleGames/Empty/Scenes/TestScene:SceneDefinition" ) ), spSceneDefinition );

	EntityDefinitionPtr spEntityDefinition;
	gAssetLoader->LoadObject( AssetPath( TXT( "/ExampleGames/Empty/Scenes/TestScene:TestBull_Entity" ) ), spEntityDefinition );

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
			
	GraphicsScene* pGraphicsScene = spWorld->GetComponents().GetFirst<GraphicsManagerComponent>()->GetGraphicsScene();
	HELIUM_ASSERT( pGraphicsScene );
	GraphicsSceneView* pMainSceneView = NULL;
	if( pGraphicsScene )
	{
		uint32_t mainSceneViewId = pGraphicsScene->AllocateSceneView();
		if( IsValid( mainSceneViewId ) )
		{
			float32_t aspectRatio =
				static_cast< float32_t >( displayWidth ) / static_cast< float32_t >( displayHeight );

			RSurface* pDepthStencilSurface = rRenderResourceManager.GetDepthStencilSurface();
			HELIUM_ASSERT( pDepthStencilSurface );

			pMainSceneView = pGraphicsScene->GetSceneView( mainSceneViewId );
			HELIUM_ASSERT( pMainSceneView );
			pMainSceneView->SetRenderContext( spMainRenderContext );
			pMainSceneView->SetDepthStencilSurface( pDepthStencilSurface );
			pMainSceneView->SetAspectRatio( aspectRatio );
			pMainSceneView->SetViewport( 0, 0, displayWidth, displayHeight );
			pMainSceneView->SetClearColor( Color( 0x00202020 ) );

			//spMainCamera->SetSceneViewId( mainSceneViewId );

#if MULTI_WINDOW
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
#endif
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

		//rSceneDrawer.Draw

		//Helium::DynamicDrawer &drawer = DynamicDrawer::GetStaticInstance();
		//drawer.
#endif
	}

	rWorldManager.Update();
	
	float time = 0.0f;

#if MULTI_WINDOW
	spSubRenderContext.Release();
#endif
	spMainRenderContext.Release();

	
	Helium::StrongPtr<Helium::Texture2d> texture;
	gAssetLoader->LoadObject( AssetPath( TXT( "/Textures:Triangle.png" ) ), texture);

	Helium::RTexture2d *rTexture2d = texture->GetRenderResource2d();


	while( windowData.bProcessMessages )
	{
#if GRAPHICS_SCENE_BUFFERED_DRAWER
		BufferedDrawer& rSceneDrawer = pGraphicsScene->GetSceneBufferedDrawer();
		rSceneDrawer.DrawScreenText(
			20,
			20,
			String( TXT( "RUNNING" ) ),
			Color( 0xffffffff ),
			RenderResourceManager::DEBUG_FONT_SIZE_LARGE );
		rSceneDrawer.DrawScreenText(
			21,
			20,
			String( TXT( "RUNNING" ) ),
			Color( 0xffffffff ),
			RenderResourceManager::DEBUG_FONT_SIZE_LARGE );

		time += 0.01f;
		DynamicArray<SimpleVertex> verticesU;

		verticesU.New( -100.0f, -100.0f, 750.0f );
		verticesU.New( 100.0f, -100.0f, 750.0f );
		verticesU.New( 100.0f, 100.0f, 750.0f );
		verticesU.New( -100.0f, 100.0f, 750.0f );
		
		rSceneDrawer.DrawLineList( verticesU.GetData(), static_cast<uint32_t>( verticesU.GetSize() ) );
		
		DynamicArray<SimpleTexturedVertex> verticesT;
		verticesT.New( Simd::Vector3( -100.0f, 100.0f, 750.0f ), Simd::Vector2( 0.0f, 0.0f ) );
		verticesT.New( Simd::Vector3( 100.0f, 100.0f, 750.0f ), Simd::Vector2( 1.0f, 0.0f ) );
		verticesT.New( Simd::Vector3( -100.0f, -100.0f, 750.0f ), Simd::Vector2( 0.0f, 1.0f ) );
		verticesT.New( Simd::Vector3( 100.0f, -100.0f, 750.0f ), Simd::Vector2( 1.0f, 1.0f ) );
		

		//rSceneDrawer.DrawTextured(
		//	RENDERER_PRIMITIVE_TYPE_TRIANGLE_STRIP,
		//	verticesT.GetData(),
		//	verticesT.GetSize(),
		//	NULL,
		//	2,
		//	rTexture2d, Helium::Color(1.0f, 1.0f, 1.0f, 1.0f), Helium::RenderResourceManager::RASTERIZER_STATE_DEFAULT, Helium::RenderResourceManager::DEPTH_STENCIL_STATE_NONE);

		//rSceneDrawer.DrawTexturedQuad(rTexture2d);

		Helium::Simd::Matrix44 transform = Helium::Simd::Matrix44::IDENTITY;
		
		Simd::Vector3 location(0.0f, 400.0f, 0.0f);
		Simd::Quat rotation(Helium::Simd::Vector3::BasisZ, time);
		Simd::Vector3 scale(1000.0f, 1000.0f, 1000.0f);
		


		transform.SetRotationTranslationScaling(rotation, location, scale);
		rSceneDrawer.DrawTexturedQuad(rTexture2d, transform, Simd::Vector2(0.0f, 0.0f), Simd::Vector2(0.5f, 0.5f));
#endif
		
		//Helium::Simd::Vector3 up = Simd::Vector3::BasisY;
		////Helium::Simd::Vector3 eye(5000.0f * sin(time), 0.0f, 5000.0f * cos(time));
		//Helium::Simd::Vector3 eye(0.0f, 0.0f, -1000.0f);
		//Helium::Simd::Vector3 forward = Simd::Vector3::Zero - eye;
		//forward.Normalize();

		////pMainSceneView->SetClearColor( Color( 0xffffffff ) );
		//pMainSceneView->SetView(eye, forward, up);




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

		rWorldManager.Update();

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
