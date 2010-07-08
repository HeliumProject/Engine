#include "Precompile.h"
#include "NavigationPanel.h"

#include "Application/UI/AutoCompleteComboBox.h"
#include "Application/UI/Button.h"
#include "Application/UI/ArtProvider.h"
#include "Application/UI/MenuButton.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
NavigationPanel::NavigationPanel( BrowserFrame* browserFrame, SearchHistory* history )
: NavigationPanelGenerated( browserFrame )
, m_BrowserFrame( browserFrame )
, m_SearchHistory( history )
, m_NavBarIsFolder( false )
, m_IgnoreNavBarTextChanged( false )
{
  // Assign the button icons
  m_BackButton->SetBitmapLabel( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_BackButton->SetBitmapDisabled( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_BackButton->SetHoldDelay( 0.5f );
  m_BackButton->Enable( false );

  m_ForwardButton->SetBitmapLabel( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_ForwardButton->SetBitmapDisabled( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_ForwardButton->SetHoldDelay( 0.5f );
  m_ForwardButton->Enable( false );

  m_UpFolderButton->SetBitmapLabel( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_UpFolderButton->SetBitmapDisabled( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_UpFolderButton->Enable( false );

  m_GoButton->SetBitmapLabel( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_GoButton->Enable( false );
  
  m_OptionsButton->SetBitmapLabel( wxArtProvider::GetBitmap( Nocturnal::ArtIDs::Unknown ) );
  m_OptionsButton->SetHoldDelay( 0.0f );

  UpdateHistoryMenus();
  UpdateNavBarMRU( m_SearchHistory->GetMRUQueries() );

  // Connect Event Listeners
  m_ForwardButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( NavigationPanel::OnFwdBackButtonMenuOpen ), NULL, this );
  m_BackButton->Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( NavigationPanel::OnFwdBackButtonMenuOpen ), NULL, this );
  m_SearchHistory->AddResetListener( Luna::SearchHistoryChangedSignature::Delegate( this, &NavigationPanel::OnSearchHistoryChanged ) );
  m_SearchHistory->AddHistoryChangedListener( Luna::SearchHistoryChangedSignature::Delegate( this, &NavigationPanel::OnSearchHistoryChanged ) );
  m_SearchHistory->AddMRUQueriesChangedListener( Luna::MRUQueriesChangedSignature::Delegate( this, &NavigationPanel::OnMRUQueriesChanged ) );
}

NavigationPanel::~NavigationPanel()
{
  // Disconnect
  Disconnect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( NavigationPanel::OnBackMenuSelect ), NULL, this );
  Disconnect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( NavigationPanel::OnForwardMenuSelect ), NULL, this );

  // Detatch existing listeners
  m_ForwardButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( NavigationPanel::OnFwdBackButtonMenuOpen ), NULL, this );
  m_BackButton->Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( NavigationPanel::OnFwdBackButtonMenuOpen ), NULL, this );
  m_SearchHistory->RemoveResetListener( Luna::SearchHistoryChangedSignature::Delegate( this, &NavigationPanel::OnSearchHistoryChanged ) );
  m_SearchHistory->RemoveHistoryChangedListener( Luna::SearchHistoryChangedSignature::Delegate( this, &NavigationPanel::OnSearchHistoryChanged ) );
  m_SearchHistory->RemoveMRUQueriesChangedListener( Luna::MRUQueriesChangedSignature::Delegate( this, &NavigationPanel::OnMRUQueriesChanged ) );

  m_HistoryIndexMap.clear();
  m_SearchHistory = NULL;
  m_BrowserFrame = NULL;
}

///////////////////////////////////////////////////////////////////////////////
wxString NavigationPanel::GetNavBarValue() const
{
  wxString comboBoxValue = m_NavBarComboBox->GetValue();
  comboBoxValue.Trim(true);  // trim white-space right 
  comboBoxValue.Trim(false); // trim white-space left

  return comboBoxValue;
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::SetNavBarValue( const tstring& navBarText, bool isFolder )
{
  if ( !m_IgnoreNavBarTextChanged )
  {
    m_IgnoreNavBarTextChanged = true;

    m_NavBarComboBox->SetValue( navBarText.c_str() );
    m_IgnoreNavBarTextChanged = false;
  }

  m_GoButton->Enable( !navBarText.empty() );

  m_NavBarIsFolder = isFolder;
  m_UpFolderButton->Enable( m_NavBarIsFolder );
  if ( m_NavBarIsFolder )
  {
      Nocturnal::Path folder( navBarText );
    //m_UpFolderButton->Enable( FileSystem::HasPrefix( File::GlobalManager().GetManagedAssetsRoot(), folder ) );
#pragma TODO( "enable/disable the button based on the location of the project's assets root" )
  }  
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::UpdateHistoryMenus()
{
  m_HistoryIndexMap.clear();

  // Back menu
  const V_SearchQuery& backHistory = m_SearchHistory->GetBackQuery();
  wxMenu* backMenu = NULL;
  if ( !backHistory.empty() )
  {
    backMenu = new wxMenu();
    int index = (int) backHistory.size() - 1;
    for ( V_SearchQuery::const_reverse_iterator itr = backHistory.rbegin(), end = backHistory.rend(); itr != end; ++itr, --index )
    {
      const SearchQuery* search = (*itr );
      wxMenuItem* item = backMenu->Append( wxID_ANY, search->GetQueryString() );
      Connect( item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( NavigationPanel::OnBackMenuSelect ), NULL, this );
      m_HistoryIndexMap.insert( M_HistoryIndexMap::value_type( item->GetId(), index ) );
    }
  }
  m_BackButton->SetContextMenu( backMenu );
  m_BackButton->Enable( m_SearchHistory->CanGoBack() );

  // Forward menu
  const V_SearchQuery& fwdHistory = m_SearchHistory->GetForwardQuery();
  wxMenu* forwardMenu = NULL;
  if ( !fwdHistory.empty() )
  {
    forwardMenu = new wxMenu();
    int index = (int) fwdHistory.size() - 1;
    for ( V_SearchQuery::const_reverse_iterator itr = fwdHistory.rbegin(), end = fwdHistory.rend(); itr != end; ++itr, --index )
    {
      const SearchQuery* search = (*itr );
      wxMenuItem* item = forwardMenu->Append( wxID_ANY, search->GetQueryString() );
      Connect( item->GetId(), wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( NavigationPanel::OnForwardMenuSelect ), NULL, this );
      m_HistoryIndexMap.insert( M_HistoryIndexMap::value_type( item->GetId(), index ) );
    }
  }
  m_ForwardButton->SetContextMenu( forwardMenu );
  m_ForwardButton->Enable( m_SearchHistory->CanGoForward() );
}

///////////////////////////////////////////////////////////////////////////////
// Updates the MRU if the MRU has changed
// 
void NavigationPanel::UpdateNavBarMRU( const OS_SearchQuery& mruQueries )
{
  wxArrayString mruEntries;
  for ( OS_SearchQuery::Iterator itr = mruQueries.Begin(), end = mruQueries.End(); itr != end; ++itr )
  {
    mruEntries.insert( mruEntries.begin(), (*itr)->GetQueryString() );
  }

  m_NavBarComboBox->Clear();
  if ( !mruEntries.empty() )
  {
    m_NavBarComboBox->Append( mruEntries );
  }
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnBackButtonClick( wxCommandEvent& event )
{
  m_SearchHistory->Back();
  event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnForwardButtonClick( wxCommandEvent& event )
{
  m_SearchHistory->Forward();
  event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnUpFolderButtonClick( wxCommandEvent& event )
{
  if ( m_NavBarIsFolder )
  {
      Nocturnal::Path folder( GetNavBarValue().c_str() );
#pragma TODO( "enable this behavior on the project's asset root" )
    //if ( FileSystem::HasPrefix( File::GlobalManager().GetManagedAssetsRoot(), folder ) )
    //{
    //  m_BrowserFrame->Search( folder );
    //}
  }

  event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnNavBarComboBox( wxCommandEvent& event )
{ 
  event.Skip(); 

  if ( m_IgnoreNavBarTextChanged )
    return;

  // HACKS!
  // TODO: this is a little hacky, faking a button click, but for some reason
  // if we run the search here the comboBox remains empty
  //m_BrowserFrame->Search( GetNavBarValue().c_str() );
  wxCommandEvent goButtonClickedEvent( wxEVT_COMMAND_BUTTON_CLICKED, m_GoButton->GetId() );
  wxPostEvent( m_GoButton, goButtonClickedEvent );
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnNavBarComboBoxMouseWheel( wxMouseEvent& event )
{
  // Swallow the mousewheel event from the combo box.  It is annoying. (Core DT#995)
  event.Skip( false );
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnNavBarText( wxCommandEvent& event )
{ 
  event.Skip(); 

  if ( m_IgnoreNavBarTextChanged )
    return;

  m_GoButton->Enable( !GetNavBarValue().empty() );  
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnNavBarTextEnter( wxCommandEvent& event )
{
  event.Skip();

  if ( m_IgnoreNavBarTextChanged )
    return;
  
  m_BrowserFrame->Search( (const wxChar*)GetNavBarValue().c_str() ); 
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnGoButtonClick( wxCommandEvent& event )
{
  event.Skip();
  
  m_BrowserFrame->Search( (const wxChar*)GetNavBarValue().c_str() );
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnOptionsButtonClick( wxCommandEvent& event )
{ 
  event.Skip(); 
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnBackMenuSelect( wxCommandEvent& event )
{
  event.Skip(); 
  
  // find what they selected in m_HistoryIndexMap
  int historyIndex = -1;
  int menuItemID = event.GetId();
  M_HistoryIndexMap::const_iterator findIndex = m_HistoryIndexMap.find( menuItemID );
  if ( findIndex != m_HistoryIndexMap.end() )
  {
    historyIndex = findIndex->second;
  }

  m_SearchHistory->Back( historyIndex );
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnForwardMenuSelect( wxCommandEvent& event )
{
  event.Skip(); 
  
  // find what they selected in m_HistoryIndexMap
  int historyIndex = -1;
  int menuItemID = event.GetId();
  M_HistoryIndexMap::const_iterator findIndex = m_HistoryIndexMap.find( menuItemID );
  if ( findIndex != m_HistoryIndexMap.end() )
  {
    historyIndex = findIndex->second;
  }

  m_SearchHistory->Forward( historyIndex );
}

///////////////////////////////////////////////////////////////////////////////
// Called when a menu is about to be opened.  Enables and disables items in the
// menu as appropriate.
// 
void NavigationPanel::OnFwdBackButtonMenuOpen( wxMenuEvent& event )
{
  event.Skip();
  UpdateHistoryMenus();
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnSearchHistoryChanged( const Luna::SearchHistoryChangeArgs& args )
{ 
  UpdateHistoryMenus();
}

///////////////////////////////////////////////////////////////////////////////
void NavigationPanel::OnMRUQueriesChanged( const Luna::MRUQueriesChangedArgs& args )
{
  UpdateNavBarMRU( args.m_MRUQueries );
}

