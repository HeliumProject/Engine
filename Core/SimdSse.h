//----------------------------------------------------------------------------------------------------------------------
// SimdSse.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_SIMD_SSE_H
#define LUNAR_CORE_SIMD_SSE_H

#include "Core/Core.h"

#if L_SIMD_SSE

#include <xmmintrin.h>

/// @defgroup simdvector SIMD Types
//@{

/// Size of SIMD vectors, in bytes.
#define L_SIMD_SIZE 16
/// Alignment of SIMD vectors, in bytes.
#define L_SIMD_ALIGNMENT 16

/// Prefix macro for declaring SIMD type or variable.
#define L_SIMD_ALIGN_PRE L_ALIGN_PRE( 16 )
/// Suffix macro for declaring SIMD type or variable alignment.
#define L_SIMD_ALIGN_POST L_ALIGN_POST( 16 )

/// Non-zero if SIMD multiply-and-add is supported in a single instruction.
#define L_SIMD_BUILTIN_MULTIPLY_ADD 0
/// Non-zero if SIMD multiply is supported in a single instruction.
#define L_SIMD_BUILTIN_MULTIPLY 1

namespace Lunar
{
    /// Generic SIMD vector.
    typedef __m128 SimdVector;

    /// Mask type for SIMD vector operations.
    typedef __m128 SimdMask;
}

//@}

#endif  // L_SIMD_SSE

#endif  // LUNAR_CORE_SIMD_SSE_H
