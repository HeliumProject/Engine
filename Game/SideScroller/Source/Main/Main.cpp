#include "ExampleMainPch.h"

#include "Components/ComponentsPch.h"
#include "EditorSupport/EditorSupportPch.h"
#include "Bullet/BulletPch.h"

#include "Ois/OisSystem.h"

#include "Framework/SceneDefinition.h"
#include "Framework/WorldManager.h"
#include "Engine/AssetPath.h"

#include "Rendering/Renderer.h"
#include "Windowing/Window.h"

#include "ExampleGame/Components/Graphics/Sprite.h"

#include "Bullet/BulletEngine.h"

#include "Framework/StateMachine.h"

#include "Persist/ArchiveJson.h"
#include "Foundation/MemoryStream.h"

using namespace Helium;

/// Windows application entry point.
///
/// @param[in] hInstance      Handle to the current instance of the application.
/// @param[in] hPrevInstance  Handle to the previous instance of the application (always null; ignored).
/// @param[in] lpCmdLine      Command line for the application, excluding the program name.
/// @param[in] nCmdShow       Flags specifying how the application window should be shown.
///
/// @return  Result code of the application.
#if HELIUM_OS_WIN
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
#else
int main( int argc, const char* argv[] )
#endif
{
	HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );

	int32_t result = 0;

	{
		// Initialize a GameSystem instance.
		CommandLineInitializationImpl commandLineInitialization;
		MemoryHeapPreInitializationImpl memoryHeapPreInitialization;
		AssetLoaderInitializationImpl assetLoaderInitialization;
		ConfigInitializationImpl configInitialization;
#if HELIUM_DIRECT3D
		WindowManagerInitializationImpl windowManagerInitialization( hInstance, nCmdShow );
#else
		WindowManagerInitializationImpl windowManagerInitialization;
#endif
		RendererInitializationImpl rendererInitialization;
		AssetPath systemDefinitionPath( "/ExampleGames/SideScroller:System" );
		//NullRendererInitialization rendererInitialization;

		GameSystem* pGameSystem = GameSystem::CreateStaticInstance();
		HELIUM_ASSERT( pGameSystem );
		bool bSystemInitSuccess = pGameSystem->Initialize(
			commandLineInitialization,
			memoryHeapPreInitialization,
			assetLoaderInitialization,
			configInitialization,
			windowManagerInitialization,
			rendererInitialization,
			systemDefinitionPath
			);
		
		{
			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
			Helium::SceneDefinitionPtr spSceneDefinition;

			AssetPath scenePath( TXT( "/ExampleGames/SideScroller/Scenes/TestScene:SceneDefinition" ) );
			pAssetLoader->LoadObject(scenePath, spSceneDefinition );

			HELIUM_ASSERT( !spSceneDefinition->GetAllFlagsSet( Asset::FLAG_BROKEN ) );

			World *world = pGameSystem->LoadScene(spSceneDefinition.Get());
		}

		if( bSystemInitSuccess )
		{
			Window::NativeHandle windowHandle = rendererInitialization.GetMainWindow()->GetNativeHandle();
			Input::Initialize(windowHandle, false);
			Input::SetWindowSize( 
				rendererInitialization.GetMainWindow()->GetWidth(),
				rendererInitialization.GetMainWindow()->GetHeight());

			// Run the application.
			result = pGameSystem->Run();
		}

		// Shut down and destroy the system.
		pGameSystem->Shutdown();
		System::DestroyStaticInstance();
	}

	// Perform final cleanup.
	ThreadLocalStackAllocator::ReleaseMemoryHeap();

#if HELIUM_ENABLE_MEMORY_TRACKING
	DynamicMemoryHeap::LogMemoryStats();
	ThreadLocalStackAllocator::ReleaseMemoryHeap();
#endif

	return result;
}
