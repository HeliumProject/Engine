//----------------------------------------------------------------------------------------------------------------------
// Vector4.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Constructor.
    ///
    /// This creates a vector with all components uninitialized.  Values should be assigned before use.
    Vector4::Vector4()
    {
    }

    /// Perform a component-wise addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Vector containing the sums of each component.
    Vector4 Vector4::Add( const Vector4& rVector ) const
    {
        Vector4 result;
        result.AddSet( *this, rVector );

        return result;
    }

    /// Perform a component-wise subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Vector containing the differences of each component.
    Vector4 Vector4::Subtract( const Vector4& rVector ) const
    {
        Vector4 result;
        result.SubtractSet( *this, rVector );

        return result;
    }

    /// Perform a component-wise multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Vector containing the products of each component.
    Vector4 Vector4::Multiply( const Vector4& rVector ) const
    {
        Vector4 result;
        result.MultiplySet( *this, rVector );

        return result;
    }

    /// Perform a component-wise division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Vector containing the quotients of each component.
    Vector4 Vector4::Divide( const Vector4& rVector ) const
    {
        Vector4 result;
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
    Vector4 Vector4::GetScaled( float32_t scale ) const
    {
        Vector4 result;
        result.MultiplySet( *this, Vector4( scale ) );

        return result;
    }

    /// Scale this vector by a given scalar value.
    ///
    /// @param[in] scale  Amount by which to scale.
    ///
    /// @see GetScaled()
    void Vector4::Scale( float32_t scale )
    {
        MultiplySet( *this, Vector4( scale ) );
    }

    /// Get the squared magnitude of this vector.
    ///
    /// @return  Squared vector magnitude.
    float32_t Vector4::GetMagnitudeSquared() const
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
    Vector4 Vector4::GetNormalized( float32_t epsilon ) const
    {
        Vector4 result = *this;
        result.Normalize( epsilon );

        return result;
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @return  Copy of this vector with the sign of each component flipped.
    ///
    /// @see Negate()
    Vector4 Vector4::GetNegated() const
    {
        Vector4 result;
        GetNegated( result );

        return result;
    }

    /// Flip the sign of each component of this vector.
    ///
    /// @see GetNegated()
    void Vector4::Negate()
    {
        GetNegated( *this );
    }

    /// Get a copy of this vector with the sign of each component flipped.
    ///
    /// @return  Copy of this vector with the sign of each component flipped.
    Vector4 Vector4::operator-() const
    {
        return GetNegated();
    }

    /// Perform a component-wise addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Vector containing the sums of each component.
    Vector4 Vector4::operator+( const Vector4& rVector ) const
    {
        return Add( rVector );
    }

    /// Perform a component-wise subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Vector containing the differences of each component.
    Vector4 Vector4::operator-( const Vector4& rVector ) const
    {
        return Subtract( rVector );
    }

    /// Perform a component-wise multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Vector containing the products of each component.
    Vector4 Vector4::operator*( const Vector4& rVector ) const
    {
        return Multiply( rVector );
    }

    /// Perform a component-wise division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Vector containing the quotients of each component.
    Vector4 Vector4::operator/( const Vector4& rVector ) const
    {
        return Divide( rVector );
    }

    /// Perform a component-wise in-place addition of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector to add.
    ///
    /// @return  Reference to this vector.
    Vector4& Vector4::operator+=( const Vector4& rVector )
    {
        AddSet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place subtraction of the given vector from this vector.
    ///
    /// @param[in] rVector  Vector to subtract.
    ///
    /// @return  Reference to this vector.
    Vector4& Vector4::operator-=( const Vector4& rVector )
    {
        SubtractSet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place multiplication of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector with which to multiply.
    ///
    /// @return  Reference to this vector.
    Vector4& Vector4::operator*=( const Vector4& rVector )
    {
        MultiplySet( *this, rVector );

        return *this;
    }

    /// Perform a component-wise in-place division of this vector and the given vector.
    ///
    /// @param[in] rVector  Vector by which to divide.
    ///
    /// @return  Reference to this vector.
    Vector4& Vector4::operator/=( const Vector4& rVector )
    {
        DivideSet( *this, rVector );

        return *this;
    }

    /// Get this vector scaled by a given scalar value.
    ///
    /// @param[in] scale  Amount by which to scale.
    ///
    /// @return  Copy of this vector scaled by the specified amount.
    Vector4 Vector4::operator*( float32_t scale ) const
    {
        return GetScaled( scale );
    }

    /// Scale this vector by a given scalar value.
    ///
    /// @param[in] scale  Amount by which to scale.
    ///
    /// @return  Reference to this vector.
    Vector4& Vector4::operator*=( float32_t scale )
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
    bool Vector4::operator==( const Vector4& rVector ) const
    {
        return Equals( rVector );
    }

    /// Test whether any component in this vector is not equal to the corresponding component in another vector within a
    /// default threshold.
    ///
    /// @param[in] rVector  Vector.
    ///
    /// @return  True if this vector and the given vector are not equal, false if they are or are nearly equal.
    bool Vector4::operator!=( const Vector4& rVector ) const
    {
        return !Equals( rVector );
    }

    /// Get a scaled copy of a vector.
    ///
    /// @param[in] scale    Amount by which to scale.
    /// @param[in] rVector  Vector to scale.
    ///
    /// @return  Scaled vector.
    Vector4 operator*( float32_t scale, const Vector4& rVector )
    {
        return rVector.GetScaled( scale );
    }
}
