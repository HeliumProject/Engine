//----------------------------------------------------------------------------------------------------------------------
// ExampleMainWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "ExampleMainPch.h"

#include "Components/ComponentsPch.h"
#include "EditorSupport/EditorSupportPch.h"
#include "Bullet/BulletPch.h"

#include "Ois/OisSystem.h"

#include "Framework/SceneDefinition.h"
#include "Framework/WorldManager.h"

#include "Rendering/Renderer.h"
#include "Windowing/Window.h"


#include "ExampleGame/Components/Graphics/Sprite.h"


using namespace Helium;

/// Windows application entry point.
///
/// @param[in] hInstance      Handle to the current instance of the application.
/// @param[in] hPrevInstance  Handle to the previous instance of the application (always null; ignored).
/// @param[in] lpCmdLine      Command line for the application, excluding the program name.
/// @param[in] nCmdShow       Flags specifying how the application window should be shown.
///
/// @return  Result code of the application.
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR /*lpCmdLine*/, int nCmdShow )
{
	ForceLoadBulletDll();
	ForceLoadComponentsDll();
	ForceLoadEditorSupportDll();
	ForceLoadExampleGameDll();

	HELIUM_TRACE_SET_LEVEL( TraceLevels::Debug );

	int32_t result = 0;

	{
		// Initialize a GameSystem instance.
		CommandLineInitializationWin commandLineInitialization;
		MemoryHeapPreInitializationWin memoryHeapPreInitialization;
		AssetLoaderInitializationWin assetLoaderInitialization;
		ConfigInitializationWin configInitialization;
		WindowManagerInitializationWin windowManagerInitialization( hInstance, nCmdShow );
		RendererInitializationWin rendererInitialization;
		//NullRendererInitialization rendererInitialization;

		GameSystem* pGameSystem = GameSystem::CreateStaticInstance();
		HELIUM_ASSERT( pGameSystem );
		bool bSystemInitSuccess = pGameSystem->Initialize(
			commandLineInitialization,
			memoryHeapPreInitialization,
			assetLoaderInitialization,
			configInitialization,
			windowManagerInitialization,
			rendererInitialization);
		
// 		{
// 			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
// 			Helium::AssetPtr spAsset;
// 
// 			AssetPath scenePath( TXT( "/ExampleGame/Scenes/TestScene:TestBull_Sprite" ) );
// 			pAssetLoader->LoadObject(scenePath, spAsset );
// 
// 
// 			ExampleGame::SpriteComponentDefinition *asset = Reflect::AssertCast<ExampleGame::SpriteComponentDefinition>(spAsset.Get());
// 			Texture *pTexture = asset->GetTexture();
// 		}

		{
			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
			Helium::SceneDefinitionPtr spSceneDefinition;

			AssetPath scenePath( TXT( "/ExampleGame/Scenes/TestScene:SceneDefinition" ) );
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
