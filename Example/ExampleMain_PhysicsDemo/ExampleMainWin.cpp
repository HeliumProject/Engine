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

#include "Bullet/BulletEngine.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"
#include "Bullet/BulletBody.h"
#include "Bullet/BulletWorldComponent.h"

#include "Persist/ArchiveJson.h"
#include "Foundation/MemoryStream.h"

#include "Framework/ParameterSet.h"

#include "ExampleGame/Components/Graphics/ScreenSpaceText.h"

using namespace Helium;
using namespace ExampleGame;

namespace Helium
{
	Helium::DynamicMemoryHeap& GetComponentsDefaultHeap();
	Helium::DynamicMemoryHeap& GetEditorSupportDefaultHeap();
	Helium::DynamicMemoryHeap& GetBulletDefaultHeap();
}

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

	Helium::GetComponentsDefaultHeap();
	Helium::GetEditorSupportDefaultHeap();
	Helium::GetBulletDefaultHeap();

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
		//NullRendererInitialization rendererInitialization;
		AssetPath systemDefinitionPath( "/ExampleGames/PhysicsDemo:System" );

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
		
		if( bSystemInitSuccess )
		{
			World *pWorld = NULL; 

			{
				AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
				SceneDefinitionPtr spSceneDefinition;

				AssetPath scenePath( TXT( "/ExampleGames/PhysicsDemo/Scenes/TestScene:SceneDefinition" ) );
				pAssetLoader->LoadObject(scenePath, spSceneDefinition );

				HELIUM_ASSERT( !spSceneDefinition->GetAllFlagsSet( Asset::FLAG_BROKEN ) );

				pWorld = pGameSystem->LoadScene(spSceneDefinition.Get());
			}

			HELIUM_ASSERT( pWorld );

			if ( pWorld )
			{
				AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();

				EntityDefinitionPtr spCubeDefinition;
				EntityDefinitionPtr spSphereDefinition;

				AssetPath spCubePath( TXT( "/ExampleGames/PhysicsDemo:Cube" ) );
				AssetPath spSpherePath( TXT( "/ExampleGames/PhysicsDemo:Sphere" ) );

				pAssetLoader->LoadObject(spCubePath, spCubeDefinition );
				pAssetLoader->LoadObject(spSpherePath, spSphereDefinition );

				Helium::StrongPtr< ParameterSet_InitLocated > locatedParamSet( new ParameterSet_InitLocated() );
				locatedParamSet->m_Position = Simd::Vector3::Zero;
				locatedParamSet->m_Rotation = Simd::Quat::IDENTITY;

				Simd::Vector3 &position = locatedParamSet->m_Position;
				Simd::Quat &rotation = locatedParamSet->m_Rotation;

				for (int i = 0; i < 25; ++i)
				{
					position = Simd::Vector3(
						50.0f * static_cast<float>(i / 5) - 100.0f + Helium::Ran(-10.0f, 10.0f), 
						Helium::Ran(150.0f, 200.0f), 
						50.0f * static_cast<float>(i % 5) - 100.0f + Helium::Ran(-10.0f, 10.0f));
					pWorld->GetRootSlice()->CreateEntity(spCubeDefinition, locatedParamSet.Get());
				}

				for (int i = 0; i < 25; ++i)
				{
					position = Simd::Vector3(
						50.0f * static_cast<float>(i / 5) - 100.0f + Helium::Ran(-10.0f, 10.0f), 
						Helium::Ran(250.0f, 300.0f), 
						50.0f * static_cast<float>(i % 5) - 100.0f + Helium::Ran(-10.0f, 10.0f));
					pWorld->GetRootSlice()->CreateEntity(spSphereDefinition, locatedParamSet.Get());
				}

				for (int i = 0; i < 25; ++i)
				{
					position = Simd::Vector3(
						50.0f * static_cast<float>(i / 5) - 100.0f + Helium::Ran(-10.0f, 10.0f), 
						Helium::Ran(350.0f, 400.0f), 
						50.0f * static_cast<float>(i % 5) - 100.0f + Helium::Ran(-10.0f, 10.0f));
					pWorld->GetRootSlice()->CreateEntity(spCubeDefinition, locatedParamSet.Get());
				}

				for (int i = 0; i < 25; ++i)
				{
					position = Simd::Vector3(
						50.0f * static_cast<float>(i / 5) - 100.0f + Helium::Ran(-10.0f, 10.0f), 
						Helium::Ran(450.0f, 500.0f), 
						50.0f * static_cast<float>(i % 5) - 100.0f + Helium::Ran(-10.0f, 10.0f));
					pWorld->GetRootSlice()->CreateEntity(spSphereDefinition, locatedParamSet.Get());
				}

				for (int i = 0; i < 25; ++i)
				{
					position = Simd::Vector3(
						50.0f * static_cast<float>(i / 5) - 100.0f + Helium::Ran(-10.0f, 10.0f), 
						Helium::Ran(550.0f, 600.0f), 
						50.0f * static_cast<float>(i % 5) - 100.0f + Helium::Ran(-10.0f, 10.0f));
					pWorld->GetRootSlice()->CreateEntity(spCubeDefinition, locatedParamSet.Get());
				}

				void *windowHandle = rendererInitialization.GetMainWindow()->GetHandle();
				Input::Initialize(&windowHandle, true);
				Input::SetWindowSize( 
					rendererInitialization.GetMainWindow()->GetWidth(),
					rendererInitialization.GetMainWindow()->GetHeight());

				// Run the application.
				result = pGameSystem->Run();
			}
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
