//----------------------------------------------------------------------------------------------------------------------
// Vector4.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_VECTOR4_H
#define LUNAR_CORE_VECTOR4_H

#include "Core/Simd.h"
#include "Core/MathCommon.h"

namespace Lunar
{
    /// Single-precision floating-point 4-component vector.  Note that this is aligned to 16 bytes for SIMD support.
    L_ALIGN_PRE( 16 ) class LUNAR_CORE_API Vector4
    {
    public:
        /// @name Construction/Destruction
        //@{
        inline Vector4();
        inline Vector4( float32_t x, float32_t y, float32_t z, float32_t w );
        inline explicit Vector4( float32_t s );
#if L_SIMD_SIZE == 16
        inline explicit Vector4( const SimdVector& rVector );
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
        //@}

        /// @name Math
        //@{
        inline Vector4 Add( const Vector4& rVector ) const;
        inline Vector4 Subtract( const Vector4& rVector ) const;
        inline Vector4 Multiply( const Vector4& rVector ) const;
        inline Vector4 Divide( const Vector4& rVector ) const;

        inline void AddSet( const Vector4& rVector0, const Vector4& rVector1 );
        inline void SubtractSet( const Vector4& rVector0, const Vector4& rVector1 );
        inline void MultiplySet( const Vector4& rVector0, const Vector4& rVector1 );
        inline void DivideSet( const Vector4& rVector0, const Vector4& rVector1 );

        inline void MultiplyAddSet( const Vector4& rVectorMul0, const Vector4& rVectorMul1, const Vector4& rVectorAdd );

        inline Vector4 GetScaled( float32_t scale ) const;
        inline void Scale( float32_t scale );

        inline float32_t Dot( const Vector4& rVector ) const;

        inline float32_t GetMagnitude() const;
        inline float32_t GetMagnitudeSquared() const;

        inline Vector4 GetNormalized( float32_t epsilon = L_EPSILON ) const;
        inline void Normalize( float32_t epsilon = L_EPSILON );

        inline Vector4 GetNegated() const;
        inline void GetNegated( Vector4& rResult ) const;
        inline void Negate();
        //@}

        /// @name Comparison
        //@{
        inline bool Equals( const Vector4& rVector, float32_t epsilon = L_EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Vector4 operator-() const;

        inline Vector4 operator+( const Vector4& rVector ) const;
        inline Vector4 operator-( const Vector4& rVector ) const;
        inline Vector4 operator*( const Vector4& rVector ) const;
        inline Vector4 operator/( const Vector4& rVector ) const;

        inline Vector4& operator+=( const Vector4& rVector );
        inline Vector4& operator-=( const Vector4& rVector );
        inline Vector4& operator*=( const Vector4& rVector );
        inline Vector4& operator/=( const Vector4& rVector );

        inline Vector4 operator*( float32_t scale ) const;
        inline Vector4& operator*=( float32_t scale );

        inline bool operator==( const Vector4& rVector ) const;
        inline bool operator!=( const Vector4& rVector ) const;
        //@}

        /// @name Friend Function
        //@{
        inline friend Vector4 operator*( float32_t scale, const Vector4& rVector );
        //@}

    private:
#if L_SIMD_SIZE == 16
        /// SIMD vector containing the vector values.
        SimdVector m_vector;
#else
        /// Vector values.
        float32_t m_vector[ 4 ];
#endif
    } L_ALIGN_POST( 16 );
}

#include "Core/Vector4.inl"

#if L_SIMD_LRBNI
#include "Core/Vector4Lrbni.inl"
#elif L_SIMD_SSE
#include "Core/Vector4Sse.inl"
#endif

#endif  // LUNAR_CORE_VECTOR4_H
