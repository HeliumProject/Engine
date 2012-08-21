#pragma once

#include "Foundation/API.h"
#include "Foundation/Math/FpuVector3.h"

namespace Helium
{
    class HELIUM_FOUNDATION_API CurveGenerator
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