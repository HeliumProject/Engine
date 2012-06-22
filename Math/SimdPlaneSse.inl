#if HELIUM_SIMD_SSE

/// Constructor.
///
/// Initializes this plane directly with the specified plane equation coefficients.
///
/// @param[in] a  Plane equation coefficient multiplied by point x-coordinates (also the x-component of the plane
///               normal).
/// @param[in] b  Plane equation coefficient multiplied by point y-coordinates (also the y-component of the plane
///               normal).
/// @param[in] c  Plane equation coefficient multiplied by point z-coordinates (also the z-component of the plane
///               normal).
/// @param[in] d  Plane equation constant (also the negative distance of the plane from the origin along the
///               direction of its normal).
Helium::Simd::Plane::Plane( float32_t a, float32_t b, float32_t c, float32_t d )
{
    m_plane = _mm_set_ps( d, c, b, a );
}

/// Constructor.
///
/// Initializes this plane directly with each value in the given vector.  The vector x, y, z, and w components are
/// mapped directly to the plane a, b, c, and d coefficients, respectively.
///
/// @param[in] rVector  Vector containing the values with which to initialize this plane.
Helium::Simd::Plane::Plane( const Vector4& rVector )
    : m_plane( rVector.GetSimdVector() )
{
}

/// Constructor.
///
/// Initializes this plane directly with the values in the given SIMD vector.
///
/// @param[in] rVector  SIMD vector from which to initialize this plane.
Helium::Simd::Plane::Plane( const Register& rVector )
    : m_plane( rVector )
{
}

/// Set the plane element stored at the specified index.
///
/// Note that accessing individual elements within a plane can incur a performance penalty, especially on particular
/// platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Reference to the value stored at the specified element.
///
/// @see SetElement()
float32_t& Helium::Simd::Plane::GetElement( size_t index )
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< float32_t* >( &m_plane )[ index ];
}

/// Set the plane element stored at the specified index.
///
/// Note that accessing individual elements within a plane can incur a performance penalty, especially on particular
/// platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Value stored at the specified element.
///
/// @see SetElement()
float32_t Helium::Simd::Plane::GetElement( size_t index ) const
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< const float32_t* >( &m_plane )[ index ];
}

/// Set the plane element at the specified index.
///
/// Note that accessing individual elements within a plane can incur a performance penalty, especially on particular
/// platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to set (less than 4).
/// @param[in] value  Value to set.
///
/// @see GetElement()
void Helium::Simd::Plane::SetElement( size_t index, float32_t value )
{
    HELIUM_ASSERT( index < 4 );

    reinterpret_cast< float32_t* >( &m_plane )[ index ] = value;
}

