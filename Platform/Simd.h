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
#include "Platform/Lrbni/Simd.h"
#elif HELIUM_SIMD_SSE
#include "Platform/Sse/Simd.h"
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
    class PLATFORM_API Simd
    {
    public:
        /// Vector filled with the default floating-point epsilon.
        static const SimdVector EPSILON;

        /// @name Memory Operations
        //@{
        HELIUM_FORCEINLINE static SimdVector LoadAligned( const void* pSource );
        HELIUM_FORCEINLINE static SimdVector LoadUnaligned( const void* pSource );

        HELIUM_FORCEINLINE static void StoreAligned( void* pDest, SimdVector vec );
        HELIUM_FORCEINLINE static void StoreUnaligned( void* pDest, SimdVector vec );

        HELIUM_FORCEINLINE static SimdVector LoadSplat32( const void* pSource );
        HELIUM_FORCEINLINE static SimdVector LoadSplat128( const void* pSource );
        HELIUM_FORCEINLINE static void Store32( void* pDest, SimdVector vec );
        HELIUM_FORCEINLINE static void Store128( void* pDest, SimdVector vec );

        HELIUM_FORCEINLINE static SimdVector SetSplatF32( float32_t value );
        HELIUM_FORCEINLINE static SimdVector SetSplatU32( uint32_t value );

        HELIUM_FORCEINLINE static SimdVector LoadZeros();
        //@}

        /// @name Data Manipulation
        //@{
        HELIUM_FORCEINLINE static SimdVector Select( SimdVector vec0, SimdVector vec1, SimdMask mask );
        //@}

        /// @name Component-wise Single-precision Floating-point Math Operations
        //@{
        HELIUM_FORCEINLINE static SimdVector AddF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector SubtractF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector MultiplyF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector DivideF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector MultiplyAddF32(
            SimdVector vecMul0, SimdVector vecMul1, SimdVector vecAdd );
        HELIUM_FORCEINLINE static SimdVector MultiplySubtractReverseF32(
            SimdVector vecMul0, SimdVector vecMul1, SimdVector vecSub );

        HELIUM_FORCEINLINE static SimdVector SqrtF32( SimdVector vec );
        HELIUM_FORCEINLINE static SimdVector InverseF32( SimdVector vec );
        HELIUM_FORCEINLINE static SimdVector InverseSqrtF32( SimdVector vec );
        HELIUM_FORCEINLINE static SimdVector MinF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector MaxF32( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Component-wise Single-precision Floating-point Logical Comparison Operations
        //@{
        HELIUM_FORCEINLINE static SimdMask EqualsF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdMask LessF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdMask GreaterF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdMask LessEqualsF32( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdMask GreaterEqualsF32( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Bitwise Comparison Operations
        //@{
        HELIUM_FORCEINLINE static SimdVector And( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector AndNot( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector Or( SimdVector vec0, SimdVector vec1 );
        HELIUM_FORCEINLINE static SimdVector Xor( SimdVector vec0, SimdVector vec1 );
        //@}

        /// @name Vector Mask Operations
        //@{
        HELIUM_FORCEINLINE static SimdMask MaskAnd( SimdMask mask0, SimdMask mask1 );
        HELIUM_FORCEINLINE static SimdMask MaskAndNot( SimdMask mask0, SimdMask mask1 );
        HELIUM_FORCEINLINE static SimdMask MaskOr( SimdMask mask0, SimdMask mask1 );
        HELIUM_FORCEINLINE static SimdMask MaskXor( SimdMask mask0, SimdMask mask1 );
        //@}
    };
}

#if HELIUM_SIMD_LRBNI
#include "Platform/Lrbni/Simd.inl"
#elif HELIUM_SIMD_SSE
#include "Platform/Sse/Simd.inl"
#endif
