//----------------------------------------------------------------------------------------------------------------------
// Vector4Soa.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_VECTOR4_SOA_H
#define LUNAR_CORE_VECTOR4_SOA_H

#include "Core/Vector4.h"

namespace Lunar
{
    /// SIMD-optimized structure-of-arrays 4-component vector.
    L_SIMD_ALIGN_PRE class LUNAR_CORE_API Vector4Soa
    {
    public:
        /// X components.
        SimdVector m_x;
        /// Y components.
        SimdVector m_y;
        /// Z components.
        SimdVector m_z;
        /// W components.
        SimdVector m_w;

        /// @name Construction/Destruction
        //@{
        inline Vector4Soa();
        inline Vector4Soa( const SimdVector& rX, const SimdVector& rY, const SimdVector& rZ, const SimdVector& rW );
        inline Vector4Soa( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline explicit Vector4Soa( const Vector4& rVector );
        //@}

        /// @name Loading and Storage
        //@{
        inline void Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline void Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline void Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );

        inline void Store( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;
        inline void Store4( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;
        inline void Store1( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;

        inline void Splat( const Vector4& rVector );
        //@}

        /// @name Math
        //@{
        inline Vector4Soa Add( const Vector4Soa& rVector ) const;
        inline Vector4Soa Subtract( const Vector4Soa& rVector ) const;
        inline Vector4Soa Multiply( const Vector4Soa& rVector ) const;
        inline Vector4Soa Divide( const Vector4Soa& rVector ) const;

        inline void AddSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 );
        inline void SubtractSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 );
        inline void MultiplySet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 );
        inline void DivideSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 );

        inline void MultiplyAddSet(
            const Vector4Soa& rVectorMul0, const Vector4Soa& rVectorMul1, const Vector4Soa& rVectorAdd );

        inline Vector4Soa GetScaled( const SimdVector& rScale ) const;
        inline void Scale( const SimdVector& rScale );

        inline SimdVector Dot( const Vector4Soa& rVector ) const;

        inline SimdVector GetMagnitude() const;
        inline SimdVector GetMagnitudeSquared() const;

        inline Vector4Soa GetNormalized( const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline void Normalize( const SimdVector& rEpsilon = Simd::EPSILON );

        inline Vector4Soa GetNegated() const;
        inline void GetNegated( Vector4Soa& rResult ) const;
        inline void Negate();
        //@}

        /// @name Comparison
        //@{
        inline SimdMask Equals( const Vector4Soa& rVector, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline SimdMask NotEquals( const Vector4Soa& rVector, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Vector4Soa operator-() const;

        inline Vector4Soa operator+( const Vector4Soa& rVector ) const;
        inline Vector4Soa operator-( const Vector4Soa& rVector ) const;
        inline Vector4Soa operator*( const Vector4Soa& rVector ) const;
        inline Vector4Soa operator/( const Vector4Soa& rVector ) const;

        inline Vector4Soa& operator+=( const Vector4Soa& rVector );
        inline Vector4Soa& operator-=( const Vector4Soa& rVector );
        inline Vector4Soa& operator*=( const Vector4Soa& rVector );
        inline Vector4Soa& operator/=( const Vector4Soa& rVector );

        inline Vector4Soa operator*( const SimdVector& rScale ) const;
        inline Vector4Soa& operator*=( const SimdVector& rScale );

        inline SimdMask operator==( const Vector4Soa& rVector ) const;
        inline SimdMask operator!=( const Vector4Soa& rVector ) const;
        //@}

        /// @name Friend Functions
        //@{
        inline friend Vector4Soa operator*( const SimdVector& rScale, const Vector4Soa& rVector );
        //@}
    } L_SIMD_ALIGN_POST;
}

#include "Core/Vector4Soa.inl"

#if L_SIMD_LRBNI
#include "Core/Vector4SoaLrbni.inl"
#elif L_SIMD_SSE
#include "Core/Vector4SoaSse.inl"
#endif

#endif  // LUNAR_CORE_VECTOR4_SOA_H
