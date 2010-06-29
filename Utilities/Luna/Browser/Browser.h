#pragma once

#include "CollectionManager.h"
#include "BrowserPreferences.h"
#include "BrowserSearch.h"
#include "BrowserEvents.h"
#include "SearchQuery.h"
#include "SearchResults.h"
#include "SearchHistory.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Foundation/InitializerStack.h"
#include "Editor/DocumentManager.h"

//
// Forwards
//
namespace Asset
{
    class SearchResults;
    typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;
}

namespace Luna
{
    //
    // Forwards
    //
    class BrowserFrame;

    class Browser
    {
    public:
        Browser( const tstring& rootDirectory, const tstring& configDirectory );
        Browser( const Browser& rhs ) {}
        Browser& operator=( const Browser& rhs ) {}

    protected:
        static Nocturnal::InitializerStack s_InitializerStack;
        static int s_InitCount;

    public:
        ~Browser();

        static void Initialize();
        static void Cleanup();

        void ShowBrowser( const tstring& queryString = TXT("") );
        bool HasFrame();

        void InitializePreferences();
        BrowserPreferences* GetBrowserPreferences();

        friend class BrowserFrame;

    protected:
        void OnCloseBrowser();
        void OnDocumentChange( const DocumentManagerChangeArgs& args );

    protected:

        //
        // Members
        //
    private:
        tstring           m_RootDirectory;
        tstring           m_ConfigDirectory;
        BrowserSearchPtr      m_BrowserSearch;
        BrowserFrame*         m_BrowserFrame;
        bool                  m_HasFrame;
        SearchHistory*        m_SearchHistory;
        BrowserPreferencesPtr m_BrowserPreferences;
        CollectionManager*    m_CollectionManager;

    private:

    };


} // namespace Luna
