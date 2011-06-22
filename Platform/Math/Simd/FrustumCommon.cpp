#include "PlatformPch.h"

#include "Platform/Math/Simd/Frustum.h"
#include "Platform/Math/Simd/Matrix44Soa.h"
#include "Platform/Math/Simd/PlaneSoa.h"
#include "Platform/Math/Simd/Vector4Soa.h"

/// Constructor.
///
/// This creates a frustum without initializing its values.  Values must be initialized explicitly before use.
Helium::Simd::Frustum::Frustum()
{
}

/// Constructor.
///
/// @param[in] rInverseViewProjectionTranspose  Transpose of the matrix transforming from world space to projected
///                                             screen space.
Helium::Simd::Frustum::Frustum( const Matrix44& rInverseViewProjectionTranspose )
{
    Set( rInverseViewProjectionTranspose );
}

/// Initialize this frustum based on the given transformation matrix.
///
/// @param[in] rInverseViewProjectionTranspose  Transpose of the matrix transforming from world space to projected
///                                             screen space.
void Helium::Simd::Frustum::Set( const Matrix44& rInverseViewProjectionTranspose )
{
    // Check whether the input matrix yields an infinite far clip plane.
    Vector4 rowDifference;
    rowDifference.SubtractSet(
        rInverseViewProjectionTranspose.GetRow( 3 ),
        rInverseViewProjectionTranspose.GetRow( 2 ) );

    m_bInfiniteFarClip =
        ( Abs( rowDifference.GetElement( 0 ) ) < HELIUM_EPSILON &&
          Abs( rowDifference.GetElement( 1 ) ) < HELIUM_EPSILON &&
          Abs( rowDifference.GetElement( 2 ) ) < HELIUM_EPSILON );

    // Splat the transformation matrix into a struct-of-arrays matrix set.
    Matrix44Soa matrixSplat( rInverseViewProjectionTranspose );

    // Initialize our pre-transformed clip plane values.
#pragma TODO("Replace these with MemoryZero")
    memset( m_planeA, 0, sizeof( m_planeA ) );
    memset( m_planeB, 0, sizeof( m_planeB ) );
    memset( m_planeC, 0, sizeof( m_planeC ) );

    m_planeA[ PLANE_LEFT   ] =  1.0f;
    m_planeA[ PLANE_RIGHT  ] = -1.0f;

    m_planeB[ PLANE_BOTTOM ] =  1.0f;
    m_planeB[ PLANE_TOP    ] = -1.0f;

    m_planeC[ PLANE_NEAR   ] =  1.0f;
    m_planeC[ PLANE_FAR    ] = -1.0f;

    m_planeD[ PLANE_LEFT   ] = 1.0f;
    m_planeD[ PLANE_RIGHT  ] = 1.0f;
    m_planeD[ PLANE_BOTTOM ] = 1.0f;
    m_planeD[ PLANE_TOP    ] = 1.0f;
    m_planeD[ PLANE_NEAR   ] = 0.0f;
    m_planeD[ PLANE_FAR    ] = 1.0f;

    // Transform the clip planes into world space.
    PlaneSoa planes;
    Vector4Soa planeVectors;

    for( size_t basePlaneIndex = 0;
         basePlaneIndex < PLANE_ARRAY_SIZE;
         basePlaneIndex += HELIUM_SIMD_SIZE / sizeof( float32_t ) )
    {
        float32_t* pPlaneA = m_planeA + basePlaneIndex;
        float32_t* pPlaneB = m_planeB + basePlaneIndex;
        float32_t* pPlaneC = m_planeC + basePlaneIndex;
        float32_t* pPlaneD = m_planeD + basePlaneIndex;

        planeVectors.Load( pPlaneA, pPlaneB, pPlaneC, pPlaneD );

        matrixSplat.Transform( planeVectors, planeVectors );

        planes.m_a = planeVectors.m_x;
        planes.m_b = planeVectors.m_y;
        planes.m_c = planeVectors.m_z;
        planes.m_d = planeVectors.m_w;
        planes.Normalize();

        planes.Store( pPlaneA, pPlaneB, pPlaneC, pPlaneD );
    }

    // Set the unused plane entries to values that ensure that tests always pass with them (if an infinite far clip
    // plane is used, its plane is also invalid and should be set to always pass as well).
    size_t unusedPlaneStart = ( m_bInfiniteFarClip ? PLANE_FAR : PLANE_MAX );
    size_t unusedPlaneCount = PLANE_ARRAY_SIZE - unusedPlaneStart;

#pragma TODO("Replace these with MemoryZero")
    memset( m_planeA + unusedPlaneStart, 0, sizeof( m_planeA[ 0 ] ) * unusedPlaneCount );
    memset( m_planeB + unusedPlaneStart, 0, sizeof( m_planeB[ 0 ] ) * unusedPlaneCount );
    memset( m_planeC + unusedPlaneStart, 0, sizeof( m_planeC[ 0 ] ) * unusedPlaneCount );

#pragma TODO("Replace these with ArraySet")
    for ( uint32_t i=0; i<unusedPlaneCount; i++ )
    {
        (m_planeD + unusedPlaneStart)[i] = 1.0f;
    }
}

/// Explicitly set the far clip plane for this frustum.
///
/// @param[in] rPlane  Plane to set.
///
/// @see SetInfiniteFarClip()
void Helium::Simd::Frustum::SetFarClip( const Plane& rPlane )
{
    m_planeA[ PLANE_FAR ] = rPlane.GetElement( 0 );
    m_planeB[ PLANE_FAR ] = rPlane.GetElement( 1 );
    m_planeC[ PLANE_FAR ] = rPlane.GetElement( 2 );
    m_planeD[ PLANE_FAR ] = rPlane.GetElement( 3 );

    m_bInfiniteFarClip = false;
}

/// Explicitly clear out the far clip plane and set this frustum as having an infinite far clip distance.
///
/// @see SetFarClip()
void Helium::Simd::Frustum::SetInfiniteFarClip()
{
    m_planeA[ PLANE_FAR ] = 0.0f;
    m_planeB[ PLANE_FAR ] = 0.0f;
    m_planeC[ PLANE_FAR ] = 0.0f;
    m_planeD[ PLANE_FAR ] = 1.0f;

    m_bInfiniteFarClip = true;
}
