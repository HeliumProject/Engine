//#include "CorePch.h"
#include "Platform/Math/Float16.h"
#include "Platform/Math/MathCommon.h"

namespace Helium
{
    static const size_t FLOAT16_EXPONENT_BIAS = 15;
    static const size_t FLOAT16_EXPONENT_MAX = ( static_cast< size_t >( 1 ) << HELIUM_FLOAT16_EXPONENT_BITS ) - 1;
    static const size_t FLOAT16_MANTISSA_MAX = ( static_cast< size_t >( 1 ) << HELIUM_FLOAT16_MANTISSA_BITS ) - 1;

    static const size_t FLOAT32_EXPONENT_BIAS = 127;
    static const size_t FLOAT32_EXPONENT_MAX = ( static_cast< size_t >( 1 ) << HELIUM_FLOAT32_EXPONENT_BITS ) - 1;
    static const size_t FLOAT32_MANTISSA_MAX = ( static_cast< size_t >( 1 ) << HELIUM_FLOAT32_MANTISSA_BITS ) - 1;

    /// Convert a Float32 value to a Float16.
    ///
    /// Values outside the minimum and maximum range supported by a half-precision float will be clamped.  Infinity
    /// values will be preserved.
    ///
    /// @param[in] value  Float32 value to convert.
    ///
    /// @return  Float16 value.
    ///
    /// @see Float16To32()
    Float16 Float32To16( Float32 value )
    {
        Float16 result;
        result.components.sign = value.components.sign;

        uint32_t exponent = value.components.exponent;
        if( exponent == FLOAT32_EXPONENT_MAX )
        {
            result.components.exponent = FLOAT16_EXPONENT_MAX;
            result.components.mantissa = 0;
        }
        else if( exponent < FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + 1 )
        {
            // Bits will shift off entirely for very small numbers, so we don't need to check for denormalized values.
            uint32_t mantissa = ( 1 << HELIUM_FLOAT32_MANTISSA_BITS ) | value.components.mantissa;
            size_t mantissaShift =
                ( HELIUM_FLOAT32_MANTISSA_BITS - HELIUM_FLOAT16_MANTISSA_BITS ) +
                ( FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + 1 ) -
                exponent;

            result.components.exponent = 0;
            result.components.mantissa = static_cast< uint16_t >( mantissa >> mantissaShift );
        }
        else if( exponent > FLOAT32_EXPONENT_BIAS + ( FLOAT16_EXPONENT_MAX - FLOAT16_EXPONENT_BIAS - 1 ) )
        {
            // Clamp values that are too large to represent in a half-precision float.
            result.components.exponent = FLOAT16_EXPONENT_MAX - 1;
            result.components.mantissa = FLOAT16_MANTISSA_MAX;
        }
        else
        {
            result.components.exponent = static_cast< uint16_t >(
                FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + exponent );
            result.components.mantissa = static_cast< uint16_t >(
                value.components.mantissa >> ( HELIUM_FLOAT32_MANTISSA_BITS - HELIUM_FLOAT16_MANTISSA_BITS ) );
        }

        return result;
    }

    /// Convert a Float16 value to a Float32.
    ///
    /// Infinity values will be preserved.
    ///
    /// @param[in] value  Float16 value to convert.
    ///
    /// @return  Float32 value.
    ///
    /// @see Float32To16()
    Float32 Float16To32( Float16 value )
    {
        Float32 result;
        result.components.sign = value.components.sign;

        uint32_t exponent = value.components.exponent;
        if( exponent == FLOAT16_EXPONENT_MAX )
        {
            result.components.exponent = FLOAT32_EXPONENT_MAX;
            result.components.mantissa = 0;
        }
        else if( exponent == 0 )
        {
            uint32_t mantissa = value.components.mantissa;
            if( mantissa == 0 )
            {
                result.components.exponent = 0;
                result.components.mantissa = 0;
            }
            else
            {
                uint32_t mantissaShift = HELIUM_FLOAT16_MANTISSA_BITS - static_cast< uint32_t >( Log2( mantissa ) );
                result.components.exponent = FLOAT32_EXPONENT_BIAS - ( FLOAT16_EXPONENT_BIAS - 1 ) - mantissaShift;
                result.components.mantissa =
                    mantissa << ( mantissaShift + HELIUM_FLOAT32_MANTISSA_BITS - HELIUM_FLOAT16_MANTISSA_BITS );
            }
        }
        else
        {
            result.components.exponent = FLOAT32_EXPONENT_BIAS - FLOAT16_EXPONENT_BIAS + exponent;
            result.components.mantissa = static_cast< uint32_t >( value.components.mantissa ) <<
                ( HELIUM_FLOAT32_MANTISSA_BITS - HELIUM_FLOAT16_MANTISSA_BITS );
        }

        return result;
    }
}
