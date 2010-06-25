#include "Precompile.h"

#include "Browser.h"
#include "BrowserToolBar.h"

#include "Platform/Assert.h"
#include "Application/UI/ImageManager.h"

#include <wx/textctrl.h>

using namespace Luna;


///////////////////////////////////////////////////////////////////////////////
BrowserToolBar::BrowserToolBar
(
 wxWindow *parent,
 wxWindowID id,
 const wxPoint& pos,
 const wxSize& size,
 long style,
 const wxString& name
 )
 : wxToolBar( parent, id, pos, size, style, name )
{
  m_SearchBox = new wxTextCtrl( this, BrowserToolBarIDs::SearchBoxID, wxEmptyString, wxDefaultPosition, wxSize( 140, -1 ), 0 );

  SetToolBitmapSize( wxSize( 16, 16 ) );
//  AddTool( BrowserToolBarIDs::ButtonID, wxT( "Vault" ), Nocturnal::GlobalImageManager().GetBitmap( "vault_16.png" ) );
  AddControl( m_SearchBox );
  Realize();

  // Connect Events
  // wxEVT_KEY_DOWN or wxEVT_CHAR?
  m_SearchBox->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( BrowserToolBar::OnKeyDown ), NULL, this );
  m_SearchBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( BrowserToolBar::OnTextEnter ), NULL, this );
  Connect( BrowserToolBarIDs::ButtonID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserToolBar::OnButtonClick ), NULL, this );
}

BrowserToolBar::~BrowserToolBar()
{
  // Disconnect Events
  m_SearchBox->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( BrowserToolBar::OnKeyDown ), NULL, this );
  m_SearchBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( BrowserToolBar::OnTextEnter ), NULL, this );
  Disconnect( BrowserToolBarIDs::ButtonID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BrowserToolBar::OnButtonClick ), NULL, this );

  delete m_SearchBox;
  m_SearchBox = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to open AssetVault. 
// Opens the asset vault with no search string
void BrowserToolBar::OnButtonClick( wxCommandEvent& args )
{
  wxString value = m_SearchBox->GetValue();
  value.Trim(true);  // trim white-space right 
  value.Trim(false); // trim white-space left
#pragma TODO( "reimplemnent without GlobalBrowser" )
  //  GlobalBrowser().ShowBrowser( value.c_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Catch the RETURN OnChar event and simulate the search button being clicked
// plus send the query string from m_SearchBox to the Browser
//
void BrowserToolBar::OnKeyDown( wxKeyEvent& evt )
{ 
  wxString key;
  long keycode = evt.GetKeyCode();
  if ( keycode == WXK_RETURN )
  {
    wxString value = m_SearchBox->GetValue();
    value.Trim(true);  // trim white-space right 
    value.Trim(false); // trim white-space left
#pragma TODO( "reimplemnent without GlobalBrowser" )
    //if ( !value.empty() )
    //{
    //  GlobalBrowser().ShowBrowser( value.c_str() );
    //}
    //else
    //{
    //  GlobalBrowser().ShowBrowser();
    //}
  }

  evt.Skip();
}

void BrowserToolBar::OnTextEnter( wxCommandEvent& event )
{
  wxString value = m_SearchBox->GetValue();
  value.Trim(true);  // trim white-space right 
  value.Trim(false); // trim white-space left
#pragma TODO( "reimplemnent without GlobalBrowser" )
  //if ( !value.empty() )
  //{
  //  GlobalBrowser().ShowBrowser( value.c_str() );

  //  // store the results from entering a search in the navigation bar
  //  BrowserSearchDatabase::UpdateSearchEvents( value.c_str() );
  //}
  //else
  //{
  //  GlobalBrowser().ShowBrowser();
  //}
  event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
wxAuiPaneInfo BrowserToolBar::GetAuiPaneInfo( int position )
{
  wxAuiPaneInfo info; 
  info.Name( wxT( "browsertoolbar" ) ); 
  info.DestroyOnClose( false ); 
  info.Caption( wxT( "Vault Toolbar" ) ); 
  info.ToolbarPane(); 
  info.Top(); 
  //info.Resizable();
  //info.MinSize( 350, -1 );

  if ( position > 0 )
  {
    info.Position( position );
  }

  info.LeftDockable( false ); 
  info.RightDockable( false ); 

  return info;
}
