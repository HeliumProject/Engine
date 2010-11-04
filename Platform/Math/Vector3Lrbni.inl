//----------------------------------------------------------------------------------------------------------------------
// Vector3Lrbni.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if HELIUM_SIMD_LRBNI

namespace Helium
{
    /// Constructor.
    ///
    /// @param[in] x  X-coordinate value.
    /// @param[in] y  Y-coordinate value.
    /// @param[in] z  Z-coordinate value.
    Vector3::Vector3( float32_t x, float32_t y, float32_t z )
    {
        m_vector[ 0 ] = x;
        m_vector[ 1 ] = y;
        m_vector[ 2 ] = z;
        m_vector[ 3 ] = 0.0f;
    }

    /// Constructor.
    ///
    /// @param[in] s  Scalar value to which each component of this vector should be set.
    Vector3::Vector3( float32_t s )
    {
        m_vector[ 0 ] = s;
        m_vector[ 1 ] = s;
        m_vector[ 2 ] = s;
        m_vector[ 3 ] = s;
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
        HELIUM_ASSERT( index < 3 );

        return m_vector[ index ];
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
        HELIUM_ASSERT( index < 3 );

        m_vector[ index ] = value;
    }

    /// Set this vector to the component-wise sum of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::AddSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        SimdVector vec0 = Simd::LoadSplat128( rVector0.m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector1.m_vector );

        SimdVector result = Simd::AddF32( vec0, vec1 );

        Simd::Store128( m_vector, result );
    }

    /// Set this vector to the component-wise difference of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::SubtractSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        SimdVector vec0 = Simd::LoadSplat128( rVector0.m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector1.m_vector );

        SimdVector result = Simd::SubtractF32( vec0, vec1 );

        Simd::Store128( m_vector, result );
    }

    /// Set this vector to the component-wise product of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::MultiplySet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        SimdVector vec0 = Simd::LoadSplat128( rVector0.m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector1.m_vector );

        SimdVector result = Simd::MultiplyF32( vec0, vec1 );

        Simd::Store128( m_vector, result );
    }

    /// Set this vector to the component-wise quotient of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::DivideSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        SimdVector vec0 = Simd::LoadSplat128( rVector0.m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector1.m_vector );

        SimdVector result = Simd::DivideF32( vec0, vec1 );

        Simd::Store128( m_vector, result );
    }

    /// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
    ///
    /// @param[in] rVectorMul0  First vector to multiply.
    /// @param[in] rVectorMul1  Second vector to multiply.
    /// @param[in] rVectorAdd   Vector to add.
    void Vector3::MultiplyAddSet( const Vector3& rVectorMul0, const Vector3& rVectorMul1, const Vector3& rVectorAdd )
    {
        SimdVector vecMul0 = Simd::LoadSplat128( rVectorMul0.m_vector );
        SimdVector vecMul1 = Simd::LoadSplat128( rVectorMul1.m_vector );
        SimdVector vecAdd = Simd::LoadSplat128( rVectorAdd.m_vector );

        SimdVector result = Simd::MultiplyAddF32( vecMul0, vecMul1, vecAdd );

        Simd::Store128( m_vector, result );
    }

    /// Compute the dot product of this vector and another 3-component vector.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  Dot product.
    float32_t Vector3::Dot( const Vector3& rVector ) const
    {
        SimdVector vec0 = Simd::LoadSplat128( m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector.m_vector );

        SimdVector product = Simd::MultiplyF32( vec0, vec1 );

        return _mm512_mask_reduce_add_ps( 0x7, product );
    }

    /// Set this vector to the cross product of two 3-component vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector3::CrossSet( const Vector3& rVector0, const Vector3& rVector1 )
    {
        SimdVector vec0a = _mm512_swizzle_r32( Simd::LoadSplat128( rVector0.m_vector ), _MM_SWIZ_REG_DACB );
        SimdVector vec0b = _mm512_swizzle_r32( vec0a, _MM_SWIZ_REG_DACB );

        SimdVector vec1a = _mm512_swizzle_r32( Simd::LoadSplat128( rVector1.m_vector ), _MM_SWIZ_REG_DACB );
        SimdVector vec1b = _mm512_swizzle_r32( vec1a, _MM_SWIZ_REG_DACB );

        SimdVector crossProduct = _mm512_msub213_ps( vec0a, vec1b, Simd::MultiplyF32( vec0b, vec1a ) );

        Simd::Store128( m_vector, crossProduct );
    }

    /// Get the magnitude of this vector.
    ///
    /// @return  Vector magnitude.
    float32_t Vector3::GetMagnitude() const
    {
        SimdVector vec = Simd::LoadSplat128( m_vector );

        SimdVector productX = Simd::MultiplyF32( vec, vec );
        SimdVector productY = _mm512_swizzle_r32( productX, _MM_SWIZ_REG_CDAB );
        SimdVector productZ = _mm512_swizzle_r32( productX, _MM_SWIZ_REG_BADC );

        SimdVector magnitude = Simd::SqrtF32( Simd::AddF32( Simd::AddF32( productX, productY ), productZ ) );

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

        SimdVector vec = Simd::LoadSplat128( m_vector );

        SimdVector productX = Simd::MultiplyF32( vec, vec );
        SimdVector productY = _mm512_swizzle_r32( productX, _MM_SWIZ_REG_CDAB );
        SimdVector productZ = _mm512_swizzle_r32( productX, _MM_SWIZ_REG_BADC );

        SimdVector magnitudeSquared = Simd::AddF32( Simd::AddF32( productX, productY ), productZ );
        magnitudeSquared = _mm512_swizzle_r32( magnitudeSquared, _MM_SWIZ_REG_AAAA );

        SimdVector epsilonVec = _mm512_set_1to16_ps( epsilon );

        SimdMask thresholdMask = Simd::LessF32( magnitudeSquared, epsilonVec );

        SimdVector invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

        SimdVector vecNormalized = Simd::MultiplyF32( vec, invMagnitude );

        HELIUM_SIMD_ALIGN_PRE const float32_t fallbackValues[ 4 ] HELIUM_SIMD_ALIGN_POST = { 1.0f, 0.0f, 0.0f, 0.0f };
        SimdVector vecFallback = Simd::LoadSplat128( fallbackValues );

        vec = _mm512_mask_movd( vecFallback, thresholdMask, vecNormalized );

        Simd::Store128( m_vector, vec );
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @param[out] rResult  Copy of this vector with the sign of each component flipped.
    ///
    /// @see Negate()
    void Vector3::GetNegated( Vector3& rResult ) const
    {
        SimdVector signFlip = Simd::SetSplatU32( 0x80000000 );
        SimdVector vec = Simd::LoadSplat128( m_vector );

        vec = Simd::Xor( vec, signFlip );

        Simd::Store128( rResult.m_vector, vec );
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

        SimdVector vec0 = Simd::LoadSplat128( m_vector );
        SimdVector vec1 = Simd::LoadSplat128( rVector.m_vector );

        SimdVector differenceSquared = Simd::SubtractF32( vec0, vec1 );
        differenceSquared = Simd::MultiplyF32( differenceSquared, differenceSquared );

        SimdVector epsilonVec = _mm512_set_1to16_ps( epsilon );

        SimdMask testResult = Simd::GreaterF32( differenceSquared, epsilonVec );

        return ( ( testResult & 0x7 ) == 0 );
    }
}

#endif  // HELIUM_SIMD_LRBNI
