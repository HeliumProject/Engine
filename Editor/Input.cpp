#include "Precompile.h"
#include "Input.h"

using namespace Helium;

#pragma TODO("Wrangle ConvertEvent and Skip")

static void ConvertMouseEvent( wxMouseEvent& event, MouseInput& input )
{
    uint32_t buttons = 0x0;

    buttons |= event.LeftIsDown() ? MouseButtons::Left : 0x0;
    buttons |= event.MiddleIsDown() ? MouseButtons::Middle : 0x0;
    buttons |= event.RightIsDown() ? MouseButtons::Right : 0x0;
    buttons |= event.Aux1IsDown() ? MouseButtons::Backward : 0x0;
    buttons |= event.Aux2IsDown() ? MouseButtons::Forward : 0x0;

    buttons |= event.AltDown() ? AcceleratorButtons::Alt : 0x0;
    buttons |= event.ControlDown() ? AcceleratorButtons::Ctrl : 0x0;
    buttons |= event.ShiftDown() ? AcceleratorButtons::Shift : 0x0;

    input.SetButtons( buttons );

    input.SetPosition( Math::Point( event.GetX(), event.GetY() ) );
}

void Helium::ConvertEvent( wxMouseEvent& event, MouseMoveInput& input )
{
    new (&input) MouseMoveInput ();

    ConvertMouseEvent( event, input );
}

void Helium::ConvertEvent( wxMouseEvent& event, MouseButtonInput& input )
{
    new (&input) MouseButtonInput ();

    ConvertMouseEvent( event, input );

    if ( event.LeftUp() )           { input.SetEvent( MouseButtonEvents::Up ); input.SetButton( MouseButtons::Left ); }
    if ( event.MiddleUp() )         { input.SetEvent( MouseButtonEvents::Up ); input.SetButton( MouseButtons::Middle ); }
    if ( event.RightUp() )          { input.SetEvent( MouseButtonEvents::Up ); input.SetButton( MouseButtons::Right ); }
    if ( event.Aux1Up() )           { input.SetEvent( MouseButtonEvents::Up ); input.SetButton( MouseButtons::Forward ); }
    if ( event.Aux2Up() )           { input.SetEvent( MouseButtonEvents::Up ); input.SetButton( MouseButtons::Backward ); }

    if ( event.LeftDown() )         { input.SetEvent( MouseButtonEvents::Down ); input.SetButton( MouseButtons::Left ); }
    if ( event.MiddleDown() )       { input.SetEvent( MouseButtonEvents::Down ); input.SetButton( MouseButtons::Middle ); }
    if ( event.RightDown() )        { input.SetEvent( MouseButtonEvents::Down ); input.SetButton( MouseButtons::Right ); }
    if ( event.Aux1Down() )         { input.SetEvent( MouseButtonEvents::Down ); input.SetButton( MouseButtons::Forward ); }
    if ( event.Aux2Down() )         { input.SetEvent( MouseButtonEvents::Down ); input.SetButton( MouseButtons::Backward ); }

    if ( event.LeftDClick() )       { input.SetEvent( MouseButtonEvents::DoubleClick ); input.SetButton( MouseButtons::Left ); }
    if ( event.MiddleDClick() )     { input.SetEvent( MouseButtonEvents::DoubleClick ); input.SetButton( MouseButtons::Middle ); }
    if ( event.RightDClick() )      { input.SetEvent( MouseButtonEvents::DoubleClick ); input.SetButton( MouseButtons::Right ); }
    if ( event.Aux1DClick() )       { input.SetEvent( MouseButtonEvents::DoubleClick ); input.SetButton( MouseButtons::Forward ); }
    if ( event.Aux2DClick() )       { input.SetEvent( MouseButtonEvents::DoubleClick ); input.SetButton( MouseButtons::Backward ); }
}

void Helium::ConvertEvent( wxMouseEvent& event, MouseScrollInput& input )
{
    new (&input) MouseScrollInput ();

    ConvertMouseEvent( event, input );

    input.SetWheelRotation( event.GetWheelRotation() );
    input.SetWheelDelta( event.GetWheelDelta() );
}

void Helium::ConvertEvent( wxKeyEvent& event, KeyboardInput& input )
{
    new (&input) KeyboardInput ();

    KeyCode code;

    switch ( event.GetKeyCode() )
    {
    case WXK_ESCAPE:    code = KeyCodes::Escape; break;
    case WXK_TAB:       code = KeyCodes::Tab; break;
    case WXK_BACK:      code = KeyCodes::Backspace; break;
    case WXK_RETURN:    code = KeyCodes::Return; break;
    case WXK_INSERT:    code = KeyCodes::Insert; break;
    case WXK_DELETE:    code = KeyCodes::Delete; break;
    case WXK_HOME:      code = KeyCodes::Home; break;
    case WXK_END:       code = KeyCodes::End; break;
    case WXK_PAGEUP:    code = KeyCodes::PageUp; break;
    case WXK_PAGEDOWN:  code = KeyCodes::PageDown; break;
    case WXK_LEFT:      code = KeyCodes::Left; break;
    case WXK_RIGHT:     code = KeyCodes::Right; break;
    case WXK_UP:        code = KeyCodes::Up; break;
    case WXK_DOWN:      code = KeyCodes::Down; break;
    case WXK_ALT:       code = KeyCodes::Alt; break;
    case WXK_CONTROL:   code = KeyCodes::Ctrl; break;
    case WXK_SHIFT:     code = KeyCodes::Shift; break;
    case WXK_COMMAND:   code = KeyCodes::System; break;
    case WXK_F1:        code = KeyCodes::F1; break;
    case WXK_F2:        code = KeyCodes::F2; break;
    case WXK_F3:        code = KeyCodes::F3; break;
    case WXK_F4:        code = KeyCodes::F4; break;
    case WXK_F5:        code = KeyCodes::F5; break;
    case WXK_F6:        code = KeyCodes::F6; break;
    case WXK_F7:        code = KeyCodes::F7; break;
    case WXK_F8:        code = KeyCodes::F8; break;
    case WXK_F9:        code = KeyCodes::F9; break;
    case WXK_F10:       code = KeyCodes::F10; break;
    case WXK_F11:       code = KeyCodes::F11; break;
    case WXK_F12:       code = KeyCodes::F12; break;
    }

    if ( event.GetKeyCode() >= KeyCodes::Space && event.GetKeyCode() <= KeyCodes::Box )
    {
        code = (KeyCode)event.GetKeyCode();
    }

    input.SetKeyCode( code );
}
