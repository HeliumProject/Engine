#include "PcSupportPch.h"
#include "LooseAssetFileWatcher.h"

#include "Foundation/DirectoryIterator.h"
#include "PcSupport/LoosePackageLoader.h"
#include "Foundation/Log.h"
#include "Persist/ArchiveJson.h"
#include "PcSupport/ResourceHandler.h"

using namespace Helium;

///////////////////////////////////////////////////////////////////////////////
// Sleep between runs and yield to other threads
// The complex loop is to prevent Editor from hanging on exit (max hang will be "increments" seconds)
inline void SleepBetweenTracking( bool* cancel = NULL, const uint32_t minutes = 1 )
{
	//const uint32_t increments = 1;
	//uint32_t totalSeconds = 60 * minutes;
	//for ( uint32_t seconds = 0; seconds < totalSeconds; seconds += increments )
	//{
	//	Thread::Sleep( increments * 1000 );

	//	if ( ( cancel != NULL )
	//		&& *cancel )
	//	{
	//		break;
	//	}
	//}

	Thread::Sleep( 1000 );
}

Helium::LooseAssetFileWatcher::LooseAssetFileWatcher() : m_StopTracking( false )
{

}

Helium::LooseAssetFileWatcher::~LooseAssetFileWatcher()
{
	if ( IsThreadRunning() )
	{
		StopThread();
	}
}

void Helium::LooseAssetFileWatcher::AddPackage( LoosePackageLoader *pPackageLoader )
{
	bool restartThread = false;
	if ( IsThreadRunning() )
	{
		restartThread = true;
		StopThread();
	}

#if HELIUM_ASSERT_ENABLED
	for ( DynamicArray<WatchedPackage>::Iterator iter = m_PathsToWatch.Begin(); iter != m_PathsToWatch.End(); ++iter )
	{
		HELIUM_ASSERT(pPackageLoader != iter->m_Loader);
		HELIUM_ASSERT(pPackageLoader->m_packageDirPath != iter->m_Path);
	}
#endif

	WatchedPackage *pWatchedPackage = m_PathsToWatch.New();
	pWatchedPackage->m_Path = pPackageLoader->m_packageDirPath;
	pWatchedPackage->m_Loader = pPackageLoader;

	if ( restartThread )
	{
		StartThread();
	}
}

void Helium::LooseAssetFileWatcher::RemovePackage( LoosePackageLoader *pPackageLoader )
{
	bool restartThread = false;
	if ( IsThreadRunning() )
	{
		restartThread = true;
		StopThread();
	}

	for ( size_t i = 0; i < m_PathsToWatch.GetSize(); ++i)
	{
		if (pPackageLoader == m_PathsToWatch[i].m_Loader)
		{
			m_PathsToWatch.RemoveSwap(i);
			break;
		}
	}

#if HELIUM_ASSERT_ENABLED
	for ( DynamicArray<WatchedPackage>::Iterator iter = m_PathsToWatch.Begin(); iter != m_PathsToWatch.End(); ++iter )
	{
		HELIUM_ASSERT(pPackageLoader != iter->m_Loader);
		HELIUM_ASSERT(pPackageLoader->m_packageDirPath != iter->m_Path);
	}
#endif

	if ( restartThread )
	{
		StartThread();
	}
}

void Helium::LooseAssetFileWatcher::StartThread()
{
	HELIUM_ASSERT( !IsThreadRunning() );

	m_StopTracking = false;

	Helium::CallbackThread::Entry entry = &Helium::CallbackThread::EntryHelper<LooseAssetFileWatcher, &LooseAssetFileWatcher::TrackEverything>;
	if ( !m_Thread.Create( entry, this, TXT( "Tracker Thread" ), ThreadPriorities::Low ) )
	{
		throw Exception( TXT( "Unable to create thread for asset tracking." ) );
	}
}

void Helium::LooseAssetFileWatcher::StopThread()
{
	HELIUM_ASSERT( IsThreadRunning() );

	m_StopTracking = true;

	m_Thread.Join();
}

