#pragma once

namespace Helium
{
	class LoosePackageLoader;

	class HELIUM_PC_SUPPORT_API LooseAssetFileWatcher
	{
	public:
		LooseAssetFileWatcher();
		virtual ~LooseAssetFileWatcher();

		void AddPackage( LoosePackageLoader *pPackageLoader );
		void RemovePackage( LoosePackageLoader *pPackageLoader );

		void StartThread();
		void StopThread();
		inline bool IsThreadRunning();

		void TrackEverything();

	protected:
		Helium::CallbackThread m_Thread;
		bool m_StopTracking;
		volatile int m_InterruptTracking;

		struct WatchedAsset
		{
			int64_t m_LastMessageTime;
		};

		struct WatchedPackage
		{
			FilePath m_Path;
			LoosePackageLoader *m_Loader;

			HashMap< Name, WatchedAsset > m_Assets;
		};

		DynamicArray<WatchedPackage> m_PathsToWatch;
		SpinLock m_PathsToWatchLock;

		DynamicArray<AssetPath> m_ChangeNotifications;
		DynamicArray<AssetPath> m_NewNotifications;
	};
}

#include "LooseAssetFileWatcher.inl"