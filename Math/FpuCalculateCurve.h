#pragma once

#include "Math/API.h"
#include "Math/FpuVector3.h"

namespace Helium
{
    class HELIUM_MATH_API CurveGenerator
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