#pragma once

#include "Math/API.h"
#include "Math/SimdVector3.h"

namespace Helium
{
    namespace Simd
    {
        /// SIMD-optimized structure-of-arrays 3-component vector.
        HELIUM_SIMD_ALIGN_PRE class MATH_API Vector3Soa
        {
        public:
            /// X components.
            Register m_x;
            /// Y components.
            Register m_y;
            /// Z components.
            Register m_z;

            /// @name Construction/Destruction
            //@{
            inline Vector3Soa();
            inline Vector3Soa( const Register& rX, const Register& rY, const Register& rZ );
            inline Vector3Soa( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
            inline explicit Vector3Soa( const Vector3& rVector );
            //@}

            /// @name Loading and Storage
            //@{
            inline void Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
            inline void Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ );
            inline void Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ );

            inline void Store( float32_t* pX, float32_t* pY, float32_t* pZ ) const;
            inline void Store4( float32_t* pX, float32_t* pY, float32_t* pZ ) const;
            inline void Store1( float32_t* pX, float32_t* pY, float32_t* pZ ) const;

            inline void Splat( const Vector3& rVector );
            //@}

            /// @name Math
            //@{
            inline Vector3Soa Add( const Vector3Soa& rVector ) const;
            inline Vector3Soa Subtract( const Vector3Soa& rVector ) const;
            inline Vector3Soa Multiply( const Vector3Soa& rVector ) const;
            inline Vector3Soa Divide( const Vector3Soa& rVector ) const;

            inline void AddSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
            inline void SubtractSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
            inline void MultiplySet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );
            inline void DivideSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );

            inline void MultiplyAddSet(
                const Vector3Soa& rVectorMul0, const Vector3Soa& rVectorMul1, const Vector3Soa& rVectorAdd );

            inline Vector3Soa GetScaled( const Register& rScale ) const;
            inline void Scale( const Register& rScale );

            inline Register Dot( const Vector3Soa& rVector ) const;

            inline Vector3Soa Cross( const Vector3Soa& rVector ) const;
            inline void CrossSet( const Vector3Soa& rVector0, const Vector3Soa& rVector1 );

            inline Register GetMagnitude() const;
            inline Register GetMagnitudeSquared() const;

            inline Vector3Soa GetNormalized( const Register& rEpsilon = Simd::EPSILON ) const;
            inline void Normalize( const Register& rEpsilon = Simd::EPSILON );

            inline Vector3Soa GetNegated() const;
            inline void GetNegated( Vector3Soa& rResult ) const;
            inline void Negate();
            //@}

            /// @name Comparison
            //@{
            inline Mask Equals( const Vector3Soa& rVector, const Register& rEpsilon = Simd::EPSILON ) const;
            inline Mask NotEquals( const Vector3Soa& rVector, const Register& rEpsilon = Simd::EPSILON ) const;
            //@}

            /// @name Overloaded Operators
            //@{
            inline Vector3Soa operator-() const;

            inline Vector3Soa operator+( const Vector3Soa& rVector ) const;
            inline Vector3Soa operator-( const Vector3Soa& rVector ) const;
            inline Vector3Soa operator*( const Vector3Soa& rVector ) const;
            inline Vector3Soa operator/( const Vector3Soa& rVector ) const;

            inline Vector3Soa& operator+=( const Vector3Soa& rVector );
            inline Vector3Soa& operator-=( const Vector3Soa& rVector );
            inline Vector3Soa& operator*=( const Vector3Soa& rVector );
            inline Vector3Soa& operator/=( const Vector3Soa& rVector );

            inline Vector3Soa operator*( const Register& rScale ) const;
            inline Vector3Soa& operator*=( const Register& rScale );

            inline Mask operator==( const Vector3Soa& rVector ) const;
            inline Mask operator!=( const Vector3Soa& rVector ) const;
            //@}

            /// @name Friend Functions
            //@{
            inline friend Vector3Soa operator*( const Register& rScale, const Vector3Soa& rVector );
            //@}
        } HELIUM_SIMD_ALIGN_POST;
    }
}

#include "Math/SimdVector3Soa.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdVector3SoaSse.inl"
#endif
