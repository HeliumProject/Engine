#pragma once

#include "VaultSearchQuery.h"
#include "VaultSearchResults.h"

#include "Editor/Tracker/TrackerDBGenerated.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/File/Directory.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Platform/Mutex.h"

//
// Forwards
//
class wxFrame;

namespace Helium
{
    namespace Editor
    {
        //
        // Forwards
        //
        class DummyWindow;
        struct DummyWindowArgs;

        ///////////////////////////////////////////////////////////////////////
        // StartSearchThread:
        // A search has been requested.
        struct StartSearchThreadArgs
        {
            VaultSearchQueryPtr m_SearchQuery;
            StartSearchThreadArgs( VaultSearchQuery* searchQuery ) : m_SearchQuery( searchQuery ) {}
        };
        typedef Helium::Signature< const StartSearchThreadArgs& > StartSearchThreadSignature;


        ///////////////////////////////////////////////////////////////////////
        // SearchThreadRunning:
        // Search thread is starting, Args contain status update info pointers.
        struct SearchThreadRunningArgs
        {
            SearchThreadRunningArgs() { }
        };
        typedef Helium::Signature< const SearchThreadRunningArgs& > SearchThreadRunningSignature;


        ///////////////////////////////////////////////////////////////////////
        // StoppingSearchThread:
        // Search is done, results are being wrangled.
        struct StoppingSearchThreadArgs
        {
            StoppingSearchThreadArgs() { }
        };
        typedef Helium::Signature< const StoppingSearchThreadArgs& > StoppingSearchThreadSignature;


        ///////////////////////////////////////////////////////////////////////
        // SearchThreadComplete: 
        // Search is complete.
        struct SearchThreadCompleteArgs
        {
            VaultSearchQueryPtr m_SearchQuery;
            SearchThreadCompleteArgs( VaultSearchQuery* searchQuery ) : m_SearchQuery( searchQuery ) {}
        };
        typedef Helium::Signature< const SearchThreadCompleteArgs& > SearchThreadCompleteSignature;


        ///////////////////////////////////////////////////////////////////////
        // SearchResultsAvailable:
        // (Some or all) search results are available, contains VaultSearchResults
        struct SearchResultsAvailableArgs
        {
            VaultSearchQueryPtr m_SearchQuery;
            VaultSearchResultsPtr m_SearchResults;

            SearchResultsAvailableArgs( VaultSearchQuery* searchQuery, VaultSearchResults* searchResults )
                : m_SearchQuery( searchQuery )
                , m_SearchResults( searchResults )
            {

            }
        };
        typedef Helium::Signature< const SearchResultsAvailableArgs& > SearchResultsAvailableSignature;


        ///////////////////////////////////////////////////////////////////////
        // SearchThreadError:
        struct SearchThreadErrorArgs
        {
            tstring m_Errors;

            SearchThreadErrorArgs( const tstring& errors ) : m_Errors( errors ) {}
        };
        typedef Helium::Signature< const SearchThreadErrorArgs& > SearchThreadErrorSignature;
        

        ///////////////////////////////////////////////////////////////////////
        /// class VaultSearch
        class VaultSearch : public Helium::RefCountBase< VaultSearch >
        {
        public:
            VaultSearch();
            virtual ~VaultSearch();

            void SetDirectory( const Helium::Path& directory );
            const Helium::Path& GetDirectory() const;

            bool StartSearchThread( VaultSearchQuery* searchQuery );
            void StopSearchThreadAndWait();

            friend class VaultSearchThread;

        private:
            Helium::Path            m_Directory;

            //----------DO NOT ACCESS outside of m_SearchResultsMutex---------//
            // VaultSearchResults and Status
            // 
            Helium::Mutex           m_SearchResultsMutex;
            i32                     m_CurrentSearchID;   // Used for debugging to track a search through the system
            VaultSearchQueryPtr     m_CurrentSearchQuery;
            VaultSearchResultsPtr   m_SearchResults;     // The results to populate and pass to SearchResultsAvailableArgs
            std::set< Helium::Path > m_FoundPaths;       // The *complete* list of found files from this section
            //---------------------------------------------------------------//

