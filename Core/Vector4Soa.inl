//----------------------------------------------------------------------------------------------------------------------
// Vector4Soa.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a vector with all components uninitialized.  Values should be assigned before use.
    Vector4Soa::Vector4Soa()
    {
    }

    /// Constructor.
    ///
    /// @param[in] rX  X components.
    /// @param[in] rY  Y components.
    /// @param[in] rZ  Z components.
    /// @param[in] rW  W components.
    Vector4Soa::Vector4Soa( const Helium::SimdVector& rX, const Helium::SimdVector& rY, const Helium::SimdVector& rZ, const Helium::SimdVector& rW )
        : m_x( rX )
        , m_y( rY )
        , m_z( rZ )
        , m_w( rW )
    {
    }

    /// Constructor.
    ///
    /// This will fully load the SIMD vectors for each component from the given addresses.
    ///
    /// @param[in] pX  X components (must be SIMD aligned).
    /// @param[in] pY  Y components (must be SIMD aligned).
    /// @param[in] pZ  Z components (must be SIMD aligned).
    /// @param[in] pW  W components (must be SIMD aligned).
    Vector4Soa::Vector4Soa( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
    {
        Load( pX, pY, pZ, pW );
    }

    /// Constructor.
    ///
    /// This will initialize this vector by splatting each component of the given vector across a full SIMD vector.
    ///
    /// @param[in] rVector  Vector from which to initialize this vector.
    Vector4Soa::Vector4Soa( const Vector4& rVector )
    {
        Splat( rVector );
    }

    /// Fully load the SIMD vectors for each vector component.
    ///
    /// @param[in] pX  X components (must be SIMD aligned).
    /// @param[in] pY  Y components (must be SIMD aligned).
    /// @param[in] pZ  Z components (must be SIMD aligned).
    /// @param[in] pW  W components (must be SIMD aligned).
    void Vector4Soa::Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
    {
        m_x = Helium::Simd::LoadAligned( pX );
        m_y = Helium::Simd::LoadAligned( pY );
        m_z = Helium::Simd::LoadAligned( pZ );
        m_w = Helium::Simd::LoadAligned( pW );
    }

    /// Load 4 single-precision floating-point values for each vector component, splatting the values to fill.
    ///
    /// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
    /// same effect as Load().
    ///
    /// @param[in] pX  X components (must be aligned to a 16-byte boundary).
    /// @param[in] pY  Y components (must be aligned to a 16-byte boundary).
    /// @param[in] pZ  Z components (must be aligned to a 16-byte boundary).
    /// @param[in] pW  W components (must be aligned to a 16-byte boundary).
    void Vector4Soa::Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
    {
        m_x = Helium::Simd::LoadSplat128( pX );
        m_y = Helium::Simd::LoadSplat128( pY );
        m_z = Helium::Simd::LoadSplat128( pZ );
        m_w = Helium::Simd::LoadSplat128( pW );
    }

    /// Load 1 single-precision floating-point value for each vector component, splatting the value to fill.
    ///
    /// @param[in] pX  X components (must be aligned to a 4-byte boundary).
    /// @param[in] pY  Y components (must be aligned to a 4-byte boundary).
    /// @param[in] pZ  Z components (must be aligned to a 4-byte boundary).
    /// @param[in] pW  W components (must be aligned to a 4-byte boundary).
    void Vector4Soa::Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
    {
        m_x = Helium::Simd::LoadSplat32( pX );
        m_y = Helium::Simd::LoadSplat32( pY );
        m_z = Helium::Simd::LoadSplat32( pZ );
        m_w = Helium::Simd::LoadSplat32( pW );
    }

    /// Fully store the SIMD vectors from each vector component into memory.
    ///
    /// @param[out] pX  X components (must be SIMD aligned).
    /// @param[out] pY  Y components (must be SIMD aligned).
    /// @param[out] pZ  Z components (must be SIMD aligned).
    /// @param[out] pW  W components (must be SIMD aligned).
    void Vector4Soa::Store( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
    {
        Helium::Simd::StoreAligned( pX, m_x );
        Helium::Simd::StoreAligned( pY, m_y );
        Helium::Simd::StoreAligned( pZ, m_z );
        Helium::Simd::StoreAligned( pW, m_w );
    }

    /// Store the lowest 4 single-precision floating-point values from each vector component into memory.
    ///
    /// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
    /// same effect as Store().
    ///
    /// @param[out] pX  X components (must be aligned to a 16-byte boundary).
    /// @param[out] pY  Y components (must be aligned to a 16-byte boundary).
    /// @param[out] pZ  Z components (must be aligned to a 16-byte boundary).
    /// @param[out] pW  W components (must be aligned to a 16-byte boundary).
    void Vector4Soa::Store4( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
    {
        Helium::Simd::Store128( pX, m_x );
        Helium::Simd::Store128( pY, m_y );
        Helium::Simd::Store128( pZ, m_z );
        Helium::Simd::Store128( pW, m_w );
    }

    /// Store the lowest single-precision floating-point value from each vector component into memory.
    ///
    /// @param[out] pX  X components (must be aligned to a 4-byte boundary).
    /// @param[out] pY  Y components (must be aligned to a 4-byte boundary).
    /// @param[out] pZ  Z components (must be aligned to a 4-byte boundary).
    /// @param[out] pW  W components (must be aligned to a 4-byte boundary).
    void Vector4Soa::Store1( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
    {
        Helium::Simd::Store32( pX, m_x );
        Helium::Simd::Store32( pY, m_y );
        Helium::Simd::Store32( pZ, m_z );
        Helium::Simd::Store32( pW, m_w );
    }

    /// Perform a component-wise addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Vector containing the sums of each component.
    Vector4Soa Vector4Soa::Add( const Vector4Soa& rVector ) const
    {
        return Vector4Soa(
            Helium::Simd::AddF32( m_x, rVector.m_x ),
            Helium::Simd::AddF32( m_y, rVector.m_y ),
            Helium::Simd::AddF32( m_z, rVector.m_z ),
            Helium::Simd::AddF32( m_w, rVector.m_w ) );
    }

    /// Perform a component-wise subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Vector containing the differences of each component.
    Vector4Soa Vector4Soa::Subtract( const Vector4Soa& rVector ) const
    {
        return Vector4Soa(
            Helium::Simd::SubtractF32( m_x, rVector.m_x ),
            Helium::Simd::SubtractF32( m_y, rVector.m_y ),
            Helium::Simd::SubtractF32( m_z, rVector.m_z ),
            Helium::Simd::SubtractF32( m_w, rVector.m_w ) );
    }

    /// Perform a component-wise multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Vector containing the products of each component.
    Vector4Soa Vector4Soa::Multiply( const Vector4Soa& rVector ) const
    {
        return Vector4Soa(
            Helium::Simd::MultiplyF32( m_x, rVector.m_x ),
            Helium::Simd::MultiplyF32( m_y, rVector.m_y ),
            Helium::Simd::MultiplyF32( m_z, rVector.m_z ),
            Helium::Simd::MultiplyF32( m_w, rVector.m_w ) );
    }

    /// Perform a component-wise division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Vector containing the quotients of each component.
    Vector4Soa Vector4Soa::Divide( const Vector4Soa& rVector ) const
    {
        return Vector4Soa(
            Helium::Simd::DivideF32( m_x, rVector.m_x ),
            Helium::Simd::DivideF32( m_y, rVector.m_y ),
            Helium::Simd::DivideF32( m_z, rVector.m_z ),
            Helium::Simd::DivideF32( m_w, rVector.m_w ) );
    }

    /// Set this vector to the component-wise sum of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector4Soa::AddSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
    {
        m_x = Helium::Simd::AddF32( rVector0.m_x, rVector1.m_x );
        m_y = Helium::Simd::AddF32( rVector0.m_y, rVector1.m_y );
        m_z = Helium::Simd::AddF32( rVector0.m_z, rVector1.m_z );
        m_w = Helium::Simd::AddF32( rVector0.m_w, rVector1.m_w );
    }

    /// Set this vector to the component-wise difference of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector4Soa::SubtractSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
    {
        m_x = Helium::Simd::SubtractF32( rVector0.m_x, rVector1.m_x );
        m_y = Helium::Simd::SubtractF32( rVector0.m_y, rVector1.m_y );
        m_z = Helium::Simd::SubtractF32( rVector0.m_z, rVector1.m_z );
        m_w = Helium::Simd::SubtractF32( rVector0.m_w, rVector1.m_w );
    }

    /// Set this vector to the component-wise product of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector4Soa::MultiplySet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
    {
        m_x = Helium::Simd::MultiplyF32( rVector0.m_x, rVector1.m_x );
        m_y = Helium::Simd::MultiplyF32( rVector0.m_y, rVector1.m_y );
        m_z = Helium::Simd::MultiplyF32( rVector0.m_z, rVector1.m_z );
        m_w = Helium::Simd::MultiplyF32( rVector0.m_w, rVector1.m_w );
    }

    /// Set this vector to the component-wise quotient of two vectors.
    ///
    /// @param[in] rVector0  First vector.
    /// @param[in] rVector1  Second vector.
    void Vector4Soa::DivideSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
    {
        m_x = Helium::Simd::DivideF32( rVector0.m_x, rVector1.m_x );
        m_y = Helium::Simd::DivideF32( rVector0.m_y, rVector1.m_y );
        m_z = Helium::Simd::DivideF32( rVector0.m_z, rVector1.m_z );
        m_w = Helium::Simd::DivideF32( rVector0.m_w, rVector1.m_w );
    }

    /// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
    ///
    /// @param[in] rVectorMul0  First vector to multiply.
    /// @param[in] rVectorMul1  Second vector to multiply.
    /// @param[in] rVectorAdd   Vector to add.
    void Vector4Soa::MultiplyAddSet(
        const Vector4Soa& rVectorMul0,
        const Vector4Soa& rVectorMul1,
        const Vector4Soa& rVectorAdd )
    {
        m_x = Helium::Simd::MultiplyAddF32( rVectorMul0.m_x, rVectorMul1.m_x, rVectorAdd.m_x );
        m_y = Helium::Simd::MultiplyAddF32( rVectorMul0.m_y, rVectorMul1.m_y, rVectorAdd.m_y );
        m_z = Helium::Simd::MultiplyAddF32( rVectorMul0.m_z, rVectorMul1.m_z, rVectorAdd.m_z );
        m_w = Helium::Simd::MultiplyAddF32( rVectorMul0.m_w, rVectorMul1.m_w, rVectorAdd.m_w );
    }

    /// Get this vector scaled by a given set of scalar values.
    ///
    /// @param[in] rScale  Amount by which to scale.
    ///
    /// @return  Copy of this vector scaled by the specified amount.
    ///
    /// @see Scale()
    Vector4Soa Vector4Soa::GetScaled( const Helium::SimdVector& rScale ) const
    {
        return Vector4Soa(
            Helium::Simd::MultiplyF32( m_x, rScale ),
            Helium::Simd::MultiplyF32( m_y, rScale ),
            Helium::Simd::MultiplyF32( m_z, rScale ),
            Helium::Simd::MultiplyF32( m_w, rScale ) );
    }

    /// Scale this vector by a given set of scalar values.
    ///
    /// @param[in] rScale  Amount by which to scale.
    ///
    /// @see GetScaled()
    void Vector4Soa::Scale( const Helium::SimdVector& rScale )
    {
        m_x = Helium::Simd::MultiplyF32( m_x, rScale );
        m_y = Helium::Simd::MultiplyF32( m_y, rScale );
        m_z = Helium::Simd::MultiplyF32( m_z, rScale );
        m_w = Helium::Simd::MultiplyF32( m_w, rScale );
    }

    /// Compute the dot product of this vector and another 4-component vector.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  Dot product.
    Helium::SimdVector Vector4Soa::Dot( const Vector4Soa& rVector ) const
    {
        Helium::SimdVector result = Helium::Simd::MultiplyF32( m_x, rVector.m_x );
        result = Helium::Simd::MultiplyAddF32( m_y, rVector.m_y, result );
        result = Helium::Simd::MultiplyAddF32( m_z, rVector.m_z, result );
        result = Helium::Simd::MultiplyAddF32( m_w, rVector.m_w, result );

        return result;
    }

    /// Get the magnitude of this vector.
    ///
    /// @return  Vector magnitude.
    Helium::SimdVector Vector4Soa::GetMagnitude() const
    {
        return Helium::Simd::SqrtF32( GetMagnitudeSquared() );
    }

    /// Get the squared magnitude of this vector.
    ///
    /// @return  Squared vector magnitude.
    Helium::SimdVector Vector4Soa::GetMagnitudeSquared() const
    {
        return Dot( *this );
    }

    /// Get a normalized copy of this vector, with safety threshold checking.
    ///
    /// If the magnitude of this vector is below the given epsilon, a unit vector pointing along the x-axis will be
    /// returned.
    ///
    /// @param[in] rEpsilon  Threshold at which to test for zero-length vectors.
    ///
    /// @return  Normalized copy of this vector.
    ///
    /// @see Normalize()
    Vector4Soa Vector4Soa::GetNormalized( const Helium::SimdVector& rEpsilon ) const
    {
        Vector4Soa result = *this;
        result.Normalize( rEpsilon );

        return result;
    }

    /// Normalize this vector, with safety threshold checking.
    ///
    /// If the magnitude of this vector is below the given epsilon, a unit vector pointing along the x-axis will be
    /// returned.
    ///
    /// @param[in] rEpsilon  Threshold at which to test for zero-length vectors.
    ///
    /// @see GetNormalized()
    void Vector4Soa::Normalize( const Helium::SimdVector& rEpsilon )
    {
        Helium::SimdVector magnitudeSquared = GetMagnitudeSquared();
        Helium::SimdVector epsilonSquared = Helium::Simd::MultiplyF32( rEpsilon, rEpsilon );

        Helium::SimdMask thresholdMask = Helium::Simd::GreaterEqualsF32( magnitudeSquared, epsilonSquared );

        Helium::SimdVector invMagnitude = Helium::Simd::InverseSqrtF32( magnitudeSquared );

        Helium::SimdVector normalizedX = Helium::Simd::MultiplyF32( m_x, invMagnitude );
        Helium::SimdVector normalizedY = Helium::Simd::MultiplyF32( m_y, invMagnitude );
        Helium::SimdVector normalizedZ = Helium::Simd::MultiplyF32( m_z, invMagnitude );
        Helium::SimdVector normalizedW = Helium::Simd::MultiplyF32( m_w, invMagnitude );

        Helium::SimdVector oneVec = Helium::Simd::SetSplatF32( 1.0f );

        m_x = Helium::Simd::Select( oneVec, normalizedX, thresholdMask );
        m_y = Helium::Simd::And( normalizedY, thresholdMask );
        m_z = Helium::Simd::And( normalizedZ, thresholdMask );
        m_w = Helium::Simd::And( normalizedW, thresholdMask );
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @return  Copy of this vector with the sign of each component flipped.
    ///
    /// @see Negate()
    Vector4Soa Vector4Soa::GetNegated() const
    {
        Vector4Soa result;
        GetNegated( result );

        return result;
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @param[out] rResult  Copy of this vector with the sign of each component flipped.
    ///
    /// @see Negate()
    void Vector4Soa::GetNegated( Vector4Soa& rResult ) const
    {
        Helium::SimdVector signFlip = Helium::Simd::SetSplatU32( 0x80000000 );

        rResult.m_x = Helium::Simd::Xor( m_x, signFlip );
        rResult.m_y = Helium::Simd::Xor( m_y, signFlip );
        rResult.m_z = Helium::Simd::Xor( m_z, signFlip );
        rResult.m_w = Helium::Simd::Xor( m_w, signFlip );
    }

    /// Flip the sign of each component of this vector.
    ///
    /// @see GetNegated()
    void Vector4Soa::Negate()
    {
        GetNegated( *this );
    }

    /// Test whether each component in this vector is equal to the corresponding component in another vector within a
    /// given threshold.
    ///
    /// @param[in] rVector   Vector.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for vectors that are equal within the given threshold.
    Helium::SimdMask Vector4Soa::Equals( const Vector4Soa& rVector, const Helium::SimdVector& rEpsilon ) const
    {
        Helium::SimdVector absMask = Helium::Simd::SetSplatU32( 0x7fffffff );

        Helium::SimdVector differenceX = Helium::Simd::SubtractF32( m_x, rVector.m_x );
        Helium::SimdVector differenceY = Helium::Simd::SubtractF32( m_y, rVector.m_y );
        Helium::SimdVector differenceZ = Helium::Simd::SubtractF32( m_z, rVector.m_z );
        Helium::SimdVector differenceW = Helium::Simd::SubtractF32( m_w, rVector.m_w );

        differenceX = Helium::Simd::And( differenceX, absMask );
        differenceY = Helium::Simd::And( differenceY, absMask );
        differenceZ = Helium::Simd::And( differenceZ, absMask );
        differenceW = Helium::Simd::And( differenceW, absMask );

        Helium::SimdMask thresholdMaskX = Helium::Simd::LessEqualsF32( differenceX, rEpsilon );
        Helium::SimdMask thresholdMaskY = Helium::Simd::LessEqualsF32( differenceY, rEpsilon );
        Helium::SimdMask thresholdMaskZ = Helium::Simd::LessEqualsF32( differenceZ, rEpsilon );
        Helium::SimdMask thresholdMaskW = Helium::Simd::LessEqualsF32( differenceW, rEpsilon );

        return Helium::Simd::MaskAnd(
            Helium::Simd::MaskAnd( Helium::Simd::MaskAnd( thresholdMaskX, thresholdMaskY ), thresholdMaskZ ),
            thresholdMaskW );
    }

    /// Test whether any component in this vector is not equal to the corresponding component in another vector within a
    /// given threshold.
    ///
    /// @param[in] rVector   Vector.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for vectors that are not equal within the given threshold.
    Helium::SimdMask Vector4Soa::NotEquals( const Vector4Soa& rVector, const Helium::SimdVector& rEpsilon ) const
    {
        Helium::SimdVector absMask = Helium::Simd::SetSplatU32( 0x7fffffff );

        Helium::SimdVector differenceX = Helium::Simd::SubtractF32( m_x, rVector.m_x );
        Helium::SimdVector differenceY = Helium::Simd::SubtractF32( m_y, rVector.m_y );
        Helium::SimdVector differenceZ = Helium::Simd::SubtractF32( m_z, rVector.m_z );
        Helium::SimdVector differenceW = Helium::Simd::SubtractF32( m_w, rVector.m_w );

        differenceX = Helium::Simd::And( differenceX, absMask );
        differenceY = Helium::Simd::And( differenceY, absMask );
        differenceZ = Helium::Simd::And( differenceZ, absMask );
        differenceW = Helium::Simd::And( differenceW, absMask );

        Helium::SimdMask thresholdMaskX = Helium::Simd::GreaterF32( differenceX, rEpsilon );
        Helium::SimdMask thresholdMaskY = Helium::Simd::GreaterF32( differenceY, rEpsilon );
        Helium::SimdMask thresholdMaskZ = Helium::Simd::GreaterF32( differenceZ, rEpsilon );
        Helium::SimdMask thresholdMaskW = Helium::Simd::GreaterF32( differenceW, rEpsilon );

        return Helium::Simd::MaskOr(
            Helium::Simd::MaskOr( Helium::Simd::MaskOr( thresholdMaskX, thresholdMaskY ), thresholdMaskZ ),
            thresholdMaskW );
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @return  Copy of this vector with the sign of each component flipped.
    Vector4Soa Vector4Soa::operator-() const
    {
        return GetNegated();
    }

    /// Perform a component-wise addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Vector containing the sums of each component.
    Vector4Soa Vector4Soa::operator+( const Vector4Soa& rVector ) const
    {
        return Add( rVector );
    }

    /// Perform a component-wise subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Vector containing the differences of each component.
    Vector4Soa Vector4Soa::operator-( const Vector4Soa& rVector ) const
    {
        return Subtract( rVector );
    }

    /// Perform a component-wise multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Vector containing the products of each component.
    Vector4Soa Vector4Soa::operator*( const Vector4Soa& rVector ) const
    {
        return Multiply( rVector );
    }

    /// Perform a component-wise division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Vector containing the quotients of each component.
    Vector4Soa Vector4Soa::operator/( const Vector4Soa& rVector ) const
    {
        return Divide( rVector );
    }

    /// Perform a component-wise in-place addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Reference to this vector.
    Vector4Soa& Vector4Soa::operator+=( const Vector4Soa& rVector )
    {
        AddSet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Reference to this vector.
    Vector4Soa& Vector4Soa::operator-=( const Vector4Soa& rVector )
    {
        SubtractSet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Reference to this vector.
    Vector4Soa& Vector4Soa::operator*=( const Vector4Soa& rVector )
    {
        MultiplySet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Reference to this vector.
    Vector4Soa& Vector4Soa::operator/=( const Vector4Soa& rVector )
    {
        DivideSet( *this, rVector );

        return *this;
    }

    /// Get this vector scaled by a given set of scalar values.
    ///
    /// @param[in] rScale  Amount by which to scale.
    ///
    /// @return  Copy of this vector scaled by the specified amount.
    Vector4Soa Vector4Soa::operator*( const Helium::SimdVector& rScale ) const
    {
        return GetScaled( rScale );
    }

    /// Scale this vector by a given set of scalar values.
    ///
    /// @param[in] rScale  Amount by which to scale.
    ///
    /// @return  Reference to this vector.
    Vector4Soa& Vector4Soa::operator*=( const Helium::SimdVector& rScale )
    {
        Scale( rScale );

        return *this;
    }

    /// Test whether each component in this vector is equal to the corresponding component in another vector within a
    /// default threshold.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  SIMD mask with bits set for vectors that are equal within the given threshold.
    Helium::SimdMask Vector4Soa::operator==( const Vector4Soa& rVector ) const
    {
        return Equals( rVector );
    }

    /// Test whether any component in this vector is not equal to the corresponding component in another vector within a
    /// default threshold.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  SIMD mask with bits set for vectors that are not equal within the given threshold.
    Helium::SimdMask Vector4Soa::operator!=( const Vector4Soa& rVector ) const
    {
        return NotEquals( rVector );
    }

    /// Get a scaled copy of a vector.
    ///
    /// @param[in] rScale   Amount by which to scale.
    /// @param[in] rVector  Vector to scale.
    ///
    /// @return  Scaled vector.
    Vector4Soa operator*( const Helium::SimdVector& rScale, const Vector4Soa& rVector )
    {
        return rVector.GetScaled( rScale );
    }
}
