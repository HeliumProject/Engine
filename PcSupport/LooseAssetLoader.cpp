#include "PcSupportPch.h"
#include "LooseAssetLoader.h"

#include "Platform/File.h"
#include "Foundation/FilePath.h"
#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/LoosePackageLoader.h"
#include "Foundation/DirectoryIterator.h"

#include "PcSupport/LoosePackageLoader.h"
#include "Foundation/Log.h"
#include "Persist/ArchiveJson.h"
#include "PcSupport/ResourceHandler.h"

using namespace Helium;

#if HELIUM_TOOLS

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

namespace Helium
{
	class LooseAssetFileWatcher
	{
	public:
		LooseAssetFileWatcher()
			: m_StopTracking( false )
			, m_Total( 0 )
			, m_CurrentProgress( 0 )
		{

		}

		virtual ~LooseAssetFileWatcher()
		{
			if ( IsThreadRunning() )
			{
				StopThread();
			}
		}

		void AddPackage( LoosePackageLoader *pPackageLoader )
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

		void RemovePackage( LoosePackageLoader *pPackageLoader )
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

		void StartThread()
		{
			HELIUM_ASSERT( !IsThreadRunning() );

			m_StopTracking = false;

			Helium::CallbackThread::Entry entry = &Helium::CallbackThread::EntryHelper<LooseAssetFileWatcher, &LooseAssetFileWatcher::TrackEverything>;
			if ( !m_Thread.Create( entry, this, TXT( "Tracker Thread" ), ThreadPriorities::Low ) )
			{
				throw Exception( TXT( "Unable to create thread for asset tracking." ) );
			}
		}

		void StopThread()
		{
			HELIUM_ASSERT( IsThreadRunning() );

			m_StopTracking = true;

			m_Thread.Join();
		}

		bool IsThreadRunning()
		{
			return ( m_Thread.IsValid() );
		}

		void TrackEverything()
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

		uint32_t GetCurrentProgress() const
		{
			return m_CurrentProgress;
		}

		uint32_t GetTrackingTotal() const
		{
			return m_Total;
		}


	protected:
		Helium::CallbackThread m_Thread;
		bool m_StopTracking;

		struct WatchedPackage
		{
			FilePath m_Path;
			LoosePackageLoader *m_Loader;
		};

		DynamicArray<WatchedPackage> m_PathsToWatch;

		uint32_t m_CurrentProgress;
		uint32_t m_Total;
	};
}

LooseAssetFileWatcher g_FileWatcher;

#endif // HELIUM_TOOLS

/// Constructor.
LooseAssetLoader::LooseAssetLoader()
{
	g_FileWatcher.StartThread();
}

/// Destructor.
LooseAssetLoader::~LooseAssetLoader()
{
	g_FileWatcher.StopThread();
}

/// Initialize the static object loader instance as an LooseAssetLoader.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool LooseAssetLoader::InitializeStaticInstance()
{
	if( sm_pInstance )
	{
		return false;
	}

	sm_pInstance = new LooseAssetLoader;
	HELIUM_ASSERT( sm_pInstance );

	return true;
}

/// @copydoc AssetLoader::GetPackageLoader()
PackageLoader* LooseAssetLoader::GetPackageLoader( AssetPath path )
{
	LoosePackageLoader* pLoader = m_packageLoaderMap.GetPackageLoader( path );

	return pLoader;
}

/// @copydoc AssetLoader::TickPackageLoaders()
void LooseAssetLoader::TickPackageLoaders()
{
	m_packageLoaderMap.TickPackageLoaders();
}

/// @copydoc AssetLoader::OnLoadComplete()
void LooseAssetLoader::OnLoadComplete( AssetPath /*path*/, Asset* pObject, PackageLoader* /*pPackageLoader*/ )
{
	if( pObject )
	{
		CacheObject( pObject, true );
	}
}

/// @copydoc AssetLoader::OnPrecacheReady()
 void LooseAssetLoader::OnPrecacheReady( Asset* pObject, PackageLoader* pPackageLoader )
 {
	 HELIUM_ASSERT( pObject );
	 HELIUM_ASSERT( pPackageLoader );
 
	 // The default template object for a given type never has its resource data preprocessed, so there's no need to
	 // precache default template objects.
	 if( pObject->IsDefaultTemplate() )
	 {
		 return;
	 }
 
	 // Retrieve the object preprocessor if it exists.
	 AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetStaticInstance();
	 if( !pAssetPreprocessor )
	 {
		 HELIUM_TRACE(
			 TraceLevels::Warning,
			 ( TXT( "LooseAssetLoader::OnPrecacheReady(): Missing AssetPreprocessor to use for resource " )
			 TXT( "preprocessing.\n" ) ) );
 
		 return;
	 }
 
	 // We only need to do precache handling for resources, so skip non-resource types.
	 Resource* pResource = Reflect::SafeCast< Resource >( pObject );
	 if( !pResource )
	 {
		 return;
	 }
 
	 // Attempt to load the resource data.
	 pAssetPreprocessor->LoadResourceData( pResource );
 }

