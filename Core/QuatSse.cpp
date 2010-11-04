//----------------------------------------------------------------------------------------------------------------------
// QuatSse.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Platform/Simd.h"

#if HELIUM_SIMD_SSE

#include "Core/Quat.h"

namespace Lunar
{
    /// Set this quaternion to an axis-angle rotation.
    ///
    /// @param[in] rAxis  Axis of rotation.
    /// @param[in] angle  Angle of rotation, in radians.
    void Quat::Set( const Vector3& rAxis, float32_t angle )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        angle *= 0.5f;

        Helium::SimdVector axisNormalized = rAxis.GetNormalized().GetSimdVector();

        Helium::SimdVector sinVec = _mm_set_ps1( Sin( angle ) );
        Helium::SimdVector cosVec = _mm_set_ps1( Cos( angle ) );

        Helium::SimdVector componentMaskVec = Helium::Simd::LoadAligned( componentMask );

        m_quat = Helium::Simd::MultiplyF32( axisNormalized, sinVec );
        m_quat = Helium::Simd::Or( Helium::Simd::And( componentMaskVec, m_quat ), Helium::Simd::AndNot( componentMaskVec, cosVec ) );
    }

    /// Set this quaternion to a rotation defined by Euler angles.
    ///
    /// @param[in] pitch  Pitch, in radians.
    /// @param[in] yaw    Yaw, in radians.
    /// @param[in] roll   Roll, in radians.
    void Quat::Set( float32_t pitch, float32_t yaw, float32_t roll )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t signFlip[ 4 ] HELIUM_SIMD_ALIGN_POST = { 0, 0, 0x80000000, 0x80000000 };

        roll *= 0.5f;
        pitch *= 0.5f;
        yaw *= 0.5f;

        float32_t cosR = Cos( roll );
        float32_t sinR = Sin( roll );
        float32_t cosP = Cos( pitch );
        float32_t sinP = Sin( pitch );
        float32_t cosY = Cos( yaw );
        float32_t sinY = Sin( yaw );

        Helium::SimdVector vecR = _mm_set_ps( cosR, sinR, sinR, cosR );
        Helium::SimdVector vecP = _mm_set_ps( cosP, cosP, sinP, sinP );

        Helium::SimdVector vecRp = Helium::Simd::MultiplyF32( vecR, vecP );

        Helium::SimdVector vecRpA = _mm_shuffle_ps( vecRp, vecRp, _MM_SHUFFLE( 3, 2, 3, 0 ) );
        Helium::SimdVector vecRpB = _mm_shuffle_ps( vecRp, vecRp, _MM_SHUFFLE( 1, 0, 1, 2 ) );

        Helium::SimdVector vecYA = _mm_set_ps( cosY, cosY, sinY, cosY );
        Helium::SimdVector vecYB = _mm_shuffle_ps( vecYA, vecYA, _MM_SHUFFLE( 1, 1, 0, 1 ) );

        Helium::SimdVector vecA = Helium::Simd::MultiplyF32( vecRpA, vecYA );
        Helium::SimdVector vecB = Helium::Simd::MultiplyF32( vecRpB, vecYB );

        Helium::SimdVector signFlipVec = Helium::Simd::LoadAligned( signFlip );

        m_quat = Helium::Simd::AddF32( vecA, Helium::Simd::Xor( vecB, signFlipVec ) );
    }
}

#endif  // HELIUM_SIMD_SSE
