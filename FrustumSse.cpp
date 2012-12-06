#include "MathSimdPch.h"

#include "MathSimd/Simd.h"

#if HELIUM_SIMD_SSE

#include "MathSimd/Frustum.h"
#include "MathSimd/AaBox.h"
#include "MathSimd/PlaneSoa.h"
#include "MathSimd/Sphere.h"
#include "MathSimd/Vector3.h"
#include "MathSimd/Vector3Soa.h"

/// Test whether this frustum fully contains a given point in world space.
///
/// @param[in] rPoint  Point to test.
///
/// @return  True if the point is within this frustum, false if not.
bool Helium::Simd::Frustum::Contains( const Vector3& rPoint ) const
{
    // Test the point against each plane set.
    Vector3Soa pointSplat( rPoint );

    Helium::Simd::Register zeroVec = Helium::Simd::LoadZeros();
    PlaneSoa planes;
    for( size_t basePlaneIndex = 0; basePlaneIndex < PLANE_ARRAY_SIZE; basePlaneIndex += 4 )
    {
        planes.Load(
            m_planeA + basePlaneIndex,
            m_planeB + basePlaneIndex,
            m_planeC + basePlaneIndex,
            m_planeD + basePlaneIndex );

        Helium::Simd::Register distances = planes.GetDistance( pointSplat );
        int resultMask = _mm_movemask_ps( Helium::Simd::GreaterEqualsF32( distances, zeroVec ) );
        if( resultMask != 0xf )
        {
            return false;
        }
    }

    return true;
}

