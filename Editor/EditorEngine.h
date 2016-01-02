#pragma once

#include "Platform/Utility.h"
#include "Foundation/Map.h"
#include "Framework/Slice.h"
#include "Framework/TaskScheduler.h"
#include "Editor/Proxy/SceneProxy.h"
#include "EditorScene/SceneManager.h"
#include "Application/TimerThread.h"

namespace Helium
{
	namespace Editor
	{
		class EditorEngine;

		// Provides API to allow the editor to force packages and their assets into existence
		class ForciblyFullyLoadedPackageManager : NonCopyable
		{
		public:
			static ForciblyFullyLoadedPackageManager* GetInstance();
			static void Startup();
			static void Shutdown();

			void Tick();

			void ForceFullyLoadRootPackages();
			void ForceFullyLoadPackage( const AssetPath &path );

			bool IsPackageForcedFullyLoaded( const AssetPath &path );

			AssetEventSignature::Event e_AssetForciblyLoadedEvent;

		private:
			struct ForciblyFullyLoadedPackage
			{
				AssetPath m_PackagePath;
				size_t m_PackageLoadId;
				StrongPtr< Package > m_Package;

				// First entry is always the package
				DynamicArray< AssetPath >        m_AssetPaths;
				DynamicArray< size_t >           m_AssetLoadIds;
				DynamicArray< StrongPtr<Asset> > m_Assets;
			};

			DynamicArray< ForciblyFullyLoadedPackage > m_ForciblyFullyLoadedPackages;

			/// Singleton instance.
			static uint32_t sm_InitCount;
			static ForciblyFullyLoadedPackageManager* sm_pInstance;
		};

		// Buffers events from AssetTracker so that we can fire them in the wxWidgets thread
		class ThreadSafeAssetTrackerListener
		{
		public:
			ThreadSafeAssetTrackerListener();
			~ThreadSafeAssetTrackerListener();

			static ThreadSafeAssetTrackerListener* GetInstance();
			static void Startup();
			static void Shutdown();

			void Sync();

			AssetEventSignature::Event e_AssetLoaded;
			AssetEventSignature::Event e_AssetChanged;
			AssetEventSignature::Event e_AssetCreatedExternally;
			AssetEventSignature::Event e_AssetChangedExternally;

		private:
			void OnAssetLoaded( const AssetEventArgs &args );
			void OnAssetChanged( const AssetEventArgs &args );
			void OnAssetCreatedExternally( const AssetEventArgs &args );
			void OnAssetChangedExternally( const AssetEventArgs &args );

			struct Buffer
			{
				DynamicArray<AssetEventArgs> m_Loaded;
				DynamicArray<AssetEventArgs> m_Changed;
				DynamicArray<AssetEventArgs> m_CreatedExternally;
				DynamicArray<AssetEventArgs> m_ChangedExternally;
			};

			Buffer m_Buffers[2];
			Mutex m_Lock;
			uint8_t m_GameThreadBufferIndex;

			/// Singleton instance.
			static uint32_t sm_InitCount;
			static ThreadSafeAssetTrackerListener* sm_pInstance;
		};

		class EngineTickTimer : public wxTimer
		{
		public:
			EngineTickTimer( EditorEngine* engine );
			~EngineTickTimer();

			virtual void Notify() HELIUM_OVERRIDE;

		private:
			EditorEngine* m_Engine;
			AssetAwareThreadSynchronizer m_AssetSyncUtil;
		};

		class EditorEngine : NonCopyable
		{
		public:
			EditorEngine();
			~EditorEngine();

			bool Initialize( Editor::SceneManager* sceneManager, void* hwnd );
			void Cleanup();

			void Tick();

		private:
			void DoAssetManagerThread();

			Editor::SceneManager* m_SceneManager;
			CallbackThread m_TickAssetManagerThread;
			bool m_bTerminateAssetManagerThread;
			EngineTickTimer m_EngineTickTimer;
			TaskSchedule m_Schedule;
		};
	}
}
