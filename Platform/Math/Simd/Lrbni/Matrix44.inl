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
    HELIUM_SIMD_ALIGN_PRE const float32_t values[ 16 ] HELIUM_SIMD_ALIGN_POST =
    {
        xAxisX,
        xAxisY,
        xAxisZ,
        xAxisW,
        yAxisX,
        yAxisY,
        yAxisZ,
        yAxisW,
        zAxisX,
        zAxisY,
        zAxisZ,
        zAxisW,
        translateX,
        translateY,
        translateZ,
        translateW
    };

    m_matrix = Simd::LoadAligned( values );
}

/// Constructor.
///
/// @param[in] rMatrix  Matrix values.
Helium::Simd::Matrix44::Matrix44( const Register& rMatrix )
{
    m_matrix = rMatrix;
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
Register& Helium::Simd::Matrix44::GetSimdVector( size_t index )
{
    HELIUM_ASSERT( index == 0 );

    return m_matrix;
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
const Register& Helium::Simd::Matrix44::GetSimdVector( size_t index ) const
{
    HELIUM_ASSERT( index == 0 );

    return m_matrix;
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
    HELIUM_ASSERT( index == 0 );

    m_matrix = rVector;
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

    return reinterpret_cast< float32_t* >( &m_matrix )[ index ];
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

    return reinterpret_cast< const float32_t* >( &m_matrix )[ index ];
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

    reinterpret_cast< float32_t* >( &m_matrix )[ index ] = value;
}

/// Set this matrix to the component-wise sum of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::AddSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix = Simd::AddF32( rMatrix0.m_matrix, rMatrix1.m_matrix );
}

/// Set this matrix to the component-wise difference of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::SubtractSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix = Simd::SubtractF32( rMatrix0.m_matrix, rMatrix1.m_matrix );
}

/// Set this matrix to the component-wise product of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::MultiplyComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix = Simd::MultiplyF32( rMatrix0.m_matrix, rMatrix1.m_matrix );
}

/// Set this matrix to the component-wise quotient of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::DivideComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    m_matrix = Simd::DivideF32( rMatrix0.m_matrix, rMatrix1.m_matrix );
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
    epsilon *= epsilon;

    Register differenceSquared = Simd::SubtractF32( m_matrix, rMatrix.m_matrix );
    differenceSquared = Simd::MultiplyF32( differenceSquared, differenceSquared );

    Register epsilonVec = _mm512_set_1to16_ps( epsilon );

    Mask testResult = Simd::GreaterF32( differenceSquared, epsilonVec );

    return ( testResult != 0 );
}