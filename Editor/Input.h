#pragma once

#include "EditorScene/Input.h"

namespace Helium
{
    void ConvertEvent( wxMouseEvent& event, MouseMoveInputEvent& input );
    void ConvertEvent( wxMouseEvent& event, MouseButtonInputEvent& input );
    void ConvertEvent( wxMouseEvent& event, MouseScrollInputEvent& input );

    void ConvertEvent( wxKeyEvent& event, KeyboardInputEvent& input );
}