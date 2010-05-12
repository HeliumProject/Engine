#include "Precompile.h"
#include "BrowserSearch.h"

#include "Browser.h"
#include "SearchResults.h"

#include "Asset/AssetFile.h"
#include "Asset/AssetFolder.h"
#include "Asset/CacheDB.h"
#include "Common/Exception.h"
#include "Common/Boost/Regex.h"
#include "Common/Container/Insert.h"
#include "Common/String/Tokenize.h"
#include "Common/String/Utilities.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"

using namespace Luna;



namespace Luna
{

  /////////////////////////////////////////////////////////////////////////////
  /// DummyWindowArgs
  /////////////////////////////////////////////////////////////////////////////
  struct DummyWindowArgs
  {
    i32 m_ThreadID;
    DummyWindowArgs( i32 threadID )
      : m_ThreadID( threadID )
    {
    }
  };
  typedef Nocturnal::Signature< void, const DummyWindowArgs& > DummyWindowSignature;

  /////////////////////////////////////////////////////////////////////////////
  /// DummyWindow
  /////////////////////////////////////////////////////////////////////////////
  static const char* s_DummyWindowName = "DummyWindowThread";

  // Custom wxEventTypes for the SearchThread to fire.
  DEFINE_EVENT_TYPE( igEVT_BEGIN_SEARCH )
  DEFINE_EVENT_TYPE( igEVT_RESULTS_AVAILABLE )
  DEFINE_EVENT_TYPE( igEVT_SEARCH_COMPLETE )

  class DummyWindow : public wxFrame
  {
  public:
    DummyWindow( const char* name = NULL )
      : wxFrame( NULL, wxID_ANY, s_DummyWindowName, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, s_DummyWindowName )
    {
      Hide();

      if ( name )
      {
        wxString newName( s_DummyWindowName );
        newName += "-";
        newName += name;
        SetName( newName );
        SetTitle( newName );
      }
    }

    virtual ~DummyWindow()
    {

    }

    void OnBeginThread( wxCommandEvent& evt )
    {
      m_BeginListeners.Raise( DummyWindowArgs( evt.GetInt() ) );
    }

    void OnThreadUpdate( wxCommandEvent& evt )
    {
      m_UpdateListeners.Raise( DummyWindowArgs( evt.GetInt() ) );
    }

    void OnEndThread( wxCommandEvent& evt )
    {
      m_EndListeners.Raise( DummyWindowArgs( evt.GetInt() ) ); 
    }

    // 
    // Events
    //
  private:
    DummyWindowSignature::Event m_BeginListeners;
  public:
    void AddBeginListener( const DummyWindowSignature::Delegate& listener )
    {
      m_BeginListeners.Add( listener );
    }
    void RemoveBeginListener( const DummyWindowSignature::Delegate& listener )
    {
      m_BeginListeners.Remove( listener );
    }

  private:
    DummyWindowSignature::Event m_UpdateListeners;
  public:
    void AddUpdateListener( const DummyWindowSignature::Delegate& listener )
    {
      m_UpdateListeners.Add( listener );
    }
    void RemoveUpdateListener( const DummyWindowSignature::Delegate& listener )
    {
      m_UpdateListeners.Remove( listener );
    }

  private:
    DummyWindowSignature::Event m_EndListeners;
  public:
    void AddEndListener( const DummyWindowSignature::Delegate& listener )
    {
      m_EndListeners.Add( listener );
    }
    void RemoveEndListener( const DummyWindowSignature::Delegate& listener )
    {
      m_EndListeners.Remove( listener );
    }

  };


  /////////////////////////////////////////////////////////////////////////////
  /// SearchThread
  /////////////////////////////////////////////////////////////////////////////
  class Luna::SearchThread : public wxThread
  {
  private:
    BrowserSearch* m_BrowserSearch;
    i32            m_SearchID;

  public:
    // Detached threads delete themselves once they have completed,
    // and thus must be created on the heap
    SearchThread( BrowserSearch* browserSearch, i32 id )
      : wxThread( wxTHREAD_DETACHED )
      , m_BrowserSearch( browserSearch )
      , m_SearchID( id )
    {
    }

    virtual ~SearchThread()
    {
    }

    virtual wxThread::ExitCode Entry() NOC_OVERRIDE
    {
      m_BrowserSearch->SearchThreadProc( m_SearchID );

      return NULL;
    }
  };
}


