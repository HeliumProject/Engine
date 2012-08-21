#pragma once

#include "Foundation/Math/Simd.h"

#if HELIUM_SIMD_SSE

#include <xmmintrin.h>

/// @defgroup simdvector SIMD Types
//@{

/// Non-zero if SIMD multiply-and-add is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY_ADD 0
/// Non-zero if SIMD multiply is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY 1

namespace Helium
{
    namespace Simd
    {
        /// Generic SIMD vector.
        typedef __m128 Register;

        /// Mask type for SIMD vector operations.
        typedef __m128 Mask;
    }
}

//@}

#endif  // HELIUM_SIMD_SSE
