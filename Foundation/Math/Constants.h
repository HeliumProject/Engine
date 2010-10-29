#pragma once

#include "Foundation/API.h"
#include "Platform/Types.h"

//
// Constants
//

namespace Helium
{
    namespace Math
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

        //
        // Constants
        //

        const float32_t ValueNearZero = 1e-20f;
        const float32_t DivisorNearZero = 1e-15f;//(0.00005f);
        const float32_t AngleNearZero = 1e-7f;

        const float32_t TwoPi = (6.283185307179586476925286766559f);
        const float32_t Pi = (3.141592653589793238462643383279f);
        const float32_t HalfPi = (1.570796326794896619231321691639f);

        const float32_t DegToRad = Pi / 180.0f;
        const float32_t RadToDeg = 180.0f / Pi;

        const float32_t CentimetersToMeters = 0.01f;
        const float32_t MetersToCentimeters = 100.0f;

        const float32_t CriticalDotProduct = 0.98f;
        const float32_t PointOnPlaneError = 0.00001f;
        const float32_t LinearIntersectionError = 0.05f;

        const int64_t BigInt = 0x40000000;
        const float32_t BigFloat = 1000000000000.0f;
        const float64_t BigDouble = 1000000000000.0;

        //
        // from http://en.wikipedia.org/wiki/Luminance_(relative)
        // these luminance weights assume the input color is linearly encoded
        //
        const float32_t LuminanceR = 0.2126f;
        const float32_t LuminanceG = 0.7152f;
        const float32_t LuminanceB = 0.0722f;
        const float32_t InverseLuminanceR = (1.0f / LuminanceR);
        const float32_t InverseLuminanceG = (1.0f / LuminanceG);
        const float32_t InverseLuminanceB = (1.0f / LuminanceB);
    }
}