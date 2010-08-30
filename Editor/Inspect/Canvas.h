#pragma once

#include "Foundation/Inspect/InspectControls.h"

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