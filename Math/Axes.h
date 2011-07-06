#pragma once

#include "Math/API.h"
#include "Math/Common.h"

namespace Helium
{
    //
    // Axes flags
    //

    namespace SingleAxes
    {
        enum Axis
        {
            X = 0,
            Y = 1,
            Z = 2,
        };
    }

    typedef SingleAxes::Axis Axis;

    inline Axis NextAxis(Axis axis)
    {
        return (Axis)( (axis+1) % 3);
    }

    inline Axis PrevAxis(Axis axis)
    {
        return (Axis)( (axis+2) % 3);
    }

    namespace MultipleAxes
    {
        enum AxesFlags
        {
            None  = 0,
            X     = 1 << 0,
            Y     = 1 << 1,
            Z     = 1 << 2,
            All   = X | Y | Z,
        };
    }

    typedef MultipleAxes::AxesFlags AxesFlags;
}
