#pragma once

#include "Platform/Math/MathCommon.h"

#include <iostream>
#include <float.h>

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "Foundation/API.h"
#include "Macros.h"
#include "Constants.h"

namespace Helium
{
    //
    // Valid
    //

    inline bool IsFinite(float32_t val)
    {
        return _finite(val) != 0;
    }

    inline bool IsFinite(float64_t val)
    {
        return _finite(val) != 0;
    }

    //
    // Clamp
    //
    inline int32_t Clamp(int32_t& val, int32_t min, int32_t max)
    {
        if (val < min)
            val = min;
        else if (val > max)
            val = max;
        return val;
    }

    //
    // Clamp
    //
    inline uint32_t Clamp(uint32_t& val, uint32_t min, uint32_t max)
    {
        if (val < min)
            val = min;
        else if (val > max)
            val = max;
        return val;
    }

    //
    // Clamp
    //
    inline float32_t Clamp(float32_t& val, float32_t min, float32_t max)
    {
        if (val < min)
            val = min;
        else if (val > max)
            val = max;
        return val;
    }

    //
    // Clamp
    //
    inline float64_t Clamp(float64_t& val, float64_t min, float64_t max)
    {
        if (val < min)
            val = min;
        else if (val > max)
            val = max;
        return val;
    }

    //
    // ClampAngle
    //
    inline float32_t ClampAngle(float32_t& v)
    {
        while( v < -static_cast< float32_t >( HELIUM_PI ) )
            v += static_cast< float32_t >( HELIUM_TWOPI );
        while( v > static_cast< float32_t >( HELIUM_PI ) )
            v -= static_cast< float32_t >( HELIUM_TWOPI );
        return v;
    }

    //
    // Limit (non ref clamp)
    //
    inline int32_t Limit(int32_t min, int32_t val, int32_t max)
    {
        if (val < min)
            val = min;
        else if (val > max)
            val = max;
        return val;
    }

    //
    // LimitAngle
    //
    inline float32_t LimitAngle(float32_t v, float32_t low, float32_t high)
    {
        if (v < low)
            v += (high - low);
        else if (v > high)
            v -= (high - low);

        return v;
    }

    //
    // Round
    //
    inline float32_t Round(float32_t d)
    {
        return floor(d + 0.5f);
    }
    inline float64_t Round(float64_t d)
    {
        return floor(d + 0.5);
    }

    //
    // Ran
    //
    inline int32_t Ran(int32_t low, int32_t high)
    {
        return (int32_t)Round((((float64_t)rand() / (float64_t) RAND_MAX) * (float64_t)(high - low)) + low);
    }

    //
    // Ran
    //
    inline float64_t Ran(float64_t low, float64_t high)
    {
        return (((float64_t)rand() / (float64_t) RAND_MAX) * (high - low)) + low;
    }

    //
    // LogBase2
    //
    inline float64_t LogBase2(float64_t v)
    {
        v = log10(v);
        v = v * 3.3219282;
        return v;
    }

    //
    // NextPowerOfTwo
    //
    // Return the next power of two, if the number is already a power of two then
    // the input is returned.
    //
    inline uint32_t NextPowerOfTwo(uint32_t in)
    {
        in -= 1;

        in |= in >> 16;
        in |= in >> 8;
        in |= in >> 4;
        in |= in >> 2;
        in |= in >> 1;

        return in + 1;
    }

    //
    // PreviousPowerOfTwo
    //
    // Return the number rounded down to the previous power of two, if the input is already a power
    // of two it is returned unmodified.
    //
    inline uint32_t PreviousPowerOfTwo(uint32_t in)
    {
        return 1<<Log2(in);
    }

    //
    // IsPowerOfTwo
    //
    // Returns true if the input is a power of 2
    //
    inline bool IsPowerOfTwo(uint32_t in)
    {
        return (in & (in-1))==0;
    }

    //
    // IsWholeNumber
    //
    inline bool IsWholeNumber(float64_t d, float64_t error)
    {
        float64_t i = Round(d);
        if (fabs(d - i) <= error)
            return true;
        return false;
    }

    //
    // Equal
    // 
    inline bool Equal( float32_t a, float32_t b, float32_t err = HELIUM_VALUE_NEAR_ZERO )
    {
        return ( fabs( a - b ) <= err ); 
    }
}