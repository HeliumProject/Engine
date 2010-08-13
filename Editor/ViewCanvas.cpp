#include "Precompile.h"
#include "ViewCanvas.h"

#include "Editor/EditorIDs.h"
#include "Editor/Input.h"
#include "Editor/App.h"

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE(Editor::ViewCanvas, wxWindow)

EVT_SIZE(ViewCanvas::OnSize)

EVT_PAINT(ViewCanvas::OnPaint)
EVT_SET_FOCUS(ViewCanvas::OnSetFocus)
EVT_KILL_FOCUS(ViewCanvas::OnKillFocus)

EVT_KEY_DOWN(ViewCanvas::OnKeyDown)
EVT_KEY_UP(ViewCanvas::OnKeyUp)
EVT_CHAR(ViewCanvas::OnChar)

EVT_LEFT_DOWN(ViewCanvas::OnMouseDown)
EVT_MIDDLE_DOWN(ViewCanvas::OnMouseDown)
EVT_RIGHT_DOWN(ViewCanvas::OnMouseDown)

EVT_LEFT_UP(ViewCanvas::OnMouseUp)
EVT_MIDDLE_UP(ViewCanvas::OnMouseUp)
EVT_RIGHT_UP(ViewCanvas::OnMouseUp)

EVT_MOTION(ViewCanvas::OnMouseMove)
EVT_MOUSEWHEEL(ViewCanvas::OnMouseScroll)
EVT_LEAVE_WINDOW(ViewCanvas::OnMouseLeave)

EVT_MOUSE_CAPTURE_LOST(ViewCanvas::OnMouseCaptureLost)

END_EVENT_TABLE()

ViewCanvas::ViewCanvas(wxWindow *parent, wxWindowID winid, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxWindow (parent, winid, pos, size, style, name)
, m_Focused (false)
, m_Viewport (GetHWND())
{
    // don't erase background
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    SetHelpText( TXT( "This is the main editing view.  It displays objects in the scene and allows manipulation of the data." ) );
}

void ViewCanvas::OnSize(wxSizeEvent& e)
{
    if ( !m_Viewport.GetDevice() )
    {
        return;
    }

    if ( e.GetSize().x > 0 && e.GetSize().y > 0 )
    {
        m_Viewport.Resize( e.GetSize().x, e.GetSize().y );
    }

    Refresh();
}

void ViewCanvas::OnPaint(wxPaintEvent& e)
{
    // draw
    m_Viewport.Draw();

    // set our entire window to be valid
    ::ValidateRect( (HWND)GetHandle(), NULL );
}

void ViewCanvas::OnSetFocus(wxFocusEvent& e)
{
    m_Focused = true;

    Refresh();
}

void ViewCanvas::OnKillFocus(wxFocusEvent& e)
{
    m_Focused = false;

    Refresh();
}

void ViewCanvas::OnKeyDown(wxKeyEvent& e)
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyDown( input );
    e.Skip( input.GetSkipped() );

    if (e.GetSkipped())
    {
        wxWindow* frame = GetParent();
        while (frame->GetParent() != NULL)
            frame = frame->GetParent();
        frame->GetEventHandler()->ProcessEvent(e);
    }
}

void ViewCanvas::OnKeyUp(wxKeyEvent& e)
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyUp( input );
    e.Skip( input.GetSkipped() );

    if (e.GetSkipped())
    {
        wxWindow* frame = GetParent();
        while (frame->GetParent() != NULL)
            frame = frame->GetParent();
        frame->GetEventHandler()->ProcessEvent(e);
    }
}

void ViewCanvas::OnChar(wxKeyEvent& e)
{
    Helium::KeyboardInput input;
    Helium::ConvertEvent( e, input );
    m_Viewport.KeyPress( input );
    e.Skip( input.GetSkipped() );

    if (e.GetSkipped())
    {
        wxWindow* frame = GetParent();
        while (frame->GetParent() != NULL)
            frame = frame->GetParent();
        frame->GetEventHandler()->ProcessEvent(e);
    }
}

void ViewCanvas::OnMouseDown(wxMouseEvent& e)
{
#pragma TODO("Freeze here -Geoff")

    if (!m_Focused)
    {
        // focus and eat the event
        SetFocus();
    }
    else
    {
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
#pragma TODO("Freeze here -Geoff")

    // focus
    SetFocus();

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
        if ( parent->IsTopLevel() )
        {
            wxTopLevelWindow* topLevel = reinterpret_cast< wxTopLevelWindow* >( parent );
            if ( topLevel->IsActive() )
            {
                SetFocus();
            }
        }
    }

    if (m_Focused)
    {
#pragma TODO("Freeze here -Geoff")

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