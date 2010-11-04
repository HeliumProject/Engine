//----------------------------------------------------------------------------------------------------------------------
// AaBoxSse.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"

#if HELIUM_SIMD_SSE

#include "Core/AaBox.h"
#include "Core/Matrix44.h"
#include "Core/Matrix44Soa.h"
#include "Core/Vector3Soa.h"

namespace Lunar
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
        Helium::SimdVector pointVec = rPoint.GetSimdVector();

        m_minimum.SetSimdVector( Helium::Simd::MinF32( m_minimum.GetSimdVector(), pointVec ) );
        m_maximum.SetSimdVector( Helium::Simd::MaxF32( m_maximum.GetSimdVector(), pointVec ) );
    }

    /// Transform this box using the specified transform matrix.
    ///
    /// @param[in] rTransform  Matrix by which to transform.
    void AaBox::TransformBy( const Matrix44& rTransform )
    {
        // Expand each corner position.
        Helium::SimdVector minVec = m_minimum.GetSimdVector();
        Helium::SimdVector maxVec = m_maximum.GetSimdVector();

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
        Helium::SimdVector minX = Helium::Simd::MinF32( corners0.m_x, corners1.m_x );
        Helium::SimdVector minY = Helium::Simd::MinF32( corners0.m_y, corners1.m_y );
        Helium::SimdVector minXYLo = _mm_unpacklo_ps( minX, minY );
        Helium::SimdVector minXYHi = _mm_unpackhi_ps( minX, minY );
        Helium::SimdVector minXY = Helium::Simd::MinF32( minXYLo, minXYHi );

        Helium::SimdVector minZ = Helium::Simd::MinF32( corners0.m_z, corners1.m_z );
        Helium::SimdVector minZLo = _mm_unpacklo_ps( minZ, minZ );
        Helium::SimdVector minZHi = _mm_unpackhi_ps( minZ, minZ );
        minZ = Helium::Simd::MinF32( minZLo, minZHi );

        Helium::SimdVector minLo = _mm_movelh_ps( minXY, minZ );
        Helium::SimdVector minHi = _mm_movehl_ps( minZ, minXY );

        m_minimum.SetSimdVector( Helium::Simd::MinF32( minLo, minHi ) );

        // Compute the maximum.
        Helium::SimdVector maxX = Helium::Simd::MaxF32( corners0.m_x, corners1.m_x );
        Helium::SimdVector maxY = Helium::Simd::MaxF32( corners0.m_y, corners1.m_y );
        Helium::SimdVector maxXYLo = _mm_unpacklo_ps( maxX, maxY );
        Helium::SimdVector maxXYHi = _mm_unpackhi_ps( maxX, maxY );
        Helium::SimdVector maxXY = Helium::Simd::MaxF32( maxXYLo, maxXYHi );

        Helium::SimdVector maxZ = Helium::Simd::MaxF32( corners0.m_z, corners1.m_z );
        Helium::SimdVector maxZLo = _mm_unpacklo_ps( maxZ, maxZ );
        Helium::SimdVector maxZHi = _mm_unpackhi_ps( maxZ, maxZ );
        maxZ = Helium::Simd::MaxF32( maxZLo, maxZHi );

        Helium::SimdVector maxLo = _mm_movelh_ps( maxXY, maxZ );
        Helium::SimdVector maxHi = _mm_movehl_ps( maxZ, maxXY );

        m_maximum.SetSimdVector( Helium::Simd::MaxF32( maxLo, maxHi ) );
    }
}

#endif  // HELIUM_SIMD_SSE
