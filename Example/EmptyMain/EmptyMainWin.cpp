#include "EmptyMainPch.h"

#include "Components/ComponentsPch.h"
#include "EditorSupport/EditorSupportPch.h"
#include "Bullet/BulletPch.h"

#include "Ois/OisSystem.h"


#include "Framework/SceneDefinition.h"
#include "Framework/WorldManager.h"

#include "Rendering/Renderer.h"

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
	ForceLoadBulletDll();
	ForceLoadComponentsDll();

#if HELIUM_TOOLS
	ForceLoadEditorSupportDll();
#endif

	HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );

	int32_t result = 0;

	{
		// Initialize a GameSystem instance.
		CommandLineInitializationImpl commandLineInitialization;
		MemoryHeapPreInitializationImpl memoryHeapPreInitialization;
		AssetLoaderInitializationImpl assetLoaderInitialization;
		ConfigInitializationImpl configInitialization;
#if HELIUM_OS_WIN
		WindowManagerInitializationImpl windowManagerInitialization( hInstance, nCmdShow );
#else
		WindowManagerInitializationImpl windowManagerInitialization;
#endif
		RendererInitializationImpl rendererInitialization;
		AssetPath systemDefinitionPath( "/ExampleGames/Empty:System" );
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
			systemDefinitionPath);
		
		{
			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
			Helium::SceneDefinitionPtr spSceneDefinition;

			AssetPath scenePath( TXT( "/ExampleGames/Empty/Scenes/TestScene:SceneDefinition" ) );
			pAssetLoader->LoadObject(scenePath, spSceneDefinition );

			pGameSystem->LoadScene(spSceneDefinition.Get());
		}

		if( bSystemInitSuccess )
		{
			void *windowHandle = rendererInitialization.GetMainWindow()->GetHandle();
			Input::Initialize(&windowHandle, false);

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
