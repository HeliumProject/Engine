#pragma once

#include "Foundation/API.h"
#include "Foundation/Math/SimdVector3.h"
#include "Foundation/Math/SimdVector4.h"

namespace Helium
{
    namespace Simd
    {
        /// @defgroup vectorconversion Vector3/Vector4 Conversion Support
        //@{
        inline Vector4 Vector3ToVector4( const Vector3& rVector );
        inline Vector4 PointToVector4( const Vector3& rVector );
        inline Vector4 RayToVector4( const Vector3& rVector );

        inline Vector3 Vector4ToVector3( const Vector4& rVector );
        //@}
    }
}

#if HELIUM_SIMD_SSE
#include "Foundation/Math/SimdVectorConversionSse.inl"
#endif
