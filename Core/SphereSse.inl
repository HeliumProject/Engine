//----------------------------------------------------------------------------------------------------------------------
// SphereSse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if HELIUM_SIMD_SSE

namespace Lunar
{
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
    float32_t& Sphere::GetElement( size_t index )
    {
        L_ASSERT( index < 4 );

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
    float32_t Sphere::GetElement( size_t index ) const
    {
        L_ASSERT( index < 4 );

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
    void Sphere::SetElement( size_t index, float32_t value )
    {
        L_ASSERT( index < 4 );

        reinterpret_cast< float32_t* >( &m_centerRadius )[ index ] = value;
    }

    /// Set the sphere center and radius.
    ///
    /// @param[in] rCenter  Sphere center.
    /// @param[in] radius   Sphere radius.
    void Sphere::Set( const Vector3& rCenter, float32_t radius )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
        Helium::SimdVector radiusMaskVec = Helium::Simd::LoadAligned( radiusMask );

        Helium::SimdVector centerVec = rCenter.GetSimdVector();
        Helium::SimdVector radiusVec = Helium::Simd::SetSplatF32( radius );

        m_centerRadius = Helium::Simd::Select( centerVec, radiusVec, radiusMaskVec );
    }

    /// Set the sphere center and radius.
    ///
    /// @param[in] centerX  X-coordinate of the sphere center.
    /// @param[in] centerY  Y-coordinate of the sphere center.
    /// @param[in] centerZ  Z-coordinate of the sphere center.
    /// @param[in] radius   Sphere radius.
    void Sphere::Set( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius )
    {
        m_centerRadius = _mm_set_ps( radius, centerZ, centerY, centerX );
    }

    /// Set the sphere values based on those stored in a 4-component vector.
    ///
    /// The sphere center is taken from the x, y, and z coordinates, while the radius is taken from the w coordinate.
    ///
    /// @param[in] rVector  Vector containing the values to set.
    void Sphere::Set( const Vector4& rVector )
    {
        m_centerRadius = rVector.GetSimdVector();
    }

    /// Set this sphere to a sphere encompassing the given axis-aligned bounding box.
    ///
    /// @param[in] rBox  Axis-aligned bounding box.
    void Sphere::Set( const AaBox& rBox )
    {
        Helium::SimdVector halfVec = Helium::Simd::SetSplatF32( 0.5f );

        HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
        Helium::SimdVector radiusMaskVec = Helium::Simd::LoadAligned( radiusMask );

        Helium::SimdVector boxMinVec = rBox.GetMinimum().GetSimdVector();
        Helium::SimdVector boxMaxVec = rBox.GetMaximum().GetSimdVector();

        Helium::SimdVector center = Helium::Simd::MultiplyF32( Helium::Simd::AddF32( boxMinVec, boxMaxVec ), halfVec );

        Helium::SimdVector centerToExtent = Helium::Simd::SubtractF32( boxMaxVec, center );
        Helium::SimdVector extentSquaredX = Helium::Simd::MultiplyF32( centerToExtent, centerToExtent );
        Helium::SimdVector extentSquaredY = _mm_shuffle_ps( extentSquaredX, extentSquaredX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector extentSquaredZ = _mm_shuffle_ps( extentSquaredX, extentSquaredX, _MM_SHUFFLE( 1, 0, 3, 2 ) );
        Helium::SimdVector radius =
            Helium::Simd::SqrtF32( Helium::Simd::AddF32( Helium::Simd::AddF32( extentSquaredX, extentSquaredY ), extentSquaredZ ) );
        radius = _mm_shuffle_ps( radius, radius, _MM_SHUFFLE( 0, 0, 0, 0 ) );

        m_centerRadius = Helium::Simd::Select( center, radius, radiusMaskVec );
    }

    /// Set the sphere center.
    ///
    /// @param[in] rCenter  Sphere center.
    ///
    /// @see Translate()
    void Sphere::SetCenter( const Vector3& rCenter )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
        Helium::SimdVector radiusMaskVec = Helium::Simd::LoadAligned( radiusMask );

        m_centerRadius = Helium::Simd::Select( rCenter.GetSimdVector(), m_centerRadius, radiusMaskVec );
    }

    /// Translate the sphere center.
    ///
    /// @param[in] rOffset  Amount by which to translate.
    ///
    /// @see SetCenter()
    void Sphere::Translate( const Vector3& rOffset )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t centerMask[] HELIUM_SIMD_ALIGN_POST = { 0xffffffff, 0xffffffff, 0xffffffff, 0x0 };
        Helium::SimdVector centerMaskVec = Helium::Simd::LoadAligned( centerMask );

        Helium::SimdVector offsetVec = Helium::Simd::And( rOffset.GetSimdVector(), centerMaskVec );

        m_centerRadius = Helium::Simd::AddF32( m_centerRadius, offsetVec );
    }

    /// Set the sphere radius.
    ///
    /// @param[in] radius  Sphere radius.
    ///
    /// @see Scale()
    void Sphere::SetRadius( float32_t radius )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
        Helium::SimdVector radiusMaskVec = Helium::Simd::LoadAligned( radiusMask );

        Helium::SimdVector radiusVec = Helium::Simd::SetSplatF32( radius );

        m_centerRadius = Helium::Simd::Select( m_centerRadius, radiusVec, radiusMaskVec );
    }

    /// Scale the sphere radius.
    ///
    /// @param[in] scale  Amount by which to scale the radius.
    ///
    /// @see SetRadius()
    void Sphere::Scale( float32_t scale )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t radiusMask[] HELIUM_SIMD_ALIGN_POST = { 0x0, 0x0, 0x0, 0xffffffff };
        Helium::SimdVector radiusMaskVec = Helium::Simd::LoadAligned( radiusMask );

        Helium::SimdVector onesVec = Helium::Simd::SetSplatF32( 1.0f );

        Helium::SimdVector scaleVec = Helium::Simd::Select( onesVec, Helium::Simd::SetSplatF32( scale ), radiusMaskVec );

        m_centerRadius = Helium::Simd::MultiplyF32( m_centerRadius, scaleVec );
    }

    /// Test whether this sphere and the given sphere intersect.
    ///
    /// @param[in] rSphere    Sphere against which to test.
    /// @param[in] threshold  Intersection threshold (higher values increase the distance at which intersection tests
    ///                       will succeed).
    ///
    /// @return  True if the two spheres intersect, false if not.
    bool Sphere::Intersects( const Sphere& rSphere, float32_t threshold ) const
    {
        Helium::SimdVector toSphere = Helium::Simd::SubtractF32( rSphere.m_centerRadius, m_centerRadius );
        Helium::SimdVector productX = Helium::Simd::MultiplyF32( toSphere, toSphere );
        Helium::SimdVector productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        Helium::SimdVector distanceSquared = Helium::Simd::AddF32( Helium::Simd::AddF32( productX, productY ), productZ );

        Helium::SimdVector thresholdVec = _mm_set_ss( threshold );
        Helium::SimdVector radii = Helium::Simd::AddF32( m_centerRadius, rSphere.m_centerRadius );
        radii = _mm_shuffle_ps( radii, radii, _MM_SHUFFLE( 3, 3, 3, 3 ) );
        radii = Helium::Simd::AddF32( radii, thresholdVec );

        Helium::SimdVector compareResult = Helium::Simd::LessEqualsF32( distanceSquared, Helium::Simd::MultiplyF32( radii, radii ) );
        int resultMask = _mm_movemask_ps( compareResult );

        return ( ( resultMask & 0x1 ) != 0 );
    }
}

#endif  // HELIUM_SIMD_SSE
