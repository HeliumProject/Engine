#include "Precompile.h"
#include "Tracker.h"


#include "Foundation/File/Path.h"
#include "Foundation/Component/SearchableProperties.h"

#include "Core/Asset/AssetClass.h"

using namespace Helium;
using namespace Helium::Editor;


int Tracker::s_InitCount = 0;
Helium::InitializerStack Tracker::s_InitializerStack;

Tracker::Tracker()
: m_TrackerDB(TXT("sqlite3"), TXT("database=trackerDBGenerated.db"))
, m_StopTracking( false )
, m_InitialIndexingCompleted( false )
, m_IndexingFailed( false )
, m_Total( 0 )
, m_CurrentProgress( 0 )
{
    if ( ++s_InitCount == 1 )
    {
        //s_InitializerStack.Push( Reflect::RegisterClass< AssetIndexData >( TXT( "AssetIndexData" ) ) );
    }
}

Tracker::~Tracker()
{
    if ( --s_InitCount == 0 )
    {
        s_InitializerStack.Cleanup();
    }

    HELIUM_ASSERT( s_InitCount >= 0 );
}



///////////////////////////////////////////////////////////////////////////////
// Sleep between runs and yield to other threads
// The complex loop is to prevent Editor from hanging on exit (max hang will be "increments" seconds)
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

void Tracker::SetDirectory( const Helium::Directory& directory )
{
    m_Directory = directory;
}

const Helium::Directory& Tracker::GetDirectory() const
{
    return m_Directory;
}

void Tracker::TrackEverything()
{
    m_StopTracking = false;
    m_InitialIndexingCompleted = false;
    m_IndexingFailed = false;

    // create tables, sequences and indexes
    m_TrackerDB.verbose = true;

    try
    {
        m_TrackerDB.create();
    }
    catch( const litesql::SQLError& )
    {
        m_TrackerDB.upgrade();
    }

    std::set< Helium::Path > assetFiles;
    while ( !m_StopTracking )
    {
        Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default,
            m_InitialIndexingCompleted ? TXT("Tracker: Looking for new or updated files...\n") : TXT("Tracker: Finding asset files...\n" ));

        // find all the files in the project
        {
            Timer timer;
            m_Directory.GetFiles( assetFiles, TXT("*.*"), true );
            Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, TXT("Tracker: File reslover database lookup took %.2fms\n"), timer.Elapsed() );
        }

        // for each file
        m_CurrentProgress = 0;
        m_Total = (u32)assetFiles.size();

        Timer timer;
        Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, TXT("Tracker: Scanning %d asset file(s) for changes...\n"), (u32)assetFiles.size() );

        for( std::set< Helium::Path >::const_iterator assetFileItr = assetFiles.begin(), assetFileItrEnd = assetFiles.end();
            !m_StopTracking && assetFileItr != assetFileItrEnd; ++assetFileItr )
        {
            Log::Listener listener ( ~Log::Streams::Error );
            ++m_CurrentProgress;

            // start transaction
            m_TrackerDB.begin();
            try
            {
                // see if the file has changed
                // insert/update the file: path, timestamp, etc...
                const Helium::Path& assetFilePath = (*assetFileItr);
                TrackedFile assetTrackedFile( m_TrackerDB );
                try
                {
                    assetTrackedFile = litesql::select<TrackedFile>( m_TrackerDB, TrackedFile::MPath == assetFilePath.Get() ).one();
                    if ( assetFilePath.ChangedSince( assetTrackedFile.mLastModified.value().timeStamp() ) )
                    {
                        assetTrackedFile.properties().del();
                        assetTrackedFile.fileReferences().del();
                    }
                    else
                    {
                        continue;
                    }
                }
                catch( const litesql::NotFound& )
                {
                    Log::Debug( TXT("Caught litesql::NotFound excption when selecting file from DB" ));
                }

                if ( assetFilePath.FullExtension() == TXT( "nrb" ) )
                {
                    const Asset::AssetClassPtr assetClass = Asset::AssetClass::LoadAssetClass( assetFilePath );
                    if ( assetClass.ReferencesObject() )
                    {
                        // get file's properties
                        Helium::SearchableProperties fileProperties;
                        assetClass->GatherSearchableProperties( &fileProperties );
                        for( std::multimap< tstring, tstring >::const_iterator filePropertiesItr = fileProperties.GetStringProperties().begin(),
                            filePropertiesItrEnd = fileProperties.GetStringProperties().end();
                            filePropertiesItr != filePropertiesItrEnd; ++filePropertiesItr )
                        {
                            //TrackedProperty
                            TrackedProperty prop( m_TrackerDB );
                            prop.mName = filePropertiesItr->first;
                            prop.update();

                            assetTrackedFile.properties().link( prop, filePropertiesItr->second );
                        }

                        // get file's dependencies
                        std::set< Helium::Path > fileReferences;
                        assetClass->GetFileReferences( fileReferences );
                        for( std::set< Helium::Path >::const_iterator fileRefsItr = fileReferences.begin(),
                            fileRefsItrEnd = fileReferences.end();
                            fileRefsItr != fileRefsItrEnd; ++fileRefsItr )
                        {
                            //   see if the file has changed
                            const Helium::Path& fileRefPath = (*fileRefsItr);

                            TrackedFile fileRefTrackedFile( m_TrackerDB );
                            fileRefTrackedFile.mPath = fileRefPath.Get();
                            fileRefTrackedFile.update();

                            assetTrackedFile.fileReferences().link( fileRefTrackedFile );
                        }

                    }
                }
                
                // update LastModified
                assetTrackedFile.mPath = assetFilePath.Get();
                assetTrackedFile.mSize = (i32) assetFilePath.Size();
                assetTrackedFile.mLastModified = litesql::DateTime( assetFilePath.ModifiedTime() );
                assetTrackedFile.update();
            }
            catch ( ... )
            {
                m_TrackerDB.rollback();
                throw;
            }

            // commit transaction
            m_TrackerDB.commit();
        }

        if ( m_StopTracking )
        {
            u32 percentComplete = (u32)(((f32)m_CurrentProgress/(f32)m_Total) * 100);
            Log::Print( m_InitialIndexingCompleted ? Log::Levels::Verbose : Log::Levels::Default, TXT("Tracker: Indexing (%d%% complete) pre-empted after %.2fm\n"), percentComplete, timer.Elapsed() / 1000.f / 60.f );
        }
        else if ( !m_InitialIndexingCompleted )
        {
            m_InitialIndexingCompleted = true;
            Log::Print( TXT("Tracker: Initial indexing completed in %.2fm\n"), timer.Elapsed() / 1000.f / 60.f );
        }
        else 
        {
            Log::Print( Log::Levels::Verbose, TXT("Tracker: Indexing updated in %.2fm\n") , timer.Elapsed() / 1000.f / 60.f );
        }

        m_Total = 0;
        m_CurrentProgress = 0;

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
