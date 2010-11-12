#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

/// Number of exponent bits in a half-precision (16-bit) IEEE 754-2008 floating-point value.
#define HELIUM_FLOAT16_EXPONENT_BITS 5
/// Number of mantissa bits in a half-precision (16-bit) IEEE 754-2008 floating-point value.
#define HELIUM_FLOAT16_MANTISSA_BITS 10

/// Number of exponent bits in a single-precision (32-bit) IEEE 754 floating-point value.
#define HELIUM_FLOAT32_EXPONENT_BITS 8
/// Number of mantissa bits in a single-precision (32-bit) IEEE 754 floating-point value.
#define HELIUM_FLOAT32_MANTISSA_BITS 23

namespace Helium
{
    /// IEEE 754-2008 half-precision (16-bit) floating-point storage.
    union Float16
    {
        /// Packed data.
        uint16_t packed;

        /// Bitmask components.
        struct
        {
#if HELIUM_ENDIAN_LITTLE
            /// Mantissa.
            uint16_t mantissa : HELIUM_FLOAT16_MANTISSA_BITS;
            /// Exponent.
            uint16_t exponent : HELIUM_FLOAT16_EXPONENT_BITS;
            /// Sign.
            uint16_t sign : 1;
#else
            /// Sign.
            uint16_t sign : 1;
            /// Exponent.
            uint16_t exponent : HELIUM_FLOAT16_EXPONENT_BITS;
            /// Mantissa.
            uint16_t mantissa : HELIUM_FLOAT16_MANTISSA_BITS;
#endif
        } components;
    };

    /// IEEE 754 single-precision (32-bit) floating-point storage.
    union Float32
    {
        /// Floating-point value.
        float32_t value;
        /// Packed data.
        uint32_t packed;

        /// Bitmask components.
        struct
        {
#if HELIUM_ENDIAN_LITTLE
            /// Mantissa.
            uint32_t mantissa : HELIUM_FLOAT32_MANTISSA_BITS;
            /// Exponent.
            uint32_t exponent : HELIUM_FLOAT32_EXPONENT_BITS;
            /// Sign.
            uint32_t sign : 1;
#else
            /// Sign.
            uint32_t sign : 1;
            /// Exponent.
            uint32_t exponent : HELIUM_FLOAT32_EXPONENT_BITS;
            /// Mantissa.
            uint32_t mantissa : HELIUM_FLOAT32_MANTISSA_BITS;
#endif
        } components;
    };

    PLATFORM_API Float16 Float32To16( Float32 value );
    PLATFORM_API Float32 Float16To32( Float16 value );
}