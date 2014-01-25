/// Constructor.
///
/// This creates a vector with all components uninitialized.  Values should be assigned before use.
Helium::Simd::Vector2::Vector2()
{
}

/// Constructor.
///
/// @param[in] x  X-coordinate value.
/// @param[in] y  Y-coordinate value.
Helium::Simd::Vector2::Vector2( float32_t x, float32_t y )
{
	m_x = x;
	m_y = y;
}

/// Constructor.
///
/// @param[in] s  Scalar value to which each component of this vector should be set.
Helium::Simd::Vector2::Vector2( float32_t s )
{
	m_x = s;
	m_y = s;
}

/// Get the x-component of this vector.
///
/// @return  Reference to the vector x-component.
///
/// @see GetY(), GetElement(), SetX(), SetY(), SetElement()
float32_t& Helium::Simd::Vector2::GetX()
{
	return m_x;
}

/// Get the x-component of this vector.
///
/// @return  Vector x-component.
///
/// @see GetY(), GetElement(), SetX(), SetY(), SetElement()
float32_t Helium::Simd::Vector2::GetX() const
{
	return m_x;
}

/// Set the x-component of this vector.
///
/// @param[in] x  Value to which the x-component should be set.
///
/// @see SetY(), SetElement(), GetX(), GetY(), GetElement()
void Helium::Simd::Vector2::SetX( float32_t x )
{
	m_x = x;
}

/// Get the y-component of this vector.
///
/// @return  Reference to the vector y-component.
///
/// @see GetX(), GetElement(), SetY(), SetX(), SetElement()
float32_t& Helium::Simd::Vector2::GetY()
{
	return m_y;
}

/// Get the y-component of this vector.
///
/// @return  Vector y-component.
///
/// @see GetX(), GetElement(), SetY(), SetX(), SetElement()
float32_t Helium::Simd::Vector2::GetY() const
{
	return m_y;
}

/// Set the y-component of this vector.
///
/// @param[in] y  Value to which the y-component should be set.
///
/// @see SetX(), SetElement(), GetY(), GetX(), GetElement()
void Helium::Simd::Vector2::SetY( float32_t y )
{
	m_y = y;
}

/// Get the vector element stored at the specified index.
///
/// @param[in] index  Index of the element to retrieve (less than 2).
///
/// @return  Reference to the value stored at the specified element.
///
/// @see GetX(), GetY(), SetElement(), SetX(), SetY()
float32_t& Helium::Simd::Vector2::GetElement( size_t index )
{
	HELIUM_ASSERT( index < 2 );

	return m_vectorAsFloatArray[ index ];
}

/// Get the vector element stored at the specified index.
///
/// @param[in] index  Index of the element to retrieve (less than 2).
///
/// @return  Value stored at the specified element.
///
/// @see GetX(), GetY(), SetElement(), SetX(), SetY()
float32_t Helium::Simd::Vector2::GetElement( size_t index ) const
{
	HELIUM_ASSERT( index < 2 );

	return m_vectorAsFloatArray[ index ];
}

