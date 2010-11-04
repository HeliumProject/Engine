//----------------------------------------------------------------------------------------------------------------------
// Vector3Sse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if HELIUM_SIMD_SSE

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] x  X-coordinate value.
    /// @param[in] y  Y-coordinate value.
    /// @param[in] z  Z-coordinate value.
    Vector3::Vector3( float32_t x, float32_t y, float32_t z )
    {
        m_vector = _mm_set_ps( 0.0f, z, y, x );
    }

    /// Constructor.
    ///
    /// @param[in] s  Scalar value to which each component of this vector should be set.
    Vector3::Vector3( float32_t s )
    {
        m_vector = _mm_set_ps1( s );
    }

    /// Constructor.
    ///
    /// @param[in] rVector  SIMD vector to copy into this vector.
    Vector3::Vector3( const Helium::SimdVector& rVector )
        : m_vector( rVector )
    {
    }

    /// Get the contents of this vector as a SIMD vector.
    ///
    /// @return  Reference to the SIMD vector in which this vector is stored.
    ///
    /// @see SetSimdVector()
    Helium::SimdVector& Vector3::GetSimdVector()
    {
        return m_vector;
    }

    /// Get the contents of this vector as a SIMD vector.
    ///
    /// @return  Constant reference to the SIMD vector in which this vector is stored.
    ///
    /// @see SetSimdVector()
    const Helium::SimdVector& Vector3::GetSimdVector() const
    {
        return m_vector;
    }

    /// Set the contents of this vector to the given SIMD vector.
    ///
    /// @param[in] rVector  SIMD vector.
    ///
    /// @see GetSimdVector()
    void Vector3::SetSimdVector( const Helium::SimdVector& rVector )
    {
        m_vector = rVector;
    }

    /// Get the vector element stored at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to retrieve (less than 3).
    ///
    /// @return  Reference to the value stored at the specified element.
    ///
    /// @see SetElement()
    float32_t& Vector3::GetElement( size_t index )
    {
        L_ASSERT( index < 3 );

        return reinterpret_cast< float32_t* >( &m_vector )[ index ];
    }

    /// Get the vector element stored at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to retrieve (less than 3).
    ///
    /// @return  Value stored at the specified element.
    ///
    /// @see SetElement()
    float32_t Vector3::GetElement( size_t index ) const
    {
        L_ASSERT( index < 3 );

        return reinterpret_cast< const float32_t* >( &m_vector )[ index ];
    }

    /// Set the vector element at the specified index.
    ///
    /// Note that accessing individual elements within a vector can incur a performance penalty, especially on
    /// particular platforms like the PowerPC, so use it with care.
    ///
    /// @param[in] index  Index of the element to set (less than 3).
    /// @param[in] value  Value to set.
    ///
    /// @see GetElement()
    void Vector3::SetElement( size_t index, float32_t value )
    {
        L_ASSERT( index < 3 );

        reinterpret_cast< float32_t* >( &m_vector )[ index ] = value;
    }

    /// Set this vector to the component-wise sum of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::AddSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        m_vector = Helium::Simd::AddF32( rVector0.m_vector, rVector1.m_vector );
    }

    /// Set this vector to the component-wise difference of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::SubtractSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        m_vector = Helium::Simd::SubtractF32( rVector0.m_vector, rVector1.m_vector );
    }

    /// Set this vector to the component-wise product of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::MultiplySet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        m_vector = Helium::Simd::MultiplyF32( rVector0.m_vector, rVector1.m_vector );
    }

    /// Set this vector to the component-wise quotient of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::DivideSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        m_vector = Helium::Simd::DivideF32( rVector0.m_vector, rVector1.m_vector );
    }

    /// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
    ///
    /// @param[in] rVectorMul0  First vector to multiply.
    /// @param[in] rVectorMul1  Second vector to multiply.
    /// @param[in] rVectorAdd   Vector to add.
    void Vector3::MultiplyAddSet( const Vector3& rVectorMul0, const Vector3& rVectorMul1, const Vector3& rVectorAdd )
    {
        m_vector = Helium::Simd::MultiplyAddF32( rVectorMul0.m_vector, rVectorMul1.m_vector, rVectorAdd.m_vector );
    }

    /// Compute the dot product of this vector and another 3-component vector.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  Dot product.
    float32_t Vector3::Dot( const Vector3& rVector ) const
    {
        Helium::SimdVector productX = Helium::Simd::MultiplyF32( m_vector, rVector.m_vector );
        Helium::SimdVector productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        Helium::SimdVector sum = Helium::Simd::AddF32( Helium::Simd::AddF32( productX, productY ), productZ );

        return reinterpret_cast< const float32_t* >( &sum )[ 0 ];
    }

    /// Set this vector to the cross product of two 3-component vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::CrossSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        Helium::SimdVector vec0, vec1;

        vec0 = _mm_shuffle_ps( rVector0.m_vector, rVector0.m_vector, _MM_SHUFFLE( 0, 0, 2, 1 ) );
        vec1 = _mm_shuffle_ps( rVector1.m_vector, rVector1.m_vector, _MM_SHUFFLE( 0, 1, 0, 2 ) );
        Helium::SimdVector productA = Helium::Simd::MultiplyF32( vec0, vec1 );

        vec0 = _mm_shuffle_ps( rVector0.m_vector, rVector0.m_vector, _MM_SHUFFLE( 0, 1, 0, 2 ) );
        vec1 = _mm_shuffle_ps( rVector1.m_vector, rVector1.m_vector, _MM_SHUFFLE( 0, 0, 2, 1 ) );
        Helium::SimdVector productB = Helium::Simd::MultiplyF32( vec0, vec1 );

        m_vector = Helium::Simd::SubtractF32( productA, productB );
    }

    /// Get the magnitude of this vector.
    ///
    /// @return  Vector magnitude.
    float32_t Vector3::GetMagnitude() const
    {
        Helium::SimdVector productX = Helium::Simd::MultiplyF32( m_vector, m_vector );
        Helium::SimdVector productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        Helium::SimdVector magnitude = Helium::Simd::SqrtF32( Helium::Simd::AddF32( Helium::Simd::AddF32( productX, productY ), productZ ) );

        return reinterpret_cast< const float32_t* >( &magnitude )[ 0 ];
    }

    /// Normalize this vector, with safety threshold checking.
    ///
    /// If the magnitude of this vector is below the given epsilon, a unit vector pointing along the x-axis will be
    /// returned.
    ///
    /// @param[in] epsilon  Threshold at which to test for zero-length vectors.
    ///
    /// @see GetNormalized()
    void Vector3::Normalize( float32_t epsilon )
    {
        epsilon *= epsilon;

        Helium::SimdVector productX = Helium::Simd::MultiplyF32( m_vector, m_vector );
        Helium::SimdVector productY = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector productZ = _mm_shuffle_ps( productX, productX, _MM_SHUFFLE( 1, 0, 3, 2 ) );

        Helium::SimdVector magnitudeSquared = Helium::Simd::AddF32( Helium::Simd::AddF32( productX, productY ), productZ );
        magnitudeSquared = _mm_shuffle_ps( magnitudeSquared, magnitudeSquared, _MM_SHUFFLE( 0, 0, 0, 0 ) );

        Helium::SimdVector epsilonVec = _mm_set_ps1( epsilon );

        Helium::SimdMask thresholdMask = Helium::Simd::LessF32( magnitudeSquared, epsilonVec );

        Helium::SimdVector invMagnitude = Helium::Simd::InverseSqrtF32( magnitudeSquared );

        Helium::SimdVector vecNormalized = Helium::Simd::MultiplyF32( m_vector, invMagnitude );
        Helium::SimdVector vecFallback = _mm_set_ps( 0.0f, 0.0f, 0.0f, 1.0f );

        vecNormalized = Helium::Simd::AndNot( thresholdMask, vecNormalized );
        vecFallback = Helium::Simd::And( thresholdMask, vecFallback );

        m_vector = Helium::Simd::Or( vecNormalized, vecFallback );
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @param[out] rResult  Copy of this vector with the sign of each component flipped.
    ///
    /// @see Negate()
    void Vector3::GetNegated( Vector3& rResult ) const
    {
        rResult.m_vector = Helium::Simd::Xor( m_vector, Helium::Simd::SetSplatU32( 0x80000000 ) );
    }

    /// Test whether each component in this vector is equal to the corresponding component in another vector within a
    /// given threshold.
    ///
    /// @param[in] rVector  Vector.
    /// @param[in] epsilon  Comparison threshold.
    ///
    /// @return  True if this vector and the given vector are equal within the given threshold, false if not.
    bool Vector3::Equals( const Vector3& rVector, float32_t epsilon ) const
    {
        epsilon *= epsilon;

        Helium::SimdVector differenceSquared = Helium::Simd::SubtractF32( m_vector, rVector.m_vector );
        differenceSquared = Helium::Simd::MultiplyF32( differenceSquared, differenceSquared );

        Helium::SimdVector epsilonVec = _mm_set_ps1( epsilon );

        Helium::SimdVector testResult = Helium::Simd::GreaterF32( differenceSquared, epsilonVec );
        testResult = Helium::Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        testResult = Helium::Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );

        return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
    }
}

#endif  // HELIUM_SIMD_SSE
