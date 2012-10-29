#pragma once

#include "SceneGraph/Input.h"

namespace Helium
{
    void ConvertEvent( wxMouseEvent& event, MouseMoveInput& input );
    void ConvertEvent( wxMouseEvent& event, MouseButtonInput& input );
    void ConvertEvent( wxMouseEvent& event, MouseScrollInput& input );

    void ConvertEvent( wxKeyEvent& event, KeyboardInput& input );
}