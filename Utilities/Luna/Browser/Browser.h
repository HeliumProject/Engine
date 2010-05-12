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

  public:
    ~Browser();
    
    static void Initialize();
    static void Cleanup();
    
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



  //// FavoriteSearchs: Mapping of display name to saved search query 
  //typedef std::map<std::string, std::string> M_FavoriteSearchs;

  //// FavoriteItems: Mapping of display name to a list of asset ids
  //typedef std::map<std::string, S_tuid> M_FavoriteItems;

  //// FavoriteGroup - Contains one or more of each:
  //// - Groups: tuids in M_FavoriteGroups
  //// - Searchs: one or more stored searches
  //// - Items: one or more stored item lists
  //class FavoriteGroup : public Reflect::Element
  //{
  //public:
  //  std::string m_Name;
  //  tuid m_ID;
  //  S_tuid m_Groups; 
  //  M_FavoriteSearchs m_Searchs;
  //  M_FavoriteItems m_Items;

  //  FavoriteGroup();
  //  virtual ~FavoriteGroup();

  //public:
  //  REFLECT_DECLARE_CLASS( FavoriteGroup, Reflect::Element );
  //  static void EnumerateClass( Reflect::Compositor<FavoriteGroup>& comp );
  //};
  //typedef Nocturnal::SmartPtr< FavoriteGroup > FavoriteGroupPtr;

  //// FavoriteGroups: Mapping of group id to FavoriteGroup
  //typedef std::map<tuid, FavoriteGroupPtr> M_FavoriteGroups;

  //// TopLevelGroup: name of top level group in M_FavoriteGroups
  //typedef tuid TopLevelGroup;


//REFLECT_DEFINE_CLASS( FavoriteGroup );
//void FavoriteGroup::EnumerateClass( Reflect::Compositor<FavoriteGroup>& comp )
//{
//  Reflect::Field* fieldName = comp.AddField( &FavoriteGroup::m_Name, "m_Name" );
//  Reflect::Field* fieldID = comp.AddField( &FavoriteGroup::m_ID, "m_ID" );
//  Reflect::Field* fieldGroups = comp.AddField( &FavoriteGroup::m_Groups, "m_Groups" );
//  Reflect::Field* fieldSearchs = comp.AddField( &FavoriteGroup::m_Searchs, "m_Searchs" );
//  Reflect::Field* fieldItems = comp.AddField( &FavoriteGroup::m_Items, "m_Items" );
//}