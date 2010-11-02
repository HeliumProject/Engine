//----------------------------------------------------------------------------------------------------------------------
// Plane.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_PLANE_H
#define LUNAR_CORE_PLANE_H

#include "Core/Vector3.h"
#include "Core/Vector4.h"

namespace Lunar
{
    /// Plane in 3D space.
    ///
    /// Planes are stored as a vector of 4 floating-point values, with each value representing a coefficient (@c A,
    /// @c B, @c C, or @c D) of the following plane equation:
    ///
    ///     <code>Ax + By + Cz + D = 0</code>
    ///
    /// Note that this is aligned to 16 bytes for SIMD support.
    L_ALIGN_PRE( 16 ) class LUNAR_CORE_API Plane
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline Plane();
        inline Plane( float32_t a, float32_t b, float32_t c, float32_t d );
        inline Plane( const Vector3& rNormal, float32_t distance );
        inline Plane( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 );
        inline explicit Plane( const Vector4& rVector );
#if L_SIMD_SIZE == 16
        inline explicit Plane( const SimdVector& rVector );
#endif
        //@}

        /// @name Data Access
        //@{
#if L_SIMD_SIZE == 16
        inline SimdVector& GetSimdVector();
        inline const SimdVector& GetSimdVector() const;
        inline void SetSimdVector( const SimdVector& rVector );
#endif

        inline float32_t& GetElement( size_t index );
        inline float32_t GetElement( size_t index ) const;
        inline void SetElement( size_t index, float32_t value );

        inline void Set( const Vector3& rNormal, float32_t distance );
        inline void Set( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 );

        inline Vector3 GetNormal() const;
        //@}

        /// @name Math
        //@{
        inline float32_t GetDistance( const Vector3& rPoint ) const;

        inline Plane GetNormalized( float32_t epsilon = L_EPSILON ) const;
        inline void Normalize( float32_t epsilon = L_EPSILON );
        //@}

        /// @name Comparison
        //@{
        inline bool Equals( const Plane& rPlane, float32_t epsilon = L_EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline bool operator==( const Plane& rPlane ) const;
        inline bool operator!=( const Plane& rPlane ) const;
        //@}

    private:
#if L_SIMD_SIZE == 16
        /// SIMD vector containing the plane equation coefficients.
        SimdVector m_plane;
#else
        /// Plane equation coefficients.
        float32_t m_plane[ 4 ];
#endif
    } L_ALIGN_POST( 16 );
}

#include "Core/Plane.inl"

#if L_SIMD_LRBNI
#include "Core/PlaneLrbni.inl"
#elif L_SIMD_SSE
#include "Core/PlaneSse.inl"
#endif

#endif  // LUNAR_CORE_PLANE_H
