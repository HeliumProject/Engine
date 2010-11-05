//----------------------------------------------------------------------------------------------------------------------
// QuatSoa.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_QUAT_SOA_H
#define LUNAR_CORE_QUAT_SOA_H

#include "Platform/Math/Quat.h"

namespace Helium
{
    /// SIMD-optimized structure-of-arrays quaternion.
    HELIUM_SIMD_ALIGN_PRE class PLATFORM_API QuatSoa
    {
    public:
        /// Identity quaternion.
        static const QuatSoa IDENTITY;

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
        inline QuatSoa();
        inline QuatSoa( const SimdVector& rX, const SimdVector& rY, const SimdVector& rZ, const SimdVector& rW );
        inline QuatSoa( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline explicit QuatSoa( const Quat& rQuat );
        //@}

        /// @name Loading and Storage
        //@{
        inline void Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline void Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );
        inline void Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW );

        inline void Store( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;
        inline void Store4( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;
        inline void Store1( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const;

        inline void Splat( const Quat& rQuat );
        //@}

        /// @name Math
        //@{
        inline QuatSoa Add( const QuatSoa& rQuat ) const;
        inline QuatSoa Subtract( const QuatSoa& rQuat ) const;
        inline QuatSoa Multiply( const QuatSoa& rQuat ) const;

        inline QuatSoa MultiplyComponents( const QuatSoa& rQuat ) const;
        inline QuatSoa DivideComponents( const QuatSoa& rQuat ) const;

        inline void AddSet( const QuatSoa& rQuat0, const QuatSoa& rQuat1 );
        inline void SubtractSet( const QuatSoa& rQuat0, const QuatSoa& rQuat1 );
        inline void MultiplySet( const QuatSoa& rQuat0, const QuatSoa& rQuat1 );

        inline void MultiplyComponentsSet( const QuatSoa& rQuat0, const QuatSoa& rQuat1 );
        inline void DivideComponentsSet( const QuatSoa& rQuat0, const QuatSoa& rQuat1 );

        inline SimdVector GetMagnitude() const;
        inline SimdVector GetMagnitudeSquared() const;

        inline QuatSoa GetNormalized( const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline void Normalize( const SimdVector& rEpsilon = Simd::EPSILON );

        inline void GetInverse( QuatSoa& rQuat ) const;
        inline QuatSoa GetInverse() const;
        inline void Invert();

        inline void GetConjugate( QuatSoa& rQuat ) const;
        inline QuatSoa GetConjugate() const;
        inline void SetConjugate();
        //@}

        /// @name Comparison
        //@{
        inline SimdMask Equals( const QuatSoa& rQuat, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        inline SimdMask NotEquals( const QuatSoa& rQuat, const SimdVector& rEpsilon = Simd::EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline QuatSoa operator+( const QuatSoa& rQuat ) const;
        inline QuatSoa operator-( const QuatSoa& rQuat ) const;
        inline QuatSoa operator*( const QuatSoa& rQuat ) const;

        inline QuatSoa& operator+=( const QuatSoa& rQuat );
        inline QuatSoa& operator-=( const QuatSoa& rQuat );
        inline QuatSoa& operator*=( const QuatSoa& rQuat );

        inline SimdMask operator==( const QuatSoa& rQuat ) const;
        inline SimdMask operator!=( const QuatSoa& rQuat ) const;
        //@}
    } HELIUM_SIMD_ALIGN_POST;
}

#include "Platform/Math/QuatSoa.inl"

#if HELIUM_SIMD_LRBNI
#include "Platform/Math/QuatSoaLrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Platform/Math/QuatSoaSse.inl"
#endif

#endif  // LUNAR_CORE_QUAT_SOA_H