/// Set the vector element at the specified index.
///
/// @param[in] index  Index of the element to set (less than 2).
/// @param[in] value  Value to set.
///
/// @see SetX(), SetY(), GetElement(), GetX(), GetY()
void Helium::Simd::Vector2::SetElement( size_t index, float32_t value )
{
	HELIUM_ASSERT( index < 2 );

	m_vectorAsFloatArray[ index ] = value;
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::Add( const Vector2& rVector ) const
{
	return Vector2( m_x + rVector.m_x, m_y + rVector.m_y );
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::Subtract( const Vector2& rVector ) const
{
	return Vector2( m_x - rVector.m_x, m_y - rVector.m_y );
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::Multiply( const Vector2& rVector ) const
{
	return Vector2( m_x * rVector.m_x, m_y * rVector.m_y );
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::Divide( const Vector2& rVector ) const
{
	return Vector2( m_x / rVector.m_x, m_y / rVector.m_y );
}

/// Set this vector to the component-wise sum of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector2::AddSet( const Vector2& rVector0, const Vector2& rVector1 )
{
	m_x = rVector0.m_x + rVector1.m_x;
	m_y = rVector0.m_y + rVector1.m_y;
}

/// Set this vector to the component-wise difference of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector2::SubtractSet( const Vector2& rVector0, const Vector2& rVector1 )
{
	m_x = rVector0.m_x - rVector1.m_x;
	m_y = rVector0.m_y - rVector1.m_y;
}

/// Set this vector to the component-wise product of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector2::MultiplySet( const Vector2& rVector0, const Vector2& rVector1 )
{
	m_x = rVector0.m_x * rVector1.m_x;
	m_y = rVector0.m_y * rVector1.m_y;
}

/// Set this vector to the component-wise quotient of two vectors.
///
/// @param[in] rVector0  First vector.
/// @param[in] rVector1  Second vector.
void Helium::Simd::Vector2::DivideSet( const Vector2& rVector0, const Vector2& rVector1 )
{
	m_x = rVector0.m_x / rVector1.m_x;
	m_y = rVector0.m_y / rVector1.m_y;
}

/// Set this vector to the component-wise product of two vectors, summed with the components of a third vector.
///
/// @param[in] rVectorMul0  First vector to multiply.
/// @param[in] rVectorMul1  Second vector to multiply.
/// @param[in] rVectorAdd   Vector to add.
void Helium::Simd::Vector2::MultiplyAddSet( const Vector2& rVectorMul0, const Vector2& rVectorMul1, const Vector2& rVectorAdd )
{
	m_x = rVectorMul0.m_x * rVectorMul1.m_x + rVectorAdd.m_x;
	m_y = rVectorMul0.m_y * rVectorMul1.m_y + rVectorAdd.m_y;
}

/// Get this vector scaled by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
///
/// @see Scale()
Helium::Simd::Vector2 Helium::Simd::Vector2::GetScaled( float32_t scale ) const
{
	return Vector2( m_x * scale, m_y * scale );
}

/// Scale this vector by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @see GetScaled()
void Helium::Simd::Vector2::Scale( float32_t scale )
{
	m_x *= scale;
	m_y *= scale;
}

/// Compute the dot product of this vector and another 2-component vector.
///
/// @param[in] rVector  Vector.
///
/// @return  Dot product.
float32_t Helium::Simd::Vector2::Dot( const Vector2& rVector ) const
{
	return ( m_x * rVector.m_x + m_y * rVector.m_y );
}

/// Compute the cross product of this vector with another 3-component vector.
///
/// @param[in] rVector  Vector.
///
/// @return  Cross product.
float32_t Helium::Simd::Vector2::Cross( const Vector2& rVector ) const
{
	return ( m_x * rVector.m_y - m_y * rVector.m_x );
}

/// Get the magnitude of this vector.
///
/// @return  Vector magnitude.
float32_t Helium::Simd::Vector2::GetMagnitude() const
{
	return Sqrt( Square( m_x ) + Square( m_y ) );
}

/// Get the squared magnitude of this vector.
///
/// @return  Squared vector magnitude.
float32_t Helium::Simd::Vector2::GetMagnitudeSquared() const
{
	return ( Square( m_x ) + Square( m_y ) );
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
Helium::Simd::Vector2 Helium::Simd::Vector2::GetNormalized( float32_t epsilon ) const
{
	Vector2 result = *this;
	result.Normalize( epsilon );

	return result;
}

/// Normalize this vector, with safety threshold checking.
///
/// If the magnitude of this vector is below the given epsilon, a unit vector pointing along the x-axis will be
/// returned.
///
/// @param[in] epsilon  Threshold at which to test for zero-length vectors.
///
/// @see GetNormalized()
void Helium::Simd::Vector2::Normalize( float32_t epsilon )
{
	float32_t magnitudeSquared = Square( m_x ) + Square( m_y );
	if( magnitudeSquared < Square( epsilon ) )
	{
		m_x = 1.0f;
		m_y = 0.0f;

		return;
	}

	float32_t invMagnitude = 1.0f / Sqrt( magnitudeSquared );
	m_x *= invMagnitude;
	m_y *= invMagnitude;
}

/// Get a normalized copy of this vector, with safety threshold checking.
///
/// If the magnitude of this vector is below the given epsilon, a zero vector will be returned
///
/// @param[in] epsilon  Threshold at which to test for zero-length vectors.
///
/// @return  Normalized copy of this vector.
///
/// @see Normalize()
Helium::Simd::Vector2 Helium::Simd::Vector2::GetNormalizedOrZeroed( float32_t epsilon ) const
{
	Vector2 result = *this;
	result.NormalizeOrZero( epsilon );

	return result;
}

/// Normalize this vector, with safety threshold checking.
///
/// If the magnitude of this vector is below the given epsilon, it will be zeroed
///
/// @param[in] epsilon  Threshold at which to test for zero-length vectors.
///
/// @see GetNormalized()
void Helium::Simd::Vector2::NormalizeOrZero( float32_t epsilon )
{
	float32_t magnitudeSquared = Square( m_x ) + Square( m_y );
	if( magnitudeSquared < Square( epsilon ) )
	{
		m_x = 0.0f;
		m_y = 0.0f;

		return;
	}

	float32_t invMagnitude = 1.0f / Sqrt( magnitudeSquared );
	m_x *= invMagnitude;
	m_y *= invMagnitude;
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
///
/// @see Negate()
Helium::Simd::Vector2 Helium::Simd::Vector2::GetNegated() const
{
	return Vector2( -m_x, -m_y );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @param[out] rResult  Copy of this vector with the sign of each component flipped.
///
/// @see Negate()
void Helium::Simd::Vector2::GetNegated( Vector2& rResult ) const
{
	rResult.m_x = -m_x;
	rResult.m_y = -m_y;
}

/// Flip the sign of each component of this vector.
///
/// @see GetNegated()
void Helium::Simd::Vector2::Negate()
{
	m_x = -m_x;
	m_y = -m_y;
}

/// Test whether each component in this vector is equal to the corresponding component in another vector within a
/// given threshold.
///
/// @param[in] rVector  Vector.
/// @param[in] epsilon  Comparison threshold.
///
/// @return  True if this vector and the given vector are equal within the given threshold, false if not.
bool Helium::Simd::Vector2::Equals( const Vector2& rVector, float32_t epsilon ) const
{
	return ( Abs( m_x - rVector.m_x ) <= epsilon &&
			 Abs( m_y - rVector.m_y ) <= epsilon );
}

/// Get a copy of this vector with the sign of each component flipped.
///
/// @return  Copy of this vector with the sign of each component flipped.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator-() const
{
	return GetNegated();
}

/// Perform a component-wise addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Vector containing the sums of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator+( const Vector2& rVector ) const
{
	return Add( rVector );
}

/// Perform a component-wise subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Vector containing the differences of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator-( const Vector2& rVector ) const
{
	return Subtract( rVector );
}

/// Perform a component-wise multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Vector containing the products of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator*( const Vector2& rVector ) const
{
	return Multiply( rVector );
}

/// Perform a component-wise division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Vector containing the quotients of each component.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator/( const Vector2& rVector ) const
{
	return Divide( rVector );
}

/// Perform a component-wise in-place addition of this vector and the given vector.
///
/// @param[in] rVector  Vector to add.
///
/// @return  Reference to this vector.
Helium::Simd::Vector2& Helium::Simd::Vector2::operator+=( const Vector2& rVector )
{
	AddSet( *this, rVector );

	return *this;
}

/// Perform a component-wise in-place subtraction of the given vector from this vector.
///
/// @param[in] rVector  Vector to subtract.
///
/// @return  Reference to this vector.
Helium::Simd::Vector2& Helium::Simd::Vector2::operator-=( const Vector2& rVector )
{
	SubtractSet( *this, rVector );

	return *this;
}

/// Perform a component-wise in-place multiplication of this vector and the given vector.
///
/// @param[in] rVector  Vector with which to multiply.
///
/// @return  Reference to this vector.
Helium::Simd::Vector2& Helium::Simd::Vector2::operator*=( const Vector2& rVector )
{
	MultiplySet( *this, rVector );

	return *this;
}

/// Perform a component-wise in-place division of this vector and the given vector.
///
/// @param[in] rVector  Vector by which to divide.
///
/// @return  Reference to this vector.
Helium::Simd::Vector2& Helium::Simd::Vector2::operator/=( const Vector2& rVector )
{
	DivideSet( *this, rVector );

	return *this;
}

/// Get this vector scaled by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Copy of this vector scaled by the specified amount.
Helium::Simd::Vector2 Helium::Simd::Vector2::operator*( float32_t scale ) const
{
	return GetScaled( scale );
}

/// Scale this vector by a given scalar value.
///
/// @param[in] scale  Amount by which to scale.
///
/// @return  Reference to this vector.
Helium::Simd::Vector2& Helium::Simd::Vector2::operator*=( float32_t scale )
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
bool Helium::Simd::Vector2::operator==( const Vector2& rVector ) const
{
	return Equals( rVector );
}

/// Test whether any component in this vector is not equal to the corresponding component in another vector within a
/// default threshold.
///
/// @param[in] rVector  Vector.
///
/// @return  True if this vector and the given vector are not equal, false if they are or are nearly equal.
bool Helium::Simd::Vector2::operator!=( const Vector2& rVector ) const
{
	return !Equals( rVector );
}

/// Get a scaled copy of a vector.
///
/// @param[in] scale    Amount by which to scale.
/// @param[in] rVector  Vector to scale.
///
/// @return  Scaled vector.
namespace Helium
{
	namespace Simd
	{
		Vector2 operator*( float32_t scale, const Vector2& rVector )
		{
			return rVector.GetScaled( scale );
		}
	}
}
