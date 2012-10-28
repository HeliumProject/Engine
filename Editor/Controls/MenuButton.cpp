#include "EditorPch.h"
#include "MenuButton.h"

#include "Editor/App.h"
#include "Editor/ArtProvider.h"
#include "Editor/Settings/EditorSettings.h"

using namespace Helium;
using namespace Helium::Editor;

//-----------------------------------------------------------
// macros
//-----------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( MenuButton, EditorButton );
BEGIN_EVENT_TABLE( MenuButton, EditorButton )
END_EVENT_TABLE();

MenuButton::MenuButton
(
 wxWindow *parent,
 wxWindowID id,
 const wxString& label,
 const wxPoint& pos,
 const wxSize& size,
 long style,
 const wxValidator& validator,
 const wxString& name
 ) 
 : EditorButton( parent, id, pos, size, style, name )
 , m_ContextMenu ( NULL )
 , m_HoldDelay( 0.5f )
 , m_TimerShowOnHold( this )
{
    SetExtraStyle( GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES );

    wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    SetSizer( sizer );

    SetLabel( label );
    m_Text = new wxStaticText( this, wxID_ANY, GetLabel() );
    sizer->Add( m_Text );

    m_Bitmap = new wxStaticBitmap( this, wxID_ANY, wxBitmap() );
    sizer->Add( m_Bitmap );

    m_Arrow = new wxStaticBitmap( this, wxID_ANY, wxArtProvider::GetBitmap( ArtIDs::Actions::Down, wxART_OTHER, wxSize( 8, 8 ) ) );
    sizer->Add( m_Arrow, 0, wxALIGN_BOTTOM | wxALIGN_RIGHT, 0 );

    Layout();

    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MenuButton::OnRightMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MenuButton::OnLeftMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_UP, wxMouseEventHandler( MenuButton::OnLeftMouseUp ), NULL, this ); 
    Connect( wxEVT_TIMER, wxTimerEventHandler( MenuButton::HandleTimerEvents ), NULL, this  );

    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuOpen ), NULL, this );
    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuClose ), NULL, this );

    Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MenuButton::OnUpdateUI ), NULL, this );
}

MenuButton::~MenuButton(void)
{ 
    if( m_ContextMenu )
    {
        delete m_ContextMenu;
    }

    Disconnect( wxEVT_TIMER, wxTimerEventHandler( MenuButton::HandleTimerEvents ), NULL, this  );
    Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( MenuButton::OnLeftMouseUp ), NULL, this ); 
    Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MenuButton::OnLeftMouseDown ), NULL, this );
    Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MenuButton::OnRightMouseDown ), NULL, this );

    Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuOpen ), NULL, this );
    Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuClose ), NULL, this );

    Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( MenuButton::OnUpdateUI ), NULL, this );
}

void MenuButton::SetContextMenu( wxMenu * menu ) 
{ 
    if( m_ContextMenu )
    {
        delete m_ContextMenu;
    }

    m_ContextMenu = menu;
}

wxMenu* MenuButton::DetachContextMenu( ) 
{ 
    wxMenu* tmp = m_ContextMenu;
    m_ContextMenu = NULL;

    return tmp; 
}

wxMenu* MenuButton::GetContextMenu( ) const 
{ 
    return m_ContextMenu; 
}

void MenuButton::SetBitmap( const wxBitmap& bitmap )
{
    HELIUM_ASSERT( bitmap.IsOk() );

    m_Bitmap->SetBitmap( bitmap );
    Refresh();
    Layout();
}

void MenuButton::OnUpdateUI( wxUpdateUIEvent& event )
{
    EditorSettings* settings = wxGetApp().GetSettingsManager()->GetSettings< EditorSettings >();
    if ( !settings )
    {
        return;
    }

    bool needsUpdate = false;

    if ( settings->m_ShowTextOnButtons != m_ShowText )
    {
        m_ShowText = settings->m_ShowTextOnButtons;
        needsUpdate = true;
    }

    if ( settings->m_ShowIconsOnButtons != m_ShowIcons )
    {
        m_ShowIcons = settings->m_ShowIconsOnButtons;
        needsUpdate = true;
    }

    if ( !needsUpdate )
    {
        return;
    }

    if ( m_ShowIcons )
    {
        m_Bitmap->Show();
    }
    else
    {
        m_Bitmap->Hide();
    }

    if ( m_ShowText )
    {
        m_Text->Show();
    }
    else
    {
        m_Text->Hide();
    }

    GetSizer()->RecalcSizes();
    Refresh();
    Layout();
    GetParent()->GetSizer()->RecalcSizes();
    GetParent()->Refresh();
    GetParent()->Layout();
    event.Skip();
}

void MenuButton::OnRightMouseDown( wxMouseEvent& event )
{
    if( IsMouseInWindow() )
    {
        ShowPopupMenu();   
    }

    // we still want other things to process this button click
    event.Skip();  
}

void MenuButton::OnLeftMouseDown( wxMouseEvent& event )
{
    if( m_ContextMenu && IsMouseInWindow() )
    {
        if ( m_HoldDelay > 0.0f )
        {
            m_TimerShowOnHold.Start( m_HoldDelay * 1000, wxTIMER_ONE_SHOT );
        }
        else
        {
            ShowPopupMenu();
        }
    }

    // we still want other things to process this button click
    event.Skip();  
}

void MenuButton::OnLeftMouseUp( wxMouseEvent& event )
{
    m_TimerShowOnHold.Stop( );

    // we still want other things to process this button click
    event.Skip();
}

void MenuButton::OnMenuOpen( wxMenuEvent& event )
{
    event.Skip();
}

void MenuButton::OnMenuClose( wxMenuEvent& event )
{
    event.Skip();
}

void MenuButton::ShowPopupMenu()
{
    if( m_ContextMenu )
    {
        int height;
        GetSize( NULL, &height );

        // we don't have the menu id here, so we use the id to specify if the event
        // was from a popup menu or a normal one
        //isPopup ? -1 : 0
        wxMenuEvent openEvent(wxEVT_MENU_OPEN, -1, m_ContextMenu);
        openEvent.SetEventObject( this );
        GetEventHandler()->ProcessEvent( openEvent );

        PopupMenu( m_ContextMenu, 0, height );

        wxMenuEvent closeEvent(wxEVT_MENU_CLOSE, -1, m_ContextMenu);
        closeEvent.SetEventObject( this );
        GetEventHandler()->ProcessEvent( closeEvent );

        wxPostEvent( this, wxFocusEvent( wxEVT_KILL_FOCUS ) );
    }
}

void MenuButton::HandleTimerEvents( wxTimerEvent& event )
{
    if( event.GetId() == m_TimerShowOnHold.GetId() )
    {
        ShowPopupMenu();
    }
}