            // Searching Thread
            Helium::Mutex           m_BeginSearchMutex;  // Take Lock until m_SearchInitializedEvent
            bool                    m_StopSearching;
            DummyWindow*            m_DummyWindow;
            HANDLE                  m_SearchInitializedEvent; // OK to cancel searches after this is set
            HANDLE                  m_EndSearchEvent;

        private:
            //
            // Callbaks to VaultSearchThread events
            //
            void OnBeginSearchThread( const Editor::DummyWindowArgs& args );
            void OnSearchResultsAvailable( const Editor::DummyWindowArgs& args );
            void OnEndSearchThread( const Editor::DummyWindowArgs& args );

            //
            // VaultSearchThread
            //
            void SearchThreadProc( i32 searchID );
            void SearchThreadEnter( i32 searchID );
            void SearchThreadPostResults( i32 searchID );
            bool CheckSearchThreadLeave( i32 searchID );
            void SearchThreadLeave( i32 searchID );

            u32 AddPath( const Helium::Path& path, i32 searchID );

            // 
            // Events
            //
        private:
            StartSearchThreadSignature::Event m_StartSearchThreadListeners;
        public:
            void AddStartSearchThreadListener( const StartSearchThreadSignature::Delegate& listener )
            {
                m_StartSearchThreadListeners.Add( listener );
            }
            void RemoveStartSearchThreadListener( const StartSearchThreadSignature::Delegate& listener )
            {
                m_StartSearchThreadListeners.Remove( listener );
            }

        private:
            SearchThreadRunningSignature::Event m_SearchThreadRunningListeners;
        public:
            void AddSearchThreadRunningListener( const SearchThreadRunningSignature::Delegate& listener )
            {
                m_SearchThreadRunningListeners.Add( listener );
            }
            void RemoveSearchThreadRunningListener( const SearchThreadRunningSignature::Delegate& listener )
            {
                m_SearchThreadRunningListeners.Remove( listener );
            }

        private:
            StoppingSearchThreadSignature::Event m_StoppingSearchThreadListeners;
        public:
            void AddStoppingSearchThreadListener( const StoppingSearchThreadSignature::Delegate& listener )
            {
                m_StoppingSearchThreadListeners.Add( listener );
            }
            void RemoveStoppingSearchThreadListener( const StoppingSearchThreadSignature::Delegate& listener )
            {
                m_StoppingSearchThreadListeners.Remove( listener );
            }

        private:
            SearchThreadCompleteSignature::Event m_SearchThreadCompleteListeners;
        public:
            void AddSearchThreadCompleteListener( const SearchThreadCompleteSignature::Delegate& listener )
            {
                m_SearchThreadCompleteListeners.Add( listener );
            }
            void RemoveSearchThreadCompleteListener( const SearchThreadCompleteSignature::Delegate& listener )
            {
                m_SearchThreadCompleteListeners.Remove( listener );
            }

        private:
            SearchResultsAvailableSignature::Event m_SearchResultsAvailableListeners;
        public:
            void AddSearchResultsAvailableListener( const SearchResultsAvailableSignature::Delegate& listener )
            {
                m_SearchResultsAvailableListeners.Add( listener );
            }
            void RemoveSearchResultsAvailableListener( const SearchResultsAvailableSignature::Delegate& listener )
            {
                m_SearchResultsAvailableListeners.Remove( listener );
            }

        private:
            SearchThreadErrorSignature::Event m_SearchThreadErrorListeners;
        public:
            void AddSearchThreadErrorListener( const SearchThreadErrorSignature::Delegate& listener )
            {
                m_SearchThreadErrorListeners.Add( listener );
            }
            void RemoveSearchThreadErrorListener( const SearchThreadErrorSignature::Delegate& listener )
            {
                m_SearchThreadErrorListeners.Remove( listener );
            }

        };
        typedef Helium::SmartPtr< VaultSearch > VaultSearchPtr;


    } // namespace Editor
}