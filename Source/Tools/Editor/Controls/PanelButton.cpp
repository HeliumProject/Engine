#include "Precompile.h"
#include "PanelButton.h"

#include "Foundation/Flags.h"
#include "Editor/ArtProvider.h"
#include "Editor/Utilities.h"

using namespace Helium;
using namespace Helium::Editor;

static int s_MouseLocationTimerFrequency = 100;
static wxColor s_HighlighColor( 8, 8, 8 );
static wxColor s_ToggledColor( 210, 210, 220 );

wxColor operator+( const wxColor& lhs, const wxColor& rhs )
{
    return wxColor( lhs.Red() + rhs.Red(), lhs.Green() + rhs.Green(), lhs.Blue() + rhs.Blue() );
}

wxColor operator-( const wxColor& lhs, const wxColor& rhs )
{
    return wxColor( lhs.Red() - rhs.Red(), lhs.Green() - rhs.Green(), lhs.Blue() - rhs.Blue() );
}

IMPLEMENT_DYNAMIC_CLASS( PanelButton, wxPanel );

PanelButton::PanelButton()
: wxPanel()
{
}

PanelButton::PanelButton( wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxPanel( parent, id, pos, size, style, name )
, m_Options( PanelButtonOptions::Default )
, m_Toggled( false )
{
    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( PanelButton::OnRightMouseDown ), NULL, this );
    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( PanelButton::OnLeftMouseDown ), NULL, this );
    Connect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( PanelButton::OnMouseEnter ), NULL, this );
    Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( PanelButton::OnMouseLeave ), NULL, this );

    m_MouseTimer.SetOwner( this );
    Connect( m_MouseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( PanelButton::OnMouseTimer ), NULL, this );
}

PanelButton::~PanelButton()
{
	// TODO: Disconnect mouse events on our children?
    Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( PanelButton::OnRightMouseDown ), NULL, this );
    Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( PanelButton::OnLeftMouseDown ), NULL, this );
    Disconnect( wxEVT_ENTER_WINDOW, wxMouseEventHandler( PanelButton::OnMouseEnter ), NULL, this );
    Disconnect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( PanelButton::OnMouseLeave ), NULL, this );

    m_MouseTimer.Stop();
    Disconnect( m_MouseTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( PanelButton::OnMouseTimer ), NULL, this );

}

PanelButtonOption PanelButton::GetOptions() const
{
    return m_Options;
}

void PanelButton::SetOptions( const PanelButtonOption option )
{
    if ( option != m_Options )
    {
        m_Options = option;

        Refresh( true );
    }
}

bool PanelButton::GetValue() const
{
    return m_Toggled;
}

void PanelButton::SetValue( bool value )
{
    m_Toggled = value;

    if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) && m_Toggled )
    {
        SetBackgroundColour( s_ToggledColor );
    }
    else
    {
        SetBackgroundColour( wxNullColour );
    }

    Refresh( true );
}

void PanelButton::OnMouseEnter( wxMouseEvent& event )
{
    if ( Helium::IsMouseInWindow( this ) )
    {
        // we have to reset the background color since we can get multiple mouse enters
        if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) && GetValue() )
        {
            SetBackgroundColour( s_ToggledColor );
        }
        else
        {
            SetBackgroundColour( wxNullColour );
        }

        SetBackgroundColour( GetBackgroundColour() + s_HighlighColor );

        Refresh( true );
        m_MouseTimer.Start( s_MouseLocationTimerFrequency );
    }
}

void PanelButton::OnMouseLeave( wxMouseEvent& event )
{
    if ( !Helium::IsMouseInWindow( this ) )
    {
        if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) && GetValue() )
        {
            SetBackgroundColour( s_ToggledColor );
        }
        else
        {
            SetBackgroundColour( wxNullColour );
        }

        Refresh( true );
    }
}

void PanelButton::OnRightMouseDown( wxMouseEvent& event )
{
    OnLeftMouseDown( event );
}

void PanelButton::OnLeftMouseDown( wxMouseEvent& event )
{
    if( Helium::IsMouseInWindow( this ) )
    {
        if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) )
        {
            SetValue( !GetValue() ); // switch our toggle state
            if ( GetValue() )
            {
                SetBackgroundColour( s_ToggledColor );
            }
            else
            {
                SetBackgroundColour( wxNullColour );
            }
        }

        SendClickOrToggleEvent();
        event.Skip();
    }
}

void PanelButton::OnMouseTimer( wxTimerEvent& event )
{
    event.Skip();

    if ( !Helium::IsMouseInWindow( this ) )
    {
        if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) && GetValue() )
        {
            SetBackgroundColour( s_ToggledColor );
        }
        else
        {
            SetBackgroundColour( wxNullColour );
        }

        Refresh( true );
        m_MouseTimer.Stop();
    } 
}

bool PanelButton::SendClickOrToggleEvent()
{
    if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) )
    {
        wxCommandEvent event( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, GetId() );
        event.SetInt( GetValue() ? 1 : 0 );
        event.SetEventObject( this );
        return ProcessEvent( event );
    }
    else
    {
        wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
        event.SetEventObject( this );
        return ProcessEvent( event );
    }
}

void PanelButton::AddChild( wxWindowBase *child )
{
    wxPanel::AddChild( child );

    child->Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( PanelButton::OnRightMouseDown ), NULL, this );
    child->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( PanelButton::OnLeftMouseDown ), NULL, this );
}

void PanelButton::RemoveChild( wxWindowBase *child )
{
    wxPanel::RemoveChild( child );

    child->Disconnect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( PanelButton::OnRightMouseDown ), NULL, this );
    child->Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( PanelButton::OnLeftMouseDown ), NULL, this );
}

void PanelButton::Command( wxCommandEvent& event )
{
    if( HasFlags< PanelButtonOption >( m_Options, PanelButtonOptions::Toggle ) )
    {
        SetValue( !GetValue() );
    }

    ProcessEvent( event );
}
