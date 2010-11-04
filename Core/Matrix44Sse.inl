//----------------------------------------------------------------------------------------------------------------------
// Matrix44Sse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
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
    Matrix44::Matrix44(
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
    Matrix44::Matrix44( const Vector4& rXAxis, const Vector4& rYAxis, const Vector4& rZAxis, const Vector4& rTranslate )
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
    Matrix44::Matrix44(
        const Helium::SimdVector& rXAxis,
        const Helium::SimdVector& rYAxis,
        const Helium::SimdVector& rZAxis,
        const Helium::SimdVector& rTranslate )
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
    Helium::SimdVector& Matrix44::GetSimdVector( size_t index )
    {
        L_ASSERT( index < 4 );

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
    const Helium::SimdVector& Matrix44::GetSimdVector( size_t index ) const
    {
        L_ASSERT( index < 4 );

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
    void Matrix44::SetSimdVector( size_t index, const Helium::SimdVector& rVector )
    {
        L_ASSERT( index < 4 );

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
    float32_t& Matrix44::GetElement( size_t index )
    {
        L_ASSERT( index < 16 );

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
    float32_t Matrix44::GetElement( size_t index ) const
    {
        L_ASSERT( index < 16 );

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
    void Matrix44::SetElement( size_t index, float32_t value )
    {
        L_ASSERT( index < 16 );

        reinterpret_cast< float32_t* >( &m_matrix[ index / 4 ] )[ index % 4 ] = value;
    }

    /// Fill out a vector with the values for a given row of this matrix.
    ///
    /// @param[in]  index  Row index (less than 4).
    /// @param[out] rRow   Vector filled with the row values.
    ///
    /// @see SetRow()
    void Matrix44::GetRow( size_t index, Vector4& rRow ) const
    {
        L_ASSERT( index < 4 );

        rRow.SetSimdVector( m_matrix[ index ] );
    }

    /// Retrieve a vector containing the values for a given row of this matrix.
    ///
    /// @param[in] index  Row index (less than 4).
    ///
    /// @return  Vector containing the row values.
    ///
    /// @see SetRow()
    Vector4 Matrix44::GetRow( size_t index ) const
    {
        L_ASSERT( index < 4 );

        return Vector4( m_matrix[ index ] );
    }

    /// Set the values for a given row of this matrix.
    ///
    /// @param[in] index  Row index (less than 4).
    /// @param[in] rRow   Row values.
    ///
    /// @see GetRow()
    void Matrix44::SetRow( size_t index, const Vector4& rRow )
    {
        L_ASSERT( index < 4 );

        m_matrix[ index ] = rRow.GetSimdVector();
    }

    /// Set this matrix to the component-wise sum of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44::AddSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
    {
        m_matrix[ 0 ] = Helium::Simd::AddF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
        m_matrix[ 1 ] = Helium::Simd::AddF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
        m_matrix[ 2 ] = Helium::Simd::AddF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
        m_matrix[ 3 ] = Helium::Simd::AddF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
    }

    /// Set this matrix to the component-wise difference of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44::SubtractSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
    {
        m_matrix[ 0 ] = Helium::Simd::SubtractF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
        m_matrix[ 1 ] = Helium::Simd::SubtractF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
        m_matrix[ 2 ] = Helium::Simd::SubtractF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
        m_matrix[ 3 ] = Helium::Simd::SubtractF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
    }

    /// Set this matrix to the component-wise product of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44::MultiplyComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
    {
        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
        m_matrix[ 3 ] = Helium::Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
    }

    /// Set this matrix to the component-wise quotient of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44::DivideComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
    {
        m_matrix[ 0 ] = Helium::Simd::DivideF32( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix[ 0 ] );
        m_matrix[ 1 ] = Helium::Simd::DivideF32( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix[ 1 ] );
        m_matrix[ 2 ] = Helium::Simd::DivideF32( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix[ 2 ] );
        m_matrix[ 3 ] = Helium::Simd::DivideF32( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix[ 3 ] );
    }

    /// Transform a 4-component vector.
    ///
    /// Note that transformation takes into account the vector w-component.
    ///
    /// @param[in]  rVector  Vector to transform.
    /// @param[out] rResult  Transformed result.
    ///
    /// @see TransformPoint(), TransformVector()
    void Matrix44::Transform( const Vector4& rVector, Vector4& rResult ) const
    {
        Helium::SimdVector vec = rVector.GetSimdVector();

        Helium::SimdVector x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
        Helium::SimdVector w = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

        x = Helium::Simd::MultiplyF32( x, m_matrix[ 0 ] );
        y = Helium::Simd::MultiplyF32( y, m_matrix[ 1 ] );
        z = Helium::Simd::MultiplyF32( z, m_matrix[ 2 ] );
        w = Helium::Simd::MultiplyF32( w, m_matrix[ 3 ] );

        Helium::SimdVector result = Helium::Simd::AddF32( x, y );
        result = Helium::Simd::AddF32( result, z );
        result = Helium::Simd::AddF32( result, w );

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
    void Matrix44::TransformPoint( const Vector3& rVector, Vector3& rResult ) const
    {
        Helium::SimdVector vec = rVector.GetSimdVector();

        Helium::SimdVector x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

        x = Helium::Simd::MultiplyF32( x, m_matrix[ 0 ] );
        y = Helium::Simd::MultiplyF32( y, m_matrix[ 1 ] );
        z = Helium::Simd::MultiplyF32( z, m_matrix[ 2 ] );

        Helium::SimdVector result = Helium::Simd::AddF32( x, y );
        result = Helium::Simd::AddF32( result, z );
        result = Helium::Simd::AddF32( result, m_matrix[ 3 ] );

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
    void Matrix44::TransformVector( const Vector3& rVector, Vector3& rResult ) const
    {
        Helium::SimdVector vec = rVector.GetSimdVector();

        Helium::SimdVector x = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( vec, vec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

        x = Helium::Simd::MultiplyF32( x, m_matrix[ 0 ] );
        y = Helium::Simd::MultiplyF32( y, m_matrix[ 1 ] );
        z = Helium::Simd::MultiplyF32( z, m_matrix[ 2 ] );

        Helium::SimdVector result = Helium::Simd::AddF32( x, y );
        result = Helium::Simd::AddF32( result, z );

        rResult.SetSimdVector( result );
    }

    /// Test whether each component in this matrix is equal to the corresponding component in another matrix within a
    /// given threshold.
    ///
    /// @param[in] rMatrix  Matrix.
    /// @param[in] epsilon  Comparison threshold.
    ///
    /// @return  True if this matrix and the given matrix are equal within the given threshold, false if not.
    bool Matrix44::Equals( const Matrix44& rMatrix, float32_t epsilon ) const
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t differenceMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0x7fffffff,
            0x7fffffff,
            0x7fffffff,
            0x7fffffff
        };

        Helium::SimdVector epsilonVec = _mm_set_ps1( epsilon );

        Helium::SimdVector differenceMaskVec = Helium::Simd::LoadAligned( differenceMask );

        Helium::SimdVector difference, testResult;
        
        difference = Helium::Simd::SubtractF32( m_matrix[ 0 ], rMatrix.m_matrix[ 0 ] );
        difference = Helium::Simd::And( difference, differenceMaskVec );
        testResult = Helium::Simd::GreaterF32( difference, epsilonVec );

        difference = Helium::Simd::SubtractF32( m_matrix[ 1 ], rMatrix.m_matrix[ 1 ] );
        difference = Helium::Simd::And( difference, differenceMaskVec );
        testResult = Helium::Simd::Or( testResult, Helium::Simd::GreaterF32( difference, epsilonVec ) );

        difference = Helium::Simd::SubtractF32( m_matrix[ 2 ], rMatrix.m_matrix[ 2 ] );
        difference = Helium::Simd::And( difference, differenceMaskVec );
        testResult = Helium::Simd::Or( testResult, Helium::Simd::GreaterF32( difference, epsilonVec ) );

        difference = Helium::Simd::SubtractF32( m_matrix[ 3 ], rMatrix.m_matrix[ 3 ] );
        difference = Helium::Simd::And( difference, differenceMaskVec );
        testResult = Helium::Simd::Or( testResult, Helium::Simd::GreaterF32( difference, epsilonVec ) );

        testResult = Helium::Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        testResult = Helium::Simd::Or( testResult, _mm_shuffle_ps( testResult, testResult, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );

        return ( reinterpret_cast< const uint32_t* >( &testResult )[ 0 ] == 0 );
    }
}
