#pragma once

#include "Math/API.h"
#include "Math/SimdVector3.h"
#include "Math/SimdVector4.h"

namespace Helium
{
    namespace Simd
    {
        /// Plane in 3D space.
        ///
        /// Planes are stored as a vector of 4 floating-point values, with each value representing a coefficient (@c A,
        /// @c B, @c C, or @c D) of the following plane equation:
        ///
        ///     <code>Ax + By + Cz + D = 0</code>
        ///
        /// Note that this is aligned to 16 bytes for SIMD support.
        HELIUM_ALIGN_PRE( 16 ) class HELIUM_MATH_API Plane
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline Plane();
            inline Plane( float32_t a, float32_t b, float32_t c, float32_t d );
            inline Plane( const Vector3& rNormal, float32_t distance );
            inline Plane( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 );
            inline explicit Plane( const Vector4& rVector );
#if HELIUM_SIMD_SIZE == 16
            inline explicit Plane( const Register& rVector );
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

            inline void Set( const Vector3& rNormal, float32_t distance );
            inline void Set( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 );

            inline Vector3 GetNormal() const;
            //@}

            /// @name Math
            //@{
            inline float32_t GetDistance( const Vector3& rPoint ) const;

            inline Plane GetNormalized( float32_t epsilon = HELIUM_EPSILON ) const;
            inline void Normalize( float32_t epsilon = HELIUM_EPSILON );
            //@}

            /// @name Comparison
            //@{
            inline bool Equals( const Plane& rPlane, float32_t epsilon = HELIUM_EPSILON ) const;
            //@}

            /// @name Overloaded Operators
            //@{
            inline bool operator==( const Plane& rPlane ) const;
            inline bool operator!=( const Plane& rPlane ) const;
            //@}

        private:
#if HELIUM_SIMD_SIZE == 16
            /// SIMD vector containing the plane equation coefficients.
            Register m_plane;
#else
            /// Plane equation coefficients.
            float32_t m_plane[ 4 ];
#endif
        } HELIUM_ALIGN_POST( 16 );
    }
}

#include "Math/SimdPlane.inl"

#if HELIUM_SIMD_SSE
#include "Math/SimdPlaneSse.inl"
#endif
