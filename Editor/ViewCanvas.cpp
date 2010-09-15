#include "Precompile.h"
#include "ViewCanvas.h"

#include "Editor/EditorIDs.h"
#include "Editor/Input.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;
using namespace Helium::Editor;

ViewCanvas::ViewCanvas( Core::SettingsManager* settingsManager, wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxWindow (parent, winid, pos, size, style, name)
, m_Focused (false)
, m_Viewport ( GetHWND(), settingsManager )
{
    // don't erase background
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    SetHelpText( TXT( "This is the main editing view.  It displays objects in the scene and allows manipulation of the data." ) );

    Connect( wxEVT_SIZE, wxSizeEventHandler( ViewCanvas::OnSize ), NULL, this );

    Connect( wxEVT_PAINT, wxPaintEventHandler( ViewCanvas::OnPaint ), NULL, this );
    Connect( wxEVT_SET_FOCUS, wxFocusEventHandler( ViewCanvas::OnSetFocus ), NULL, this );
    Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( ViewCanvas::OnKillFocus ), NULL, this );

    Connect( wxEVT_KEY_DOWN, wxKeyEventHandler( ViewCanvas::OnKeyDown ), NULL, this );
    Connect( wxEVT_KEY_UP, wxKeyEventHandler( ViewCanvas::OnKeyUp ), NULL, this );
    Connect( wxEVT_CHAR, wxKeyEventHandler( ViewCanvas::OnChar ), NULL, this );

    Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( ViewCanvas::OnMouseDown ), NULL, this );
    Connect( wxEVT_MIDDLE_DOWN, wxMouseEventHandler( ViewCanvas::OnMouseDown ), NULL, this );
    Connect( wxEVT_RIGHT_DOWN, wxMouseEventHandler( ViewCanvas::OnMouseDown ), NULL, this );

    Connect( wxEVT_LEFT_UP, wxMouseEventHandler( ViewCanvas::OnMouseUp ), NULL, this );
    Connect( wxEVT_MIDDLE_UP, wxMouseEventHandler( ViewCanvas::OnMouseUp ), NULL, this );
    Connect( wxEVT_RIGHT_UP, wxMouseEventHandler( ViewCanvas::OnMouseUp ), NULL, this );

    Connect( wxEVT_MOTION, wxMouseEventHandler( ViewCanvas::OnMouseMove ), NULL, this );
    Connect( wxEVT_MOUSEWHEEL, wxMouseEventHandler( ViewCanvas::OnMouseScroll ), NULL, this );
    Connect( wxEVT_LEAVE_WINDOW, wxMouseEventHandler( ViewCanvas::OnMouseLeave ), NULL, this );

    Connect( wxEVT_MOUSE_CAPTURE_LOST, wxMouseCaptureLostEventHandler( ViewCanvas::OnMouseCaptureLost ), NULL, this );

}

void ViewCanvas::OnSize(wxSizeEvent& e)
{
    if ( !m_Viewport.GetDevice() )
    {
        return;
    }

    if ( e.GetSize().x > 0 && e.GetSize().y > 0 )
    {
        m_Viewport.SetSize( e.GetSize().x, e.GetSize().y );
    }

    Refresh();
}

void ViewCanvas::OnPaint(wxPaintEvent& e)
{
    m_Viewport.Draw();
}

void ViewCanvas::OnSetFocus(wxFocusEvent& e)
{
    m_Viewport.SetFocused( true );
    m_Focused = true;
    Refresh();
}

void ViewCanvas::OnKillFocus(wxFocusEvent& e)
{
    m_Viewport.SetFocused( false );
    m_Focused = false;
    Refresh();
}

void ViewCanvas::OnKeyDown( wxKeyEvent& e )
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyDown( input );
    e.Skip( input.GetSkipped() );
    if ( input.GetSkipped() )
    {
        e.ResumePropagation( wxEVENT_PROPAGATE_MAX );
    }
}

void ViewCanvas::OnKeyUp( wxKeyEvent& e )
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyUp( input );
    e.Skip( input.GetSkipped() );
    if ( input.GetSkipped() )
    {
        e.ResumePropagation( wxEVENT_PROPAGATE_MAX );
    }
}

void ViewCanvas::OnChar( wxKeyEvent& e )
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyPress( input );
    e.Skip( input.GetSkipped() );
    if ( input.GetSkipped() )
    {
        e.ResumePropagation( wxEVENT_PROPAGATE_MAX );
    }
}

void ViewCanvas::OnMouseDown( wxMouseEvent& e )
{
    if (!m_Focused)
    {
        // focus and eat the event
        SetFocus();
    }
    else
    {
        // if any key is down
        if (e.LeftIsDown() || e.MiddleIsDown() || e.RightIsDown())
        {
            // and we don't already own the mouse
            if (GetCapture() != this)
            {
                // capture it
                CaptureMouse();
            }
        }

        Helium::MouseButtonInput input;
        Helium::ConvertEvent( e, input );
        m_Viewport.MouseDown( input );
        e.Skip( input.GetSkipped() );
    }

    Refresh();
    Update();
}

void ViewCanvas::OnMouseUp(wxMouseEvent& e)
{
    if (!e.LeftIsDown() && !e.MiddleIsDown() && !e.RightIsDown())
    {
        if (GetCapture() == this)
        {
            ReleaseMouse();
        }
    }

    Helium::MouseButtonInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.MouseUp( input );
    e.Skip( input.GetSkipped() );

    Refresh();
    Update();
}

void ViewCanvas::OnMouseMove(wxMouseEvent& e)
{
    // Grab focus on mouse move if our top level window is active
    if ( !m_Focused )
    {
        wxWindow* parent = GetParent();
        while ( !parent->IsTopLevel() )
            parent = parent->GetParent();

        wxTopLevelWindow* topLevel = reinterpret_cast< wxTopLevelWindow* >( parent );
        if ( topLevel->IsActive() )
        {
            SetFocus();
        }
    }

    if (m_Focused)
    {
        Helium::MouseMoveInput input;
        Helium::ConvertEvent( e, input );
        m_Viewport.MouseMove( input );
        e.Skip( input.GetSkipped() );

        Refresh();
        Update();
    }
}

void ViewCanvas::OnMouseScroll(wxMouseEvent& e)
{
    if (m_Focused)
    {
        Helium::MouseScrollInput input;
        Helium::ConvertEvent( e, input );
        m_Viewport.MouseScroll( input );
        e.Skip( input.GetSkipped() );

        Refresh();
        Update();
    }
}

void ViewCanvas::OnMouseLeave(wxMouseEvent& e)
{
    if (m_Focused)
    {
        // if the mouse skips out without a move event, make sure to clear our highlight
        m_Viewport.RaiseClearHighlight( ClearHighlightArgs (true) );
    }
}

void ViewCanvas::OnMouseCaptureLost(wxMouseCaptureLostEvent& e)
{
    // If this function does not exist, then an assert is thrown off by the wxWidgets 
    // framework when you hold down either the left or middle mouse buttons and click
    // the right mouse button.
}