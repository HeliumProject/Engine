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

		for ( DynamicArray<WatchedPackage>::Iterator packageIter = m_PathsToWatch.Begin(); packageIter != m_PathsToWatch.End(); ++packageIter)
		{
			MutexScopeLock scopeLock( packageIter->m_Loader->m_accessLock );

			//Log::Print( Log::Levels::Default, TXT("Tracker: Scanning package %s\n"), packageIter->m_Path.c_str() );

			SimpleTimer packageTimer;
			Helium::DirectoryIterator directory( packageIter->m_Path );

			for( ; !directory.IsDone(); directory.Next() )
			{
				const DirectoryIteratorItem& item = directory.GetItem();

				if ( !item.m_Path.IsDirectory() )
				{
					if ( item.m_Path.Extension() == Persist::ArchiveExtensions[ Persist::ArchiveTypes::Json ] )
					{
						//HELIUM_TRACE( TraceLevels::Info, TXT("- Reading file [%s]\n"), item.m_Path.c_str() );

						bool isNewFile = true;
						bool isChangedFile = false;

						for ( DynamicArray< LoosePackageLoader::SerializedObjectData >::Iterator objectIter = packageIter->m_Loader->m_objects.Begin(); 
							objectIter != packageIter->m_Loader->m_objects.End(); ++objectIter)
						{
							if ( objectIter->filePath == item.m_Path )
							{
								isNewFile = false;

								if ( objectIter->fileTimeStamp < static_cast<int64_t>( item.m_ModTime ) )
								{
									isChangedFile = true;
								}

								break;
							}
						}

						if (isNewFile)
						{
							HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS NEW\n"), item.m_Path.c_str());
						}
						else if (isChangedFile)
						{
							HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS MODIFIED\n"), item.m_Path.c_str());
						}

					}
					else
					{
						// It might be a raw asset
						String objectNameString( item.m_Path.Filename().c_str() );

						ResourceHandler* pBestHandler = ResourceHandler::GetBestResourceHandlerForFile( objectNameString );

						if (pBestHandler)
						{
							//HELIUM_TRACE( TraceLevels::Info, TXT("- Reading file [%s]\n"), item.m_Path.c_str() );

							Name objectName( item.m_Path.Filename().c_str() );
							size_t objectIndex = packageIter->m_Loader->FindObjectByName( objectName );

							bool isNewFile = true;
							bool isChangedFile = false;

							if( objectIndex != Invalid< size_t >() )
							{
								isNewFile = false;
								if ( packageIter->m_Loader->m_objects[objectIndex].fileTimeStamp < static_cast<int64_t>( item.m_ModTime ) )
								{
									HELIUM_TRACE( 
										TraceLevels::Info, 
										TXT(" %d vs %d\n"), 
										packageIter->m_Loader->m_objects[objectIndex].fileTimeStamp, 
										static_cast<int64_t>( item.m_ModTime ));

									isChangedFile = true;
								}
							}

							if (isNewFile)
							{
								HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS NEW\n"), item.m_Path.c_str());
							}
							else if (isChangedFile)
							{
								HELIUM_TRACE( TraceLevels::Info, TXT(" %s IS MODIFIED\n"), item.m_Path.c_str());
							}
						}
					}
				}

				if ( m_StopTracking )
				{
					break;
				}
			}

			if ( m_StopTracking )
			{
				//Log::Print( Log::Levels::Default, TXT("Tracker: Pre-empted after %.2fm\n"), packageTimer.Elapsed() / 1000.f / 60.f );

				break;
			}
			else 
			{
				//Log::Print( Log::Levels::Verbose, TXT("Tracker: Package scanned in %.2fm\n") , packageTimer.Elapsed() / 1000.f / 60.f );
			}
		}

		////////////////////////////////
		// Recurse
		if ( !m_StopTracking )
		{
			// Sleep between runs and yield to other threads
			// The complex loop is to prevent Editor from hanging on exit (max hang will be "increments" seconds)
			SleepBetweenTracking( &m_StopTracking );
		}
	}
}