/////////////////////////////////////////////////////////////////////////////
/// BrowserSearch
/////////////////////////////////////////////////////////////////////////////
BrowserSearch::BrowserSearch()
: m_SearchResults( NULL )
, m_StopSearching( true )
, m_DummyWindow( NULL )
, m_CurrentSearchID( -1 )
, m_CurrentSearchQuery( NULL )
{
  m_CacheDB = new Asset::CacheDB( "LunaBrowserSearch-AssetCacheDB" );

  m_SearchInitializedEvent = ::CreateEvent( NULL, TRUE, TRUE, "BrowserBeginSearchEvent" );
  m_EndSearchEvent = ::CreateEvent( NULL, TRUE, TRUE, "BrowserEndSearchEvent" );
}

BrowserSearch::~BrowserSearch()
{
  // wait for searching thread to complete
  RequestStop();
  ::CloseHandle( m_SearchInitializedEvent );
  ::CloseHandle( m_EndSearchEvent );

  m_CurrentSearchQuery = NULL;
  m_SearchResults = NULL;
  m_FoundFileIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Creates and starts the SearchThread
//
// Detached threads delete themselves once they have completed,
// and thus must be created on the heap
//
// Everything comes through here
//  - collection path
//  - folder path
//  - search query
//
bool BrowserSearch::RequestSearch( SearchQuery* searchQuery )
{
  Platform::TakeMutex beginMutex( m_BeginSearchMutex );

  // kill current search, if any
  RequestStop();

  Platform::TakeMutex resultsMutex( m_SearchResultsMutex );
  {
    // reset event to lockout new searches from starting
    ++m_CurrentSearchID;
    ::ResetEvent( m_SearchInitializedEvent );
    m_StopSearching = false;
    m_RequestSearchListeners.Raise( RequestSearchArgs( searchQuery ) );

    // clear previous results, if any
    m_CurrentSearchQuery = searchQuery;
    m_SearchResults = new SearchResults( m_CurrentSearchID );
    m_FoundFileIDs.clear();

    NOC_ASSERT( !m_DummyWindow );
    m_DummyWindow = new DummyWindow( "BrowserSearch" );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), igEVT_BEGIN_SEARCH, wxCommandEventHandler( DummyWindow::OnBeginThread ), NULL, m_DummyWindow );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), igEVT_RESULTS_AVAILABLE, wxCommandEventHandler( DummyWindow::OnThreadUpdate ), NULL, m_DummyWindow );
    m_DummyWindow->Connect( m_DummyWindow->GetId(), igEVT_SEARCH_COMPLETE, wxCommandEventHandler( DummyWindow::OnEndThread ), NULL, m_DummyWindow );

    m_DummyWindow->AddBeginListener( Luna::DummyWindowSignature::Delegate( this, &BrowserSearch::OnBeginSearchThread ) );
    m_DummyWindow->AddUpdateListener( Luna::DummyWindowSignature::Delegate( this, &BrowserSearch::OnResultsAvailable ) );
    m_DummyWindow->AddEndListener( Luna::DummyWindowSignature::Delegate( this, &BrowserSearch::OnEndSearchThread ) );

    // start the search thread
    SearchThread* searchThread = new SearchThread( this, m_CurrentSearchID );
    searchThread->Create();
    searchThread->Run();
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Something has requested the search thread to stop; possibly
// a new browserSearch is ready to be run, stop the old and start
// the new.
void BrowserSearch::RequestStop()
{
  // cant cancel a search until the search is initialized and m_SearchInitializedEvent is set
  ::WaitForSingleObject( m_SearchInitializedEvent, INFINITE );

  m_StopSearching = true;
  ::WaitForSingleObject( m_EndSearchEvent, INFINITE );

  if ( m_DummyWindow )
  {
    m_DummyWindow->Destroy();
    m_DummyWindow = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Main thread callbacks to notify listeners when the search has started,
// when results are available for use and when the search has completed
void BrowserSearch::OnBeginSearchThread( const Luna::DummyWindowArgs& args )
{
  if ( args.m_ThreadID != m_CurrentSearchID )
    return;

  m_BeginSearchingListeners.Raise( BeginSearchingArgs() );
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearch::OnResultsAvailable( const Luna::DummyWindowArgs& args )
{
  if ( args.m_ThreadID != m_CurrentSearchID )
    return;

  Platform::TakeMutex mutex (m_SearchResultsMutex);

  if ( m_SearchResults->GetSearchID() != m_CurrentSearchID )
    return;

  // "Publish" these results, null our pointer, and clear FoundFilesIDs to continue searching
  u32 searchID = m_SearchResults->GetSearchID();
  if ( m_SearchResults && m_SearchResults->HasResults() )
  {
    m_ResultsAvailableListeners.Raise( ResultsAvailableArgs( m_CurrentSearchQuery, m_SearchResults ) );
  }
  m_SearchResults = new SearchResults( searchID );
  m_FoundFileIDs.clear();
}

///////////////////////////////////////////////////////////////////////////////
void BrowserSearch::OnEndSearchThread( const Luna::DummyWindowArgs& args )
{
  if ( args.m_ThreadID != m_CurrentSearchID )
    return;

  m_SearchCompleteListeners.Raise( SearchCompleteArgs( m_CurrentSearchQuery ) );
}



///////////////////////////////////////////////////////////////////////////////
//
// SearchThreadProc - Called from the SearchThread
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Called by SearchThread
// Fills out the SearchResults structure to pass to ResultsAvailable event
//
void BrowserSearch::SearchThreadProc( i32 searchID )
{
  SearchThreadEnter( searchID );

  //-------------------------------------------
  // AssetFolder
  if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::Folder
       && FoundAssetFolder( m_CurrentSearchQuery->GetQueryString(), searchID ) )
  {
    // it's an AssetFolder - early out
    SearchThreadLeave( searchID );
    return;
  }

  if ( CheckSearchThreadLeave( searchID ) )
  {
    return;
  }

  //-------------------------------------------
  // AssetFile
  if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::File )
  {
    std::string searchFolder( m_CurrentSearchQuery->GetQueryString() );
    FileSystem::StripLeaf( searchFolder );
    
    if ( FoundAssetFolder( searchFolder, searchID ) )
    {
      SearchThreadLeave( searchID );
      return;
    }
  }

  if ( CheckSearchThreadLeave( searchID ) )
  {
    return;
  }

  //-------------------------------------------
  // File ID
  if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::ID 
    && FoundAssetFile( m_CurrentSearchQuery->GetQueryFileID() ) )
  {
    // early out
    SearchThreadLeave( searchID );
    return;
  }

  if ( CheckSearchThreadLeave( searchID ) )
  {
    return;
  }

  //-------------------------------------------
  // CacheDB Search
  if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::DBSearch )
  {
    S_tuid assetFileIDs;

    Asset::CacheDBQuery* search = m_CurrentSearchQuery->GetCacheDBQuery();
    AssetCollection* collection = m_CurrentSearchQuery->GetCollection();    

    // Empty collection
    if ( collection && collection->GetAssetIDs().empty() )
    {
      SearchThreadLeave( searchID );
      return;
    }
    // Both collection and search
    else if ( collection && search )
    {
      S_tuid searchFileIDs;
      m_CacheDB->Search( search, searchFileIDs, &m_StopSearching );
      if ( searchFileIDs.empty() )
      {
        SearchThreadLeave( searchID );
        return;
      }

      S_tuid collectionIDs = collection->GetAssetIDs();

      // filter by collection
      if ( collectionIDs.size() > searchFileIDs.size() )
      {
        for ( S_tuid::iterator itr = searchFileIDs.begin(), end = searchFileIDs.end(); itr != end; ++itr )
        {
          if ( collectionIDs.find( *itr ) != collectionIDs.end() )
          {
            assetFileIDs.insert( *itr );
          }
        }
      }
      else
      {
        for ( S_tuid::iterator itr = collectionIDs.begin(), end = collectionIDs.end(); itr != end; ++itr )
        {
          if ( searchFileIDs.find( *itr ) != searchFileIDs.end() )
          {
            assetFileIDs.insert( *itr );
          }
        }
      }
    }
    // Just the Search
    else if ( search )
    {
      m_CacheDB->Search( search, assetFileIDs, &m_StopSearching );
      if ( assetFileIDs.empty() )
      {
        SearchThreadLeave( searchID );
        return;
      }
    }
    // Just the collection 
    else if ( collection )
    {
      assetFileIDs = collection->GetAssetIDs();
    }

    if ( CheckSearchThreadLeave( searchID ) )
    {
      return;
    }

    // FoundAssetFiles
    if ( !assetFileIDs.empty() && FoundAssetFiles( assetFileIDs, searchID ) )
    {
      SearchThreadLeave( searchID );
      return;
    }
  }
  
  SearchThreadLeave( searchID );
}


