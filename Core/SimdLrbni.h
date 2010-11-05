//----------------------------------------------------------------------------------------------------------------------
// SimdLrbni.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_SIMD_LRBNI_H
#define LUNAR_CORE_SIMD_LRBNI_H

#include "Core/Core.h"

#if L_SIMD_LRBNI

// Allow SSE optimizations of LRBni prototype intrinsics on x86 platforms.
#if L_CPU_X86
#define USE_C_PROTOTYPE_PRIMITIVES 0
#endif

#if L_CC_MSC
#pragma warning( push )
#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#endif

#include "lrb_prototype_primitives.inl"

#if L_CC_MSC
#pragma warning( pop )
#endif

/// @defgroup simdvector SIMD Types
//@{

/// Size of SIMD vectors, in bytes.
#define L_SIMD_SIZE 64
/// Alignment of SIMD vectors, in bytes.
#define L_SIMD_ALIGNMENT 64

/// Prefix macro for declaring SIMD type or variable.
#define L_SIMD_ALIGN_PRE L_ALIGN_PRE( 64 )
/// Suffix macro for declaring SIMD type or variable alignment.
#define L_SIMD_ALIGN_POST L_ALIGN_POST( 64 )

/// Non-zero if SIMD multiply-and-add is supported in a single instruction.
#define L_SIMD_BUILTIN_MULTIPLY_ADD 1
/// Non-zero if SIMD multiply is supported in a single instruction.
#define L_SIMD_BUILTIN_MULTIPLY 1

namespace Lunar
{
    /// Generic SIMD vector.
    typedef __m512 SimdVector;

    /// Mask type for SIMD vector operations.
    typedef __mmask SimdMask;
}

//@}

#endif  // L_SIMD_LRBNI

#endif  // LUNAR_CORE_SIMD_LRBNI_H
