#pragma once

#include "BrowserGenerated.h"
#include "BrowserFrame.h"
#include "SearchHistory.h"

namespace Luna
{

  ///////////////////////////////////////////////////////////////////////////////
  /// Class NavigationPanel
  ///////////////////////////////////////////////////////////////////////////////
  class NavigationPanel : public NavigationPanelGenerated 
  {
  public:
    NavigationPanel( BrowserFrame* browserFrame, SearchHistory* history );
    ~NavigationPanel();

    wxString GetNavBarValue() const;
    void SetNavBarValue( const tstring& navBarText, bool isFolder = false );

  protected:
    // Virtual event handlers, overide them in your derived class
		virtual void OnBackButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnForwardButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnUpFolderButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnNavBarComboBox( wxCommandEvent& event ) NOC_OVERRIDE;
    virtual void OnNavBarComboBoxMouseWheel( wxMouseEvent& event ) NOC_OVERRIDE;
    virtual void OnNavBarText( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnNavBarTextEnter( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnGoButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;
		virtual void OnOptionsButtonClick( wxCommandEvent& event ) NOC_OVERRIDE;

    virtual void OnBackMenuSelect( wxCommandEvent& event );
    virtual void OnForwardMenuSelect( wxCommandEvent& event );

    // Event Listener Callbacks
    void OnFwdBackButtonMenuOpen( wxMenuEvent& event );
    void OnSearchHistoryChanged( const Luna::SearchHistoryChangeArgs& args );
    void OnMRUQueriesChanged( const Luna::MRUQueriesChangedArgs& args );

  private:
    typedef std::map< int, int > M_HistoryIndexMap;
    M_HistoryIndexMap m_HistoryIndexMap;

    BrowserFrame*     m_BrowserFrame;
    SearchHistory*    m_SearchHistory;

    bool m_NavBarIsFolder;
    bool m_IgnoreNavBarTextChanged;

    void UpdateHistoryMenus();
    void UpdateNavBarMRU( const OS_SearchQuery& mruQueries );
  };
}