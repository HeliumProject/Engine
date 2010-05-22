#pragma once

#include "CollectionManager.h"
#include "BrowserPreferences.h"
#include "BrowserSearch.h"
#include "BrowserEvents.h"
#include "SearchQuery.h"
#include "SearchResults.h"
#include "SearchHistory.h"
#include "Asset/CacheDB.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"
#include "Common/InitializerStack.h"
#include "Editor/DocumentManager.h"

//
// Forwards
//
namespace Asset
{
  class CacheDB;
  typedef Nocturnal::SmartPtr< CacheDB > CacheDBPtr;

  class SearchResults;
  typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;
}

namespace Luna
{
  //
  // Forwards
  //
  class BrowserFrame;

  /////////////////////////////////////////////////////////////////////////////
  class Browser& GlobalBrowser();

  class Browser
  {
  private:
    // Browser is a singleton - should only be using GlobalBrowser;
    // Hide the ctor, copy ctor and assignment operator
    Browser();
    Browser( const Browser& rhs ) {}
    Browser& operator=( const Browser& rhs ) {}


  protected:
    static Nocturnal::InitializerStack s_InitializerStack;
    static int s_InitCount;
    static Browser* s_GlobalBrowser;

  public:
    ~Browser();
    
    static void Initialize();
    static void Cleanup();

    static Browser* GlobalBrowser()
    {
        return s_GlobalBrowser;
    }
    
    void ShowBrowser( const std::string& queryString = std::string("") );
    bool HasFrame();
    
    void InitializePreferences();
    BrowserPreferences* GetBrowserPreferences();

    Asset::CacheDB* GetCacheDB() { return m_CacheDB; }

    friend class BrowserFrame;

  protected:
    void OnCloseBrowser();
    void OnDocumentChange( const DocumentManagerChangeArgs& args );

  protected:
    
    //
    // Members
    //
  private:
    Asset::CacheDBPtr     m_CacheDB;
    BrowserSearchPtr      m_BrowserSearch;
    BrowserFrame*         m_BrowserFrame;
    bool                  m_HasFrame;
    SearchHistory*        m_SearchHistory;
    BrowserPreferencesPtr m_BrowserPreferences;
    CollectionManager*    m_CollectionManager;

  private:
    
  };


} // namespace Luna
