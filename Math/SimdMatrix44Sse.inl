/// Constructor.
///
/// @param[in] xAxisX      X-axis, x-component.
/// @param[in] xAxisY      X-axis, y-component.
/// @param[in] xAxisZ      X-axis, z-component.
/// @param[in] xAxisW      X-axis, w-component.
/// @param[in] yAxisX      Y-axis, x-component.
/// @param[in] yAxisY      Y-axis, y-component.
/// @param[in] yAxisZ      Y-axis, z-component.
/// @param[in] yAxisW      Y-axis, w-component.
/// @param[in] zAxisX      Z-axis, x-component.
/// @param[in] zAxisY      Z-axis, y-component.
/// @param[in] zAxisZ      Z-axis, z-component.
/// @param[in] zAxisW      Z-axis, w-component.
/// @param[in] translateX  Translation, x-component.
/// @param[in] translateY  Translation, y-component.
/// @param[in] translateZ  Translation, z-component.
/// @param[in] translateW  Translation, w-component.
Helium::Simd::Matrix44::Matrix44(
    float32_t xAxisX,
    float32_t xAxisY,
    float32_t xAxisZ,
    float32_t xAxisW,
    float32_t yAxisX,
    float32_t yAxisY,
    float32_t yAxisZ,
    float32_t yAxisW,
    float32_t zAxisX,
    float32_t zAxisY,
    float32_t zAxisZ,
    float32_t zAxisW,
    float32_t translateX,
    float32_t translateY,
    float32_t translateZ,
    float32_t translateW )
{
    m_matrix[ 0 ] = _mm_set_ps( xAxisW, xAxisZ, xAxisY, xAxisX );
    m_matrix[ 1 ] = _mm_set_ps( yAxisW, yAxisZ, yAxisY, yAxisX );
    m_matrix[ 2 ] = _mm_set_ps( zAxisW, zAxisZ, zAxisY, zAxisX );
    m_matrix[ 3 ] = _mm_set_ps( translateW, translateZ, translateY, translateX );
}

/// Constructor.
///
/// @param[in] rXAxis      X-axis values.
/// @param[in] rYAxis      Y-axis values.
/// @param[in] rZAxis      Z-axis values.
/// @param[in] rTranslate  Translation values.
Helium::Simd::Matrix44::Matrix44( const Vector4& rXAxis, const Vector4& rYAxis, const Vector4& rZAxis, const Vector4& rTranslate )
{
    m_matrix[ 0 ] = rXAxis.GetSimdVector();
    m_matrix[ 1 ] = rYAxis.GetSimdVector();
    m_matrix[ 2 ] = rZAxis.GetSimdVector();
    m_matrix[ 3 ] = rTranslate.GetSimdVector();
}

/// Constructor.
///
/// @param[in] rXAxis      X-axis values.
/// @param[in] rYAxis      Y-axis values.
/// @param[in] rZAxis      Z-axis values.
/// @param[in] rTranslate  Translation values.
Helium::Simd::Matrix44::Matrix44(
    const Register& rXAxis,
    const Register& rYAxis,
    const Register& rZAxis,
    const Register& rTranslate )
{
    m_matrix[ 0 ] = rXAxis;
    m_matrix[ 1 ] = rYAxis;
    m_matrix[ 2 ] = rZAxis;
    m_matrix[ 3 ] = rTranslate;
}

/// Get the SIMD vector for a given portion of this array.
///
/// @param[in] index  Index of the portion to retrieve.
///                   - For 16-byte SIMD platforms, this will retrieve a single row (x-axis, y-axis, z-axis, or
///                     translation component).
///                   - For 64-byte SIMD platforms, this will retrieve the entire matrix (index must always be
///                     zero).
///
/// @return  Reference to the SIMD vector for the requested array section.
///
/// @see SetSimdVector()
Helium::Simd::Register& Helium::Simd::Matrix44::GetSimdVector( size_t index )
{
    HELIUM_ASSERT( index < 4 );

    return m_matrix[ index ];
}

/// Get the SIMD vector for a given portion of this array.
///
/// @param[in] index  Index of the portion to retrieve.
///                   - For 16-byte SIMD platforms, this will retrieve a single row (x-axis, y-axis, z-axis, or
///                     translation component).
///                   - For 64-byte SIMD platforms, this will retrieve the entire matrix (index must always be
///                     zero).
///
/// @return  Constant reference to the SIMD vector for the requested array section.
///
/// @see SetSimdVector()
const Helium::Simd::Register& Helium::Simd::Matrix44::GetSimdVector( size_t index ) const
{
    HELIUM_ASSERT( index < 4 );

    return m_matrix[ index ];
}

