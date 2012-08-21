/// Constructor.
///
/// This creates a plane with all components uninitialized.  Values should be assigned before use.
Helium::Simd::PlaneSoa::PlaneSoa()
{
}

/// Constructor.
///
/// Initializes this plane set directly with the specified sets of plane equation coefficients.
///
/// @param[in] rA  Plane equation coefficients multiplied by point x-coordinates (also the x-components of the plane
///                normals).
/// @param[in] rB  Plane equation coefficients multiplied by point y-coordinates (also the y-components of the plane
///                normals).
/// @param[in] rC  Plane equation coefficients multiplied by point z-coordinates (also the z-components of the plane
///                normals).
/// @param[in] rD  Plane equation constants (also the negative distances of each plane from the origin along the
///                directions of their normals).
Helium::Simd::PlaneSoa::PlaneSoa( const Register& rA, const Register& rB, const Register& rC, const Register& rD )
    : m_a( rA )
    , m_b( rB )
    , m_c( rC )
    , m_d( rD )
{
}

/// Constructor.
///
/// Initializes this plane set directly with the specified sets of plane equation coefficients.  This will fully
/// load the SIMD vectors for each component from the given addresses.
///
/// @param[in] pA  A components (must be SIMD aligned).
/// @param[in] pB  B components (must be SIMD aligned).
/// @param[in] pC  C components (must be SIMD aligned).
/// @param[in] pD  D components (must be SIMD aligned).
Helium::Simd::PlaneSoa::PlaneSoa( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
{
    Load( pA, pB, pC, pD );
}

/// Constructor.
///
/// Initializes this plane set based on a set of vectors normal to each plane and the distances of each plane from
/// the origin.
///
/// @param[in] rNormal    Plane normals.
/// @param[in] rDistance  Distances of each plane from the origin along the plane normals, scaled by the magnitudes
///                       of each normal.
Helium::Simd::PlaneSoa::PlaneSoa( const Vector3Soa& rNormal, const Register& rDistance )
{
    Set( rNormal, rDistance );
}

/// Constructor.
///
/// Initializes this plane set based on three sets of points for each plane.  The plane normal are computed using
/// the cross products of the vectors from the first set of points to the second and the vector- from the first set
/// of points to the third.
///
/// Note that the plane normals are normalized automatically using Vector3Soa::Normalize() with the default epsilon
/// value.
///
/// @param[in] rPoint0  First set of points on each plane.
/// @param[in] rPoint1  Second set of points on each plane.
/// @param[in] rPoint2  Third set of points on each plane.
Helium::Simd::PlaneSoa::PlaneSoa( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 )
{
    Set( rPoint0, rPoint1, rPoint2 );
}

/// Constructor.
///
/// Initializes this plane set directly with each value in the given set of vectors.  Vector x, y, z, and w
/// components are mapped directly to plane a, b, c, and d coefficients, respectively.
///
/// @param[in] rVector  Vector containing the values with which to initialize each plane.
Helium::Simd::PlaneSoa::PlaneSoa( const Vector4Soa& rVector )
    : m_a( rVector.m_x )
    , m_b( rVector.m_y )
    , m_c( rVector.m_z )
    , m_d( rVector.m_w )
{
}

/// Constructor.
///
/// This will initialize this plane by splatting each component of the given plane across a full SIMD vector.
///
/// @param[in] rPlane  Plane from which to initialize this plane.
Helium::Simd::PlaneSoa::PlaneSoa( const Plane& rPlane )
{
    Splat( rPlane );
}

/// Fully load the SIMD vectors for each plane component.
///
/// @param[in] pA  A components (must be SIMD aligned).
/// @param[in] pB  B components (must be SIMD aligned).
/// @param[in] pC  C components (must be SIMD aligned).
/// @param[in] pD  D components (must be SIMD aligned).
void Helium::Simd::PlaneSoa::Load( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
{
    m_a = Simd::LoadAligned( pA );
    m_b = Simd::LoadAligned( pB );
    m_c = Simd::LoadAligned( pC );
    m_d = Simd::LoadAligned( pD );
}

/// Load 4 single-precision floating-point values for each plane component, splatting the values to fill.
///
/// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
/// same effect as Load().
///
/// @param[in] pA  A components (must be aligned to a 16-byte boundary).
/// @param[in] pB  B components (must be aligned to a 16-byte boundary).
/// @param[in] pC  C components (must be aligned to a 16-byte boundary).
/// @param[in] pD  D components (must be aligned to a 16-byte boundary).
void Helium::Simd::PlaneSoa::Load4Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
{
    m_a = Simd::LoadSplat128( pA );
    m_b = Simd::LoadSplat128( pB );
    m_c = Simd::LoadSplat128( pC );
    m_d = Simd::LoadSplat128( pD );
}

/// Load 1 single-precision floating-point value for each plane component, splatting the value to fill.
///
/// @param[in] pA  A components (must be aligned to a 4-byte boundary).
/// @param[in] pB  B components (must be aligned to a 4-byte boundary).
/// @param[in] pC  C components (must be aligned to a 4-byte boundary).
/// @param[in] pD  D components (must be aligned to a 4-byte boundary).
void Helium::Simd::PlaneSoa::Load1Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
{
    m_a = Simd::LoadSplat32( pA );
    m_b = Simd::LoadSplat32( pB );
    m_c = Simd::LoadSplat32( pC );
    m_d = Simd::LoadSplat32( pD );
}

/// Fully store the SIMD vectors from each vector component into memory.
///
/// @param[out] pA  A components (must be SIMD aligned).
/// @param[out] pB  B components (must be SIMD aligned).
/// @param[out] pC  C components (must be SIMD aligned).
/// @param[out] pD  D components (must be SIMD aligned).
void Helium::Simd::PlaneSoa::Store( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
{
    Simd::StoreAligned( pA, m_a );
    Simd::StoreAligned( pB, m_b );
    Simd::StoreAligned( pC, m_c );
    Simd::StoreAligned( pD, m_d );
}

/// Store the lowest 4 single-precision floating-point values from each plane component into memory.
///
/// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
/// same effect as Store().
///
/// @param[out] pA  A components (must be aligned to a 16-byte boundary).
/// @param[out] pB  B components (must be aligned to a 16-byte boundary).
/// @param[out] pC  C components (must be aligned to a 16-byte boundary).
/// @param[out] pD  D components (must be aligned to a 16-byte boundary).
void Helium::Simd::PlaneSoa::Store4( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
{
    Simd::Store128( pA, m_a );
    Simd::Store128( pB, m_b );
    Simd::Store128( pC, m_c );
    Simd::Store128( pD, m_d );
}

/// Store the lowest single-precision floating-point value from each vector component into memory.
///
/// @param[out] pA  A components (must be aligned to a 4-byte boundary).
/// @param[out] pB  B components (must be aligned to a 4-byte boundary).
/// @param[out] pC  C components (must be aligned to a 4-byte boundary).
/// @param[out] pD  D components (must be aligned to a 4-byte boundary).
void Helium::Simd::PlaneSoa::Store1( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
{
    Simd::Store32( pA, m_a );
    Simd::Store32( pB, m_b );
    Simd::Store32( pC, m_c );
    Simd::Store32( pD, m_d );
}

/// Set this plane set based on a set of vectors normal to each plane and the distances of each plane from the
/// origin.
///
/// @param[in] rNormal    Plane normals.
/// @param[in] rDistance  Distances of each plane from the origin along the plane normals, scaled by the magnitudes
///                       of each normal.
void Helium::Simd::PlaneSoa::Set( const Vector3Soa& rNormal, const Register& rDistance )
{
    Register signFlip = Simd::SetSplatU32( 0x80000000 );

    m_a = rNormal.m_x;
    m_b = rNormal.m_y;
    m_c = rNormal.m_z;

    m_d = Simd::Xor( rDistance, signFlip );
}

/// Constructor.
///
/// Set this plane set based on three sets of points for each plane.  The plane normal are computed using the cross
/// products of the vectors from the first set of points to the second and the vector- from the first set of points
/// to the third.
///
/// Note that the plane normals are normalized automatically using Vector3Soa::Normalize() with the default epsilon
/// value.
///
/// @param[in] rPoint0  First set of points on each plane.
/// @param[in] rPoint1  Second set of points on each plane.
/// @param[in] rPoint2  Third set of points on each plane.
void Helium::Simd::PlaneSoa::Set( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 )
{
    Vector3Soa toPoint1, toPoint2;
    toPoint1.SubtractSet( rPoint1, rPoint0 );
    toPoint2.SubtractSet( rPoint2, rPoint0 );

    Vector3Soa normal;
    normal.CrossSet( toPoint1, toPoint2 );
    normal.Normalize();

    Register normalDotPoint = normal.Dot( rPoint0 );

    Set( normal, normalDotPoint );
}

/// Apply the equations for each plane to the given set of points in order to compute their distances from each
/// plane.
///
/// Note that the distances are scaled by the magnitudes of each plane normal.  In order to get the actual distance
/// of a point from a plane, the plane must first be normalized.
///
/// @param[in] rPoint  Points to which the plane equations should be applied.
///
/// @return  Distances of each point from each plane, scaled by the magnitudes of each plane normal.
///
/// @see GetNormalized(), Normalize()
Helium::Simd::Register Helium::Simd::PlaneSoa::GetDistance( const Vector3Soa& rPoint ) const
{
#if HELIUM_SIMD_BUILTIN_MULTIPLY_ADD
    Register distance = Simd::MultiplyAddF32( m_a, rPoint.m_x, m_d );
    distance = Simd::MultiplyAddF32( m_b, rPoint.m_y, distance );
    distance = Simd::MultiplyAddF32( m_c, rPoint.m_z, distance );
#else
    Register normalPointX = Simd::MultiplyF32( m_a, rPoint.m_x );
    Register normalPointY = Simd::MultiplyF32( m_b, rPoint.m_y );
    Register normalPointZ = Simd::MultiplyF32( m_c, rPoint.m_z );

    Register distance = Simd::AddF32( normalPointX, normalPointY );
    distance = Simd::AddF32( normalPointZ, distance );
    distance = Simd::AddF32( m_d, distance );
#endif

    return distance;
}

/// Get a copy of this plane set with each plane normal normalized and the D components adjusted accordingly, with
/// safety threshold checking.
///
/// If any of the magnitudes of each plane normal are below the corresponding epsilon value, the given plane normal
/// will be set to a unit vector pointing along the x-axis, and the D component will be set to zero.
///
/// @param[in] rEpsilon  Thresholds at which to test for zero-length plane normals.
///
/// @return  Normalized copy of this plane set.
///
/// @see Normalize()
Helium::Simd::PlaneSoa Helium::Simd::PlaneSoa::GetNormalized( const Register& rEpsilon ) const
{
    PlaneSoa result = *this;
    result.Normalize( rEpsilon );

    return result;
}

/// Normalize this plane set, with safety threshold checking.
///
/// If any of the magnitudes of each plane normal are below the corresponding epsilon value, the given plane normal
/// will be set to a unit vector pointing along the x-axis, and the D component will be set to zero.
///
/// @param[in] rEpsilon  Thresholds at which to test for zero-length plane normals.
///
/// @see GetNormalized()
void Helium::Simd::PlaneSoa::Normalize( const Register& rEpsilon )
{
    Register epsilonSquared = Simd::MultiplyF32( rEpsilon, rEpsilon );

    Register magnitudeSquared = Simd::MultiplyF32( m_a, m_a );
#if HELIUM_SIMD_BUILTIN_MULTIPLY_ADD
    magnitudeSquared = Simd::MultiplyAddF32( m_b, m_b, magnitudeSquared );
    magnitudeSquared = Simd::MultiplyAddF32( m_c, m_c, magnitudeSquared );
#else
    Register bSquared = Simd::MultiplyF32( m_b, m_b );
    Register cSquared = Simd::MultiplyF32( m_c, m_c );

    magnitudeSquared = Simd::AddF32( magnitudeSquared, bSquared );
    magnitudeSquared = Simd::AddF32( magnitudeSquared, cSquared );
#endif

    Mask thresholdMask = Simd::GreaterEqualsF32( magnitudeSquared, epsilonSquared );

    Register invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

    Register normalizedA = Simd::MultiplyF32( m_a, invMagnitude );
    Register normalizedB = Simd::MultiplyF32( m_b, invMagnitude );
    Register normalizedC = Simd::MultiplyF32( m_c, invMagnitude );
    Register normalizedD = Simd::MultiplyF32( m_d, invMagnitude );

    Register oneVec = Simd::SetSplatF32( 1.0f );

    m_a = Simd::Select( oneVec, normalizedA, thresholdMask );
    m_b = Simd::And( normalizedB, thresholdMask );
    m_c = Simd::And( normalizedC, thresholdMask );
    m_d = Simd::And( normalizedD, thresholdMask );
}

/// Test whether each component in this plane is equal to the corresponding component in another plane within a
/// given threshold.
///
/// @param[in] rPlane    Plane.
/// @param[in] rEpsilon  Comparison threshold.
///
/// @return  SIMD mask with bits set for planes that are equal within the given threshold.
Helium::Simd::Mask Helium::Simd::PlaneSoa::Equals( const PlaneSoa& rPlane, const Register& rEpsilon ) const
{
    Register absMask = Simd::SetSplatU32( 0x7fffffff );

    Register differenceA = Simd::SubtractF32( m_a, rPlane.m_a );
    Register differenceB = Simd::SubtractF32( m_b, rPlane.m_b );
    Register differenceC = Simd::SubtractF32( m_c, rPlane.m_c );
    Register differenceD = Simd::SubtractF32( m_d, rPlane.m_d );

    differenceA = Simd::And( differenceA, absMask );
    differenceB = Simd::And( differenceB, absMask );
    differenceC = Simd::And( differenceC, absMask );
    differenceD = Simd::And( differenceD, absMask );

    Mask thresholdMaskA = Simd::LessEqualsF32( differenceA, rEpsilon );
    Mask thresholdMaskB = Simd::LessEqualsF32( differenceB, rEpsilon );
    Mask thresholdMaskC = Simd::LessEqualsF32( differenceC, rEpsilon );
    Mask thresholdMaskD = Simd::LessEqualsF32( differenceD, rEpsilon );

    return Simd::MaskAnd(
        Simd::MaskAnd( Simd::MaskAnd( thresholdMaskA, thresholdMaskB ), thresholdMaskC ),
        thresholdMaskD );
}

/// Test whether any component in this plane is not equal to the corresponding component in another plane within a
/// given threshold.
///
/// @param[in] rPlane    Plane.
/// @param[in] rEpsilon  Comparison threshold.
///
/// @return  SIMD mask with bits set for planes that are not equal within the given threshold.
Helium::Simd::Mask Helium::Simd::PlaneSoa::NotEquals( const PlaneSoa& rPlane, const Register& rEpsilon ) const
{
    Register absMask = Simd::SetSplatU32( 0x7fffffff );

    Register differenceA = Simd::SubtractF32( m_a, rPlane.m_a );
    Register differenceB = Simd::SubtractF32( m_b, rPlane.m_b );
    Register differenceC = Simd::SubtractF32( m_c, rPlane.m_c );
    Register differenceD = Simd::SubtractF32( m_d, rPlane.m_d );

    differenceA = Simd::And( differenceA, absMask );
    differenceB = Simd::And( differenceB, absMask );
    differenceC = Simd::And( differenceC, absMask );
    differenceD = Simd::And( differenceD, absMask );

    Mask thresholdMaskA = Simd::GreaterF32( differenceA, rEpsilon );
    Mask thresholdMaskB = Simd::GreaterF32( differenceB, rEpsilon );
    Mask thresholdMaskC = Simd::GreaterF32( differenceC, rEpsilon );
    Mask thresholdMaskD = Simd::GreaterF32( differenceD, rEpsilon );

    return Simd::MaskOr(
        Simd::MaskOr( Simd::MaskOr( thresholdMaskA, thresholdMaskB ), thresholdMaskC ),
        thresholdMaskD );
}

/// Test whether each component in this plane is equal to the corresponding component in another plane within a
/// default threshold.
///
/// @param[in] rPlane  Plane.
///
/// @return  SIMD mask with bits set for planes that are equal within the given threshold.
Helium::Simd::Mask Helium::Simd::PlaneSoa::operator==( const PlaneSoa& rPlane ) const
{
    return Equals( rPlane );
}

/// Test whether any component in this plane is not equal to the corresponding component in another plane within a
/// default threshold.
///
/// @param[in] rPlane  Plane.
///
/// @return  SIMD mask with bits set for planes that are not equal within the given threshold.
Helium::Simd::Mask Helium::Simd::PlaneSoa::operator!=( const PlaneSoa& rPlane ) const
{
    return NotEquals( rPlane );
}
