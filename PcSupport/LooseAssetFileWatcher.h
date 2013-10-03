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

		struct WatchedPackage
		{
			FilePath m_Path;
			LoosePackageLoader *m_Loader;
		};

		DynamicArray<WatchedPackage> m_PathsToWatch;
	};
}

#include "LooseAssetFileWater.inl"