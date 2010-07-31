#pragma once

#include "Utils.h"

namespace Helium
{
    namespace Math
    {
        static const i32 I16_LOWER_BOUND = -32768;
        static const i32 I16_UPPER_BOUND =  32767;

        bool ExceedsRange16( i32 val )
        {
            return ( ( val < I16_LOWER_BOUND ) || ( val > I16_UPPER_BOUND ) );
        }

        bool FloatToI16( i16 &out, f32 val, f32 scale = 1.0 )
        {
            val *= scale;

            bool stat = !ExceedsRange16( (i32) val );

            i32 convertedValue = 0;
            if ( val < 0.0f )
                convertedValue = (i32)( val - 0.5f );
            else
                convertedValue = (i32)( val + 0.5f );

            out = Clamp( convertedValue, I16_LOWER_BOUND, I16_UPPER_BOUND );
            return stat;
        }
    }
}