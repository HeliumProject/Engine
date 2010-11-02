//----------------------------------------------------------------------------------------------------------------------
// Float16.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_FLOAT16_H
#define LUNAR_CORE_FLOAT16_H

#include "Core/Core.h"

/// Number of exponent bits in a half-precision (16-bit) IEEE 754-2008 floating-point value.
#define L_FLOAT16_EXPONENT_BITS 5
/// Number of mantissa bits in a half-precision (16-bit) IEEE 754-2008 floating-point value.
#define L_FLOAT16_MANTISSA_BITS 10

/// Number of exponent bits in a single-precision (32-bit) IEEE 754 floating-point value.
#define L_FLOAT32_EXPONENT_BITS 8
/// Number of mantissa bits in a single-precision (32-bit) IEEE 754 floating-point value.
#define L_FLOAT32_MANTISSA_BITS 23

namespace Lunar
{
    /// IEEE 754-2008 half-precision (16-bit) floating-point storage.
    union Float16
    {
        /// Packed data.
        uint16_t packed;

        /// Bitmask components.
        struct
        {
#if L_ENDIAN_LITTLE
            /// Mantissa.
            uint16_t mantissa : L_FLOAT16_MANTISSA_BITS;
            /// Exponent.
            uint16_t exponent : L_FLOAT16_EXPONENT_BITS;
            /// Sign.
            uint16_t sign : 1;
#else
            /// Sign.
            uint16_t sign : 1;
            /// Exponent.
            uint16_t exponent : L_FLOAT16_EXPONENT_BITS;
            /// Mantissa.
            uint16_t mantissa : L_FLOAT16_MANTISSA_BITS;
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
#if L_ENDIAN_LITTLE
            /// Mantissa.
            uint32_t mantissa : L_FLOAT32_MANTISSA_BITS;
            /// Exponent.
            uint32_t exponent : L_FLOAT32_EXPONENT_BITS;
            /// Sign.
            uint32_t sign : 1;
#else
            /// Sign.
            uint32_t sign : 1;
            /// Exponent.
            uint32_t exponent : L_FLOAT32_EXPONENT_BITS;
            /// Mantissa.
            uint32_t mantissa : L_FLOAT32_MANTISSA_BITS;
#endif
        } components;
    };

    LUNAR_CORE_API Float16 Float32To16( Float32 value );
    LUNAR_CORE_API Float32 Float16To32( Float16 value );
}

#endif  // LUNAR_CORE_FLOAT16_H
