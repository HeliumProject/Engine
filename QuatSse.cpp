#include "MathSimdPch.h"
#include "MathSimd/Simd.h"

#if HELIUM_SIMD_SSE

#include "MathSimd/Quat.h"

/// Set this quaternion to an axis-angle rotation.
///
/// @param[in] rAxis  Axis of rotation.
/// @param[in] angle  Angle of rotation, in radians.
void Helium::Simd::Quat::Set( const Vector3& rAxis, float32_t angle )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
    {
        0xffffffff,
        0xffffffff,
        0xffffffff,
        0
    };

    angle *= 0.5f;

    Register axisNormalized = rAxis.GetNormalized().GetSimdVector();

    Register sinVec = _mm_set_ps1( Sin( angle ) );
    Register cosVec = _mm_set_ps1( Cos( angle ) );

    Register componentMaskVec = Simd::LoadAligned( componentMask );

    m_quat = Simd::MultiplyF32( axisNormalized, sinVec );
    m_quat = Simd::Or( Simd::And( componentMaskVec, m_quat ), Simd::AndNot( componentMaskVec, cosVec ) );
}

/// Set this quaternion to a rotation defined by Euler angles.
///
/// @param[in] pitch  Pitch, in radians.
/// @param[in] yaw    Yaw, in radians.
/// @param[in] roll   Roll, in radians.
void Helium::Simd::Quat::Set( float32_t pitch, float32_t yaw, float32_t roll )
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

    Register vecR = _mm_set_ps( cosR, sinR, sinR, cosR );
    Register vecP = _mm_set_ps( cosP, cosP, sinP, sinP );

    Register vecRp = Simd::MultiplyF32( vecR, vecP );

    Register vecRpA = _mm_shuffle_ps( vecRp, vecRp, _MM_SHUFFLE( 3, 2, 3, 0 ) );
    Register vecRpB = _mm_shuffle_ps( vecRp, vecRp, _MM_SHUFFLE( 1, 0, 1, 2 ) );

    Register vecYA = _mm_set_ps( cosY, cosY, sinY, cosY );
    Register vecYB = _mm_shuffle_ps( vecYA, vecYA, _MM_SHUFFLE( 1, 1, 0, 1 ) );

    Register vecA = Simd::MultiplyF32( vecRpA, vecYA );
    Register vecB = Simd::MultiplyF32( vecRpB, vecYB );

    Register signFlipVec = Simd::LoadAligned( signFlip );

    m_quat = Simd::AddF32( vecA, Simd::Xor( vecB, signFlipVec ) );
}

#endif  // HELIUM_SIMD_SSE
