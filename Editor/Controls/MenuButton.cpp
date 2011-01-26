#include "Precompile.h"
#include "MenuButton.h"

#include "Editor/ArtProvider.h"

using namespace Helium;
using namespace Helium::Editor;

//-----------------------------------------------------------
// macros
//-----------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS( MenuButton, wxButton );
BEGIN_EVENT_TABLE( MenuButton, wxButton)
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
 : Button( parent, id, label, pos, size, style, validator, name )
 , m_ContextMenu ( NULL )
 , m_HoldDelay( 0.5f )
 , m_TimerShowOnHold( this )
{
    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( MenuButton::OnRightMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( MenuButton::OnLeftMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_UP, wxMouseEventHandler( MenuButton::OnLeftMouseUp ), NULL, this ); 
    Connect( wxEVT_TIMER, wxTimerEventHandler( MenuButton::HandleTimerEvents ), NULL, this  );

    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuOpen ), NULL, this );
    Connect( wxEVT_MENU_OPEN, wxMenuEventHandler( MenuButton::OnMenuClose ), NULL, this );
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

void MenuButton::DoSetBitmap(const wxBitmap& bitmap, State which)
{
    if ( bitmap.IsOk() )
    {
        wxImage downArrowImage =  wxArtProvider::GetBitmap( ArtIDs::Actions::Down, wxART_OTHER, wxSize( 8, 8 ) ).ConvertToImage();
        
        int separator = 4;
        int width = bitmap.GetWidth() + downArrowImage.GetWidth() + separator;
        int height = bitmap.GetHeight();
        wxImage image( width, height, true );
        image.InitAlpha();

        // set to 0
        for ( int x = 0; x < width; ++x )
        {
            for ( int y = 0; y < height; ++y )
            {
                image.SetAlpha( x, y, 0 );
            }
        }

        if ( image.Ok() && downArrowImage.Ok() )
        {
            image.Paste( bitmap.ConvertToImage(), 0, 0, wxIMAGE_ALPHA_BLEND_COMPOSITE );

            image.Paste(
                ( which == wxButtonBase::State_Disabled ? downArrowImage.ConvertToDisabled() : downArrowImage ),
                ( width - downArrowImage.GetWidth() ),
                ( height/2 - downArrowImage.GetHeight()/2 ),
                wxIMAGE_ALPHA_BLEND_COMPOSITE );
            
            wxBitmap newBitmap = wxBitmap( image );
            __super::DoSetBitmap( newBitmap, which );

            return;
        }
    }
   
    __super::DoSetBitmap( bitmap, which );
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
