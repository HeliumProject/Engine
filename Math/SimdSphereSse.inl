#if HELIUM_SIMD_SSE

/// Get the sphere element stored at the specified index.
///
/// The first three elements represent the sphere center, while the fourth element represents the sphere radius.
///
/// Note that accessing individual elements within a sphere can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Reference to the value stored at the specified element.
///
/// @see SetElement()
float32_t& Helium::Simd::Sphere::GetElement( size_t index )
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< float32_t* >( &m_centerRadius )[ index ];
}

/// Get the sphere element stored at the specified index.
///
/// The first three elements represent the sphere center, while the fourth element represents the sphere radius.
///
/// Note that accessing individual elements within a sphere can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Value stored at the specified element.
///
/// @see SetElement()
float32_t Helium::Simd::Sphere::GetElement( size_t index ) const
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< const float32_t* >( &m_centerRadius )[ index ];
}

/// Set the sphere element at the specified index.
///
/// The first three elements represent the sphere center, while the fourth element represents the sphere radius.
///
/// Note that accessing individual elements within a sphere can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to set (less than 4).
/// @param[in] value  Value to set.
///
/// @see GetElement()
void Helium::Simd::Sphere::SetElement( size_t index, float32_t value )
{
    HELIUM_ASSERT( index < 4 );

    reinterpret_cast< float32_t* >( &m_centerRadius )[ index ] = value;
}

/// Set the sphere center and radius.
///
/// @param[in] rCenter  Sphere center.
/// @param[in] radius   Sphere radius.
void Helium::Simd::Sphere::Set( const Vector3& rCenter, float32_t radius )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register radiusMaskVec = Simd::LoadAligned( radiusMask );

    Register centerVec = rCenter.GetSimdVector();
    Register radiusVec = Simd::SetSplatF32( radius );

    m_centerRadius = Simd::Select( centerVec, radiusVec, radiusMaskVec );
}

/// Set the sphere center and radius.
///
/// @param[in] centerX  X-coordinate of the sphere center.
/// @param[in] centerY  Y-coordinate of the sphere center.
/// @param[in] centerZ  Z-coordinate of the sphere center.
/// @param[in] radius   Sphere radius.
void Helium::Simd::Sphere::Set( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius )
{
    m_centerRadius = _mm_set_ps( radius, centerZ, centerY, centerX );
}

/// Set the sphere values based on those stored in a 4-component vector.
///
/// The sphere center is taken from the x, y, and z coordinates, while the radius is taken from the w coordinate.
///
/// @param[in] rVector  Vector containing the values to set.
void Helium::Simd::Sphere::Set( const Vector4& rVector )
{
    m_centerRadius = rVector.GetSimdVector();
}

/// Set this sphere to a sphere encompassing the given axis-aligned bounding box.
///
/// @param[in] rBox  Axis-aligned bounding box.
void Helium::Simd::Sphere::Set( const AaBox& rBox )
{
    Register halfVec = Simd::SetSplatF32( 0.5f );

    HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register radiusMaskVec = Simd::LoadAligned( radiusMask );

    Register boxMinVec = rBox.GetMinimum().GetSimdVector();
    Register boxMaxVec = rBox.GetMaximum().GetSimdVector();

    Register center = Simd::MultiplyF32( Simd::AddF32( boxMinVec, boxMaxVec ), halfVec );

    Register centerToExtent = Simd::SubtractF32( boxMaxVec, center );
    Register extentSquaredX = Simd::MultiplyF32( centerToExtent, centerToExtent );
    Register extentSquaredY = _mm_shuffle_ps( extentSquaredX, extentSquaredX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
    Register extentSquaredZ = _mm_shuffle_ps( extentSquaredX, extentSquaredX, _MM_SHUFFLE( 1, 0, 3, 2 ) );
    Register radius =
        Simd::SqrtF32( Simd::AddF32( Simd::AddF32( extentSquaredX, extentSquaredY ), extentSquaredZ ) );
    radius = _mm_shuffle_ps( radius, radius, _MM_SHUFFLE( 0, 0, 0, 0 ) );

    m_centerRadius = Simd::Select( center, radius, radiusMaskVec );
}

/// Set the sphere center.
///
/// @param[in] rCenter  Sphere center.
///
/// @see Translate()
void Helium::Simd::Sphere::SetCenter( const Vector3& rCenter )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register radiusMaskVec = Simd::LoadAligned( radiusMask );

    m_centerRadius = Simd::Select( rCenter.GetSimdVector(), m_centerRadius, radiusMaskVec );
}

/// Translate the sphere center.
///
/// @param[in] rOffset  Amount by which to translate.
///
/// @see SetCenter()
void Helium::Simd::Sphere::Translate( const Vector3& rOffset )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t centerMask[] HELIUM_SIMD_ALIGN_POST = { 0xffffffff, 0xffffffff, 0xffffffff, 0x0 };
    Register centerMaskVec = Simd::LoadAligned( centerMask );

    Register offsetVec = Simd::And( rOffset.GetSimdVector(), centerMaskVec );

    m_centerRadius = Simd::AddF32( m_centerRadius, offsetVec );
}

/// Set the sphere radius.
///
/// @param[in] radius  Sphere radius.
///
/// @see Scale()
void Helium::Simd::Sphere::SetRadius( float32_t radius )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register radiusMaskVec = Simd::LoadAligned( radiusMask );

    Register radiusVec = Simd::SetSplatF32( radius );

    m_centerRadius = Simd::Select( m_centerRadius, radiusVec, radiusMaskVec );
}

/// Scale the sphere radius.
///
/// @param[in] scale  Amount by which to scale the radius.
///
/// @see SetRadius()
void Helium::Simd::Sphere::Scale( float32_t scale )
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
    Register radiusMaskVec = Simd::LoadAligned( radiusMask );

    Register onesVec = Simd::SetSplatF32( 1.0f );

    Register scaleVec = Simd::Select( onesVec, Simd::SetSplatF32( scale ), radiusMaskVec );

    m_centerRadius = Simd::MultiplyF32( m_centerRadius, scaleVec );
}

/// Test whether this sphere and the given sphere intersect.
///
/// @param[in] rSphere    Sphere against which to test.
/// @param[in] threshold  Intersection threshold (higher values increase the distance at which intersection tests
///                       will succeed).
///
/// @return  True if the two spheres intersect, false if not.
bool Helium::Simd::Sphere::Intersects( const Sphere& rSphere, float32_t threshold ) const
{
    Register toSphere = Simd::SubtractF32( rSphere.m_centerRadius, m_centerRadius );
    Register productX = Simd::MultiplyF32( toSphere, toSphere );
    Register productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
    Register productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

    Register distanceSquared = Simd::AddF32( Simd::AddF32( productX, productY ), productZ );

    Register thresholdVec = _mm_set_ss( threshold );
    Register radii = Simd::AddF32( m_centerRadius, rSphere.m_centerRadius );
    radii = _mm_shuffle_ps( radii, radii, _MM_SHUFFLE( 3, 3, 3, 3 ) );
    radii = Simd::AddF32( radii, thresholdVec );

    Register compareResult = Simd::LessEqualsF32( distanceSquared, Simd::MultiplyF32( radii, radii ) );
    int resultMask = _mm_movemask_ps( compareResult );

    return ( ( resultMask & 0x1 ) != 0 );
}

#endif  // HELIUM_SIMD_SSE
