#pragma once

#include "Vector3.h"

namespace Helium
{
    namespace Math
    {
        class FOUNDATION_API Curve
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
}