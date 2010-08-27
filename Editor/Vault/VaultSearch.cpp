#include "Precompile.h"
#include "VaultSearch.h"

#include "SearchResults.h"

#include "Platform/Exception.h"
#include "Foundation/Regex.h"
#include "Foundation/Container/Insert.h"
#include "Foundation/File/Directory.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/String/Utilities.h"

using namespace Helium;
using namespace Helium::Editor;

namespace Helium
{
    namespace Editor
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
        typedef Helium::Signature< void, const DummyWindowArgs& > DummyWindowSignature;

        /////////////////////////////////////////////////////////////////////////////
        /// DummyWindow
        /////////////////////////////////////////////////////////////////////////////
        static const tchar* s_DummyWindowName = TXT( "DummyWindowThread" );

        // Custom wxEventTypes for the SearchThread to fire.
        DEFINE_EVENT_TYPE( EDITOR_EVT_BEGIN_SEARCH )
            DEFINE_EVENT_TYPE( EDITOR_EVT_RESULTS_AVAILABLE )
            DEFINE_EVENT_TYPE( EDITOR_EVT_SEARCH_COMPLETE )

        class DummyWindow : public wxFrame
        {
        public:
            DummyWindow( const tchar* name = NULL )
                : wxFrame( NULL, wxID_ANY, s_DummyWindowName, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, s_DummyWindowName )
            {
                Hide();

                if ( name )
                {
                    wxString newName( s_DummyWindowName );
                    newName += TXT( "-" );
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
        class Editor::SearchThread : public wxThread
        {
        private:
            VaultSearch* m_VaultSearch;
            i32            m_SearchID;

        public:
            // Detached threads delete themselves once they have completed,
            // and thus must be created on the heap
            SearchThread( VaultSearch* browserSearch, i32 id )
                : wxThread( wxTHREAD_DETACHED )
                , m_VaultSearch( browserSearch )
                , m_SearchID( id )
            {
            }

            virtual ~SearchThread()
            {
            }

            virtual wxThread::ExitCode Entry() HELIUM_OVERRIDE
            {
                m_VaultSearch->SearchThreadProc( m_SearchID );

                return NULL;
            }
        };
    }
}


/////////////////////////////////////////////////////////////////////////////
/// VaultSearch
/////////////////////////////////////////////////////////////////////////////
VaultSearch::VaultSearch()
: m_SearchResults( NULL )
, m_StopSearching( true )
, m_DummyWindow( NULL )
, m_CurrentSearchID( -1 )
, m_CurrentSearchQuery( NULL )
{
    m_SearchInitializedEvent = ::CreateEvent( NULL, TRUE, TRUE, TXT( "VaultBeginSearchEvent" ) );
    m_EndSearchEvent = ::CreateEvent( NULL, TRUE, TRUE, TXT( "VaultEndSearchEvent" ) );
}

VaultSearch::~VaultSearch()
{
    // wait for searching thread to complete
    RequestStop();
    ::CloseHandle( m_SearchInitializedEvent );
    ::CloseHandle( m_EndSearchEvent );

    m_CurrentSearchQuery = NULL;
    m_SearchResults = NULL;
    m_FoundPaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Creates and starts the SearchThread
//
// Detached threads delete themselves once they have completed,
// and thus must be created on the heap
//
// Everything comes through here
//  - folder path
//  - search query
//
bool VaultSearch::RequestSearch( SearchQuery* searchQuery )
{
    Helium::TakeMutex beginMutex( m_BeginSearchMutex );

    // kill current search, if any
    RequestStop();

    Helium::TakeMutex resultsMutex( m_SearchResultsMutex );
    {
        // reset event to lockout new searches from starting
        ++m_CurrentSearchID;
        ::ResetEvent( m_SearchInitializedEvent );
        m_StopSearching = false;
        m_RequestSearchListeners.Raise( RequestSearchArgs( searchQuery ) );

        // clear previous results, if any
        m_CurrentSearchQuery = searchQuery;
        m_SearchResults = new SearchResults( m_CurrentSearchID );
        m_FoundPaths.clear();

        HELIUM_ASSERT( !m_DummyWindow );
        m_DummyWindow = new DummyWindow( TXT( "VaultSearch" ) );
        m_DummyWindow->Connect( m_DummyWindow->GetId(), EDITOR_EVT_BEGIN_SEARCH, wxCommandEventHandler( DummyWindow::OnBeginThread ), NULL, m_DummyWindow );
        m_DummyWindow->Connect( m_DummyWindow->GetId(), EDITOR_EVT_RESULTS_AVAILABLE, wxCommandEventHandler( DummyWindow::OnThreadUpdate ), NULL, m_DummyWindow );
        m_DummyWindow->Connect( m_DummyWindow->GetId(), EDITOR_EVT_SEARCH_COMPLETE, wxCommandEventHandler( DummyWindow::OnEndThread ), NULL, m_DummyWindow );

        m_DummyWindow->AddBeginListener( Editor::DummyWindowSignature::Delegate( this, &VaultSearch::OnBeginSearchThread ) );
        m_DummyWindow->AddUpdateListener( Editor::DummyWindowSignature::Delegate( this, &VaultSearch::OnResultsAvailable ) );
        m_DummyWindow->AddEndListener( Editor::DummyWindowSignature::Delegate( this, &VaultSearch::OnEndSearchThread ) );

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
void VaultSearch::RequestStop()
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
void VaultSearch::OnBeginSearchThread( const Editor::DummyWindowArgs& args )
{
    if ( args.m_ThreadID != m_CurrentSearchID )
        return;

    m_BeginSearchingListeners.Raise( BeginSearchingArgs() );
}

///////////////////////////////////////////////////////////////////////////////
void VaultSearch::OnResultsAvailable( const Editor::DummyWindowArgs& args )
{
    if ( args.m_ThreadID != m_CurrentSearchID )
        return;

    Helium::TakeMutex mutex (m_SearchResultsMutex);

    if ( m_SearchResults->GetSearchID() != m_CurrentSearchID )
        return;

    // "Publish" these results, null our pointer, and clear FoundFilesIDs to continue searching
    u32 searchID = m_SearchResults->GetSearchID();
    if ( m_SearchResults && m_SearchResults->HasResults() )
    {
        m_ResultsAvailableListeners.Raise( ResultsAvailableArgs( m_CurrentSearchQuery, m_SearchResults ) );
    }
    m_SearchResults = new SearchResults( searchID );
    m_FoundPaths.clear();
}

///////////////////////////////////////////////////////////////////////////////
void VaultSearch::OnEndSearchThread( const Editor::DummyWindowArgs& args )
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
void VaultSearch::SearchThreadProc( i32 searchID )
{
    SearchThreadEnter( searchID );

    //-------------------------------------------
    // AssetFolder
    Helium::Path searchPath( m_CurrentSearchQuery->GetQueryString() );

    if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::File )
    {
        searchPath.Set( searchPath.Directory() );
    }

    if ( m_CurrentSearchQuery->GetSearchType() == SearchTypes::File
        || m_CurrentSearchQuery->GetSearchType() == SearchTypes::Folder )
    {
        AddPath( searchPath, searchID );
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
        std::set< Helium::Path > assetFiles;

#pragma TODO( "Search Cache DB")
        if ( CheckSearchThreadLeave( searchID ) )
        {
            return;
        }

        // FoundAssetFiles
        if ( !assetFiles.empty() && AddPaths( assetFiles, searchID ) )
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
inline void VaultSearch::SearchThreadEnter( i32 searchID )
{
    ::ResetEvent( m_EndSearchEvent );

    wxCommandEvent evt( EDITOR_EVT_BEGIN_SEARCH, m_DummyWindow->GetId() );
    evt.SetInt( searchID );
    wxPostEvent( m_DummyWindow, evt );

    // Main thread is deadlocked until SearchThread sets this event
    ::SetEvent( m_SearchInitializedEvent );
}

///////////////////////////////////////////////////////////////////////////////
inline void VaultSearch::SearchThreadPostResults( i32 searchID )
{
    Helium::TakeMutex mutex (m_SearchResultsMutex);

    if ( m_SearchResults && m_SearchResults->HasResults() )
    {
        wxCommandEvent evt( EDITOR_EVT_RESULTS_AVAILABLE, m_DummyWindow->GetId() );
        evt.SetInt( searchID );
        wxPostEvent( m_DummyWindow, evt );
    }
}

///////////////////////////////////////////////////////////////////////////////
inline bool VaultSearch::CheckSearchThreadLeave( i32 searchID )
{
    if ( m_StopSearching )
    {
        SearchThreadLeave( searchID );
        return true;
    }
    return false;
}

inline void VaultSearch::SearchThreadLeave( i32 searchID )
{
    m_StopSearching = true;

    SearchThreadPostResults( searchID );

    wxCommandEvent evt( EDITOR_EVT_SEARCH_COMPLETE, m_DummyWindow->GetId() );
    evt.SetInt( searchID );
    wxPostEvent( m_DummyWindow, evt );
    if ( m_DummyWindow )
    {
        m_DummyWindow->Destroy();
        m_DummyWindow = NULL;
    }

    // Main thread is deadlocked until SearchThread sets this event
    ::SetEvent( m_EndSearchEvent );
}


/////////////////////////////////////////////////////////////////////////////
// SearchThreadProc Helper Functions - Wrangle SearchResults
/////////////////////////////////////////////////////////////////////////////

u32 VaultSearch::AddPath( const Helium::Path& path, i32 searchID )
{ 
    u32 numFilesAdded = 0;

    Helium::TakeMutex mutex (m_SearchResultsMutex);

    Helium::Insert<std::set< Helium::Path >>::Result inserted = m_FoundPaths.insert( path );
    if ( inserted.second )
    {
        m_SearchResults->AddPath( path );
        ++numFilesAdded;
    }

    if ( path.IsDirectory() )
    {
        std::set< Helium::Path > files;
        Helium::Directory::GetFiles( path.Get(), files );
        for ( std::set< Helium::Path >::const_iterator itr = files.begin(), end = files.end(); itr != end; ++itr )
        {
            Helium::Insert<std::set< Helium::Path >>::Result inserted = m_FoundPaths.insert( *itr );
            if ( inserted.second )
            {
                m_SearchResults->AddPath( *itr );
                ++numFilesAdded;
            }
        }   
    }

    return numFilesAdded;
}

u32 VaultSearch::AddPaths( const std::set< Helium::Path >& paths, i32 searchID )
{
    u32 numFilesAdded = 0;
    Helium::TakeMutex mutex (m_SearchResultsMutex);

    for ( std::set< Helium::Path >::const_iterator itr = paths.begin(), end = paths.end(); itr != end; ++itr )
    {
        Helium::Insert<std::set< Helium::Path >>::Result inserted = m_FoundPaths.insert( *itr );
        if ( inserted.second )
        {
            m_SearchResults->AddPath( *itr );
            ++numFilesAdded;
        }
    }

    return numFilesAdded;
}
