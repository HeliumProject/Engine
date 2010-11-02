//----------------------------------------------------------------------------------------------------------------------
// QuatSse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] x  X component value.
    /// @param[in] y  Y component value.
    /// @param[in] z  Z component value.
    /// @param[in] w  W component value.
    Quat::Quat( float32_t x, float32_t y, float32_t z, float32_t w )
    {
        m_quat = _mm_set_ps( w, z, y, x );
    }

    /// Constructor.
    ///
    /// @param[in] rVector  SIMD vector to copy into this quaternion.
    Quat::Quat( const SimdVector& rVector )
    {
        m_quat = rVector;
    }

    /// Get the contents of this quaternion as a SIMD vector.
    ///
    /// @return  Reference to the SIMD vector in which this quaternion is stored.
    ///
    /// @see SetSimdVector()
    SimdVector& Quat::GetSimdVector()
    {
        return m_quat;
    }

    /// Get the contents of this quaternion as a SIMD vector.
    ///
    /// @return  Constant reference to the SIMD vector in which this quaternion is stored.
    ///
    /// @see SetSimdVector()
    const SimdVector& Quat::GetSimdVector() const
    {
        return m_quat;
    }

    /// Set the contents of this quaternion to the given SIMD vector.
    ///
    /// @param[in] rVector  SIMD vector.
    ///
    /// @see GetSimdVector()
    void Quat::SetSimdVector( const SimdVector& rVector )
    {
        m_quat = rVector;
    }

    /// Get the quaternion element stored at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to retrieve (less than 4).
    ///
    /// @return  Reference to the value stored at the specified element.
    ///
    /// @see SetElement()
    float32_t& Quat::GetElement( size_t index )
    {
        L_ASSERT( index < 4 );

        return reinterpret_cast< float32_t* >( &m_quat )[ index ];
    }

    /// Get the quaternion element stored at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to retrieve (less than 4).
    ///
    /// @return  Value stored at the specified element.
    ///
    /// @see SetElement()
    float32_t Quat::GetElement( size_t index ) const
    {
        L_ASSERT( index < 4 );

        return reinterpret_cast< const float32_t* >( &m_quat )[ index ];
    }

    /// Set the quaternion element at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to set (less than 4).
    /// @param[in] value  Value to set.
    ///
    /// @see GetElement()
    void Quat::SetElement( size_t index, float32_t value )
    {
        L_ASSERT( index < 4 );

        reinterpret_cast< float32_t* >( &m_quat )[ index ] = value;
    }

    /// Set this quaternion to the component-wise sum of two quaternions.
    ///
    /// @param[in] rQuat0  First quaternion.
    /// @param[in] rQuat1  Second quaternion.
    void Quat::AddSet( const Quat& rQuat0, const Quat& rQuat1 )
    {
        m_quat = Simd::AddF32( rQuat0.m_quat, rQuat1.m_quat );
    }

    /// Set this quaternion to the component-wise difference of two quaternions.
    ///
    /// @param[in] rQuat0  First quaternion.
    /// @param[in] rQuat1  Second quaternion.
    void Quat::SubtractSet( const Quat& rQuat0, const Quat& rQuat1 )
    {
        m_quat = Simd::SubtractF32( rQuat0.m_quat, rQuat1.m_quat );
    }

    /// Set this quaternion to the product of two quaternions.
    ///
    /// @param[in] rQuat0  First quaternion.
    /// @param[in] rQuat1  Second quaternion.
    void Quat::MultiplySet( const Quat& rQuat0, const Quat& rQuat1 )
    {
        L_SIMD_ALIGN_PRE const uint32_t signFlip[ 4 ] L_SIMD_ALIGN_POST = { 0, 0, 0, 0x80000000 };

        SimdVector result = Simd::MultiplyF32(
            _mm_shuffle_ps( rQuat0.m_quat, rQuat0.m_quat, _MM_SHUFFLE( 0, 2, 1, 0 ) ),
            _mm_shuffle_ps( rQuat1.m_quat, rQuat1.m_quat, _MM_SHUFFLE( 0, 3, 3, 3 ) ) );

        SimdVector product = Simd::MultiplyF32(
            _mm_shuffle_ps( rQuat0.m_quat, rQuat0.m_quat, _MM_SHUFFLE( 1, 1, 0, 2 ) ),
            _mm_shuffle_ps( rQuat1.m_quat, rQuat1.m_quat, _MM_SHUFFLE( 1, 0, 2, 1 ) ) );
        result = Simd::AddF32( result, product );

        result = Simd::Xor( result, Simd::LoadAligned( signFlip ) );

        product = Simd::MultiplyF32(
            _mm_shuffle_ps( rQuat0.m_quat, rQuat0.m_quat, _MM_SHUFFLE( 2, 0, 2, 1 ) ),
            _mm_shuffle_ps( rQuat1.m_quat, rQuat1.m_quat, _MM_SHUFFLE( 2, 1, 0, 2 ) ) );
        result = Simd::SubtractF32( result, product );

        product = Simd::MultiplyF32(
            _mm_shuffle_ps( rQuat0.m_quat, rQuat0.m_quat, _MM_SHUFFLE( 3, 3, 3, 3 ) ),
            rQuat1.m_quat );
        result = Simd::AddF32( result, product );

        m_quat = result;
    }

    /// Set this quaternion to the component-wise product of two quaternions.
    ///
    /// @param[in] rQuat0  First quaternion.
    /// @param[in] rQuat1  Second quaternion.
    void Quat::MultiplyComponentsSet( const Quat& rQuat0, const Quat& rQuat1 )
    {
        m_quat = Simd::MultiplyF32( rQuat0.m_quat, rQuat1.m_quat );
    }

    /// Set this quaternion to the component-wise quotient of two quaternions.
    ///
    /// @param[in] rQuat0  First quaternion.
    /// @param[in] rQuat1  Second quaternion.
    void Quat::DivideComponentsSet( const Quat& rQuat0, const Quat& rQuat1 )
    {
        m_quat = Simd::DivideF32( rQuat0.m_quat, rQuat1.m_quat );
    }

    /// Get the magnitude of this quaternion.
    ///
    /// @return  Quaternion magnitude.
    float32_t Quat::GetMagnitude() const
    {
        SimdVector productLo = Simd::MultiplyF32( m_quat, m_quat );
        SimdVector productHi = _mm_movehl_ps( productLo, productLo );

        SimdVector magnitude = Simd::AddF32( productLo, productHi );
        magnitude = Simd::AddF32( magnitude, _mm_shuffle_ps( magnitude, magnitude, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        magnitude = Simd::SqrtF32( magnitude );

        return reinterpret_cast< const float32_t* >( &magnitude )[ 0 ];
    }

    /// Get the squared magnitude of this quaternion.
    ///
    /// @return  Squared quaternion magnitude.
    float32_t Quat::GetMagnitudeSquared() const
    {
        SimdVector productLo = Simd::MultiplyF32( m_quat, m_quat );
        SimdVector productHi = _mm_movehl_ps( productLo, productLo );

        SimdVector sum = Simd::AddF32( productLo, productHi );
        sum = Simd::AddF32( sum, _mm_shuffle_ps( sum, sum, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

        return reinterpret_cast< const float32_t* >( &sum )[ 0 ];
    }

    /// Normalize this quaternion, with safety threshold checking.
    ///
    /// If the magnitude of this quaternion is below the given epsilon, it will be set to an identity quaternion.
    ///
    /// @param[in] epsilon  Threshold at which to test for zero-length quaternions.
    ///
    /// @see GetNormalized()
    void Quat::Normalize( float32_t epsilon )
    {
        epsilon *= epsilon;

        SimdVector productLo = Simd::MultiplyF32( m_quat, m_quat );
        SimdVector productHi = _mm_shuffle_ps( productLo, productLo, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        SimdVector magnitudeSquared = Simd::AddF32( productLo, productHi );
        magnitudeSquared = Simd::AddF32(
            magnitudeSquared,
            _mm_shuffle_ps( magnitudeSquared, magnitudeSquared, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

        SimdVector epsilonVec = _mm_set_ps1( epsilon );

        SimdMask thresholdMask = Simd::LessF32( magnitudeSquared, epsilonVec );

        SimdVector invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

        SimdVector quatNormalized = Simd::MultiplyF32( m_quat, invMagnitude );
        SimdVector quatFallback = IDENTITY.m_quat;

        quatNormalized = Simd::AndNot( thresholdMask, quatNormalized );
        quatFallback = Simd::And( thresholdMask, quatFallback );

        m_quat = Simd::Or( quatNormalized, quatFallback );
    }

    /// Get the inverse of this quaternion.
    ///
    /// @param[out] rQuat  Quaternion inverse.
    ///
    /// @see Invert(), GetConjugate(), SetConjugate()
    void Quat::GetInverse( Quat& rQuat ) const
    {
        L_SIMD_ALIGN_PRE const uint32_t signFlip[ 4 ] L_SIMD_ALIGN_POST = { 0x80000000, 0x80000000, 0x80000000, 0 };

        SimdVector productLo = Simd::MultiplyF32( m_quat, m_quat );
        SimdVector productHi = _mm_shuffle_ps( productLo, productLo, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        SimdVector invMagSquared = Simd::AddF32( productLo, productHi );
        invMagSquared = Simd::AddF32(
            invMagSquared,
            _mm_shuffle_ps( invMagSquared, invMagSquared, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        invMagSquared = Simd::InverseF32( invMagSquared );

        rQuat.m_quat = Simd::MultiplyF32( Simd::Xor( m_quat, Simd::LoadAligned( signFlip ) ), invMagSquared );
    }

    /// Get the conjugate of this quaternion.
    ///
    /// @param[out] rQuat  Quaternion conjugate.
    ///
    /// @see SetConjugate(), GetInverse(), Invert()
    void Quat::GetConjugate( Quat& rQuat ) const
    {
        L_SIMD_ALIGN_PRE const uint32_t signFlip[ 4 ] L_SIMD_ALIGN_POST = { 0x80000000, 0x80000000, 0x80000000, 0 };

        rQuat.m_quat = Simd::Xor( m_quat, Simd::LoadAligned( signFlip ) );
    }

    /// Test whether each component in this quaternion is equal to the corresponding component in another quaternion
    /// within a given threshold.
    ///
    /// @param[in] rQuat    Quaternion.
    /// @param[in] epsilon  Comparison threshold.
    ///
    /// @return  True if this quaternion and the given quaternion are equal within the given threshold, false if not.
    bool Quat::Equals( const Quat& rQuat, float32_t epsilon ) const
    {
        epsilon *= epsilon;

        SimdVector differenceSquared = Simd::SubtractF32( m_quat, rQuat.m_quat );
        differenceSquared = Simd::MultiplyF32( differenceSquared, differenceSquared );

        SimdVector epsilonVec = _mm_set_ps1( epsilon );

        SimdVector testResult = Simd::GreaterF32( differenceSquared, epsilonVec );
        testResult = Simd::Or( testResult, _mm_movehl_ps( testResult, testResult ) );
        testResult = Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

        return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
    }
}
