#include "Platform/Windows/Windows.h"

#include "Tracker.h"
#include "CacheDB.h"

#include "Pipeline/Asset/AssetExceptions.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/AssetVisitor.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"
#include "Pipeline/Asset/Components/ArtFileComponent.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "Application/Application.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/Flags.h"
#include "Foundation/String/Utilities.h"
#include "Platform/Types.h"
#include "Foundation/Log.h"
#include "Pipeline/Content/ContentInit.h"
#include "Foundation/InitializerStack.h"
#include "Finder/Finder.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/LunaSpecs.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/FinderSpec.h"
#include "Finder/ProjectSpecs.h"
#include "Foundation/Reflect/Class.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/TUID.h"

#define SHUTDOWN_THREAD 0


using namespace Asset;

//
// Typedefs
//

typedef std::map< tuid, S_tuid > M_AssetDependencies; 


///////////////////////////////////////////////////////////////////////////////
static inline void PrependFilePath( const std::string& projectAssets, std::string& path )
{
    if ( path.find( projectAssets ) != 0 )
    {
        path = projectAssets + path;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Tracker
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
Tracker::Tracker( const std::string& rootDirectory, const std::string& configDirectory )
: m_RootDirectory( rootDirectory )
, m_Thread( NULL )
, m_ThreadID( 0x0 )
, m_StopTracking( false )
, m_InitialIndexingCompleted( false )
, m_IndexingFailed( false )
, m_Total( 0 )
, m_CurrentProgress( 0 )
{
    Nocturnal::Path cacheDBFilepath( rootDirectory + "/.tracker/cache.db" );
    m_AssetCacheDB = new Asset::CacheDB( "AssetTracker-AssetCacheDB", cacheDBFilepath.Get(), configDirectory );
}

Tracker::~Tracker()
{
    StopThread();
    if ( m_AssetCacheDB )
    {
        delete m_AssetCacheDB;
        m_AssetCacheDB = NULL;
    }
    m_AssetFiles.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Entry point for AssetVisitor
//
bool Tracker::TrackAssetFile( Nocturnal::Path& filePath, M_AssetFiles* assetFiles )
{
    if ( !assetFiles )
    {
        NOC_BREAK()
    }

    if ( !filePath.Exists() )
    {
        return false;
    }

    Nocturnal::Insert<M_AssetFiles>::Result inserted = assetFiles->insert( M_AssetFiles::value_type( filePath.Hash(), new AssetFile( filePath ) ) );
    if ( inserted.second )
    {
        // this tuid may represent an asset file
        if ( filePath.Extension() == Reflect::Archive::GetExtension( Reflect::ArchiveTypes::Binary ) )
        {
            try
            {
                Asset::AssetClassPtr assetClass = Asset::AssetClass::LoadAssetClass( filePath );
                if ( assetClass && !m_StopTracking )
                {
                    AssetFilePtr& assetFile = inserted.first->second;
                    AssetVisitor assetVisitor( assetFiles, assetClass, &m_StopTracking );
                    assetClass->Host( assetVisitor );
                }
            }
            catch( const Nocturnal::Exception& ex )
            {
                Log::Warning( Log::Levels::Verbose, "Tracker: %s\n", ex.what() );
            }
        }
    }

    return true;
}


///////////////////////////////////////////////////////////////////////////////
bool Tracker::TrackFile( const std::string& path )
{
    Nocturnal::Path filePath( path );
    return TrackFile( filePath );
}

bool Tracker::TrackFile( Nocturnal::Path& filePath )
{
    bool result = false;

    bool newTransOpened = false;


    try
    {
        //if ( file->m_WasDeleted || !FileSystem::Exists( file->m_Path ) )
        //{
        //  AssetFilePtr assetFile = new AssetFile( file );

        //  // make sure a transaction is open before inserting
        //  if ( !m_AssetCacheDB->IsTransOpen() )
        //  {
        //    m_AssetCacheDB->BeginTrans();
        //    newTransOpened = true;
        //  }
        //  m_AssetCacheDB->DeleteAssetFile( assetFile );
        //  result = true;
        //}
        //else
        if ( m_AssetCacheDB->HasAssetChangedOnDisk( filePath, &m_StopTracking ) )
        {
            Nocturnal::S_Path visited;
            if ( TrackAssetFile( filePath, &m_AssetFiles ) )
            {
                // update the DB
                M_AssetFiles::iterator found = m_AssetFiles.find( filePath.Hash() );
                if ( found != m_AssetFiles.end() )
                {
                    AssetFilePtr& assetFile = found->second;

                    // make sure a transaction is open before inserting
                    if ( !m_AssetCacheDB->IsTransOpen() )
                    {
                        m_AssetCacheDB->BeginTrans();
                        newTransOpened = true;
                    }

                    m_AssetCacheDB->InsertAssetFile( assetFile, &m_AssetFiles, visited, &m_StopTracking );
                    result = true;
                }
            }

            if ( !result )
            {
                // still insert the file, even if we have no data for it
                AssetFilePtr assetFile = new AssetFile( filePath );

                // make sure a transaction is open before inserting
                if ( !m_AssetCacheDB->IsTransOpen() )
                {
                    m_AssetCacheDB->BeginTrans();
                    newTransOpened = true;
                }

                m_AssetCacheDB->InsertAssetFile( assetFile, &m_AssetFiles, visited, &m_StopTracking );
            }
        }
    }
    catch( ... )
    {
        if ( newTransOpened )
        {
            m_AssetCacheDB->RollbackTrans();
        }
        throw;
    }

    if ( newTransOpened )
    {
        m_AssetCacheDB->CommitTrans();
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Sleep between runs and yield to other threads
// The complex loop is to prevent Luna from hanging on exit (max hang will be "increments" seconds)
inline void SleepBetweenTracking( bool* cancel = NULL, const u32 minutes = 1 )
{
    const u32 increments = 5;
    u32 totalSeconds = 60 * minutes;
    for ( u32 seconds = 0; seconds < totalSeconds; seconds += increments )
    {
        Sleep( increments * 1000 );

        if ( ( cancel != NULL )
            && *cancel )
        {
            break;
        }
    }
}

void Tracker::HandleDirectoryItem( const Nocturnal::DirectoryItem& dirItem )
{
    Nocturnal::Path path( dirItem.m_Path );
    if ( path.IsFile() )
    {
        m_FoundPaths.insert( path );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Process over all of the assets and insert them into the asset DB
// 
void Tracker::TrackEverything()
{
    m_StopTracking = false;
    m_InitialIndexingCompleted = false;
    m_IndexingFailed = false;

    while ( !m_StopTracking )
    {  
        Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default,
            m_InitialIndexingCompleted ? "Tracker: Looking for new or updated files...\n" : "Tracker: Finding asset files...\n" );

        ////////////////////////////////
        // Find Files
        {
            Profile::Timer timer;
            Nocturnal::DirectoryItemSignature::Delegate callback( this, &Tracker::HandleDirectoryItem );
            Nocturnal::RecurseDirectories( callback, m_RootDirectory, std::string( "*." ) + Reflect::Archive::GetExtension( Reflect::ArchiveTypes::Binary ) );
            Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, "Tracker: File reslover database lookup took %.2fms\n", timer.Elapsed() );
        }

        ////////////////////////////////
        // Track Files
        m_CurrentProgress = 0;
        m_Total = (u32)m_FoundPaths.size();
        {
            Profile::Timer timer;
            Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, "Tracker: Scanning %d asset file(s) for changes...\n", (u32)m_FoundPaths.size() );

            while ( !m_StopTracking && !m_FoundPaths.empty() )
            {
                Log::Listener listener ( ~Log::Streams::Error );

                try
                {
                    TrackFile( *(m_FoundPaths.begin()) );
                }
                catch( const Nocturnal::Exception& ex )
                {
                    Log::Warning( "Tracker: %s\n", ex.what() );
                }

                m_FoundPaths.erase( m_FoundPaths.begin() );
                m_CurrentProgress = m_Total - (u32)m_FoundPaths.size();
            }

            if ( m_StopTracking )
            {
                u32 percentComplete = (u32)(((f32)m_CurrentProgress/(f32)m_Total) * 100);
                Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, "Tracker: Indexing (%d%% complete) pre-empted after %.2fm\n", percentComplete, timer.Elapsed() / 1000.f / 60.f );
            }
            else if ( !m_InitialIndexingCompleted )
            {
                m_InitialIndexingCompleted = true;
                Log::Print( "Tracker: Initial indexing completed in %.2fm\n", timer.Elapsed() / 1000.f / 60.f );
            }
            else 
            {
                Log::Print( Log::Levels::Verbose, "Tracker: Indexing updated in %.2fm\n" , timer.Elapsed() / 1000.f / 60.f );
            }
        }
        m_FoundPaths.clear();

        m_Total = 0;
        m_CurrentProgress = 0;

        ////////////////////////////////
        // Recurse
        if ( !m_StopTracking )
        {
            // Sleep between runs and yield to other threads
            // The complex loop is to prevent Luna from hanging on exit (max hang will be "increments" seconds)
            SleepBetweenTracking( &m_StopTracking );
        }
    }

    m_AssetCacheDB->Close();
}

///////////////////////////////////////////////////////////////////////////////
bool Tracker::IsTracking() const
{
    bool ret = false;

    if ( m_Thread )
    {
        DWORD status;
        ::GetExitCodeThread( m_Thread, &status );
        if ( status == STILL_ACTIVE )
        {
            ret = true;
        }
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
void Tracker::StartThread()
{
    StopThread();
    m_Thread = CreateThread( NULL, NULL, TrackEverythingThread, this, NULL, &m_ThreadID );
}

///////////////////////////////////////////////////////////////////////////////
void Tracker::StopThread()
{
    if ( m_Thread )
    {
        m_StopTracking = true;

        do
        {
            Log::Print( "Waiting for Tracker Thread shutdown...\n" );
        }
        while ( WaitForSingleObject( m_Thread, 5 * 1000 /*wait 5 seconds*/ ) == WAIT_TIMEOUT );

        m_ThreadID = 0x0;
        ::CloseHandle( m_Thread );
        m_Thread = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////
// Thread entry point to start tracking everything
//
DWORD WINAPI Tracker::TrackEverythingThread(LPVOID pTracker)
{
    ((Tracker*)pTracker)->TrackEverything();

    return (DWORD)0;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current progress through TrackEverything (just the files returned
// from the local resolver)
//
u32 Tracker::GetTrackingProgress()
{
    return m_CurrentProgress;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the total number of files that TrackEverything will process
//
u32 Tracker::GetTrackingTotal()
{
    return m_Total;
}