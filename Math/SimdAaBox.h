#pragma once

#include "Math/API.h"
#include "Math/SimdVector3.h"

namespace Helium
{
    namespace Simd
    {
        class Matrix44;

        /// Axis-aligned bounding box.
        HELIUM_ALIGN_PRE( 16 ) class HELIUM_MATH_API AaBox
        {
        public:
            /// @name Construction/Destruction
            //@{
            AaBox();
            AaBox( const Vector3& rMinimum, const Vector3& rMaximum );
            //@}

            /// @name Data Access
            //@{
            void Set( const Vector3& rMinimum, const Vector3& rMaximum );
            inline const Vector3& GetMinimum() const;
            inline const Vector3& GetMaximum() const;

            void Expand( const Vector3& rPoint );

            void TransformBy( const Matrix44& rTransform );
            //@}

        private:
            /// Box minimum.
            Vector3 m_minimum;
            /// Box maximum.
            Vector3 m_maximum;
        } HELIUM_ALIGN_POST( 16 );
    }
}

#include "Math/SimdAaBox.inl"