/// Test whether this frustum intersects a given axis-aligned bounding box in world space.
///
/// @param[in] rBox  Box to test.
///
/// @return  True if the box intersects this frustum, false if not.
bool Helium::Simd::Frustum::Intersects( const AaBox& rBox ) const
{
    Helium::Simd::Register boxMinVec = rBox.GetMinimum().GetSimdVector();
    Helium::Simd::Register boxMaxVec = rBox.GetMaximum().GetSimdVector();

    Helium::Simd::Register boxX0 = _mm_shuffle_ps( boxMinVec, boxMinVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Helium::Simd::Register boxX1 = _mm_shuffle_ps( boxMaxVec, boxMaxVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Helium::Simd::Register boxY = _mm_shuffle_ps( boxMinVec, boxMaxVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    Helium::Simd::Register boxZ = _mm_unpackhi_ps( boxMinVec, boxMaxVec );
    boxZ = _mm_movelh_ps( boxZ, boxZ );

    PlaneSoa plane;
    Vector3Soa points( boxX0, boxY, boxZ );
    Helium::Simd::Register zeroVec = Helium::Simd::LoadZeros();

    size_t planeCount = ( m_bInfiniteFarClip ? PLANE_FAR : PLANE_MAX );
    for( size_t planeIndex = 0; planeIndex < planeCount; ++planeIndex )
    {
        plane.Load1Splat(
            m_planeA + planeIndex,
            m_planeB + planeIndex,
            m_planeC + planeIndex,
            m_planeD + planeIndex );

        points.m_x = boxX0;
        Helium::Simd::Mask containsPoints0 = Helium::Simd::GreaterEqualsF32( plane.GetDistance( points ), zeroVec );

        points.m_x = boxX1;
        Helium::Simd::Mask containsPoints1 = Helium::Simd::GreaterEqualsF32( plane.GetDistance( points ), zeroVec );

        int resultMask = _mm_movemask_ps( Helium::Simd::Or( containsPoints0, containsPoints1 ) );
        if( resultMask == 0 )
        {
            return false;
        }
    }

    return true;
}

/// Test whether this frustum intersects a given sphere in world space.
///
/// @param[in] rSphere  Sphere to test.
///
/// @return  True if the sphere intersects this frustum, false if not.
bool Helium::Simd::Frustum::Intersects( const Sphere& rSphere ) const
{
    Helium::Simd::Register sphereVec = rSphere.GetSimdVector();

    Vector3Soa center(
        _mm_shuffle_ps( sphereVec, sphereVec, _MM_SHUFFLE( 0, 0, 0, 0 ) ),
        _mm_shuffle_ps( sphereVec, sphereVec, _MM_SHUFFLE( 1, 1, 1, 1 ) ),
        _mm_shuffle_ps( sphereVec, sphereVec, _MM_SHUFFLE( 2, 2, 2, 2 ) ) );

    Helium::Simd::Register radius = _mm_shuffle_ps( sphereVec, sphereVec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

    Helium::Simd::Register zeroVec = Helium::Simd::LoadZeros();
    PlaneSoa planes;
    for( size_t basePlaneIndex = 0; basePlaneIndex < PLANE_ARRAY_SIZE; basePlaneIndex += 4 )
    {
        planes.Load(
            m_planeA + basePlaneIndex,
            m_planeB + basePlaneIndex,
            m_planeC + basePlaneIndex,
            m_planeD + basePlaneIndex );

        Helium::Simd::Register distances = Helium::Simd::AddF32( planes.GetDistance( center ), radius );
        int resultMask = _mm_movemask_ps( Helium::Simd::GreaterEqualsF32( distances, zeroVec ) );
        if( resultMask != 0xf )
        {
            return false;
        }
    }

    return true;
}

/// Compute the corners of this view frustum.
///
/// A view frustum can have either four or eight corners depending on whether a far clip plane exists (eight
/// corners) or whether an infinite far clip plane is used (four corners).
///
/// Note that this assumes that the frustum is always properly defined, with each possible combination of
/// neighboring clip planes intersecting at a valid point.
///
/// @param[out] pCorners  Array in which the frustum corners will be stored.  This must point to a region of memory
///                       large enough for four points if this frustum has an infinite far clip plane, or eight
///                       points if this frustum has a normal far clip plane.
///
/// @return  Number of clip planes computed (either four or eight).
size_t Helium::Simd::Frustum::ComputeCorners( Vector3* pCorners ) const
{
    HELIUM_ASSERT( pCorners );

    // Compute the corners in struct-of-arrays format.
    HELIUM_SIMD_ALIGN_PRE float32_t cornersX[ 8 ] HELIUM_SIMD_ALIGN_POST;
    HELIUM_SIMD_ALIGN_PRE float32_t cornersY[ 8 ] HELIUM_SIMD_ALIGN_POST;
    HELIUM_SIMD_ALIGN_PRE float32_t cornersZ[ 8 ] HELIUM_SIMD_ALIGN_POST;

    size_t cornerCount = ComputeCornersSoa( cornersX, cornersY, cornersZ );
    HELIUM_ASSERT( cornerCount == 4 || cornerCount == 8 );

    // Swizzle the results and store in the output array.
    Helium::Simd::Register cornerXVec = Helium::Simd::LoadAligned( cornersX );
    Helium::Simd::Register cornerYVec = Helium::Simd::LoadAligned( cornersY );
    Helium::Simd::Register cornerZVec = Helium::Simd::LoadAligned( cornersZ );

    Helium::Simd::Register xy01 = _mm_unpacklo_ps( cornerXVec, cornerYVec );
    Helium::Simd::Register xy23 = _mm_unpackhi_ps( cornerXVec, cornerYVec );
    Helium::Simd::Register zz01 = _mm_unpacklo_ps( cornerZVec, cornerZVec );
    Helium::Simd::Register zz23 = _mm_unpackhi_ps( cornerZVec, cornerZVec );

    pCorners[ 0 ].SetSimdVector( _mm_movelh_ps( xy01, zz01 ) );
    pCorners[ 1 ].SetSimdVector( _mm_movehl_ps( zz01, xy01 ) );
    pCorners[ 2 ].SetSimdVector( _mm_movelh_ps( xy23, zz23 ) );
    pCorners[ 3 ].SetSimdVector( _mm_movehl_ps( zz23, xy23 ) );

    if( cornerCount == 8 )
    {
        cornerXVec = Helium::Simd::LoadAligned( cornersX + 4 );
        cornerYVec = Helium::Simd::LoadAligned( cornersY + 4 );
        cornerZVec = Helium::Simd::LoadAligned( cornersZ + 4 );

        xy01 = _mm_unpacklo_ps( cornerXVec, cornerYVec );
        xy23 = _mm_unpackhi_ps( cornerXVec, cornerYVec );
        zz01 = _mm_unpacklo_ps( cornerZVec, cornerZVec );
        zz23 = _mm_unpackhi_ps( cornerZVec, cornerZVec );

        pCorners[ 4 ].SetSimdVector( _mm_movelh_ps( xy01, zz01 ) );
        pCorners[ 5 ].SetSimdVector( _mm_movehl_ps( zz01, xy01 ) );
        pCorners[ 6 ].SetSimdVector( _mm_movelh_ps( xy23, zz23 ) );
        pCorners[ 7 ].SetSimdVector( _mm_movehl_ps( zz23, xy23 ) );
    }

    return cornerCount;
}

/// Compute the corners of this view frustum, outputting the result in separate arrays for each component.
///
/// A view frustum can have either four or eight corners depending on whether a far clip plane exists (eight
/// corners) or whether an infinite far clip plane is used (four corners).
///
/// Note that this assumes that the frustum is always properly defined, with each possible combination of
/// neighboring clip planes intersecting at a valid point.
///
/// @param[out] pCornersX  SIMD-aligned array in which the frustum corner x coordinates will be stored.  This must
///                        point to a region of memory large enough for four points if this frustum has an infinite
///                        far clip plane, or eight points if this frustum has a normal far clip plane.
/// @param[out] pCornersY  SIMD-aligned array in which the frustum corner y coordinates will be stored.  This must
///                        point to a region of memory large enough for four points if this frustum has an infinite
///                        far clip plane, or eight points if this frustum has a normal far clip plane.
/// @param[out] pCornersZ  SIMD-aligned array in which the frustum corner z coordinates will be stored.  This must
///                        point to a region of memory large enough for four points if this frustum has an infinite
///                        far clip plane, or eight points if this frustum has a normal far clip plane.
///
/// @return  Number of clip planes computed (either four or eight).
size_t Helium::Simd::Frustum::ComputeCornersSoa( float32_t* pCornersX, float32_t* pCornersY, float32_t* pCornersZ ) const
{
    HELIUM_ASSERT( pCornersX );
    HELIUM_ASSERT( pCornersY );
    HELIUM_ASSERT( pCornersZ );

    // Load the plane combinations used to compute the four corners on the near clip plane.
    Helium::Simd::Register plane0A = Helium::Simd::LoadAligned( m_planeA );
    Helium::Simd::Register plane0B = Helium::Simd::LoadAligned( m_planeB );
    Helium::Simd::Register plane0C = Helium::Simd::LoadAligned( m_planeC );
    Helium::Simd::Register plane0D = Helium::Simd::LoadAligned( m_planeD );

    Helium::Simd::Register plane1A = _mm_shuffle_ps( plane0A, plane0A, _MM_SHUFFLE( 0, 1, 3, 2 ) );
    Helium::Simd::Register plane1B = _mm_shuffle_ps( plane0B, plane0B, _MM_SHUFFLE( 0, 1, 3, 2 ) );
    Helium::Simd::Register plane1C = _mm_shuffle_ps( plane0C, plane0C, _MM_SHUFFLE( 0, 1, 3, 2 ) );
    Helium::Simd::Register plane1D = _mm_shuffle_ps( plane0D, plane0D, _MM_SHUFFLE( 0, 1, 3, 2 ) );

    Helium::Simd::Register plane2A = Helium::Simd::LoadSplat32( m_planeA + PLANE_NEAR );
    Helium::Simd::Register plane2B = Helium::Simd::LoadSplat32( m_planeB + PLANE_NEAR );
    Helium::Simd::Register plane2C = Helium::Simd::LoadSplat32( m_planeC + PLANE_NEAR );
    Helium::Simd::Register plane2D = Helium::Simd::LoadSplat32( m_planeD + PLANE_NEAR );

    // Compute all four near clip corners.
    Helium::Simd::Register detAB = _mm_sub_ps( _mm_mul_ps( plane0A, plane1B ), _mm_mul_ps( plane0B, plane1A ) );
    Helium::Simd::Register detAC = _mm_sub_ps( _mm_mul_ps( plane0A, plane1C ), _mm_mul_ps( plane0C, plane1A ) );
    Helium::Simd::Register detBC = _mm_sub_ps( _mm_mul_ps( plane0B, plane1C ), _mm_mul_ps( plane0C, plane1B ) );

    Helium::Simd::Register detAD = _mm_sub_ps( _mm_mul_ps( plane0A, plane1D ), _mm_mul_ps( plane0D, plane1A ) );
    Helium::Simd::Register detBD = _mm_sub_ps( _mm_mul_ps( plane0B, plane1D ), _mm_mul_ps( plane0D, plane1B ) );
    Helium::Simd::Register detDC = _mm_sub_ps( _mm_mul_ps( plane0D, plane1C ), _mm_mul_ps( plane0C, plane1D ) );

    // XXX: Denominator sign is flipped here to handle the fact our plane D component is the negative distance from
    // the origin (sign gets flipped when placed at the opposite side of the linear equation set for each plane when
    // solving).
    //
    // Our plane equation:
    //     Ax + By + Cz + D = 0
    ///
    // ...in the form needed for solving using Cramer's rule:
    //     Ax + By + Cz = -D
    Helium::Simd::Register denominator = _mm_rcp_ps(
        _mm_sub_ps(
            _mm_mul_ps( plane2B, detAC ),
            _mm_add_ps(
                _mm_mul_ps( plane2A, detBC ),
                _mm_mul_ps( plane2C, detAB ) ) ) );

    //Helium::Simd::Register cornerXVec = _mm_mul_ps(
    //    _mm_add_ps(
    //        _mm_sub_ps(
    //            _mm_mul_ps( plane2D, detBC ),
    //            _mm_mul_ps( plane2B, detDC ) ),
    //        _mm_mul_ps( plane2C, detDB ) ),
    //    denominator );
    Helium::Simd::Register cornerXVec = _mm_mul_ps(
        _mm_sub_ps(  // Note that we subtract...
            _mm_sub_ps(
                _mm_mul_ps( plane2D, detBC ),
                _mm_mul_ps( plane2B, detDC ) ),
            _mm_mul_ps( plane2C, detBD ) ),  // ...because the 2x2 sub-matrix components are switched here.
        denominator );

    Helium::Simd::Register cornerYVec = _mm_mul_ps(
        _mm_add_ps(
            _mm_sub_ps(
                _mm_mul_ps( plane2A, detDC ),
                _mm_mul_ps( plane2D, detAC ) ),
            _mm_mul_ps( plane2C, detAD ) ),
        denominator );

    Helium::Simd::Register cornerZVec = _mm_mul_ps(
        _mm_add_ps(
            _mm_sub_ps(
                _mm_mul_ps( plane2A, detBD ),
                _mm_mul_ps( plane2B, detAD ) ),
            _mm_mul_ps( plane2D, detAB ) ),
        denominator );

    Helium::Simd::StoreAligned( pCornersX, cornerXVec );
    Helium::Simd::StoreAligned( pCornersY, cornerYVec );
    Helium::Simd::StoreAligned( pCornersZ, cornerZVec );

    // If this frustum has an infinite far clip plane, we are done.
    if( m_bInfiniteFarClip )
    {
        return 4;
    }

    // Compute the far clip plane corners, reusing data from the near clip plane corner calculations where possible.
    plane2A = Helium::Simd::LoadSplat32( m_planeA + PLANE_FAR );
    plane2B = Helium::Simd::LoadSplat32( m_planeB + PLANE_FAR );
    plane2C = Helium::Simd::LoadSplat32( m_planeC + PLANE_FAR );
    plane2D = Helium::Simd::LoadSplat32( m_planeD + PLANE_FAR );

    denominator = _mm_rcp_ps(
        _mm_sub_ps(
            _mm_mul_ps( plane2B, detAC ),
            _mm_add_ps(
                _mm_mul_ps( plane2A, detBC ),
                _mm_mul_ps( plane2C, detAB ) ) ) );

    //cornerXVec = _mm_mul_ps(
    //    _mm_add_ps(
    //        _mm_sub_ps(
    //            _mm_mul_ps( plane2D, detBC ),
    //            _mm_mul_ps( plane2B, detDC ) ),
    //        _mm_mul_ps( plane2C, detDB ) ),
    //    denominator );
    cornerXVec = _mm_mul_ps(
        _mm_sub_ps(  // Note that we subtract...
            _mm_sub_ps(
                _mm_mul_ps( plane2D, detBC ),
                _mm_mul_ps( plane2B, detDC ) ),
            _mm_mul_ps( plane2C, detBD ) ),  // ...because the 2x2 sub-matrix components are switched here.
        denominator );

    cornerYVec = _mm_mul_ps(
        _mm_add_ps(
            _mm_sub_ps(
                _mm_mul_ps( plane2A, detDC ),
                _mm_mul_ps( plane2D, detAC ) ),
            _mm_mul_ps( plane2C, detAD ) ),
        denominator );

    cornerZVec = _mm_mul_ps(
        _mm_add_ps(
            _mm_sub_ps(
                _mm_mul_ps( plane2A, detBD ),
                _mm_mul_ps( plane2B, detAD ) ),
            _mm_mul_ps( plane2D, detAB ) ),
        denominator );

    Helium::Simd::StoreAligned( pCornersX + 4, cornerXVec );
    Helium::Simd::StoreAligned( pCornersY + 4, cornerYVec );
    Helium::Simd::StoreAligned( pCornersZ + 4, cornerZVec );

    return 8;
}

#endif  // HELIUM_SIMD_SSE
