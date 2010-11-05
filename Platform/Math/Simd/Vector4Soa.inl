/// Constructor.
///
/// This creates a vector with all components uninitialized.  Values should be assigned before use.
Helium::Simd::Vector4Soa::Vector4Soa()
{
}

/// Constructor.
///
/// @param[in] rX  X components.
/// @param[in] rY  Y components.
/// @param[in] rZ  Z components.
/// @param[in] rW  W components.
Helium::Simd::Vector4Soa::Vector4Soa( const Register& rX, const Register& rY, const Register& rZ, const Register& rW )
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
Helium::Simd::Vector4Soa::Vector4Soa( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
{
    Load( pX, pY, pZ, pW );
}

/// Constructor.
///
/// This will initialize this vector by splatting each component of the given vector across a full SIMD vector.
///
/// @param[in] rVector  Vector from which to initialize this vector.
Helium::Simd::Vector4Soa::Vector4Soa( const Vector4& rVector )
{
    Splat( rVector );
}

/// Fully load the SIMD vectors for each vector component.
///
/// @param[in] pX  X components (must be SIMD aligned).
/// @param[in] pY  Y components (must be SIMD aligned).
/// @param[in] pZ  Z components (must be SIMD aligned).
/// @param[in] pW  W components (must be SIMD aligned).
void Helium::Simd::Vector4Soa::Load( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
{
    m_x = Simd::LoadAligned( pX );
    m_y = Simd::LoadAligned( pY );
    m_z = Simd::LoadAligned( pZ );
    m_w = Simd::LoadAligned( pW );
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
void Helium::Simd::Vector4Soa::Load4Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
{
    m_x = Simd::LoadSplat128( pX );
    m_y = Simd::LoadSplat128( pY );
    m_z = Simd::LoadSplat128( pZ );
    m_w = Simd::LoadSplat128( pW );
}

/// Load 1 single-precision floating-point value for each vector component, splatting the value to fill.
///
/// @param[in] pX  X components (must be aligned to a 4-byte boundary).
/// @param[in] pY  Y components (must be aligned to a 4-byte boundary).
/// @param[in] pZ  Z components (must be aligned to a 4-byte boundary).
/// @param[in] pW  W components (must be aligned to a 4-byte boundary).
void Helium::Simd::Vector4Soa::Load1Splat( const float32_t* pX, const float32_t* pY, const float32_t* pZ, const float32_t* pW )
{
    m_x = Simd::LoadSplat32( pX );
    m_y = Simd::LoadSplat32( pY );
    m_z = Simd::LoadSplat32( pZ );
    m_w = Simd::LoadSplat32( pW );
}

/// Fully store the SIMD vectors from each vector component into memory.
///
/// @param[out] pX  X components (must be SIMD aligned).
/// @param[out] pY  Y components (must be SIMD aligned).
/// @param[out] pZ  Z components (must be SIMD aligned).
/// @param[out] pW  W components (must be SIMD aligned).
void Helium::Simd::Vector4Soa::Store( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
{
    Simd::StoreAligned( pX, m_x );
    Simd::StoreAligned( pY, m_y );
    Simd::StoreAligned( pZ, m_z );
    Simd::StoreAligned( pW, m_w );
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
void Helium::Simd::Vector4Soa::Store4( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
{
    Simd::Store128( pX, m_x );
    Simd::Store128( pY, m_y );
    Simd::Store128( pZ, m_z );
    Simd::Store128( pW, m_w );
}

/// Store the lowest single-precision floating-point value from each vector component into memory.
///
/// @param[out] pX  X components (must be aligned to a 4-byte boundary).
/// @param[out] pY  Y components (must be aligned to a 4-byte boundary).
/// @param[out] pZ  Z components (must be aligned to a 4-byte boundary).
/// @param[out] pW  W components (must be aligned to a 4-byte boundary).
void Helium::Simd::Vector4Soa::Store1( float32_t* pX, float32_t* pY, float32_t* pZ, float32_t* pW ) const
{
    Simd::Store32( pX, m_x );
    Simd::Store32( pY, m_y );
    Simd::Store32( pZ, m_z );
    Simd::Store32( pW, m_w );
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::Add( const Vector4Soa& rVector ) const
{
    return Vector4Soa(
        Simd::AddF32( m_x, rVector.m_x ),
        Simd::AddF32( m_y, rVector.m_y ),
        Simd::AddF32( m_z, rVector.m_z ),
        Simd::AddF32( m_w, rVector.m_w ) );
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::Subtract( const Vector4Soa& rVector ) const
{
    return Vector4Soa(
        Simd::SubtractF32( m_x, rVector.m_x ),
        Simd::SubtractF32( m_y, rVector.m_y ),
        Simd::SubtractF32( m_z, rVector.m_z ),
        Simd::SubtractF32( m_w, rVector.m_w ) );
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::Multiply( const Vector4Soa& rVector ) const
{
    return Vector4Soa(
        Simd::MultiplyF32( m_x, rVector.m_x ),
        Simd::MultiplyF32( m_y, rVector.m_y ),
        Simd::MultiplyF32( m_z, rVector.m_z ),
        Simd::MultiplyF32( m_w, rVector.m_w ) );
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::Divide( const Vector4Soa& rVector ) const
{
    return Vector4Soa(
        Simd::DivideF32( m_x, rVector.m_x ),
        Simd::DivideF32( m_y, rVector.m_y ),
        Simd::DivideF32( m_z, rVector.m_z ),
        Simd::DivideF32( m_w, rVector.m_w ) );
}

/// Set this vector to the component-wise sum of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4Soa::AddSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
{
    m_x = Simd::AddF32( rVector0.m_x, rVector1.m_x );
    m_y = Simd::AddF32( rVector0.m_y, rVector1.m_y );
    m_z = Simd::AddF32( rVector0.m_z, rVector1.m_z );
    m_w = Simd::AddF32( rVector0.m_w, rVector1.m_w );
}

/// Set this vector to the component-wise difference of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4Soa::SubtractSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
{
    m_x = Simd::SubtractF32( rVector0.m_x, rVector1.m_x );
    m_y = Simd::SubtractF32( rVector0.m_y, rVector1.m_y );
    m_z = Simd::SubtractF32( rVector0.m_z, rVector1.m_z );
    m_w = Simd::SubtractF32( rVector0.m_w, rVector1.m_w );
}

/// Set this vector to the component-wise product of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4Soa::MultiplySet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
{
    m_x = Simd::MultiplyF32( rVector0.m_x, rVector1.m_x );
    m_y = Simd::MultiplyF32( rVector0.m_y, rVector1.m_y );
    m_z = Simd::MultiplyF32( rVector0.m_z, rVector1.m_z );
    m_w = Simd::MultiplyF32( rVector0.m_w, rVector1.m_w );
}

/// Set this vector to the component-wise quotient of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector4Soa::DivideSet( const Vector4Soa& rVector0, const Vector4Soa& rVector1 )
{
    m_x = Simd::DivideF32( rVector0.m_x, rVector1.m_x );
    m_y = Simd::DivideF32( rVector0.m_y, rVector1.m_y );
    m_z = Simd::DivideF32( rVector0.m_z, rVector1.m_z );
    m_w = Simd::DivideF32( rVector0.m_w, rVector1.m_w );
}

/// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
///
/// @param[in] rVectorMul0  First vector to multiply.
/// @param[in] rVectorMul1  Second vector to multiply.
/// @param[in] rVectorAdd   Vector to add.
void Helium::Simd::Vector4Soa::MultiplyAddSet(
    const Vector4Soa& rVectorMul0,
    const Vector4Soa& rVectorMul1,
    const Vector4Soa& rVectorAdd )
{
    m_x = Simd::MultiplyAddF32( rVectorMul0.m_x, rVectorMul1.m_x, rVectorAdd.m_x );
    m_y = Simd::MultiplyAddF32( rVectorMul0.m_y, rVectorMul1.m_y, rVectorAdd.m_y );
    m_z = Simd::MultiplyAddF32( rVectorMul0.m_z, rVectorMul1.m_z, rVectorAdd.m_z );
    m_w = Simd::MultiplyAddF32( rVectorMul0.m_w, rVectorMul1.m_w, rVectorAdd.m_w );
}

/// Get this vector scaled by a given set of scalar values.
///
/// @param[in] rScale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
///
/// @see Scale()
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::GetScaled( const Register& rScale ) const
{
    return Vector4Soa(
        Simd::MultiplyF32( m_x, rScale ),
        Simd::MultiplyF32( m_y, rScale ),
        Simd::MultiplyF32( m_z, rScale ),
        Simd::MultiplyF32( m_w, rScale ) );
}

/// Scale this vector by a given set of scalar values.
///
/// @param[in] rScale  Amount by which to scale.
///
/// @see GetScaled()
void Helium::Simd::Vector4Soa::Scale( const Register& rScale )
{
    m_x = Simd::MultiplyF32( m_x, rScale );
    m_y = Simd::MultiplyF32( m_y, rScale );
    m_z = Simd::MultiplyF32( m_z, rScale );
    m_w = Simd::MultiplyF32( m_w, rScale );
}

/// Compute the dot product of this vector and another 4-component vector.
///
/// @param[in] rVector  Vector.
///
/// @return  Dot product.
Helium::Simd::Register Helium::Simd::Vector4Soa::Dot( const Vector4Soa& rVector ) const
{
    Register result = Simd::MultiplyF32( m_x, rVector.m_x );
    result = Simd::MultiplyAddF32( m_y, rVector.m_y, result );
    result = Simd::MultiplyAddF32( m_z, rVector.m_z, result );
    result = Simd::MultiplyAddF32( m_w, rVector.m_w, result );

    return result;
}

/// Get the magnitude of this vector.
///
/// @return  Vector magnitude.
Helium::Simd::Register Helium::Simd::Vector4Soa::GetMagnitude() const
{
    return Simd::SqrtF32( GetMagnitudeSquared() );
}

/// Get the squared magnitude of this vector.
///
/// @return  Squared vector magnitude.
Helium::Simd::Register Helium::Simd::Vector4Soa::GetMagnitudeSquared() const
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
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::GetNormalized( const Register& rEpsilon ) const
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
void Helium::Simd::Vector4Soa::Normalize( const Register& rEpsilon )
{
    Register magnitudeSquared = GetMagnitudeSquared();
    Register epsilonSquared = Simd::MultiplyF32( rEpsilon, rEpsilon );

    Mask thresholdMask = Simd::GreaterEqualsF32( magnitudeSquared, epsilonSquared );

    Register invMagnitude = Simd::InverseSqrtF32( magnitudeSquared );

    Register normalizedX = Simd::MultiplyF32( m_x, invMagnitude );
    Register normalizedY = Simd::MultiplyF32( m_y, invMagnitude );
    Register normalizedZ = Simd::MultiplyF32( m_z, invMagnitude );
    Register normalizedW = Simd::MultiplyF32( m_w, invMagnitude );

    Register oneVec = Simd::SetSplatF32( 1.0f );

    m_x = Simd::Select( oneVec, normalizedX, thresholdMask );
    m_y = Simd::And( normalizedY, thresholdMask );
    m_z = Simd::And( normalizedZ, thresholdMask );
    m_w = Simd::And( normalizedW, thresholdMask );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
///
/// @see Negate()
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::GetNegated() const
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
void Helium::Simd::Vector4Soa::GetNegated( Vector4Soa& rResult ) const
{
    Register signFlip = Simd::SetSplatU32( 0x80000000 );

    rResult.m_x = Simd::Xor( m_x, signFlip );
    rResult.m_y = Simd::Xor( m_y, signFlip );
    rResult.m_z = Simd::Xor( m_z, signFlip );
    rResult.m_w = Simd::Xor( m_w, signFlip );
}

/// Flip the sign of each component of this vector.
///
/// @see GetNegated()
void Helium::Simd::Vector4Soa::Negate()
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
Helium::Simd::Mask Helium::Simd::Vector4Soa::Equals( const Vector4Soa& rVector, const Register& rEpsilon ) const
{
    Register absMask = Simd::SetSplatU32( 0x7fffffff );

    Register differenceX = Simd::SubtractF32( m_x, rVector.m_x );
    Register differenceY = Simd::SubtractF32( m_y, rVector.m_y );
    Register differenceZ = Simd::SubtractF32( m_z, rVector.m_z );
    Register differenceW = Simd::SubtractF32( m_w, rVector.m_w );

    differenceX = Simd::And( differenceX, absMask );
    differenceY = Simd::And( differenceY, absMask );
    differenceZ = Simd::And( differenceZ, absMask );
    differenceW = Simd::And( differenceW, absMask );

    Mask thresholdMaskX = Simd::LessEqualsF32( differenceX, rEpsilon );
    Mask thresholdMaskY = Simd::LessEqualsF32( differenceY, rEpsilon );
    Mask thresholdMaskZ = Simd::LessEqualsF32( differenceZ, rEpsilon );
    Mask thresholdMaskW = Simd::LessEqualsF32( differenceW, rEpsilon );

    return Simd::MaskAnd(
        Simd::MaskAnd( Simd::MaskAnd( thresholdMaskX, thresholdMaskY ), thresholdMaskZ ),
        thresholdMaskW );
}

/// Test whether any component in this vector is not equal to the corresponding component in another vector within a
/// given threshold.
///
/// @param[in] rVector   Vector.
/// @param[in] rEpsilon  Comparison threshold.
///
/// @return  SIMD mask with bits set for vectors that are not equal within the given threshold.
Helium::Simd::Mask Helium::Simd::Vector4Soa::NotEquals( const Vector4Soa& rVector, const Register& rEpsilon ) const
{
    Register absMask = Simd::SetSplatU32( 0x7fffffff );

    Register differenceX = Simd::SubtractF32( m_x, rVector.m_x );
    Register differenceY = Simd::SubtractF32( m_y, rVector.m_y );
    Register differenceZ = Simd::SubtractF32( m_z, rVector.m_z );
    Register differenceW = Simd::SubtractF32( m_w, rVector.m_w );

    differenceX = Simd::And( differenceX, absMask );
    differenceY = Simd::And( differenceY, absMask );
    differenceZ = Simd::And( differenceZ, absMask );
    differenceW = Simd::And( differenceW, absMask );

    Mask thresholdMaskX = Simd::GreaterF32( differenceX, rEpsilon );
    Mask thresholdMaskY = Simd::GreaterF32( differenceY, rEpsilon );
    Mask thresholdMaskZ = Simd::GreaterF32( differenceZ, rEpsilon );
    Mask thresholdMaskW = Simd::GreaterF32( differenceW, rEpsilon );

    return Simd::MaskOr(
        Simd::MaskOr( Simd::MaskOr( thresholdMaskX, thresholdMaskY ), thresholdMaskZ ),
        thresholdMaskW );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator-() const
{
    return GetNegated();
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator+( const Vector4Soa& rVector ) const
{
    return Add( rVector );
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator-( const Vector4Soa& rVector ) const
{
    return Subtract( rVector );
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator*( const Vector4Soa& rVector ) const
{
    return Multiply( rVector );
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator/( const Vector4Soa& rVector ) const
{
    return Divide( rVector );
}

/// Perform a component-wise in-place addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Reference to this vector.
Helium::Simd::Vector4Soa& Helium::Simd::Vector4Soa::operator+=( const Vector4Soa& rVector )
{
    AddSet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Reference to this vector.
Helium::Simd::Vector4Soa& Helium::Simd::Vector4Soa::operator-=( const Vector4Soa& rVector )
{
    SubtractSet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Reference to this vector.
Helium::Simd::Vector4Soa& Helium::Simd::Vector4Soa::operator*=( const Vector4Soa& rVector )
{
    MultiplySet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Reference to this vector.
Helium::Simd::Vector4Soa& Helium::Simd::Vector4Soa::operator/=( const Vector4Soa& rVector )
{
    DivideSet( *this, rVector );

    return *this;
}

/// Get this vector scaled by a given set of scalar values.
///
/// @param[in] rScale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
Helium::Simd::Vector4Soa Helium::Simd::Vector4Soa::operator*( const Register& rScale ) const
{
    return GetScaled( rScale );
}

/// Scale this vector by a given set of scalar values.
///
/// @param[in] rScale  Amount by which to scale.
///
/// @return  Reference to this vector.
Helium::Simd::Vector4Soa& Helium::Simd::Vector4Soa::operator*=( const Register& rScale )
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
Helium::Simd::Mask Helium::Simd::Vector4Soa::operator==( const Vector4Soa& rVector ) const
{
    return Equals( rVector );
}

/// Test whether any component in this vector is not equal to the corresponding component in another vector within a
/// default threshold.
///
/// @param[in] rVector  Vector.
///
/// @return  SIMD mask with bits set for vectors that are not equal within the given threshold.
Helium::Simd::Mask Helium::Simd::Vector4Soa::operator!=( const Vector4Soa& rVector ) const
{
    return NotEquals( rVector );
}

/// Get a scaled copy of a vector.
///
/// @param[in] rScale   Amount by which to scale.
/// @param[in] rVector  Vector to scale.
///
/// @return  Scaled vector.
Helium::Simd::Vector4Soa Helium::Simd::operator*( const Register& rScale, const Vector4Soa& rVector )
{
    return rVector.GetScaled( rScale );
}
