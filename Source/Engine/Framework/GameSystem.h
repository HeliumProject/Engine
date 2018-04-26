#pragma once

#include "Platform/Utility.h"
#include "Framework/SystemDefinition.h"
#include "Framework/TaskScheduler.h"

#define NO_GFX (1)

namespace Helium
{
	class AssetType;

	class MemoryHeapPreInitialization;
	class AssetLoaderInitialization;
	class ConfigInitialization;
	class WindowManagerInitialization;
	class RendererInitialization;
	class SceneDefinition;
	class Window;
	class World;

	/// Base interface for game application systems.
	class HELIUM_FRAMEWORK_API GameSystem : NonCopyable
	{
	public:
		/// @name Construction/Destruction
		//@{
		GameSystem();
		//@}

		/// @name Initialization
		//@{
		virtual bool Initialize(
			MemoryHeapPreInitialization& rMemoryHeapPreInitialization,
			AssetLoaderInitialization& rAssetLoaderInitialization, 
			ConfigInitialization& rConfigInitialization,
			WindowManagerInitialization& rWindowManagerInitialization,
			RendererInitialization& rRendererInitialization,
			AssetPath &rSystemDefinitionPath);
		virtual void Cleanup();
		
		World *LoadScene( Helium::SceneDefinition *spSceneDefinition );
		//@}

		/// @name Application Loop
		//@{
		virtual int32_t Run();
		//@}

		/// @name Static Initialization
		//@{
		static GameSystem* GetInstance();
		static void Startup();
		static void Shutdown();
		//@}

		virtual void StopRunning();

	protected:
		/// Module file name.
		String m_moduleName;

		/// Singleton instance.
		static GameSystem* sm_pInstance;

		AssetLoaderInitialization*   m_pAssetLoaderInitialization;
		RendererInitialization*      m_pRendererInitialization;
		WindowManagerInitialization* m_pWindowManagerInitialization;
		SystemDefinitionPtr          m_spSystemDefinition;
		AssetAwareThreadSynchronizer m_AssetSyncUtility;
		TaskSchedule                 m_Schedule;
		bool                         m_bStopRunning;
	};
}
