#include "Precompile.h"
#include "MenuButton.h"

using namespace Helium;
using namespace Helium::Editor;

//-----------------------------------------------------------
// macros
//-----------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( MenuButton, wxButton );
BEGIN_EVENT_TABLE( MenuButton, wxButton)
END_EVENT_TABLE();

MenuButton::MenuButton(wxWindow *parent,
                       wxWindowID id,
                       const wxBitmap& bitmap,
                       const wxPoint& pos,
                       const wxSize& size,
                       long style,
                       const wxValidator& validator,
                       const wxString& name ) 
                       : wxButton( parent, id, wxEmptyString, pos, size, style, validator, name )
                       , m_contextMenu ( NULL )
                       , m_holdDelay( 0.5f )
                       , m_timerShowOnHold( this )
{
    SetBitmap( bitmap );

    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MenuButton::OnRightMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MenuButton::OnLeftMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_UP, wxMouseEventHandler( MenuButton::OnLeftMouseUp ), NULL, this ); 
    Connect( wxEVT_TIMER, wxTimerEventHandler( MenuButton::HandleTimerEvents ), NULL, this  );

    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuOpen ), NULL, this );
    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuClose ), NULL, this );
}

MenuButton::~MenuButton(void)
{ 
    if( m_contextMenu )
    {
        delete m_contextMenu;
    }

    Disconnect( wxEVT_TIMER, wxTimerEventHandler( MenuButton::HandleTimerEvents ), NULL, this  );
    Disconnect( wxEVT_LEFT_UP, wxMouseEventHandler( MenuButton::OnLeftMouseUp ), NULL, this ); 
    Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MenuButton::OnLeftMouseDown ), NULL, this );
    Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MenuButton::OnRightMouseDown ), NULL, this );

    Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuOpen ), NULL, this );
    Disconnect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuClose ), NULL, this );
}

void MenuButton::SetContextMenu( wxMenu * menu ) 
{ 
    if( m_contextMenu )
    {
        delete m_contextMenu;
    }

    m_contextMenu = menu;
}

wxMenu* MenuButton::DetachContextMenu( ) 
{ 
    wxMenu* tmp = m_contextMenu;
    m_contextMenu = NULL;

    return tmp; 
}

wxMenu* MenuButton::GetContextMenu( ) const 
{ 
    return m_contextMenu; 
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
    if( m_contextMenu && IsMouseInWindow() )
    {
        if ( m_holdDelay > 0.0f )
        {
            m_timerShowOnHold.Start( m_holdDelay * 1000, wxTIMER_ONE_SHOT );
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
    m_timerShowOnHold.Stop( );

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
    if( m_contextMenu )
    {
        int height;
        GetSize( NULL, &height );

        // we don't have the menu id here, so we use the id to specify if the event
        // was from a popup menu or a normal one
        //isPopup ? -1 : 0
        wxMenuEvent openEvent(wxEVT_MENU_OPEN, -1, m_contextMenu);
        openEvent.SetEventObject( this );
        GetEventHandler()->ProcessEvent( openEvent );

        PopupMenu( m_contextMenu, 0, height );

        wxMenuEvent closeEvent(wxEVT_MENU_CLOSE, -1, m_contextMenu);
        closeEvent.SetEventObject( this );
        GetEventHandler()->ProcessEvent( closeEvent );

        wxPostEvent( this, wxFocusEvent( wxEVT_KILL_FOCUS ) );
    }
}

void MenuButton::HandleTimerEvents( wxTimerEvent& event )
{
    if( event.GetId() == m_timerShowOnHold.GetId() )
    {
        ShowPopupMenu();
    }
}