/// @copydoc AssetLoader::CacheObject()
bool LooseAssetLoader::CacheObject( Asset* pAsset, bool bEvictPlatformPreprocessedResourceData )
{
	HELIUM_ASSERT( pAsset );
	
	HELIUM_TRACE(
		TraceLevels::Info,
		TXT( "LooseAssetLoader::CacheObject(): Caching asset %s.\n" ), *pAsset->GetPath().ToString() );

	// Don't cache broken objects or packages.
	if( pAsset->GetAnyFlagSet( Asset::FLAG_BROKEN ) || pAsset->IsPackage() )
	{
		return false;
	}

	// Make sure we have an object preprocessor instance with which to cache the object.
	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::GetStaticInstance();
	if( !pAssetPreprocessor )
	{
		HELIUM_TRACE(
			TraceLevels::Warning,
			TXT( "LooseAssetLoader::CacheObject(): Missing AssetPreprocessor to use for caching.\n" ) );

		return false;
	}

	// User configuration objects should not be cached.
	AssetPath objectPath = pAsset->GetPath();

	Config& rConfig = Config::GetStaticInstance();

	// Only cache the files we care about
	if ( rConfig.IsAssetPathInUserConfigPackage(objectPath) )
	{
		return false;
	}

	int64_t objectTimestamp = pAsset->GetAssetFileTimeStamp();

	if( !pAsset->IsDefaultTemplate() )
	{
		Resource* pResource = Reflect::SafeCast< Resource >( pAsset );
		if( pResource )
		{
			AssetPath baseResourcePath = pResource->GetPath();
			HELIUM_ASSERT( !baseResourcePath.IsPackage() );
			for( ; ; )
			{
				AssetPath parentPath = baseResourcePath.GetParent();
				if( parentPath.IsEmpty() || parentPath.IsPackage() )
				{
					break;
				}

				baseResourcePath = parentPath;
			}

			FilePath sourceFilePath;
			if ( !FileLocations::GetDataDirectory( sourceFilePath ) )
			{
				HELIUM_TRACE(
					TraceLevels::Warning,
					TXT( "LooseAssetLoader::CacheObject(): Could not obtain data directory.\n" ) );

				return false;
			}

			sourceFilePath += baseResourcePath.ToFilePathString().GetData();

			Status stat;
			stat.Read( sourceFilePath.Get().c_str() );

			int64_t sourceFileTimestamp = stat.m_ModifiedTime;
			if( sourceFileTimestamp > objectTimestamp )
			{
				objectTimestamp = sourceFileTimestamp;
			}
		}
	}

	// Cache the object.
	bool bSuccess = pAssetPreprocessor->CacheObject(
		pAsset,
		objectTimestamp,
		bEvictPlatformPreprocessedResourceData );
	if( !bSuccess )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "LooseAssetLoader: Failed to cache object \"%s\".\n" ),
			*objectPath.ToString() );
	}

	return bSuccess;
}

#if HELIUM_TOOLS

void Helium::LooseAssetLoader::EnumerateRootPackages( DynamicArray< AssetPath > &packagePaths )
{
	FilePath dataDirectory;
	FileLocations::GetDataDirectory( dataDirectory );

	DirectoryIterator packageDirectory( dataDirectory );
	for( ; !packageDirectory.IsDone(); packageDirectory.Next() )
	{
		if (packageDirectory.GetItem().m_Path.IsDirectory())
		{
			AssetPath path;

			//std::string filename = packageDirectory.GetItem().m_Path.Parent();
			std::vector< std::string > filename = packageDirectory.GetItem().m_Path.DirectoryAsVector();
			HELIUM_ASSERT(!filename.empty());
			std::string directory = filename.back();

			if (directory.size() <= 0)
			{
				continue;
			}
			path.Set( Name( directory.c_str() ), true, AssetPath(NULL_NAME) );

			packagePaths.Add( path );
		}

	}
}
#endif


void LooseAssetLoader::OnPackagePreloaded( LoosePackageLoader *pPackageLoader )
{
#if HELIUM_TOOLS
	g_FileWatcher.AddPackage( pPackageLoader );
#endif
}
