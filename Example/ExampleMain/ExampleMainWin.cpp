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


#include "Bullet/BulletEngine.h"
#include "Bullet/BulletWorld.h"
#include "Bullet/BulletWorldDefinition.h"
#include "Bullet/BulletBodyDefinition.h"
#include "Bullet/BulletShapes.h"
#include "Bullet/BulletBody.h"
#include "Bullet/BulletWorldComponent.h"

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

//		{
//			BulletShapeSpherePtr sphere = new BulletShapeSphere();
//			sphere->m_Radius = 5.0f;
//			sphere->m_Mass = 1.0f;
//
//			BulletShapeBoxPtr box = new BulletShapeBox();
//			box->m_Mass = 0.0f;
//			box->m_Extents = Vector3(50.0f, 50.0f, 50.0f);
//
//			BulletWorldDefinitionPtr spWorldDefinition;
//			BulletWorldDefinition::Create(spWorldDefinition, Name( TXT( "BulletWorldDefinition" ) ), NULL);
//			spWorldDefinition->m_Gravity = Helium::Simd::Vector3(0.0f, -9.8f, 0.0f);
//
//			BulletBodyDefinitionPtr spBodyDefinitionGround;
//			BulletBodyDefinition::Create(spBodyDefinitionGround, Name( TXT( "BulletBodyDefinition_Ground" ) ), NULL);
//			spBodyDefinitionGround->m_Shapes.New(box);
//			spBodyDefinitionGround->m_Restitution = 0.9f;
//
//			BulletBodyDefinitionPtr spBodyDefinitionSphere;
//			BulletBodyDefinition::Create(spBodyDefinitionSphere, Name( TXT( "BulletBodyDefinition_Sphere" ) ), NULL);
//			spBodyDefinitionSphere->m_Shapes.New(sphere);
//			spBodyDefinitionSphere->m_Restitution = 0.9f;
//
//			Helium::BulletWorld bullet_world;
//			bullet_world.Initialize(*spWorldDefinition);
//
//			//BulletBody groundBody;
//			//groundBody.Initialize(bullet_world, *spBodyDefinitionGround, Helium::Simd::Vector3(0.0f, -30.0f, 0.0f), Helium::Simd::Quat::IDENTITY);
//
//			//BulletBody sphereBody;
//			//sphereBody.Initialize(bullet_world, *spBodyDefinitionSphere, Helium::Simd::Vector3(0.0f, 10.0f, 0.0f), Helium::Simd::Quat::IDENTITY);
//
//			//for (int i = 0; i < 75; ++i)
//			//{
//			//	bullet_world.Simulate(0.05f);
//			//	Helium::Simd::Vector3 position;
//			//	sphereBody.GetPosition(position);
//
//			//	HELIUM_TRACE(TraceLevels::Info, "Object Position: %f  %f  %f\n", position.GetElement(0), position.GetElement(1), position.GetElement(2));
//			//}
//
//			//sphereBody.Destruct(bullet_world);
//			//groundBody.Destruct(bullet_world);
//
//#if 0
//			Helium::FilePath filePath;
//			filePath.Set(String("c://helium//Data/ExampleGame/Test.json"));
//
//			Helium::DynamicArray< uint8_t > buffer;
//			Helium::DynamicMemoryStream archiveStream ( &buffer );
//			Persist::ArchiveWriterJson archive ( &archiveStream, NULL );
//			archive.Write( spBodyDefinitionSphere.Get() );
//			archiveStream.Flush();
//
//			archiveStream.Seek(0, SeekOrigins::Begin);
//			Persist::ArchiveReaderJson archive2 ( &archiveStream, NULL );
//
//			Helium::Reflect::ObjectPtr ptr2;
//			archive2.Read(ptr2);
//
//			BulletBodyDefinition *pBd = static_cast<BulletBodyDefinition *>(ptr2.Get());
//			BulletShape *pShape = pBd->m_Shapes[0].Get();
//#endif
//
//
//			//File f;
//			//f.Open("c://helium//Data/ExampleGame/Test.json", FileMode::Write, true);
//			//f.Write(buffer.GetData(), buffer.GetSize());
//			//f.Close();
//
//			//printf("%s", buffer.GetData());
//
//			Helium::BulletBodyDefinitionPtr bd;
//
//			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
//
//			AssetPath scenePath( TXT( "/ExampleGame:PlayerAvatar_BulletBody_Body" ) );
//			pAssetLoader->LoadObject(scenePath, bd );
//
//			BulletBodyDefinition *pBd = bd.Get();
//			BulletShape *pShape = pBd->m_Shapes[0].Get();
//
//			int i = 0;
//			++i;
//		}

		{
			Helium::AssetLoader *pAssetLoader = AssetLoader::GetStaticInstance();
			Helium::SceneDefinitionPtr spSceneDefinition;

			AssetPath scenePath( TXT( "/ExampleGame/Scenes/TestScene:SceneDefinition" ) );
			pAssetLoader->LoadObject(scenePath, spSceneDefinition );

			HELIUM_ASSERT( !spSceneDefinition->GetAllFlagsSet( Asset::FLAG_BROKEN ) );

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