/// Set the SIMD vector for a given portion of this array.
///
/// @param[in] index    Index of the portion to set.
///                     - For 16-byte SIMD platforms, this will set a single row (x-axis, y-axis, z-axis, or
///                       translation component).
///                     - For 64-byte SIMD platforms, this will set the entire matrix (index must always be zero).
/// @param[in] rVector  SIMD vector to set.
///
/// @see GetSimdVector()
void Helium::Simd::Matrix44::SetSimdVector( size_t index, const Register& rVector )
{
    HELIUM_ASSERT( index < 4 );

    m_matrix[ index ] = rVector;
}

/// Get the matrix element stored at the specified index.
///
/// Matrices are stored in row-major format (x-axis is stored in the first four elements, y-axis is stored in the
/// second four, etc.).
///
/// Note that accessing individual elements within a matrix can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 16).
///
/// @return  Reference to the value stored at the specified element.
///
/// @see SetElement()
float32_t& Helium::Simd::Matrix44::GetElement( size_t index )
{
    HELIUM_ASSERT( index < 16 );

    return reinterpret_cast< float32_t* >( &m_matrix[ index / 4 ] )[ index % 4 ];
}

/// Get the matrix element stored at the specified index.
///
/// Matrices are stored in row-major format (x-axis is stored in the first four elements, y-axis is stored in the
/// second four, etc.).
///
/// Note that accessing individual elements within a matrix can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to retrieve (less than 16).
///
/// @return  Value stored at the specified element.
///
/// @see SetElement()
float32_t Helium::Simd::Matrix44::GetElement( size_t index ) const
{
    HELIUM_ASSERT( index < 16 );

    return reinterpret_cast< const float32_t* >( &m_matrix[ index / 4 ] )[ index % 4 ];
}

/// Set the matrix element at the specified index.
///
/// Matrices are stored in row-major format (x-axis is stored in the first four elements, y-axis is stored in the
/// second four, etc.).
///
/// Note that accessing individual elements within a matrix can incur a performance penalty, especially on
/// particular platforms like the PowerPC, so use it with care.
///
/// @param[in] index  Index of the element to set (less than 16).
/// @param[in] value  Value to set.
///
/// @see GetElement()
void Helium::Simd::Matrix44::SetElement( size_t index, float32_t value )
{
    HELIUM_ASSERT( index < 16 );

    reinterpret_cast< float32_t* >( &m_matrix[ index / 4 ] )[ index % 4 ] = value;
}

/// Fill out a vector with the values for a given row of this matrix.
///
/// @param[in]  index  Row index (less than 4).
/// @param[out] rRow   Vector filled with the row values.
///
/// @see SetRow()
void Helium::Simd::Matrix44::GetRow( size_t index, Vector4& rRow ) const
{
    HELIUM_ASSERT( index < 4 );

    rRow.SetSimdVector( m_matrix[ index ] );
}

/// Retrieve a vector containing the values for a given row of this matrix.
///
/// @param[in] index  Row index (less than 4).
///
/// @return  Vector containing the row values.
///
/// @see SetRow()
Helium::Simd::Vector4 Helium::Simd::Matrix44::GetRow( size_t index ) const
{
    HELIUM_ASSERT( index < 4 );

    return Vector4( m_matrix[ index ] );
}

/// Set the values for a given row of this matrix.
///
/// @param[in] index  Row index (less than 4).
/// @param[in] rRow   Row values.
///
/// @see GetRow()
void Helium::Simd::Matrix44::SetRow( size_t index, const Vector4& rRow )
{
    HELIUM_ASSERT( index < 4 );

    m_matrix[ index ] = rRow.GetSimdVector();
}

/// Set this matrix to the component-wise sum of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::AddSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix[ 0 ] = Simd::AddF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
    m_matrix[ 1 ] = Simd::AddF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
    m_matrix[ 2 ] = Simd::AddF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
    m_matrix[ 3 ] = Simd::AddF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
}

