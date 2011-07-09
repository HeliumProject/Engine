#pragma once

#include "Math/API.h"
#include "Math/Simd.h"
#include "Math/Common.h"
#include "Math/SimdVector3.h"

namespace Helium
{
    namespace Simd
    {
        /// Quaternion.  Note that this is aligned to 16 bytes for SIMD support.
        HELIUM_ALIGN_PRE( 16 ) class HELIUM_MATH_API Quat
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
#if HELIUM_SIMD_SIZE == 16
            inline explicit Quat( const Register& rVector );
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

            inline Quat GetNormalized( float32_t epsilon = HELIUM_EPSILON ) const;
            inline void Normalize( float32_t epsilon = HELIUM_EPSILON );

            inline void GetInverse( Quat& rQuat ) const;
            inline Quat GetInverse() const;
            inline void Invert();

            inline void GetConjugate( Quat& rQuat ) const;
            inline Quat GetConjugate() const;
            inline void SetConjugate();
            //@}

            /// @name Comparison
            //@{
            inline bool Equals( const Quat& rQuat, float32_t epsilon = HELIUM_EPSILON ) const;
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
#if HELIUM_SIMD_SIZE == 16
            /// SIMD vector containing the quaternion values.
            Register m_quat;
#else
            /// Quaternion values.
            float32_t m_quat[ 4 ];
#endif
        } HELIUM_ALIGN_POST( 16 );
    }
}

#include "Math/SimdQuat.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdQuatSse.inl"
#endif
