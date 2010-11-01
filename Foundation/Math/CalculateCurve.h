#pragma once

#include "Vector3.h"

namespace Helium
{
    class FOUNDATION_API CurveGenerator
    {
    public:
        enum Type
        {
            kLinear,
            kBSpline,
            kCatmullRom,
        };

        static bool ComputeCurve( const V_Vector3& controlPoints, const uint32_t resolution, const bool closed, const Type type, V_Vector3& points );
    };
}