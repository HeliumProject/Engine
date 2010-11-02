//----------------------------------------------------------------------------------------------------------------------
// Sphere.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_SPHERE_H
#define LUNAR_CORE_SPHERE_H

#include "Core/AaBox.h"
#include "Core/Vector4.h"

namespace Lunar
{
    /// Sphere representation.
    L_ALIGN_PRE( 16 ) class LUNAR_CORE_API Sphere
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline Sphere();
        inline Sphere( const Vector3& rCenter, float32_t radius );
        inline Sphere( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius );
        inline explicit Sphere( const Vector4& rVector );
        inline explicit Sphere( const AaBox& rBox );
#if L_SIMD_SIZE == 16
        inline explicit Sphere( const SimdVector& rVector );
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

        inline void Set( const Vector3& rCenter, float32_t radius );
        inline void Set( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius );
        inline void Set( const Vector4& rVector );
        inline void Set( const AaBox& rBox );
        //@}

        /// @name Math
        //@{
        inline void SetCenter( const Vector3& rCenter );
        inline void Translate( const Vector3& rOffset );

        inline void SetRadius( float32_t radius );
        inline void Scale( float32_t scale );
        //@}

        /// @name Testing
        //@{
        inline bool Intersects( const Sphere& rSphere, float32_t threshold = L_EPSILON ) const;
        //@}

    private:
#if L_SIMD_SIZE == 16
        /// Sphere center and radius.
        SimdVector m_centerRadius;
#else
        /// Sphere center and radius.
        float32_t m_centerRadius[ 4 ];
#endif
    } L_ALIGN_POST( 16 );
}

#include "Core/Sphere.inl"

#if L_SIMD_LRBNI
#include "Core/SphereLrbni.inl"
#elif L_SIMD_SSE
#include "Core/SphereSse.inl"
#endif

#endif  // LUNAR_CORE_SPHERE_H
