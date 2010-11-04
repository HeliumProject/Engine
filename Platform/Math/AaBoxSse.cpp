//----------------------------------------------------------------------------------------------------------------------
// AaBoxSse.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//#include "CorePch.h"

#if HELIUM_SIMD_SSE

#include "Platform/Math/AaBox.h"
#include "Platform/Math/Matrix44.h"
#include "Platform/Math/Matrix44Soa.h"
#include "Platform/Math/Vector3Soa.h"

namespace Helium
{
    /// Expand this box to include the given point.
    ///
    /// Note that this will expand this box based on whatever current minimum and maximum are set.  If the box is newly
    /// created, this will also include the origin (0, 0, 0).  To prevent including the origin, explicitly set the box
    /// minimum and maximum to the first point in the sample set and expand the box with each subsequent point.
    ///
    /// @param[in] rPoint  Point to encompass.
    void AaBox::Expand( const Vector3& rPoint )
    {
        SimdVector pointVec = rPoint.GetSimdVector();

        m_minimum.SetSimdVector( Simd::MinF32( m_minimum.GetSimdVector(), pointVec ) );
        m_maximum.SetSimdVector( Simd::MaxF32( m_maximum.GetSimdVector(), pointVec ) );
    }

    /// Transform this box using the specified transform matrix.
    ///
    /// @param[in] rTransform  Matrix by which to transform.
    void AaBox::TransformBy( const Matrix44& rTransform )
    {
        // Expand each corner position.
        SimdVector minVec = m_minimum.GetSimdVector();
        SimdVector maxVec = m_maximum.GetSimdVector();

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
        SimdVector minX = Simd::MinF32( corners0.m_x, corners1.m_x );
        SimdVector minY = Simd::MinF32( corners0.m_y, corners1.m_y );
        SimdVector minXYLo = _mm_unpacklo_ps( minX, minY );
        SimdVector minXYHi = _mm_unpackhi_ps( minX, minY );
        SimdVector minXY = Simd::MinF32( minXYLo, minXYHi );

        SimdVector minZ = Simd::MinF32( corners0.m_z, corners1.m_z );
        SimdVector minZLo = _mm_unpacklo_ps( minZ, minZ );
        SimdVector minZHi = _mm_unpackhi_ps( minZ, minZ );
        minZ = Simd::MinF32( minZLo, minZHi );

        SimdVector minLo = _mm_movelh_ps( minXY, minZ );
        SimdVector minHi = _mm_movehl_ps( minZ, minXY );

        m_minimum.SetSimdVector( Simd::MinF32( minLo, minHi ) );

        // Compute the maximum.
        SimdVector maxX = Simd::MaxF32( corners0.m_x, corners1.m_x );
        SimdVector maxY = Simd::MaxF32( corners0.m_y, corners1.m_y );
        SimdVector maxXYLo = _mm_unpacklo_ps( maxX, maxY );
        SimdVector maxXYHi = _mm_unpackhi_ps( maxX, maxY );
        SimdVector maxXY = Simd::MaxF32( maxXYLo, maxXYHi );

        SimdVector maxZ = Simd::MaxF32( corners0.m_z, corners1.m_z );
        SimdVector maxZLo = _mm_unpacklo_ps( maxZ, maxZ );
        SimdVector maxZHi = _mm_unpackhi_ps( maxZ, maxZ );
        maxZ = Simd::MaxF32( maxZLo, maxZHi );

        SimdVector maxLo = _mm_movelh_ps( maxXY, maxZ );
        SimdVector maxHi = _mm_movehl_ps( maxZ, maxXY );

        m_maximum.SetSimdVector( Simd::MaxF32( maxLo, maxHi ) );
    }
}

#endif  // HELIUM_SIMD_SSE