/////////////////////////////////////////////////////////////////////////////
// SearchThreadProc Helper Functions
// Used in Search() to create begin and end search events
//
inline void BrowserSearch::SearchThreadEnter( i32 searchID )
{
  ::ResetEvent( m_EndSearchEvent );

  // Connect the DB
  std::string rootDir = Finder::ProjectAssets() + FinderSpecs::Project::ASSET_TRACKER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( rootDir );
  FileSystem::MakePath( rootDir );
  m_CacheDB->Open( FinderSpecs::Project::ASSET_TRACKER_DB.GetFile( rootDir ),
    FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder(),
    m_CacheDB->s_TrackerDBVersion,
    SQLITE_OPEN_READONLY );

  wxCommandEvent evt( igEVT_BEGIN_SEARCH, m_DummyWindow->GetId() );
  evt.SetInt( searchID );
  wxPostEvent( m_DummyWindow, evt );

  // Main thread is deadlocked until SearchThread sets this event
  ::SetEvent( m_SearchInitializedEvent );
}

///////////////////////////////////////////////////////////////////////////////
inline void BrowserSearch::SearchThreadPostResults( i32 searchID )
{
  Platform::TakeMutex mutex (m_SearchResultsMutex);

  if ( m_SearchResults && m_SearchResults->HasResults() )
  {
    wxCommandEvent evt( igEVT_RESULTS_AVAILABLE, m_DummyWindow->GetId() );
    evt.SetInt( searchID );
    wxPostEvent( m_DummyWindow, evt );
  }
}

