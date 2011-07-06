#pragma once

#include "Math/API.h"
#include "Math/SimdQuat.h"

namespace Helium
{
    namespace Simd
    {
        /// SIMD-optimized structure-of-arrays quaternion.
        HELIUM_SIMD_ALIGN_PRE class MATH_API QuatSoa
        {
        public:
            /// Identity quaternion.
            static const QuatSoa IDENTITY;

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
            inline QuatSoa();
            inline QuatSoa( const Register& rX, const Register& rY, const Register& rZ, const Register& rW );
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

            inline Register GetMagnitude() const;
            inline Register GetMagnitudeSquared() const;

            inline QuatSoa GetNormalized( const Register& rEpsilon = Simd::EPSILON ) const;
            inline void Normalize( const Register& rEpsilon = Simd::EPSILON );

            inline void GetInverse( QuatSoa& rQuat ) const;
            inline QuatSoa GetInverse() const;
            inline void Invert();

            inline void GetConjugate( QuatSoa& rQuat ) const;
            inline QuatSoa GetConjugate() const;
            inline void SetConjugate();
            //@}

            /// @name Comparison
            //@{
            inline Mask Equals( const QuatSoa& rQuat, const Register& rEpsilon = Simd::EPSILON ) const;
            inline Mask NotEquals( const QuatSoa& rQuat, const Register& rEpsilon = Simd::EPSILON ) const;
            //@}

            /// @name Overloaded Operators
            //@{
            inline QuatSoa operator+( const QuatSoa& rQuat ) const;
            inline QuatSoa operator-( const QuatSoa& rQuat ) const;
            inline QuatSoa operator*( const QuatSoa& rQuat ) const;

            inline QuatSoa& operator+=( const QuatSoa& rQuat );
            inline QuatSoa& operator-=( const QuatSoa& rQuat );
            inline QuatSoa& operator*=( const QuatSoa& rQuat );

            inline Mask operator==( const QuatSoa& rQuat ) const;
            inline Mask operator!=( const QuatSoa& rQuat ) const;
            //@}
        } HELIUM_SIMD_ALIGN_POST;
    }
}

#include "Math/SimdQuatSoa.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdQuatSoaSse.inl"
#endif
