#pragma once

#include "Application/Inspect/InspectControls.h"

namespace Helium
{
    namespace Editor
    {
        class Canvas : public Inspect::Canvas
        {
            void RealizeControl( Inspect::Control* control, Inspect::Control* parent );
        };
    }
}