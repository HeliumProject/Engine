#pragma once

#include "VaultEvents.h"
#include "SearchQuery.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Platform/Mutex.h"

//
// Forwards
//
class wxFrame;

namespace Editor
{
    //
    // Forwards
    //
    class DummyWindow;
    struct DummyWindowArgs;

    class SearchResults;
    typedef Helium::SmartPtr< SearchResults > SearchResultsPtr;


    /////////////////////////////////////////////////////////////////////////////
    /// class VaultSearch
    class VaultSearch;
    typedef Helium::SmartPtr< VaultSearch > VaultSearchPtr;

    class VaultSearch : public Helium::RefCountBase< VaultSearch >
    {
    public:
        VaultSearch();
        ~VaultSearch();

        void SetRootDirectory( const Helium::Path& path )
        {
            m_RootDirectory = path;
        }

        bool RequestSearch( SearchQuery* searchQuery );
        void RequestStop();

        friend class SearchThread;

    private:
        //
        // Members
        //

        Helium::Path       m_RootDirectory;

        // SearchResults and Status
        // DO NO CHANGE OR ACCESS outside of m_SearchResultsMutex
        i32                     m_CurrentSearchID;   // Used for debugging to track a search through the system
        SearchQueryPtr          m_CurrentSearchQuery;
        SearchResultsPtr        m_SearchResults;     // The results to populate and pass to ResultsAvailableArgs
        std::set< Helium::Path > m_FoundPaths;        // The *complete* list of found files from this section
        Helium::Mutex         m_SearchResultsMutex;

        // Searching Thread
        bool                    m_StopSearching;
        DummyWindow*            m_DummyWindow;
        HANDLE                  m_SearchInitializedEvent;   // OK to cancel searches after this is set
        Helium::Mutex         m_BeginSearchMutex;         // Take Lock until m_SearchInitializedEvent

        HANDLE                  m_EndSearchEvent;


    private:
        //
        // Callbaks to SearchThread events
        //
        void OnBeginSearchThread( const Editor::DummyWindowArgs& args );
        void OnResultsAvailable( const Editor::DummyWindowArgs& args );
        void OnEndSearchThread( const Editor::DummyWindowArgs& args );

        //
        // SearchThread
        //
        void SearchThreadProc( i32 searchID );
        void SearchThreadEnter( i32 searchID );
        void SearchThreadPostResults( i32 searchID );
        bool CheckSearchThreadLeave( i32 searchID );
        void SearchThreadLeave( i32 searchID );

        u32 AddPath( const Helium::Path& path, i32 searchID );
        u32 AddPaths( const std::set< Helium::Path >& paths, i32 searchID );

        // 
        // Events
        //
    private:
        RequestSearchSignature::Event m_RequestSearchListeners;
    public:
        void AddRequestSearchListener( const RequestSearchSignature::Delegate& listener )
        {
            m_RequestSearchListeners.Add( listener );
        }
        void RemoveRequestSearchListener( const RequestSearchSignature::Delegate& listener )
        {
            m_RequestSearchListeners.Remove( listener );
        }

    private:
        SearchErrorSignature::Event m_SearchErrorListeners;
    public:
        void AddSearchErrorListener( const SearchErrorSignature::Delegate& listener )
        {
            m_SearchErrorListeners.Add( listener );
        }
        void RemoveSearchErrorListener( const SearchErrorSignature::Delegate& listener )
        {
            m_SearchErrorListeners.Remove( listener );
        }

    private:
        BeginSearchingSignature::Event m_BeginSearchingListeners;
    public:
        void AddBeginSearchingListener( const BeginSearchingSignature::Delegate& listener )
        {
            m_BeginSearchingListeners.Add( listener );
        }
        void RemoveBeginSearchingListener( const BeginSearchingSignature::Delegate& listener )
        {
            m_BeginSearchingListeners.Remove( listener );
        }

    private:
        StoppingSearchSignature::Event m_StoppingSearchListeners;
    public:
        void AddStoppingSearchListener( const StoppingSearchSignature::Delegate& listener )
        {
            m_StoppingSearchListeners.Add( listener );
        }
        void RemoveStoppingSearchListener( const StoppingSearchSignature::Delegate& listener )
        {
            m_StoppingSearchListeners.Remove( listener );
        }

    private:
        ResultsAvailableSignature::Event m_ResultsAvailableListeners;
    public:
        void AddResultsAvailableListener( const ResultsAvailableSignature::Delegate& listener )
        {
            m_ResultsAvailableListeners.Add( listener );
        }
        void RemoveResultsAvailableListener( const ResultsAvailableSignature::Delegate& listener )
        {
            m_ResultsAvailableListeners.Remove( listener );
        }

    private:
        SearchCompleteSignature::Event m_SearchCompleteListeners;
    public:
        void AddSearchCompleteListener( const SearchCompleteSignature::Delegate& listener )
        {
            m_SearchCompleteListeners.Add( listener );
        }
        void RemoveSearchCompleteListener( const SearchCompleteSignature::Delegate& listener )
        {
            m_SearchCompleteListeners.Remove( listener );
        }

        //private:
        //  RequestStopSignature::Event m_RequestStopListeners;
        //public:
        //  void AddRequestStopListener( const RequestStopSignature::Delegate& listener )
        //  {
        //    m_RequestStopListeners.Add( listener );
        //  }
        //  void RemoveRequestStopListener( const RequestStopSignature::Delegate& listener )
        //  {
        //    m_RequestStopListeners.Remove( listener );
        //  }

    };


} // namespace Editor
