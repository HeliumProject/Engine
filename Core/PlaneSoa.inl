//----------------------------------------------------------------------------------------------------------------------
// PlaneSoa.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a plane with all components uninitialized.  Values should be assigned before use.
    PlaneSoa::PlaneSoa()
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
    PlaneSoa::PlaneSoa( const Helium::SimdVector& rA, const Helium::SimdVector& rB, const Helium::SimdVector& rC, const Helium::SimdVector& rD )
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
    PlaneSoa::PlaneSoa( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
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
    PlaneSoa::PlaneSoa( const Vector3Soa& rNormal, const Helium::SimdVector& rDistance )
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
    PlaneSoa::PlaneSoa( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 )
    {
        Set( rPoint0, rPoint1, rPoint2 );
    }

    /// Constructor.
    ///
    /// Initializes this plane set directly with each value in the given set of vectors.  Vector x, y, z, and w
    /// components are mapped directly to plane a, b, c, and d coefficients, respectively.
    ///
    /// @param[in] rVector  Vector containing the values with which to initialize each plane.
    PlaneSoa::PlaneSoa( const Vector4Soa& rVector )
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
    PlaneSoa::PlaneSoa( const Plane& rPlane )
    {
        Splat( rPlane );
    }

    /// Fully load the SIMD vectors for each plane component.
    ///
    /// @param[in] pA  A components (must be SIMD aligned).
    /// @param[in] pB  B components (must be SIMD aligned).
    /// @param[in] pC  C components (must be SIMD aligned).
    /// @param[in] pD  D components (must be SIMD aligned).
    void PlaneSoa::Load( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
    {
        m_a = Helium::Simd::LoadAligned( pA );
        m_b = Helium::Simd::LoadAligned( pB );
        m_c = Helium::Simd::LoadAligned( pC );
        m_d = Helium::Simd::LoadAligned( pD );
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
    void PlaneSoa::Load4Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
    {
        m_a = Helium::Simd::LoadSplat128( pA );
        m_b = Helium::Simd::LoadSplat128( pB );
        m_c = Helium::Simd::LoadSplat128( pC );
        m_d = Helium::Simd::LoadSplat128( pD );
    }

    /// Load 1 single-precision floating-point value for each plane component, splatting the value to fill.
    ///
    /// @param[in] pA  A components (must be aligned to a 4-byte boundary).
    /// @param[in] pB  B components (must be aligned to a 4-byte boundary).
    /// @param[in] pC  C components (must be aligned to a 4-byte boundary).
    /// @param[in] pD  D components (must be aligned to a 4-byte boundary).
    void PlaneSoa::Load1Splat( const float32_t* pA, const float32_t* pB, const float32_t* pC, const float32_t* pD )
    {
        m_a = Helium::Simd::LoadSplat32( pA );
        m_b = Helium::Simd::LoadSplat32( pB );
        m_c = Helium::Simd::LoadSplat32( pC );
        m_d = Helium::Simd::LoadSplat32( pD );
    }

    /// Fully store the SIMD vectors from each vector component into memory.
    ///
    /// @param[out] pA  A components (must be SIMD aligned).
    /// @param[out] pB  B components (must be SIMD aligned).
    /// @param[out] pC  C components (must be SIMD aligned).
    /// @param[out] pD  D components (must be SIMD aligned).
    void PlaneSoa::Store( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
    {
        Helium::Simd::StoreAligned( pA, m_a );
        Helium::Simd::StoreAligned( pB, m_b );
        Helium::Simd::StoreAligned( pC, m_c );
        Helium::Simd::StoreAligned( pD, m_d );
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
    void PlaneSoa::Store4( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
    {
        Helium::Simd::Store128( pA, m_a );
        Helium::Simd::Store128( pB, m_b );
        Helium::Simd::Store128( pC, m_c );
        Helium::Simd::Store128( pD, m_d );
    }

    /// Store the lowest single-precision floating-point value from each vector component into memory.
    ///
    /// @param[out] pA  A components (must be aligned to a 4-byte boundary).
    /// @param[out] pB  B components (must be aligned to a 4-byte boundary).
    /// @param[out] pC  C components (must be aligned to a 4-byte boundary).
    /// @param[out] pD  D components (must be aligned to a 4-byte boundary).
    void PlaneSoa::Store1( float32_t* pA, float32_t* pB, float32_t* pC, float32_t* pD ) const
    {
        Helium::Simd::Store32( pA, m_a );
        Helium::Simd::Store32( pB, m_b );
        Helium::Simd::Store32( pC, m_c );
        Helium::Simd::Store32( pD, m_d );
    }

    /// Set this plane set based on a set of vectors normal to each plane and the distances of each plane from the
    /// origin.
    ///
    /// @param[in] rNormal    Plane normals.
    /// @param[in] rDistance  Distances of each plane from the origin along the plane normals, scaled by the magnitudes
    ///                       of each normal.
    void PlaneSoa::Set( const Vector3Soa& rNormal, const Helium::SimdVector& rDistance )
    {
        Helium::SimdVector signFlip = Helium::Simd::SetSplatU32( 0x80000000 );

        m_a = rNormal.m_x;
        m_b = rNormal.m_y;
        m_c = rNormal.m_z;

        m_d = Helium::Simd::Xor( rDistance, signFlip );
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
    void PlaneSoa::Set( const Vector3Soa& rPoint0, const Vector3Soa& rPoint1, const Vector3Soa& rPoint2 )
    {
        Vector3Soa toPoint1, toPoint2;
        toPoint1.SubtractSet( rPoint1, rPoint0 );
        toPoint2.SubtractSet( rPoint2, rPoint0 );

        Vector3Soa normal;
        normal.CrossSet( toPoint1, toPoint2 );
        normal.Normalize();

        Helium::SimdVector normalDotPoint = normal.Dot( rPoint0 );

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
    Helium::SimdVector PlaneSoa::GetDistance( const Vector3Soa& rPoint ) const
    {
#if HELIUM_SIMD_BUILTIN_MULTIPLY_ADD
        Helium::SimdVector distance = Helium::Simd::MultiplyAddF32( m_a, rPoint.m_x, m_d );
        distance = Helium::Simd::MultiplyAddF32( m_b, rPoint.m_y, distance );
        distance = Helium::Simd::MultiplyAddF32( m_c, rPoint.m_z, distance );
#else
        Helium::SimdVector normalPointX = Helium::Simd::MultiplyF32( m_a, rPoint.m_x );
        Helium::SimdVector normalPointY = Helium::Simd::MultiplyF32( m_b, rPoint.m_y );
        Helium::SimdVector normalPointZ = Helium::Simd::MultiplyF32( m_c, rPoint.m_z );

        Helium::SimdVector distance = Helium::Simd::AddF32( normalPointX, normalPointY );
        distance = Helium::Simd::AddF32( normalPointZ, distance );
        distance = Helium::Simd::AddF32( m_d, distance );
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
    PlaneSoa PlaneSoa::GetNormalized( const Helium::SimdVector& rEpsilon ) const
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
    void PlaneSoa::Normalize( const Helium::SimdVector& rEpsilon )
    {
        Helium::SimdVector epsilonSquared = Helium::Simd::MultiplyF32( rEpsilon, rEpsilon );

        Helium::SimdVector magnitudeSquared = Helium::Simd::MultiplyF32( m_a, m_a );
#if HELIUM_SIMD_BUILTIN_MULTIPLY_ADD
        magnitudeSquared = Helium::Simd::MultiplyAddF32( m_b, m_b, magnitudeSquared );
        magnitudeSquared = Helium::Simd::MultiplyAddF32( m_c, m_c, magnitudeSquared );
#else
        Helium::SimdVector bSquared = Helium::Simd::MultiplyF32( m_b, m_b );
        Helium::SimdVector cSquared = Helium::Simd::MultiplyF32( m_c, m_c );

        magnitudeSquared = Helium::Simd::AddF32( magnitudeSquared, bSquared );
        magnitudeSquared = Helium::Simd::AddF32( magnitudeSquared, cSquared );
#endif

        Helium::SimdMask thresholdMask = Helium::Simd::GreaterEqualsF32( magnitudeSquared, epsilonSquared );

        Helium::SimdVector invMagnitude = Helium::Simd::InverseSqrtF32( magnitudeSquared );

        Helium::SimdVector normalizedA = Helium::Simd::MultiplyF32( m_a, invMagnitude );
        Helium::SimdVector normalizedB = Helium::Simd::MultiplyF32( m_b, invMagnitude );
        Helium::SimdVector normalizedC = Helium::Simd::MultiplyF32( m_c, invMagnitude );
        Helium::SimdVector normalizedD = Helium::Simd::MultiplyF32( m_d, invMagnitude );

        Helium::SimdVector oneVec = Helium::Simd::SetSplatF32( 1.0f );

        m_a = Helium::Simd::Select( oneVec, normalizedA, thresholdMask );
        m_b = Helium::Simd::And( normalizedB, thresholdMask );
        m_c = Helium::Simd::And( normalizedC, thresholdMask );
        m_d = Helium::Simd::And( normalizedD, thresholdMask );
    }

    /// Test whether each component in this plane is equal to the corresponding component in another plane within a
    /// given threshold.
    ///
    /// @param[in] rPlane    Plane.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for planes that are equal within the given threshold.
    Helium::SimdMask PlaneSoa::Equals( const PlaneSoa& rPlane, const Helium::SimdVector& rEpsilon ) const
    {
        Helium::SimdVector absMask = Helium::Simd::SetSplatU32( 0x7fffffff );

        Helium::SimdVector differenceA = Helium::Simd::SubtractF32( m_a, rPlane.m_a );
        Helium::SimdVector differenceB = Helium::Simd::SubtractF32( m_b, rPlane.m_b );
        Helium::SimdVector differenceC = Helium::Simd::SubtractF32( m_c, rPlane.m_c );
        Helium::SimdVector differenceD = Helium::Simd::SubtractF32( m_d, rPlane.m_d );

        differenceA = Helium::Simd::And( differenceA, absMask );
        differenceB = Helium::Simd::And( differenceB, absMask );
        differenceC = Helium::Simd::And( differenceC, absMask );
        differenceD = Helium::Simd::And( differenceD, absMask );

        Helium::SimdMask thresholdMaskA = Helium::Simd::LessEqualsF32( differenceA, rEpsilon );
        Helium::SimdMask thresholdMaskB = Helium::Simd::LessEqualsF32( differenceB, rEpsilon );
        Helium::SimdMask thresholdMaskC = Helium::Simd::LessEqualsF32( differenceC, rEpsilon );
        Helium::SimdMask thresholdMaskD = Helium::Simd::LessEqualsF32( differenceD, rEpsilon );

        return Helium::Simd::MaskAnd(
            Helium::Simd::MaskAnd( Helium::Simd::MaskAnd( thresholdMaskA, thresholdMaskB ), thresholdMaskC ),
            thresholdMaskD );
    }

    /// Test whether any component in this plane is not equal to the corresponding component in another plane within a
    /// given threshold.
    ///
    /// @param[in] rPlane    Plane.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for planes that are not equal within the given threshold.
    Helium::SimdMask PlaneSoa::NotEquals( const PlaneSoa& rPlane, const Helium::SimdVector& rEpsilon ) const
    {
        Helium::SimdVector absMask = Helium::Simd::SetSplatU32( 0x7fffffff );

        Helium::SimdVector differenceA = Helium::Simd::SubtractF32( m_a, rPlane.m_a );
        Helium::SimdVector differenceB = Helium::Simd::SubtractF32( m_b, rPlane.m_b );
        Helium::SimdVector differenceC = Helium::Simd::SubtractF32( m_c, rPlane.m_c );
        Helium::SimdVector differenceD = Helium::Simd::SubtractF32( m_d, rPlane.m_d );

        differenceA = Helium::Simd::And( differenceA, absMask );
        differenceB = Helium::Simd::And( differenceB, absMask );
        differenceC = Helium::Simd::And( differenceC, absMask );
        differenceD = Helium::Simd::And( differenceD, absMask );

        Helium::SimdMask thresholdMaskA = Helium::Simd::GreaterF32( differenceA, rEpsilon );
        Helium::SimdMask thresholdMaskB = Helium::Simd::GreaterF32( differenceB, rEpsilon );
        Helium::SimdMask thresholdMaskC = Helium::Simd::GreaterF32( differenceC, rEpsilon );
        Helium::SimdMask thresholdMaskD = Helium::Simd::GreaterF32( differenceD, rEpsilon );

        return Helium::Simd::MaskOr(
            Helium::Simd::MaskOr( Helium::Simd::MaskOr( thresholdMaskA, thresholdMaskB ), thresholdMaskC ),
            thresholdMaskD );
    }

    /// Test whether each component in this plane is equal to the corresponding component in another plane within a
    /// default threshold.
    ///
    /// @param[in] rPlane  Plane.
    ///
    /// @return  SIMD mask with bits set for planes that are equal within the given threshold.
    Helium::SimdMask PlaneSoa::operator==( const PlaneSoa& rPlane ) const
    {
        return Equals( rPlane );
    }

    /// Test whether any component in this plane is not equal to the corresponding component in another plane within a
    /// default threshold.
    ///
    /// @param[in] rPlane  Plane.
    ///
    /// @return  SIMD mask with bits set for planes that are not equal within the given threshold.
    Helium::SimdMask PlaneSoa::operator!=( const PlaneSoa& rPlane ) const
    {
        return NotEquals( rPlane );
    }
}
