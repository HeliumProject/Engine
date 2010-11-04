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
    HELIUM_SIMD_ALIGN_PRE class LUNAR_CORE_API Vector4Soa
    {
    public:
        /// X components.
        Helium::SimdVector m_x;
        /// Y components.
        Helium::SimdVector m_y;
        /// Z components.
        Helium::SimdVector m_z;
        /// W components.
        Helium::SimdVector m_w;

        /// @name Construction/Destruction
        //@{
        inline Vector4Soa();
        inline Vector4Soa( const Helium::SimdVector& rX, const Helium::SimdVector& rY, const Helium::SimdVector& rZ, const Helium::SimdVector& rW );
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

        inline Vector4Soa GetScaled( const Helium::SimdVector& rScale ) const;
        inline void Scale( const Helium::SimdVector& rScale );

        inline Helium::SimdVector Dot( const Vector4Soa& rVector ) const;

        inline Helium::SimdVector GetMagnitude() const;
        inline Helium::SimdVector GetMagnitudeSquared() const;

        inline Vector4Soa GetNormalized( const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON ) const;
        inline void Normalize( const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON );

        inline Vector4Soa GetNegated() const;
        inline void GetNegated( Vector4Soa& rResult ) const;
        inline void Negate();
        //@}

        /// @name Comparison
        //@{
        inline Helium::SimdMask Equals( const Vector4Soa& rVector, const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON ) const;
        inline Helium::SimdMask NotEquals( const Vector4Soa& rVector, const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON ) const;
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

        inline Vector4Soa operator*( const Helium::SimdVector& rScale ) const;
        inline Vector4Soa& operator*=( const Helium::SimdVector& rScale );

        inline Helium::SimdMask operator==( const Vector4Soa& rVector ) const;
        inline Helium::SimdMask operator!=( const Vector4Soa& rVector ) const;
        //@}

        /// @name Friend Functions
        //@{
        inline friend Vector4Soa operator*( const Helium::SimdVector& rScale, const Vector4Soa& rVector );
        //@}
    } HELIUM_SIMD_ALIGN_POST;
}

#include "Core/Vector4Soa.inl"

#if HELIUM_SIMD_LRBNI
#include "Core/Vector4SoaLrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Core/Vector4SoaSse.inl"
#endif

#endif  // LUNAR_CORE_VECTOR4_SOA_H
