#pragma once

#include "Platform/Simd.h"

#if HELIUM_SIMD_LRBNI

// Allow SSE optimizations of LRBni prototype intrinsics on x86 platforms.
#if HELIUM_CPU_X86
#define USE_C_PROTOTYPE_PRIMITIVES 0
#endif

#if HELIUM_CC_MSC
#pragma warning( push )
#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#endif

#include "lrb_prototype_primitives.inl"

#if HELIUM_CC_MSC
#pragma warning( pop )
#endif

/// @defgroup simdvector SIMD Types
//@{

/// Size of SIMD vectors, in bytes.
#define HELIUM_SIMD_SIZE 64
/// Alignment of SIMD vectors, in bytes.
#define HELIUM_SIMD_ALIGNMENT 64

/// Prefix macro for declaring SIMD type or variable.
#define HELIUM_SIMD_ALIGN_PRE HELIUM_ALIGN_PRE( 64 )
/// Suffix macro for declaring SIMD type or variable alignment.
#define HELIUM_SIMD_ALIGN_POST HELIUM_ALIGN_POST( 64 )

/// Non-zero if SIMD multiply-and-add is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY_ADD 1
/// Non-zero if SIMD multiply is supported in a single instruction.
#define HELIUM_SIMD_BUILTIN_MULTIPLY 1

namespace Helium
{
    /// Generic SIMD vector.
    typedef __m512 SimdVector;

    /// Mask type for SIMD vector operations.
    typedef __mmask SimdMask;
}

//@}

#endif  // HELIUM_SIMD_LRBNI
