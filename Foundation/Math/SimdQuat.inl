/// Constructor.
///
/// This creates a quaternion with all components uninitialized.  Values should be assigned before use.
Helium::Simd::Quat::Quat()
{
}

/// Constructor.
///
/// Initializes this quaternion to the given axis-angle rotation.
///
/// @param[in] rAxis  Axis of rotation.
/// @param[in] angle  Angle of rotation, in radians.
Helium::Simd::Quat::Quat( const Vector3& rAxis, float32_t angle )
{
    Set( rAxis, angle );
}

/// Constructor.
///
/// Initializes this quaternion to the given Euler angle rotation.
///
/// @param[in] pitch  Pitch, in radians.
/// @param[in] yaw    Yaw, in radians.
/// @param[in] roll   Roll, in radians.
Helium::Simd::Quat::Quat( float32_t pitch, float32_t yaw, float32_t roll )
{
    Set( pitch, yaw, roll );
}

/// Perform a component-wise addition of this quaternion and another quaternion.
///
/// @param[in] rQuat  Quaternion to add.
///
/// @return  Quaternion containing the sums of each component.
Helium::Simd::Quat Helium::Simd::Quat::Add( const Quat& rQuat ) const
{
    Quat result;
    result.AddSet( *this, rQuat );

    return result;
}

/// Perform a component-wise subtraction of a quaternion from this quaternion.
///
/// @param[in] rQuat  Quaternion to subtract.
///
/// @return  Quaternion containing the differences of each component.
Helium::Simd::Quat Helium::Simd::Quat::Subtract( const Quat& rQuat ) const
{
    Quat result;
    result.SubtractSet( *this, rQuat );

    return result;
}

/// Perform a quaternion multiplication between this quaternion and another quaternion.
///
/// @param[in] rQuat  Quaternion with which to multiply.
///
/// @return  Product quaternion.
Helium::Simd::Quat Helium::Simd::Quat::Multiply( const Quat& rQuat ) const
{
    Quat result;
    result.MultiplySet( *this, rQuat );

    return result;
}

/// Perform a component-wise multiplication of this quaternion and another quaternion.
///
/// @param[in] rQuat  Quaternion with which to multiply.
///
/// @return  Quaternion containing the products of each component.
Helium::Simd::Quat Helium::Simd::Quat::MultiplyComponents( const Quat& rQuat ) const
{
    Quat result;
    result.MultiplyComponentsSet( *this, rQuat );

    return result;
}

/// Perform a component-wise division of this quaternion and another quaternion.
///
/// @param[in] rQuat  Quaternion with which to divide.
///
/// @return  Quaternion containing the quotients of each component.
Helium::Simd::Quat Helium::Simd::Quat::DivideComponents( const Quat& rQuat ) const
{
    Quat result;
    result.DivideComponentsSet( *this, rQuat );

    return result;
}

/// Get a normalized copy of this quaternion, with safety threshold checking.
///
/// If the magnitude of this quaternion is below the given epsilon, an identity quaternion will be returned.
///
/// @param[in] epsilon  Threshold at which to test for zero-length quaternions.
///
/// @return  Normalized copy of this quaternion.
///
/// @see Normalize()
Helium::Simd::Quat Helium::Simd::Quat::GetNormalized( float32_t epsilon ) const
{
    Quat result = *this;
    result.Normalize( epsilon );

    return result;
}

/// Get the inverse of this quaternion.
///
/// @return  Quaternion inverse.
///
/// @see Invert(), GetConjugate(), SetConjugate()
Helium::Simd::Quat Helium::Simd::Quat::GetInverse() const
{
    Quat result;
    GetInverse( result );

    return result;
}

/// Set this quaternion to its inverse.
///
/// @see GetInverse(), GetConjugate(), SetConjugate()
void Helium::Simd::Quat::Invert()
{
    GetInverse( *this );
}

/// Get the conjugate of this quaternion.
///
/// @return  Quaternion conjugate.
///
/// @see SetConjugate(), GetInverse(), Invert()
Helium::Simd::Quat Helium::Simd::Quat::GetConjugate() const
{
    Quat result;
    GetConjugate( result );

    return result;
}

/// Set this quaternion to its conjugate.
///
/// @see GetConjugate(), GetInverse(), Invert()
void Helium::Simd::Quat::SetConjugate()
{
    GetConjugate( *this );
}

/// Perform a component-wise addition of this quaternion and the given quaternion.
///
/// @param[in] rQuat  Quaternion to add.
///
/// @return  Quaternion containing the sums of each component.
Helium::Simd::Quat Helium::Simd::Quat::operator+( const Quat& rQuat ) const
{
    return Add( rQuat );
}

/// Perform a component-wise subtraction of the given quaternion from this quaternion.
///
/// @param[in] rQuat  Quaternion to subtract.
///
/// @return  Quaternion containing the differences of each component.
Helium::Simd::Quat Helium::Simd::Quat::operator-( const Quat& rQuat ) const
{
    return Subtract( rQuat );
}

/// Perform a quaternion multiplication between this quaternion and another quaternion.
///
/// @param[in] rQuat  Quaternion with which to multiply.
///
/// @return  Product quaternion.
Helium::Simd::Quat Helium::Simd::Quat::operator*( const Quat& rQuat ) const
{
    return Multiply( rQuat );
}

/// Perform a component-wise in-place addition of this quaternion and the given quaternion.
///
/// @param[in] rQuat  Quaternion to add.
///
/// @return  Reference to this quaternion.
Helium::Simd::Quat& Helium::Simd::Quat::operator+=( const Quat& rQuat )
{
    AddSet( *this, rQuat );

    return *this;
}

/// Perform a component-wise in-place subtraction of the given quaternion from this quaternion.
///
/// @param[in] rQuat  Quaternion to subtract.
///
/// @return  Reference to this quaternion.
Helium::Simd::Quat& Helium::Simd::Quat::operator-=( const Quat& rQuat )
{
    SubtractSet( *this, rQuat );

    return *this;
}

/// Perform in-place multiplication of this quaternion and the given quaternion.
///
/// @param[in] rQuat  Quaternion with which to multiply.
///
/// @return  Reference to this quaternion.
Helium::Simd::Quat& Helium::Simd::Quat::operator*=( const Quat& rQuat )
{
    MultiplySet( *this, rQuat );

    return *this;
}

/// Test whether each component in this quaternion is equal to the corresponding component in another quaternion
/// within a default threshold.
///
/// @param[in] rQuat  Quaternion.
///
/// @return  True if this quaternion and the given quaternion are equal within the given threshold, false if not.
bool Helium::Simd::Quat::operator==( const Quat& rQuat ) const
{
    return Equals( rQuat );
}

/// Test whether any component in this quaternion is not equal to the corresponding component in another quaternion
/// within a default threshold.
///
/// @param[in] rQuat  Quaternion.
///
/// @return  True if this quaternion and the given quaternion are not equal, false if they are or are nearly equal.
bool Helium::Simd::Quat::operator!=( const Quat& rQuat ) const
{
    return !Equals( rQuat );
}
