//----------------------------------------------------------------------------------------------------------------------
// Quat.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_QUAT_H
#define LUNAR_CORE_QUAT_H

#include "Core/Simd.h"
#include "Core/MathCommon.h"

namespace Lunar
{
    /// Quaternion.  Note that this is aligned to 16 bytes for SIMD support.
    L_ALIGN_PRE( 16 ) class LUNAR_CORE_API Quat
    {
    public:
        /// Identity quaternion.
        static const Quat IDENTITY;

        /// @name Construction/Destruction
        //@{
        inline Quat();
        inline Quat( float32_t x, float32_t y, float32_t z, float32_t w );
        inline Quat( const Vector3& rAxis, float32_t angle );
        inline Quat( float32_t pitch, float32_t yaw, float32_t roll );
#if L_SIMD_SIZE == 16
        inline explicit Quat( const SimdVector& rVector );
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

        /// @name Component Initialization
        //@{
        void Set( const Vector3& rAxis, float32_t angle );
        void Set( float32_t pitch, float32_t yaw, float32_t roll );
        //@}

        /// @name Math
        //@{
        inline Quat Add( const Quat& rQuat ) const;
        inline Quat Subtract( const Quat& rQuat ) const;
        inline Quat Multiply( const Quat& rQuat ) const;

        inline Quat MultiplyComponents( const Quat& rQuat ) const;
        inline Quat DivideComponents( const Quat& rQuat ) const;

        inline void AddSet( const Quat& rQuat0, const Quat& rQuat1 );
        inline void SubtractSet( const Quat& rQuat0, const Quat& rQuat1 );
        inline void MultiplySet( const Quat& rQuat0, const Quat& rQuat1 );

        inline void MultiplyComponentsSet( const Quat& rQuat0, const Quat& rQuat1 );
        inline void DivideComponentsSet( const Quat& rQuat0, const Quat& rQuat1 );

        inline float32_t GetMagnitude() const;
        inline float32_t GetMagnitudeSquared() const;

        inline Quat GetNormalized( float32_t epsilon = L_EPSILON ) const;
        inline void Normalize( float32_t epsilon = L_EPSILON );

        inline void GetInverse( Quat& rQuat ) const;
        inline Quat GetInverse() const;
        inline void Invert();

        inline void GetConjugate( Quat& rQuat ) const;
        inline Quat GetConjugate() const;
        inline void SetConjugate();
        //@}

        /// @name Comparison
        //@{
        inline bool Equals( const Quat& rQuat, float32_t epsilon = L_EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Quat operator+( const Quat& rQuat ) const;
        inline Quat operator-( const Quat& rQuat ) const;
        inline Quat operator*( const Quat& rQuat ) const;

        inline Quat& operator+=( const Quat& rQuat );
        inline Quat& operator-=( const Quat& rQuat );
        inline Quat& operator*=( const Quat& rQuat );

        inline bool operator==( const Quat& rQuat ) const;
        inline bool operator!=( const Quat& rQuat ) const;
        //@}

    private:
#if L_SIMD_SIZE == 16
        /// SIMD vector containing the quaternion values.
        SimdVector m_quat;
#else
        /// Quaternion values.
        float32_t m_quat[ 4 ];
#endif
    } L_ALIGN_POST( 16 );
}

#include "Core/Quat.inl"

#if L_SIMD_LRBNI
#include "Core/QuatLrbni.inl"
#elif L_SIMD_SSE
#include "Core/QuatSse.inl"
#endif

#endif  // LUNAR_CORE_QUAT_H
