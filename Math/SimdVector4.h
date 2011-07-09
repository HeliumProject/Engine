#pragma once

#include "Math/API.h"
#include "Math/Common.h"
#include "Math/Simd.h"

namespace Helium
{
    namespace Simd
    {
        /// Single-precision floating-point 4-component vector.  Note that this is aligned to 16 bytes for SIMD support.
        HELIUM_ALIGN_PRE( 16 ) class HELIUM_MATH_API Vector4
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline Vector4();
            inline Vector4( float32_t x, float32_t y, float32_t z, float32_t w );
            inline explicit Vector4( float32_t s );
#if HELIUM_SIMD_SIZE == 16
            inline explicit Vector4( const Register& rVector );
#endif
            //@}

            /// @name Data Access
            //@{
#if HELIUM_SIMD_SIZE == 16
            inline Register& GetSimdVector();
            inline const Register& GetSimdVector() const;
            inline void SetSimdVector( const Register& rVector );
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

            inline Vector4 GetNormalized( float32_t epsilon = HELIUM_EPSILON ) const;
            inline void Normalize( float32_t epsilon = HELIUM_EPSILON );

            inline Vector4 GetNegated() const;
            inline void GetNegated( Vector4& rResult ) const;
            inline void Negate();
            //@}

            /// @name Comparison
            //@{
            inline bool Equals( const Vector4& rVector, float32_t epsilon = HELIUM_EPSILON ) const;
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
#if HELIUM_SIMD_SIZE == 16
            /// SIMD vector containing the vector values.
            Register m_vector;
#else
            /// Vector values.
            float32_t m_vector[ 4 ];
#endif
        } HELIUM_ALIGN_POST( 16 );
    }
}

#include "Math/SimdVector4.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdVector4Sse.inl"
#endif
