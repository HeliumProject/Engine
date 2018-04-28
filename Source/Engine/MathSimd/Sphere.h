#pragma once

#include "MathSimd/API.h"
#include "MathSimd/AaBox.h"
#include "MathSimd/Vector4.h"

namespace Helium
{
    namespace Simd
    {
        /// Sphere representation.
        HELIUM_ALIGN_PRE( 16 ) class HELIUM_MATH_SIMD_API Sphere
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline Sphere();
            inline Sphere( const Vector3& rCenter, float32_t radius );
            inline Sphere( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius );
            inline explicit Sphere( const Vector4& rVector );
            inline explicit Sphere( const AaBox& rBox );
#if HELIUM_SIMD_SIZE == 16
            inline explicit Sphere( const Register& rVector );
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

            inline void Set( const Vector3& rCenter, float32_t radius );
            inline void Set( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius );
            inline void Set( const Vector4& rVector );
            inline void Set( const AaBox& rBox );
            //@}

            /// @name Math
            //@{
            inline void SetCenter( const Vector3& rCenter );
            inline void Translate( const Vector3& rOffset );

            inline void SetRadius( float32_t radius );
            inline void Scale( float32_t scale );
            //@}

            /// @name Testing
            //@{
            inline bool Intersects( const Sphere& rSphere, float32_t threshold = HELIUM_EPSILON ) const;
            //@}

        private:
#if HELIUM_SIMD_SIZE == 16
            /// Sphere center and radius.
            Register m_centerRadius;
#else
            /// Sphere center and radius.
            float32_t m_centerRadius[ 4 ];
#endif
        } HELIUM_ALIGN_POST( 16 );
    }
}

#include "MathSimd/Sphere.inl"

#if HELIUM_SIMD_SSE
#include "MathSimd/SphereSse.inl"
#endif
