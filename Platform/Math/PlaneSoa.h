//----------------------------------------------------------------------------------------------------------------------
// PlaneSoa.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLANE_SOA_H
#define LUNAR_CORE_PLANE_SOA_H

#include "Platform/Math/Plane.h"
#include "Platform/Math/Vector3Soa.h"
#include "Platform/Math/Vector4Soa.h"

namespace Helium
{
    /// SIMD-optimized structure-of-arrays plane.
    ///
    /// Planes are stored as a vector of 4 sets of floating-point values, with each set of values representing a set of
    /// coefficients (@c A, @c B, @c C, or @c D) of the following plane equation:
    ///
    ///     <code>Ax + By + Cz + D = 0</code>
    HELIUM_SIMD_ALIGN_PRE class PLATFORM_API PlaneSoa
    {
    public:
        /// A coefficients.
        SimdVector m_a;
        /// B coefficients.
        SimdVector m_b;
        /// C coefficients.
        SimdVector m_c;
        /// D coefficients.
        SimdVector m_d;

        /// @name Construction/Destruction
        //@{
        inline PlaneSoa();
        inline PlaneSoa( const SimdVector& rA, const SimdVector& rB, const SimdVector& rC, const SimdVector& rD );
        inline PlaneSoa( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD );
        inline PlaneSoa( const Vector3Soa& rNormal, const SimdVector& rDistance );
        inline PlaneSoa( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 );
        inline explicit PlaneSoa( const Vector4Soa& rVector );
        inline explicit PlaneSoa( const Plane& rPlane );
        //@}

        /// @name Loading and Storage
        //@{
        inline void Load( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD );
        inline void Load4Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD );
        inline void Load1Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD );

        inline void Store( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const;
        inline void Store4( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const;
        inline void Store1( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const;

        inline void Splat( const Plane& rPlane );
        //@}

        /// @name Data Access
        //@{
        inline void Set( const Vector3Soa& rNormal, const SimdVector& rDistance );
        inline void Set( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 );
        //@}

        /// @name Math
        //@{
        inline SimdVector GetDistance( const Vector3Soa& rPoint ) const;

        inline PlaneSoa GetNormalized( const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline void Normalize( const SimdVector& rEpsilon = Simd::EPSILON );
        //@}

        /// @name Comparison
        //@{
        inline SimdMask Equals( const PlaneSoa& rPlane, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline SimdMask NotEquals( const PlaneSoa& rPlane, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline SimdMask operator==( const PlaneSoa& rPlane ) const;
        inline SimdMask operator!=( const PlaneSoa& rPlane ) const;
        //@}
    } HELIUM_SIMD_ALIGN_POST;
}

#include "Platform/Math/PlaneSoa.inl"

#if HELIUM_SIMD_LRBNI
#include "Platform/Math/PlaneSoaLrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Platform/Math/PlaneSoaSse.inl"
#endif

#endif  // LUNAR_CORE_PLANE_SOA_H
