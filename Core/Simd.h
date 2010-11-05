//----------------------------------------------------------------------------------------------------------------------
// Simd.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_SIMD_H
#define LUNAR_CORE_SIMD_H

#include "Core/Core.h"

/// Set to 1 to enable Larrabee New Instructions ("LRBni") prototype support.
#define L_USE_LRBNI 0

#if L_USE_LRBNI
#define L_SIMD_LRBNI 1
#elif L_CPU_X86
#define L_SIMD_SSE 1
#endif

#if L_SIMD_LRBNI
#include "Core/SimdLrbni.h"
#elif L_SIMD_SSE
#include "Core/SimdSse.h"
#else
#define L_SIMD_DISABLED 1
#define L_SIMD_SIZE 0
#define L_SIMD_ALIGNMENT 8
#define L_SIMD_ALIGN_PRE L_ALIGN_PRE( 8 )
#define L_SIMD_ALIGN_POST L_ALIGN_POST( 8 )
#endif

namespace Lunar
{
    /// General SIMD operation support.
    class LUNAR_CORE_API Simd
    {
    public:
        /// Vector filled with the default floating-point epsilon.
        static const SimdVector EPSILON;

        /// @name Memory Operations
        //@{
        L_FORCEINLINE static SimdVector LoadAligned( const void* pSource );
        L_FORCEINLINE static SimdVector LoadUnaligned( const void* pSource );

        L_FORCEINLINE static void StoreAligned( void* pDest, SimdVector vec );
        L_FORCEINLINE static void StoreUnaligned( void* pDest, SimdVector vec );

        L_FORCEINLINE static SimdVector LoadSplat32( const void* pSource );
        L_FORCEINLINE static SimdVector LoadSplat128( const void* pSource );
        L_FORCEINLINE static void Store32( void* pDest, SimdVector vec );
        L_FORCEINLINE static void Store128( void* pDest, SimdVector vec );

        L_FORCEINLINE static SimdVector SetSplatF32( float32_t value );
        L_FORCEINLINE static SimdVector SetSplatU32( uint32_t value );

        L_FORCEINLINE static SimdVector LoadZeros();
        //@}

        /// @name Data Manipulation
        //@{
        L_FORCEINLINE static SimdVector Select( SimdVector vec0, SimdVector vec1, SimdMask mask );
        //@}

        /// @name Component-wise Single-precision Floating-point Math Operations
        //@{
        L_FORCEINLINE static SimdVector AddF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector SubtractF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector MultiplyF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector DivideF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector MultiplyAddF32( SimdVector vecMul0, SimdVector vecMul1, SimdVector vecAdd );
        L_FORCEINLINE static SimdVector MultiplySubtractReverseF32(
            SimdVector vecMul0, SimdVector vecMul1, SimdVector vecSub );

        L_FORCEINLINE static SimdVector SqrtF32( SimdVector vec );
        L_FORCEINLINE static SimdVector InverseF32( SimdVector vec );
        L_FORCEINLINE static SimdVector InverseSqrtF32( SimdVector vec );
        L_FORCEINLINE static SimdVector MinF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector MaxF32( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Component-wise Single-precision Floating-point Logical Comparison Operations
        //@{
        L_FORCEINLINE static SimdMask EqualsF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdMask LessF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdMask GreaterF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdMask LessEqualsF32( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdMask GreaterEqualsF32( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Bitwise Comparison Operations
        //@{
        L_FORCEINLINE static SimdVector And( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector AndNot( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector Or( SimdVector vec0, SimdVector vec1 );
        L_FORCEINLINE static SimdVector Xor( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Vector Mask Operations
        //@{
        L_FORCEINLINE static SimdMask MaskAnd( SimdMask mask0, SimdMask mask1 );
        L_FORCEINLINE static SimdMask MaskAndNot( SimdMask mask0, SimdMask mask1 );
        L_FORCEINLINE static SimdMask MaskOr( SimdMask mask0, SimdMask mask1 );
        L_FORCEINLINE static SimdMask MaskXor( SimdMask mask0, SimdMask mask1 );
        //@}
    };
}

#if L_SIMD_LRBNI
#include "Core/SimdLrbni.inl"
#elif L_SIMD_SSE
#include "Core/SimdSse.inl"
#endif

#endif  // LUNAR_CORE_SIMD_H
