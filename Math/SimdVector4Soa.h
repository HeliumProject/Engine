#pragma once

#include "Math/API.h"
#include "Math/SimdVector4.h"

namespace Helium
{
    namespace Simd
    {
        /// SIMD-optimized structure-of-arrays 4-component vector.
        HELIUM_SIMD_ALIGN_PRE class HELIUM_MATH_API Vector4Soa
        {
        public:
            /// X components.
            Register m_x;
            /// Y components.
            Register m_y;
            /// Z components.
            Register m_z;
            /// W components.
            Register m_w;

            /// @name Construction/Destruction
            //@{
            inline Vector4Soa();
            inline Vector4Soa( const Register& rX, const Register& rY, const Register& rZ, const Register& rW );
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

            inline Vector4Soa GetScaled( const Register& rScale ) const;
            inline void Scale( const Register& rScale );

            inline Register Dot( const Vector4Soa& rVector ) const;

            inline Register GetMagnitude() const;
            inline Register GetMagnitudeSquared() const;

            inline Vector4Soa GetNormalized( const Register& rEpsilon = Simd::EPSILON ) const;
            inline void Normalize( const Register& rEpsilon = Simd::EPSILON );

            inline Vector4Soa GetNegated() const;
            inline void GetNegated( Vector4Soa& rResult ) const;
            inline void Negate();
            //@}

            /// @name Comparison
            //@{
            inline Mask Equals( const Vector4Soa& rVector, const Register& rEpsilon = Simd::EPSILON ) const;
            inline Mask NotEquals( const Vector4Soa& rVector, const Register& rEpsilon = Simd::EPSILON ) const;
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

            inline Vector4Soa operator*( const Register& rScale ) const;
            inline Vector4Soa& operator*=( const Register& rScale );

            inline Mask operator==( const Vector4Soa& rVector ) const;
            inline Mask operator!=( const Vector4Soa& rVector ) const;
            //@}

            /// @name Friend Functions
            //@{
            inline friend Vector4Soa operator*( const Register& rScale, const Vector4Soa& rVector );
            //@}
        } HELIUM_SIMD_ALIGN_POST;
    }
}

#include "Math/SimdVector4Soa.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdVector4SoaSse.inl"
#endif