void Helium::LooseAssetFileWatcher::TrackEverything()
{
	m_StopTracking = false;

	while ( !m_StopTracking )
	{
		Log::Print( Log::Levels::Default, TXT("Tracker: Scanning packages for changes...\n"));

		// Go through all the packages we're tracking
		for ( DynamicArray<WatchedPackage>::Iterator packageIter = m_PathsToWatch.Begin(); packageIter != m_PathsToWatch.End(); ++packageIter)
		{
			MutexScopeLock scopeLock( packageIter->m_Loader->m_accessLock );

			//Log::Print( Log::Levels::Default, TXT("Tracker: Scanning package %s\n"), packageIter->m_Path.c_str() );

			SimpleTimer packageTimer;
			Helium::DirectoryIterator directory( packageIter->m_Path );

			// For each file
			for( ; !directory.IsDone(); directory.Next() )
			{
				// If our thread is supposed to die, bail early
				if ( m_StopTracking )
				{
					break;
				}

				const DirectoryIteratorItem& item = directory.GetItem();

				bool isNewFile = true;
				bool isChangedFile = false;

				Name objectName;
				size_t objectIndex = Invalid< size_t >();

				if ( item.m_Path.IsDirectory() )
				{
					// Skip directories
					continue;
				}
				else if ( item.m_Path.Extension() == Persist::ArchiveExtensions[ Persist::ArchiveTypes::Json ] )
				{
					// JSON files get handled special
					objectName.Set( item.m_Path.Basename().c_str() );
					objectIndex = packageIter->m_Loader->FindObjectByName( objectName );
				}
				else
				{
					// See if it's a raw asset that we can handle
					String objectNameString( item.m_Path.Filename().c_str() );

					ResourceHandler* pBestHandler = ResourceHandler::GetBestResourceHandlerForFile( objectNameString );

					if (!pBestHandler)
					{
						// We don't know what this file is.. skip it
						continue;
					}

					objectName.Set( item.m_Path.Filename().c_str() );
					objectIndex = packageIter->m_Loader->FindObjectByName( objectName );
				}

				// If the package says it loaded something as fresh as the file, do nothing
				if ( objectIndex != Invalid< size_t >() &&
					packageIter->m_Loader->m_objects[objectIndex].fileTimeStamp >= static_cast<int64_t>( directory.GetItem().m_ModTime ))
				{
					continue;
				}

				// If we have already emitted a message for this object, skip it
				HashMap< Name, WatchedAsset >::Iterator watchedAssetItr = packageIter->m_Assets.Find( objectName );
				if (watchedAssetItr != packageIter->m_Assets.End())
				{
					if (watchedAssetItr->Second().m_LastMessageTime >= static_cast<int64_t>( directory.GetItem().m_ModTime ) )
					{
						// We already emitted a message for this file change, so don't do anything
						continue;
					}

					// We've emitted a message, but it's been modified again. Emit another message and update the timestamp
					watchedAssetItr->Second().m_LastMessageTime = static_cast<int64_t>( directory.GetItem().m_ModTime );
				}
				else
				{
					// We've never emitted a message, so record that we will
					WatchedAsset watchedAsset;
					watchedAsset.m_LastMessageTime = static_cast<int64_t>( directory.GetItem().m_ModTime );

					packageIter->m_Assets.Insert(
						watchedAssetItr, 
						KeyValue< Name, WatchedAsset >( objectName, watchedAsset ) );
				}
				
				// We know the file is changed and we should throw an event.. choose a different event based on new vs. changed
				if (objectIndex != Invalid< size_t >())
				{
					HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS MODIFIED\n"), item.m_Path.c_str());
					AssetTracker::GetStaticInstance()->NotifyAssetChangedExternally( packageIter->m_Loader->m_objects[objectIndex].objectPath );
				}
				else
				{
					AssetPath path;
					path.Set( objectName, false, packageIter->m_Loader->GetPackagePath());

					HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS NEW\n"), item.m_Path.c_str());
					AssetTracker::GetStaticInstance()->NotifyAssetCreatedExternally( path );
				}
			}

			if ( m_StopTracking )
			{
				// Our thread is supposed to die, bail early
				Log::Print( Log::Levels::Default, TXT("Tracker: Pre-empted after %.2fm\n"), packageTimer.Elapsed() / 1000.f / 60.f );
				break;
			}
			else 
			{
				Log::Print( Log::Levels::Default, TXT("Tracker: Package scanned in %.2fm\n") , packageTimer.Elapsed() / 1000.f / 60.f );
			}
		}

		if ( !m_StopTracking )
		{
			// Sleep between runs and yield to other threads
			// The complex loop is to prevent Editor from hanging on exit (max hang will be "increments" seconds)
			SleepBetweenTracking( &m_StopTracking );
		}
	}
}