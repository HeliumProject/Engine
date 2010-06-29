#pragma once

#include "AssetCollection.h"
#include "BrowserEvents.h"
#include "BrowserGenerated.h"
#include "BrowserMenuIDs.h"
#include "BrowserStatusBar.h"
#include "DependencyCollection.h"
#include "SearchQuery.h"
#include "SearchHistory.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Editor/UpdateStatusEvent.h"
#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/TUID.h"

#include <wx/listctrl.h>

namespace Asset
{
  struct TrackerArgs;
}

namespace Luna
{
  //
  // Forwards
  //
  class Browser;
  class NavigationPanel;
  class ResultsPanel;
  class PreviewPanel;
  class FoldersPanel;
  class CollectionsPanel;
  class HelpPanel;
  struct ResultChangeArgs;

  class BrowserSearch;
  typedef Nocturnal::SmartPtr< BrowserSearch > BrowserSearchPtr;

  ///////////////////////////////////////////////////////////////////////////////
  /// Class Browser
  //
  // Basic rules:
  //  - Searching will:
  //   o hide the FoldersPanel
  //   o clear and display the serach results in the ResultsPanel
  //   o Change the Tile bar to the search query
  //   o Change the Address bar 
  //  - Double clicking a folder in the Folders Panel will change the results
  //
  ///////////////////////////////////////////////////////////////////////////////
  class BrowserFrame : public BrowserFrameGenerated
  {
  public:
    BrowserFrame( Browser* browser, BrowserSearch* browserSearch, SearchHistory* searchHistory, wxWindow* parent = NULL );
    virtual ~BrowserFrame();

    void Search( const tstring& queryString, const AssetCollection* collection = NULL );

    void GetSelectedFilesAndFolders( Asset::V_AssetFiles& files, Asset::V_AssetFolders& folders );

    static bool IsPreviewable( Asset::AssetFile* file );

    Browser* GetBrowser() { return m_Browser; }

    wxMenu* GetNewAssetMenu( bool forceEnableAll = false );
    bool InFolder();

  public:
    // Virtual overrides of Frame class
    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual const tstring& GetPreferencePrefix() const NOC_OVERRIDE;

  protected:
    virtual void OnClose( wxCloseEvent& event );

    void OnOptionsMenuOpen( wxMenuEvent& event );
    void OnOptionsMenuClose( wxMenuEvent& event );
    void OnOptionsMenuSelect( wxCommandEvent& event );
    void OnFolderSelected( wxTreeEvent& event );

    void OnAdvancedSearch( wxCommandEvent& event );
    void OnAdvancedSearchGoButton( wxCommandEvent& event );
    void OnAdvancedSearchCancelButton( wxCommandEvent& event );

    void OnOpen( wxCommandEvent& event );
    void OnPreview( wxCommandEvent& event );
    void OnSync( wxCommandEvent& event );
    void OnCheckOut( wxCommandEvent& event );
    void OnRevisionHistory( wxCommandEvent& event );
    void OnCopyPath( wxCommandEvent& event );
    void OnShowInFolders( wxCommandEvent& event );
    void OnShowInPerforce( wxCommandEvent& event );
    void OnShowInWindowsExplorer( wxCommandEvent& event );
    void OnNewCollectionFromSelection( wxCommandEvent& event );

    void OnRefresh( wxCommandEvent& args );

    void OnNew( wxCommandEvent& args );
    void OnNewFolder( wxCommandEvent& args );
    void OnCut( wxCommandEvent& args );
    void OnCopy( wxCommandEvent& args );
    void OnPaste( wxCommandEvent& args );

    void OnPreferences( wxCommandEvent& event );

    void OnUpdateStatusBar( Luna::UpdateStatusEvent& event );

    // Event Listener Callbacks
    //void OnSearchQueryChanged( const Reflect::ElementChangeArgs& args );
    void OnRequestSearch( const Luna::RequestSearchArgs& args );
    void OnBeginSearching( const Luna::BeginSearchingArgs& args );
    void OnResultsAvailable( const Luna::ResultsAvailableArgs& args );
    void OnSearchComplete( const Luna::SearchCompleteArgs& args );
    void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );
    void OnResultsPanelUpdated( const ResultChangeArgs& args );
    
    
    //
    // Helper Functions
    //

  private:
    void SetFolderPath( const tstring& folderPath );
    void UpdateNavBar( const SearchQueryPtr& searchQuery );
    void UpdateResultsView( u16 customSize = ThumbnailSizes::Medium );
    void UpdateStatusBar( size_t numFolders, size_t numFiles, size_t numSelected, const tstring& hover );

    DECLARE_EVENT_TABLE();

  private:
    Browser*                m_Browser;
    BrowserSearch*          m_BrowserSearch;
    SearchHistory*          m_SearchHistory;

    tstring             m_PreferencePrefix;
    NavigationPanel*        m_NavigationPanel;
    ResultsPanel*           m_ResultsPanel;
    PreviewPanel*           m_PreviewPanel;
    FoldersPanel*           m_FoldersPanel;
    CollectionsPanel*       m_CollectionsPanel;
    HelpPanel*              m_HelpPanel;
    BrowserStatusBar*       m_StatusBar;

    wxTextAttr              m_DefaultTextAttr;
    wxTextAttr              m_TitleTextAttr;
    wxTextAttr              m_ItalicTextAttr;

    std::map< i32, i32 >                   m_MenuItemToAssetType;

    //
    // NavBar Menus
    // 
    ViewOptionID            m_CurrentViewOption;             
    wxMenu*                 m_OptionsMenu;
    wxMenu*                 m_ThumbnailViewMenu;
    wxMenu*                 m_PanelsMenu;

    bool                    m_IsSearching;
    bool                    m_IgnoreFolderSelect;
  };

}
