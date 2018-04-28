#include "Precompile.h"

#include "MathSimd/Simd.h"

#if HELIUM_SIMD_SSE

#include "MathSimd/AaBox.h"
#include "MathSimd/Matrix44.h"
#include "MathSimd/Matrix44Soa.h"
#include "MathSimd/Vector3Soa.h"

/// Expand this box to include the given point.
///
/// Note that this will expand this box based on whatever current minimum and maximum are set.  If the box is newly
/// created, this will also include the origin (0, 0, 0).  To prevent including the origin, explicitly set the box
/// minimum and maximum to the first point in the sample set and expand the box with each subsequent point.
///
/// @param[in] rPoint  Point to encompass.
void Helium::Simd::AaBox::Expand( const Vector3& rPoint )
{
    Register pointVec = rPoint.GetSimdVector();

    m_minimum.SetSimdVector( Simd::MinF32( m_minimum.GetSimdVector(), pointVec ) );
    m_maximum.SetSimdVector( Simd::MaxF32( m_maximum.GetSimdVector(), pointVec ) );
}

/// Transform this box using the specified transform matrix.
///
/// @param[in] rTransform  Matrix by which to transform.
void Helium::Simd::AaBox::TransformBy( const Matrix44& rTransform )
{
    // Expand each corner position.
    Register minVec = m_minimum.GetSimdVector();
    Register maxVec = m_maximum.GetSimdVector();

    Vector3Soa corners0;
    corners0.m_x = _mm_shuffle_ps( minVec, minVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    corners0.m_y = _mm_shuffle_ps( minVec, maxVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    corners0.m_z = _mm_unpackhi_ps( minVec, maxVec );
    corners0.m_z = _mm_movelh_ps( corners0.m_z, corners0.m_z );

    Vector3Soa corners1;
    corners1.m_x = _mm_shuffle_ps( maxVec, maxVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    corners1.m_y = corners0.m_y;
    corners1.m_z = corners0.m_z;

    // Transform all corners by the provided transformation matrix.
    Matrix44Soa transformSplat( rTransform );
    transformSplat.TransformPoint( corners0, corners0 );
    transformSplat.TransformPoint( corners1, corners1 );

    // Compute the minimum.
    Register minX = Simd::MinF32( corners0.m_x, corners1.m_x );
    Register minY = Simd::MinF32( corners0.m_y, corners1.m_y );
    Register minXYLo = _mm_unpacklo_ps( minX, minY );
    Register minXYHi = _mm_unpackhi_ps( minX, minY );
    Register minXY = Simd::MinF32( minXYLo, minXYHi );

    Register minZ = Simd::MinF32( corners0.m_z, corners1.m_z );
    Register minZLo = _mm_unpacklo_ps( minZ, minZ );
    Register minZHi = _mm_unpackhi_ps( minZ, minZ );
    minZ = Simd::MinF32( minZLo, minZHi );

    Register minLo = _mm_movelh_ps( minXY, minZ );
    Register minHi = _mm_movehl_ps( minZ, minXY );

    m_minimum.SetSimdVector( Simd::MinF32( minLo, minHi ) );

    // Compute the maximum.
    Register maxX = Simd::MaxF32( corners0.m_x, corners1.m_x );
    Register maxY = Simd::MaxF32( corners0.m_y, corners1.m_y );
    Register maxXYLo = _mm_unpacklo_ps( maxX, maxY );
    Register maxXYHi = _mm_unpackhi_ps( maxX, maxY );
    Register maxXY = Simd::MaxF32( maxXYLo, maxXYHi );

    Register maxZ = Simd::MaxF32( corners0.m_z, corners1.m_z );
    Register maxZLo = _mm_unpacklo_ps( maxZ, maxZ );
    Register maxZHi = _mm_unpackhi_ps( maxZ, maxZ );
    maxZ = Simd::MaxF32( maxZLo, maxZHi );

    Register maxLo = _mm_movelh_ps( maxXY, maxZ );
    Register maxHi = _mm_movehl_ps( maxZ, maxXY );

    m_maximum.SetSimdVector( Simd::MaxF32( maxLo, maxHi ) );
}

#endif  // HELIUM_SIMD_SSE
