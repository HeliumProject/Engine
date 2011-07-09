#pragma once

#include "Math/API.h"
#include "Math/SimdMatrix44.h"

namespace Helium
{
    namespace Simd
    {
        class Plane;
        class AaBox;
        class Sphere;

        /// View frustum.
        HELIUM_SIMD_ALIGN_PRE class HELIUM_MATH_API Frustum
        {
        public:
            /// Frustum planes.
            enum EPlane
            {
                PLANE_FIRST   =  0,
                PLANE_INVALID = -1,

                /// Left clip plane.
                PLANE_LEFT,
                /// Right clip plane.
                PLANE_RIGHT,
                /// Bottom clip plane.
                PLANE_BOTTOM,
                /// Top clip plane.
                PLANE_TOP,
                /// Near clip plane.
                PLANE_NEAR,

                /// Far clip plane.  Note that the internal implementation always expects this to be last for infinite far
                /// clip plane handling.
                PLANE_FAR,

                PLANE_MAX,
                PLANE_LAST = PLANE_MAX - 1
            };

            /// @name Construction/Destruction
            //@{
            Frustum();
            explicit Frustum( const Matrix44& rInverseViewProjectionTranspose );
            //@}

            /// @name Data Access
            //@{
            void Set( const Matrix44& rInverseViewProjectionTranspose );

            void SetFarClip( const Plane& rPlane );
            void SetInfiniteFarClip();
            //@}

            /// @name Testing
            //@{
            bool Contains( const Vector3& rPoint ) const;
            bool Intersects( const AaBox& rBox ) const;
            bool Intersects( const Sphere& rSphere ) const;
            //@}

            /// @name Math
            //@{
            size_t ComputeCorners( Vector3* pCorners ) const;
            size_t ComputeCornersSoa( float32_t* pCornersX, float32_t* pCornersY, float32_t* pCornersZ ) const;
            //@}

        private:
            /// Frustum plane component array size.
            static const size_t PLANE_ARRAY_SIZE =
                ( PLANE_MAX + ( HELIUM_SIMD_SIZE / sizeof( float32_t ) ) - 1 ) & ~( ( HELIUM_SIMD_SIZE / sizeof( float32_t ) ) - 1 );

            /// Frustum plane A coefficients.
            HELIUM_SIMD_ALIGN_PRE float32_t m_planeA[ PLANE_ARRAY_SIZE ] HELIUM_SIMD_ALIGN_POST;
            /// Frustum plane B coefficients.
            HELIUM_SIMD_ALIGN_PRE float32_t m_planeB[ PLANE_ARRAY_SIZE ] HELIUM_SIMD_ALIGN_POST;
            /// Frustum plane C coefficients.
            HELIUM_SIMD_ALIGN_PRE float32_t m_planeC[ PLANE_ARRAY_SIZE ] HELIUM_SIMD_ALIGN_POST;
            /// Frustum plane D coefficients.
            HELIUM_SIMD_ALIGN_PRE float32_t m_planeD[ PLANE_ARRAY_SIZE ] HELIUM_SIMD_ALIGN_POST;

            /// True if the frustum has an infinite far clip (far plane is invalid), false if not.
            bool m_bInfiniteFarClip;
        } HELIUM_SIMD_ALIGN_POST;
    }
}
