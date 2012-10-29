#pragma once

#include "Math/API.h"
#include "Foundation/Math.h"

namespace Helium
{
    static const int32_t I16_LOWER_BOUND = -32768;
    static const int32_t I16_UPPER_BOUND =  32767;

    bool ExceedsRange16( int32_t val )
    {
        return ( ( val < I16_LOWER_BOUND ) || ( val > I16_UPPER_BOUND ) );
    }

    bool FloatToI16( int16_t &out, float32_t val, float32_t scale = 1.0 )
    {
        val *= scale;

        bool stat = !ExceedsRange16( (int32_t) val );

        int32_t convertedValue = 0;
        if ( val < 0.0f )
            convertedValue = (int32_t)( val - 0.5f );
        else
            convertedValue = (int32_t)( val + 0.5f );

        out = Clamp( convertedValue, I16_LOWER_BOUND, I16_UPPER_BOUND );
        return stat;
    }
}