/// Constructor.
///
/// This creates a vector with all components uninitialized.  Values should be assigned before use.
Helium::Simd::Vector3::Vector3()
{
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::Add( const Vector3& rVector ) const
{
    Vector3 result;
    result.AddSet( *this, rVector );

    return result;
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::Subtract( const Vector3& rVector ) const
{
    Vector3 result;
    result.SubtractSet( *this, rVector );

    return result;
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::Multiply( const Vector3& rVector ) const
{
    Vector3 result;
    result.MultiplySet( *this, rVector );

    return result;
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::Divide( const Vector3& rVector ) const
{
    Vector3 result;
    result.DivideSet( *this, rVector );

    return result;
}

/// Get this vector scaled by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
///
/// @see Scale()
Helium::Simd::Vector3 Helium::Simd::Vector3::GetScaled( float32_t scale ) const
{
    Vector3 result;
    result.MultiplySet( *this, Vector3( scale ) );

    return result;
}

/// Scale this vector by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @see GetScaled()
void Helium::Simd::Vector3::Scale( float32_t scale )
{
    MultiplySet( *this, Vector3( scale ) );
}

/// Compute the cross product of this vector with another 3-component vector.
///
/// @param[in] rVector  Vector.
///
/// @return  Cross product.
Helium::Simd::Vector3 Helium::Simd::Vector3::Cross( const Vector3& rVector ) const
{
    Vector3 result;
    result.CrossSet( *this, rVector );

    return result;
}

/// Get the squared magnitude of this vector.
///
/// @return  Squared vector magnitude.
float32_t Helium::Simd::Vector3::GetMagnitudeSquared() const
{
    return Dot( *this );
}

/// Get a normalized copy of this vector, with safety threshold checking.
///
/// If the magnitude of this vector is below the given epsilon, a unit vector pointing along the x-axis will be
/// returned.
///
/// @param[in] epsilon  Threshold at which to test for zero-length vectors.
///
/// @return  Normalized copy of this vector.
///
/// @see Normalize()
Helium::Simd::Vector3 Helium::Simd::Vector3::GetNormalized( float32_t epsilon ) const
{
    Vector3 result = *this;
    result.Normalize( epsilon );

    return result;
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
///
/// @see Negate()
Helium::Simd::Vector3 Helium::Simd::Vector3::GetNegated() const
{
    Vector3 result;
    GetNegated( result );

    return result;
}

/// Flip the sign of each component of this vector.
///
/// @see GetNegated()
void Helium::Simd::Vector3::Negate()
{
    GetNegated( *this );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator-() const
{
    return GetNegated();
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator+( const Vector3& rVector ) const
{
    return Add( rVector );
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator-( const Vector3& rVector ) const
{
    return Subtract( rVector );
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator*( const Vector3& rVector ) const
{
    return Multiply( rVector );
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator/( const Vector3& rVector ) const
{
    return Divide( rVector );
}

/// Perform a component-wise in-place addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Reference to this vector.
Helium::Simd::Vector3& Helium::Simd::Vector3::operator+=( const Vector3& rVector )
{
    AddSet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Reference to this vector.
Helium::Simd::Vector3& Helium::Simd::Vector3::operator-=( const Vector3& rVector )
{
    SubtractSet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Reference to this vector.
Helium::Simd::Vector3& Helium::Simd::Vector3::operator*=( const Vector3& rVector )
{
    MultiplySet( *this, rVector );

    return *this;
}

/// Perform a component-wise in-place division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Reference to this vector.
Helium::Simd::Vector3& Helium::Simd::Vector3::operator/=( const Vector3& rVector )
{
    DivideSet( *this, rVector );

    return *this;
}

/// Get this vector scaled by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
Helium::Simd::Vector3 Helium::Simd::Vector3::operator*( float32_t scale ) const
{
    return GetScaled( scale );
}

/// Scale this vector by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Reference to this vector.
Helium::Simd::Vector3& Helium::Simd::Vector3::operator*=( float32_t scale )
{
    Scale( scale );

    return *this;
}

/// Test whether each component in this vector is equal to the corresponding component in another vector within a
/// default threshold.
///
/// @param[in] rVector  Vector.
///
/// @return  True if this vector and the given vector are equal or nearly equal, false if not.
bool Helium::Simd::Vector3::operator==( const Vector3& rVector ) const
{
    return Equals( rVector );
}

/// Test whether any component in this vector is not equal to the corresponding component in another vector within a
/// default threshold.
///
/// @param[in] rVector  Vector.
///
/// @return  True if this vector and the given vector are not equal, false if they are or are nearly equal.
bool Helium::Simd::Vector3::operator!=( const Vector3& rVector ) const
{
    return !Equals( rVector );
}

/// Get a scaled copy of a vector.
///
/// @param[in] scale    Amount by which to scale.
/// @param[in] rVector  Vector to scale.
///
/// @return  Scaled vector.
Helium::Simd::Vector3 Helium::Simd::operator*( float32_t scale, const Helium::Simd::Vector3& rVector )
{
    return rVector.GetScaled( scale );
}