/// Set this plane based on a vector normal to the plane and the distance of the plane from the origin.
///
/// @param[in] rNormal   Plane normal.
/// @param[in] distance  Distance of the plane from the origin along the plane normal, scaled by the normal
///                      magnitude.
void Helium::Simd::Plane::Set( const Vector3& rNormal, float32_t distance )
{
    Register distanceSplat = _mm_set_ps1( distance );

    HELIUM_SIMD_ALIGN_PRE const uint32_t distanceMaskValues[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register distanceMask = _mm_load_ps( reinterpret_cast< const float32_t* >( distanceMaskValues ) );

    m_plane = _mm_sub_ps(
        _mm_andnot_ps( distanceMask, rNormal.GetSimdVector() ),
        _mm_and_ps( distanceMask, distanceSplat ) );
}

/// Set this plane based on three points on the plane.  The plane normal is computed using the cross product of the
/// vector from the first point to the second and the vector from the first point to the third.
///
/// Note that the plane normal is normalized automatically using Vector3::Normalize() with the default epsilon
/// value.
///
/// @param[in] rPoint0  First point on the plane.
/// @param[in] rPoint1  Second point on the plane.
/// @param[in] rPoint2  Third point on the plane.
void Helium::Simd::Plane::Set( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 )
{
    Vector3 toPoint1, toPoint2;
    toPoint1.SubtractSet( rPoint1, rPoint0 );
    toPoint1.SubtractSet( rPoint2, rPoint0 );

    Vector3 normal;
    normal.CrossSet( toPoint1, toPoint2 );
    normal.Normalize();

    // Make sure the dot product of the plane normal and a point on the plane (Ax + By + Cz portion of the plane
    // equation, which equates to -D) is stored in the w-component of the dot product vector so we can mask and
    // apply it to proper location in the final coefficient vector.
    Register normalPointProduct = _mm_mul_ps( normal.GetSimdVector(), rPoint0.GetSimdVector() );
    Register productX = _mm_shuffle_ps( normalPointProduct, normalPointProduct, _MM_SHUFFLE( 0, 3, 2, 1 ) );
    Register productY = _mm_shuffle_ps( normalPointProduct, normalPointProduct, _MM_SHUFFLE( 1, 0, 3, 2 ) );
    Register productZ = _mm_shuffle_ps( normalPointProduct, normalPointProduct, _MM_SHUFFLE( 2, 1, 0, 3 ) );
    Register normalDotPoint = _mm_add_ps( _mm_add_ps( productX, productY ), productZ );

    HELIUM_SIMD_ALIGN_PRE const uint32_t distanceMaskValues[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register distanceMask = _mm_load_ps( reinterpret_cast< const float32_t* >( distanceMaskValues ) );

    m_plane = _mm_sub_ps(
        _mm_andnot_ps( distanceMask, normal.GetSimdVector() ),
        _mm_and_ps( distanceMask, normalDotPoint ) );
}

/// Apply the equation for this plane to the given point in order to compute its distance from this plane.
///
/// Note that the distance is scaled by the magnitude of the plane normal.  In order to get the actual distance of
/// a point from a plane, the plane must first be normalized.
///
/// @param[in] rPoint  Point to which the plane equation should be applied.
///
/// @return  Distance of the given point from this plane, scaled by the magnitude of the plane normal.
///
/// @see GetNormalized(), Normalize()
float32_t Helium::Simd::Plane::GetDistance( const Vector3& rPoint ) const
{
    Register productX = Simd::MultiplyF32( m_plane, rPoint.GetSimdVector() );
    Register productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
    Register productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );
    Register constant = _mm_shuffle_ps( m_plane, m_plane, _MM_SHUFFLE( 2, 1, 0, 3 ) );

    Register sum = Simd::AddF32( Simd::AddF32( Simd::AddF32( productX, productY ), productZ ), constant );

    return reinterpret_cast< const float32_t* >( &sum )[ 0 ];
}

/// Normalize this plane, with safety threshold checking.
///
/// If the magnitude of the plane normal is below the given epsilon, the normal will be set to a unit vector
/// pointing along the x-axis, and the D component will be set to zero.
///
/// @param[in] epsilon  Threshold at which to test for zero-length plane normals.
///
/// @see GetNormalized()
void Helium::Simd::Plane::Normalize( float32_t epsilon )
{
    epsilon *= epsilon;

    Register productX = Simd::MultiplyF32( m_plane, m_plane );
    Register productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
    Register productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

    Register magnitudeSquared = Simd::AddF32( Simd::AddF32( productX, productY ), productZ );
    magnitudeSquared = _mm_shuffle_ps( magnitudeSquared, magnitudeSquared, _MM_SHUFFLE( 0, 0, 0, 0 ) );

    Register epsilonVec = _mm_set_ps1( epsilon );

    Mask thresholdMask = Simd::LessF32( magnitudeSquared, epsilonVec );

    Register invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

    Register planeNormalized = Simd::MultiplyF32( m_plane, invMagnitude );
    Register planeFallback = _mm_set_ps( 0.0f, 0.0f, 0.0f, 1.0f );

    planeNormalized = Simd::AndNot( thresholdMask, planeNormalized );
    planeFallback = Simd::And( thresholdMask, planeFallback );

    m_plane = Simd::Or( planeNormalized, planeFallback );
}

/// Test whether each component in this plane is equal to the corresponding component in another plane within a
/// given threshold.
///
/// @param[in] rPlane   Plane.
/// @param[in] epsilon  Comparison threshold.
///
/// @return  True if this plane and the given plane are equal within the given threshold, false if not.
bool Helium::Simd::Plane::Equals( const Plane& rPlane, float32_t epsilon ) const
{
    epsilon *= epsilon;

    Register differenceSquared = Simd::SubtractF32( m_plane, rPlane.m_plane );
    differenceSquared = Simd::MultiplyF32( differenceSquared, differenceSquared );

    Register epsilonVec = _mm_set_ps1( epsilon );

    Register testResult = Simd::GreaterF32( differenceSquared, epsilonVec );
    testResult = Simd::Or( testResult, _mm_movehl_ps( testResult, testResult ) );
    testResult = Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

    return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
}

#endif  // HELIUM_SIMD_SSE
