#if HELIUM_SIMD_SSE

/// Constructor.
///
/// @param[in] x  X-coordinate value.
/// @param[in] y  Y-coordinate value.
/// @param[in] z  Z-coordinate value.
/// @param[in] w  W-coordinate value.
Helium::Simd::Vector4::Vector4( float32_t x, float32_t y, float32_t z, float32_t w )
{
    m_vector = _mm_set_ps( w, z, y, x );
}

/// Constructor.
///
/// @param[in] s  Scalar value to which each component of this vector should be set.
Helium::Simd::Vector4::Vector4( float32_t s )
{
    m_vector = _mm_set_ps1( s );
}

/// Constructor.
///
/// @param[in] rVector  SIMD vector to copy into this vector.
Helium::Simd::Vector4::Vector4( const Register& rVector )
    : m_vector( rVector )
{
}

/// Get the contents of this vector as a SIMD vector.
///
/// @return  Reference to the SIMD vector in which this vector is stored.
///
/// @see SetSimdVector()
Helium::Simd::Register& Helium::Simd::Vector4::GetSimdVector()
{
    return m_vector;
}

/// Get the contents of this vector as a SIMD vector.
///
/// @return  Constant reference to the SIMD vector in which this vector is stored.
///
/// @see SetSimdVector()
const Helium::Simd::Register& Helium::Simd::Vector4::GetSimdVector() const
{
    return m_vector;
}

/// Set the contents of this vector to the given SIMD vector.
///
/// @param[in] rVector  SIMD vector.
///
/// @see GetSimdVector()
void Helium::Simd::Vector4::SetSimdVector( const Register& rVector )
{
    m_vector = rVector;
}

/// Get the vector element stored at the specified index.
///
/// Note that accessing individual elements within a vector can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Reference to the value stored at the specified element.
///
/// @see SetElement()
float32_t& Helium::Simd::Vector4::GetElement( size_t index )
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< float32_t* >( &m_vector )[ index ];
}

/// Get the vector element stored at the specified index.
///
/// Note that accessing individual elements within a vector can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 4).
///
/// @return  Value stored at the specified element.
///
/// @see SetElement()
float32_t Helium::Simd::Vector4::GetElement( size_t index ) const
{
    HELIUM_ASSERT( index < 4 );

    return reinterpret_cast< const float32_t* >( &m_vector )[ index ];
}

/// Set the vector element at the specified index.
///
/// Note that accessing individual elements within a vector can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to set (less than 4).
/// @param[in] value  Value to set.
///
/// @see GetElement()
void Helium::Simd::Vector4::SetElement( size_t index, float32_t value )
{
    HELIUM_ASSERT( index < 4 );

    reinterpret_cast< float32_t* >( &m_vector )[ index ] = value;
}

/// Set this vector to the component-wise sum of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4::AddSet( const Vector4& rVector0, const Vector4& rVector1 )
{
    m_vector = Simd::AddF32( rVector0.m_vector, rVector1.m_vector );
}

/// Set this vector to the component-wise difference of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4::SubtractSet( const Vector4& rVector0, const Vector4& rVector1 )
{
    m_vector = Simd::SubtractF32( rVector0.m_vector, rVector1.m_vector );
}

/// Set this vector to the component-wise product of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4::MultiplySet( const Vector4& rVector0, const Vector4& rVector1 )
{
    m_vector = Simd::MultiplyF32( rVector0.m_vector, rVector1.m_vector );
}

/// Set this vector to the component-wise quotient of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4::DivideSet( const Vector4& rVector0, const Vector4& rVector1 )
{
    m_vector = Simd::DivideF32( rVector0.m_vector, rVector1.m_vector );
}

/// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
///
/// @param[in] rVectorMul0  First vector to multiply.
/// @param[in] rVectorMul1  Second vector to multiply.
/// @param[in] rVectorAdd   Vector to add.
void Helium::Simd::Vector4::MultiplyAddSet( const Vector4& rVectorMul0, const Vector4& rVectorMul1, const Vector4& rVectorAdd )
{
    m_vector = Simd::MultiplyAddF32( rVectorMul0.m_vector, rVectorMul1.m_vector, rVectorAdd.m_vector );
}

/// Compute the dot product of this vector and another 4-component vector.
///
/// @param[in] rVector  Vector.
///
/// @return  Dot product.
float32_t Helium::Simd::Vector4::Dot( const Vector4& rVector ) const
{
    Register productLo = Simd::MultiplyF32( m_vector, rVector.m_vector );
    Register productHi = _mm_movehl_ps( productLo, productLo );

    Register sum = Simd::AddF32( productLo, productHi );
    sum = Simd::AddF32( sum, _mm_shuffle_ps( sum, sum, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

    return reinterpret_cast< const float32_t* >( &sum )[ 0 ];
}

/// Get the magnitude of this vector.
///
/// @return  Vector magnitude.
float32_t Helium::Simd::Vector4::GetMagnitude() const
{
    Register productLo = Simd::MultiplyF32( m_vector, m_vector );
    Register productHi = _mm_movehl_ps( productLo, productLo );

    Register magnitude = Simd::AddF32( productLo, productHi );
    magnitude = Simd::AddF32( magnitude, _mm_shuffle_ps( magnitude, magnitude, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
    magnitude = Simd::SqrtF32( magnitude );

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
void Helium::Simd::Vector4::Normalize( float32_t epsilon )
{
    epsilon *= epsilon;

    Register productLo = Simd::MultiplyF32( m_vector, m_vector );
    Register productHi = _mm_shuffle_ps( productLo, productLo, _MM_SHUFFLE( 1, 0, 3, 2 ) );

    Register magnitudeSquared = Simd::AddF32( productLo, productHi );
    magnitudeSquared = Simd::AddF32(
        magnitudeSquared,
        _mm_shuffle_ps( magnitudeSquared, magnitudeSquared, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

    Register epsilonVec = _mm_set_ps1( epsilon );

    Mask thresholdMask = Simd::LessF32( magnitudeSquared, epsilonVec );

    Register invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

    Register vecNormalized = Simd::MultiplyF32( m_vector, invMagnitude );
    Register vecFallback = _mm_set_ps( 0.0f, 0.0f, 0.0f, 1.0f );

    vecNormalized = Simd::AndNot( thresholdMask, vecNormalized );
    vecFallback = Simd::And( thresholdMask, vecFallback );

    m_vector = Simd::Or( vecNormalized, vecFallback );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @param[out] rResult  Copy of this vector with the sign of each component flipped.
///
/// @see Negate()
void Helium::Simd::Vector4::GetNegated( Vector4& rResult ) const
{
    rResult.m_vector = Simd::Xor( m_vector, Simd::SetSplatU32( 0x80000000 ) );
}

/// Test whether each component in this vector is equal to the corresponding component in another vector within a
/// given threshold.
///
/// @param[in] rVector  Vector.
/// @param[in] epsilon  Comparison threshold.
///
/// @return  True if this vector and the given vector are equal within the given threshold, false if not.
bool Helium::Simd::Vector4::Equals( const Vector4& rVector, float32_t epsilon ) const
{
    epsilon *= epsilon;

    Register differenceSquared = Simd::SubtractF32( m_vector, rVector.m_vector );
    differenceSquared = Simd::MultiplyF32( differenceSquared, differenceSquared );

    Register epsilonVec = _mm_set_ps1( epsilon );

    Register testResult = Simd::GreaterF32( differenceSquared, epsilonVec );
    testResult = Simd::Or( testResult, _mm_movehl_ps( testResult, testResult ) );
    testResult = Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );

    return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
}

#endif  // HELIUM_SIMD_SSE