/// Set this matrix to the component-wise difference of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::SubtractSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix[ 0 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
    m_matrix[ 1 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
    m_matrix[ 2 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
    m_matrix[ 3 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
}

/// Set this matrix to the component-wise product of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::MultiplyComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix[ 0 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
    m_matrix[ 1 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
    m_matrix[ 2 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
    m_matrix[ 3 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
}

/// Set this matrix to the component-wise quotient of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::DivideComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix[ 0 ] = Simd::DivideF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
    m_matrix[ 1 ] = Simd::DivideF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
    m_matrix[ 2 ] = Simd::DivideF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
    m_matrix[ 3 ] = Simd::DivideF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
}

/// Transform a 4-component vector.
///
/// Note that transformation takes into account the vector w-component.
///
/// @param[in]  rVector  Vector to transform.
/// @param[out] rResult  Transformed result.
///
/// @see TransformPoint(), TransformVector()
void Helium::Simd::Matrix44::Transform( const Vector4& rVector, Vector4& rResult ) const
{
    Register vec = rVector.GetSimdVector();

    Register x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Register y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    Register z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
    Register w = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

    x = Simd::MultiplyF32( x, m_matrix[ 0 ] );
    y = Simd::MultiplyF32( y, m_matrix[ 1 ] );
    z = Simd::MultiplyF32( z, m_matrix[ 2 ] );
    w = Simd::MultiplyF32( w, m_matrix[ 3 ] );

    Register result = Simd::AddF32( x, y );
    result = Simd::AddF32( result, z );
    result = Simd::AddF32( result, w );

    rResult.SetSimdVector( result );
}

/// Transform a 3-component vector as a point in 3D space.
///
/// This is equivalent to calling Transform() on a Vector4 filled with the same values as the given vector, with the
/// w-component set to 1.
///
/// @param[in]  rVector  Vector to transform.
/// @param[out] rResult  Transformed result.
///
/// @see TransformVector(), Transform()
void Helium::Simd::Matrix44::TransformPoint( const Vector3& rVector, Vector3& rResult ) const
{
    Register vec = rVector.GetSimdVector();

    Register x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Register y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    Register z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

    x = Simd::MultiplyF32( x, m_matrix[ 0 ] );
    y = Simd::MultiplyF32( y, m_matrix[ 1 ] );
    z = Simd::MultiplyF32( z, m_matrix[ 2 ] );

    Register result = Simd::AddF32( x, y );
    result = Simd::AddF32( result, z );
    result = Simd::AddF32( result, m_matrix[ 3 ] );

    rResult.SetSimdVector( result );
}

/// Transform a 3-component vector as a directional vector in 3D space.
///
/// This is equivalent to calling Transform() on a Vector4 filled with the same values as the given vector, with the
/// w-component set to 0.
///
/// @param[in]  rVector  Vector to transform.
/// @param[out] rResult  Transformed result.
///
/// @see TransformPoint(), Transform()
void Helium::Simd::Matrix44::TransformVector( const Vector3& rVector, Vector3& rResult ) const
{
    Register vec = rVector.GetSimdVector();

    Register x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    Register y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    Register z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

    x = Simd::MultiplyF32( x, m_matrix[ 0 ] );
    y = Simd::MultiplyF32( y, m_matrix[ 1 ] );
    z = Simd::MultiplyF32( z, m_matrix[ 2 ] );

    Register result = Simd::AddF32( x, y );
    result = Simd::AddF32( result, z );

    rResult.SetSimdVector( result );
}

/// Test whether each component in this matrix is equal to the corresponding component in another matrix within a
/// given threshold.
///
/// @param[in] rMatrix  Matrix.
/// @param[in] epsilon  Comparison threshold.
///
/// @return  True if this matrix and the given matrix are equal within the given threshold, false if not.
bool Helium::Simd::Matrix44::Equals( const Matrix44& rMatrix, float32_t epsilon ) const
{
    HELIUM_SIMD_ALIGN_PRE const uint32_t differenceMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
    {
        0x7fffffff,
        0x7fffffff,
        0x7fffffff,
        0x7fffffff
    };

    Register epsilonVec = _mm_set_ps1( epsilon );

    Register differenceMaskVec = Simd::LoadAligned( differenceMask );

    Register difference, testResult;
    
    difference = Simd::SubtractF32( m_matrix[ 0 ], rMatrix.m_matrix[ 0 ] );
    difference = Simd::And( difference, differenceMaskVec );
    testResult = Simd::GreaterF32( difference, epsilonVec );

    difference = Simd::SubtractF32( m_matrix[ 1 ], rMatrix.m_matrix[ 1 ] );
    difference = Simd::And( difference, differenceMaskVec );
    testResult = Simd::Or( testResult, Simd::GreaterF32( difference, epsilonVec ) );

    difference = Simd::SubtractF32( m_matrix[ 2 ], rMatrix.m_matrix[ 2 ] );
    difference = Simd::And( difference, differenceMaskVec );
    testResult = Simd::Or( testResult, Simd::GreaterF32( difference, epsilonVec ) );

    difference = Simd::SubtractF32( m_matrix[ 3 ], rMatrix.m_matrix[ 3 ] );
    difference = Simd::And( difference, differenceMaskVec );
    testResult = Simd::Or( testResult, Simd::GreaterF32( difference, epsilonVec ) );

    testResult = Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
    testResult = Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );

    return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
}
