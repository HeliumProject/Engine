#pragma once

#include "Platform/Simd.h"

#if HELIUM_SIMD_SSE

#include <xmmintrin.h>

/// @defgroup simdvector SIMD Types
//@{

/// Size of SIMD vectors, in bytes.
#define HELIUM_SIMD_SIZE 16
/// Alignment of SIMD vectors, in bytes.
#define HELIUM_SIMD_ALIGNMENT 16

/// Prefix macro for declaring SIMD type or variable.
#define HELIUM_SIMD_ALIGN_PRE HELIUM_ALIGN_PRE( 16 )
/// Suffix macro for declaring SIMD type or variable alignment.
#define HELIUM_SIMD_ALIGN_POST HELIUM_ALIGN_POST( 16 )

/// Non-zero if SIMD multiply-and-add is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY_ADD 0
/// Non-zero if SIMD multiply is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY 1

namespace Helium
{
    /// Generic SIMD vector.
    typedef __m128 SimdVector;

    /// Mask type for SIMD vector operations.
    typedef __m128 SimdMask;
}

//@}

#endif  // HELIUM_SIMD_SSE
