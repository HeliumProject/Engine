#include "Precompile.h"

#include "Editor/App.h"
#include "Vault.h"
#include "VaultToolBar.h"

#include "Platform/Assert.h"
#include "Application/UI/ArtProvider.h"

#include <wx/textctrl.h>

using namespace Helium;
using namespace Helium::Editor;


///////////////////////////////////////////////////////////////////////////////
VaultToolBar::VaultToolBar
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
    SetToolBitmapSize( wxSize( 16, 16 ) );
    AddTool( VaultToolBarIDs::ButtonID, wxT( "Vault" ), wxArtProvider::GetBitmap( wxART_FIND ) );

    m_SearchBox = new wxTextCtrl( this, VaultToolBarIDs::SearchBoxID, wxEmptyString, wxDefaultPosition, wxSize( 140, -1 ), 0 );
    AddControl( m_SearchBox );

    // Connect Events
    // wxEVT_KEY_DOWN or wxEVT_CHAR?
    m_SearchBox->Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( VaultToolBar::OnKeyDown ), NULL, this );
    m_SearchBox->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultToolBar::OnTextEnter ), NULL, this );
    Connect( VaultToolBarIDs::ButtonID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultToolBar::OnButtonClick ), NULL, this );
}

VaultToolBar::~VaultToolBar()
{
    // Disconnect Events
    m_SearchBox->Disconnect( wxEVT_KEY_DOWN, wxKeyEventHandler( VaultToolBar::OnKeyDown ), NULL, this );
    m_SearchBox->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( VaultToolBar::OnTextEnter ), NULL, this );
    Disconnect( VaultToolBarIDs::ButtonID, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( VaultToolBar::OnButtonClick ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the user causes a UI event to open AssetVault. 
// Opens the asset vault with no search string
void VaultToolBar::OnButtonClick( wxCommandEvent& args )
{
    wxString value = m_SearchBox->GetValue();
    value.Trim(true);  // trim white-space right 
    value.Trim(false); // trim white-space left
    wxGetApp().GetVault().ShowVault( value.wx_str() );
}

///////////////////////////////////////////////////////////////////////////////
// Catch the RETURN OnChar event and simulate the search button being clicked
// plus send the query string from m_SearchBox to the Vault
//
void VaultToolBar::OnKeyDown( wxKeyEvent& evt )
{ 
    wxString key;
    long keycode = evt.GetKeyCode();
    if ( keycode == WXK_RETURN )
    {
        wxString value = m_SearchBox->GetValue();
        value.Trim(true);  // trim white-space right 
        value.Trim(false); // trim white-space left

        wxGetApp().GetVault().ShowVault( value.wx_str() );
    }

    evt.Skip();
}

void VaultToolBar::OnTextEnter( wxCommandEvent& event )
{
    wxString value = m_SearchBox->GetValue();
    value.Trim(true);  // trim white-space right 
    value.Trim(false); // trim white-space left

    wxGetApp().GetVault().ShowVault( value.wx_str() );

    //// store the results from entering a search in the navigation bar
    //VaultSearchDatabase::UpdateSearchEvents( value.wx_str() );

    event.Skip();
}

///////////////////////////////////////////////////////////////////////////////
wxAuiPaneInfo VaultToolBar::GetAuiPaneInfo( int position )
{
    wxAuiPaneInfo info; 
    info.Name( wxT( "vaulttoolbar" ) ); 
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
