#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

/// Set to 1 to enable Larrabee New Instructions ("LRBni") prototype support.
#define HELIUM_USE_LRBNI 0

#if HELIUM_USE_LRBNI
#define HELIUM_SIMD_LRBNI 1
#elif HELIUM_CPU_X86
#define HELIUM_SIMD_SSE 1
#endif

#if HELIUM_SIMD_LRBNI
#include "Platform/Math/Simd/Lrbni/Simd.h"
#elif HELIUM_SIMD_SSE
#include "Platform/Math/Simd/Sse/Simd.h"
#else
#define HELIUM_SIMD_DISABLED 1
#define HELIUM_SIMD_SIZE 0
#define HELIUM_SIMD_ALIGNMENT 8
#define HELIUM_SIMD_ALIGN_PRE HELIUM_ALIGN_PRE( 8 )
#define HELIUM_SIMD_ALIGN_POST HELIUM_ALIGN_POST( 8 )
#endif

namespace Helium
{
    /// General SIMD operation support.
    namespace Simd
    {
        /// Vector filled with the default floating-point epsilon.
        PLATFORM_API extern const Register EPSILON;

        /// @name Memory Operations
        //@{
        HELIUM_FORCEINLINE Register LoadAligned( const void* pSource );
        HELIUM_FORCEINLINE Register LoadUnaligned( const void* pSource );

        HELIUM_FORCEINLINE void StoreAligned( void* pDest, Register vec );
        HELIUM_FORCEINLINE void StoreUnaligned( void* pDest, Register vec );

        HELIUM_FORCEINLINE Register LoadSplat32( const void* pSource );
        HELIUM_FORCEINLINE Register LoadSplat128( const void* pSource );
        HELIUM_FORCEINLINE void Store32( void* pDest, Register vec );
        HELIUM_FORCEINLINE void Store128( void* pDest, Register vec );

        HELIUM_FORCEINLINE Register SetSplatF32( float32_t value );
        HELIUM_FORCEINLINE Register SetSplatU32( uint32_t value );

        HELIUM_FORCEINLINE Register LoadZeros();
        //@}

        /// @name Data Manipulation
        //@{
        HELIUM_FORCEINLINE Register Select( Register vec0, Register vec1, Mask mask );
        //@}

        /// @name Component-wise Single-precision Floating-point Math Operations
        //@{
        HELIUM_FORCEINLINE Register AddF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register SubtractF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register MultiplyF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register DivideF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register MultiplyAddF32(
            Register vecMul0, Register vecMul1, Register vecAdd );
        HELIUM_FORCEINLINE Register MultiplySubtractReverseF32(
            Register vecMul0, Register vecMul1, Register vecSub );

        HELIUM_FORCEINLINE Register SqrtF32( Register vec );
        HELIUM_FORCEINLINE Register InverseF32( Register vec );
        HELIUM_FORCEINLINE Register InverseSqrtF32( Register vec );
        HELIUM_FORCEINLINE Register MinF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register MaxF32( Register vec0, Register vec1 );
        //@}

        /// @name Component-wise Single-precision Floating-point Logical Comparison Operations
        //@{
        HELIUM_FORCEINLINE Mask EqualsF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Mask LessF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Mask GreaterF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Mask LessEqualsF32( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Mask GreaterEqualsF32( Register vec0, Register vec1 );
        //@}

        /// @name Bitwise Comparison Operations
        //@{
        HELIUM_FORCEINLINE Register And( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register AndNot( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register Or( Register vec0, Register vec1 );
        HELIUM_FORCEINLINE Register Xor( Register vec0, Register vec1 );
        //@}

        /// @name Vector Mask Operations
        //@{
        HELIUM_FORCEINLINE Mask MaskAnd( Mask mask0, Mask mask1 );
        HELIUM_FORCEINLINE Mask MaskAndNot( Mask mask0, Mask mask1 );
        HELIUM_FORCEINLINE Mask MaskOr( Mask mask0, Mask mask1 );
        HELIUM_FORCEINLINE Mask MaskXor( Mask mask0, Mask mask1 );
        //@}
    };
}

#if HELIUM_SIMD_LRBNI
#include "Platform/Math/Simd/Lrbni/Simd.inl"
#elif HELIUM_SIMD_SSE
#include "Platform/Math/Simd/Sse/Simd.inl"
#endif