///////////////////////////////////////////////////////////////////////////////
inline bool BrowserSearch::CheckSearchThreadLeave( i32 searchID )
{
  if ( m_StopSearching )
  {
    SearchThreadLeave( searchID );
    return true;
  }
  return false;
}

inline void BrowserSearch::SearchThreadLeave( i32 searchID )
{
  m_StopSearching = true;

  SearchThreadPostResults( searchID );

  wxCommandEvent evt( igEVT_SEARCH_COMPLETE, m_DummyWindow->GetId() );
  evt.SetInt( searchID );
  wxPostEvent( m_DummyWindow, evt );
  if ( m_DummyWindow )
  {
    m_DummyWindow->Destroy();
    m_DummyWindow = NULL;
  }

  m_CacheDB->Close();

  // Main thread is deadlocked until SearchThread sets this event
  ::SetEvent( m_EndSearchEvent );
}


/////////////////////////////////////////////////////////////////////////////
// SearchThreadProc Helper Functions - Wrangle SearchResults
/////////////////////////////////////////////////////////////////////////////

bool BrowserSearch::FoundAssetFile( const std::string& path )
{ 
  Platform::TakeMutex mutex (m_SearchResultsMutex);

  Asset::AssetFilePtr assetFile = Asset::AssetFile::FindAssetFile( path );
  if ( assetFile 
    && m_FoundFileIDs.find( assetFile->GetFileID() ) == m_FoundFileIDs.end()
    && m_SearchResults->AddFile( assetFile ) )
  {
    m_FoundFileIDs.insert( assetFile->GetFileID() );
    return true;
  }

  return false;
}

u32 BrowserSearch::FoundAssetFile( tuid assetFileID )
{
  Platform::TakeMutex mutex (m_SearchResultsMutex);

  if ( m_FoundFileIDs.find( assetFileID ) == m_FoundFileIDs.end() )
  {
    Asset::AssetFilePtr assetFile = Asset::AssetFile::FindAssetFile( assetFileID );
    if ( assetFile )
    {
      if ( m_SearchResults->AddFile( assetFile ) )
      {
        m_FoundFileIDs.insert( assetFileID );
        return true;
      }
    }
  }

  return false;
}

u32 BrowserSearch::FoundAssetFiles( const S_tuid& assetFileIDs, i32 searchID )
{
  u32 numFilesAdded = 0;

  S_tuid::const_iterator itr = assetFileIDs.begin();
  S_tuid::const_iterator end = assetFileIDs.end();
  for ( ; itr != end; ++itr )
  {
    if ( m_StopSearching )
    {
      return numFilesAdded;
    }

    if ( FoundAssetFile( *itr ) )
    {
      ++numFilesAdded;
    }
  }

  return numFilesAdded;
}

u32 BrowserSearch::FoundAssetFolder( const std::string& folder, i32 searchID )
{  
  u32 numFilesAdded = 0;
  
  FileSystem::FileIterator fileIter( folder, "*" );
  while ( !fileIter.IsDone() )
  {
    if ( m_StopSearching )
    {
      return numFilesAdded;
    }

    std::string path = fileIter.Item();
    FileSystem::CleanName( path );
    
    if ( FileSystem::IsFolder( path ) ) 
    {
      Platform::TakeMutex mutex (m_SearchResultsMutex);

      FileSystem::GuaranteeSlash( path );
      if ( m_SearchResults->AddFolder( new Asset::AssetFolder( path ) ) )
      {
        ++numFilesAdded;
      }
    }
    else
    {
      if ( FoundAssetFile( path ) )
      {
        ++numFilesAdded;
      }
    }

    ++numFilesAdded;
    fileIter.Next();
  }

  return numFilesAdded;
}


