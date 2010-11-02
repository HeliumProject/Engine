//----------------------------------------------------------------------------------------------------------------------
// Matrix44.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a matrix with all components uninitialized.  Values should be assigned before use.
    Matrix44::Matrix44()
    {
    }

    /// Constructor.
    ///
    /// Initializes to a rotation matrix.
    ///
    /// @param[in] rRotation  Rotation to set.
    Matrix44::Matrix44( EInitRotation, const Quat& rRotation )
    {
        SetRotation( rRotation );
    }

    /// Constructor.
    ///
    /// Initializes to a translation matrix.
    ///
    /// @param[in] rTranslation  Translation to set.
    Matrix44::Matrix44( EInitTranslation, const Vector3& rTranslation )
    {
        SetTranslation( rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a translation matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rTranslation  Translation to set.
    Matrix44::Matrix44( EInitTranslation, const Vector4& rTranslation )
    {
        SetTranslation( rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a uniform scaling matrix.
    ///
    /// @param[in] scaling  Scaling factor.
    Matrix44::Matrix44( EInitScaling, float32_t scaling )
    {
        SetScaling( scaling );
    }

    /// Constructor.
    ///
    /// Initializes to a non-uniform scaling matrix.
    ///
    /// @param[in] rScaling  Vector specifying the scaling factors along each axis.
    Matrix44::Matrix44( EInitScaling, const Vector3& rScaling )
    {
        SetScaling( rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    Matrix44::Matrix44( EInitRotationTranslation, const Quat& rRotation, const Vector3& rTranslation )
    {
        SetRotationTranslation( rRotation, rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    Matrix44::Matrix44( EInitRotationTranslation, const Quat& rRotation, const Vector4& rTranslation )
    {
        SetRotationTranslation( rRotation, rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] scaling       Scaling to set.
    Matrix44::Matrix44(
        EInitRotationTranslationScaling,
        const Quat& rRotation,
        const Vector3& rTranslation,
        float32_t scaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, scaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] scaling       Scaling to set.
    Matrix44::Matrix44(
        EInitRotationTranslationScaling,
        const Quat& rRotation,
        const Vector4& rTranslation,
        float32_t scaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, scaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    Matrix44::Matrix44(
        EInitRotationTranslationScaling,
        const Quat& rRotation,
        const Vector3& rTranslation,
        const Vector3& rScaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    Matrix44::Matrix44(
        EInitRotationTranslationScaling,
        const Quat& rRotation,
        const Vector4& rTranslation,
        const Vector3& rScaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a perspective projection matrix.
    ///
    /// @param[in] horizontalFovRadians  Horizontal field-of-view angle, in radians.
    /// @param[in] aspectRatio           Aspect ratio (width to height).
    /// @param[in] nearClip              Near clip plane distance.
    /// @param[in] farClip               Far clip plane distance.
    Matrix44::Matrix44(
        EPerspectiveProjection,
        float32_t horizontalFovRadians,
        float32_t aspectRatio,
        float32_t nearClip,
        float32_t farClip )
    {
        SetPerspectiveProjection( horizontalFovRadians, aspectRatio, nearClip, farClip );
    }

    /// Constructor.
    ///
    /// Initializes to a perspective projection matrix with an infinite far clip plane.
    ///
    /// @param[in] horizontalFovRadians  Horizontal field-of-view angle, in radians.
    /// @param[in] aspectRatio           Aspect ratio (width to height).
    /// @param[in] nearClip              Near clip plane distance.
    /// @param[in] farClip               Far clip plane distance.
    Matrix44::Matrix44(
        EPerspectiveProjection,
        float32_t horizontalFovRadians,
        float32_t aspectRatio,
        float32_t nearClip )
    {
        SetPerspectiveProjection( horizontalFovRadians, aspectRatio, nearClip );
    }

    /// Constructor.
    ///
    /// Initializes to an orthogonal projection matrix.
    ///
    /// @param[in] width     View volume width.
    /// @param[in] height    View volume height.
    /// @param[in] nearClip  Near clip plane distance.
    /// @param[in] farClip   Far clip plane distance.
    Matrix44::Matrix44(
        EOrthogonalProjection,
        float32_t width,
        float32_t height,
        float32_t nearClip,
        float32_t farClip )
    {
        SetOrthogonalProjection( width, height, nearClip, farClip );
    }

    /// Perform a component-wise addition of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Matrix containing the sums of each component.
    Matrix44 Matrix44::Add( const Matrix44& rMatrix ) const
    {
        Matrix44 result;
        result.AddSet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise subtraction of a matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Matrix containing the differences of each component.
    Matrix44 Matrix44::Subtract( const Matrix44& rMatrix ) const
    {
        Matrix44 result;
        result.SubtractSet( *this, rMatrix );

        return result;
    }

    /// Perform a matrix multiplication between this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Product matrix.
    Matrix44 Matrix44::Multiply( const Matrix44& rMatrix ) const
    {
        Matrix44 result;
        result.MultiplySet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise multiplication of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Matrix containing the products of each component.
    Matrix44 Matrix44::MultiplyComponents( const Matrix44& rMatrix ) const
    {
        Matrix44 result;
        result.MultiplyComponentsSet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise division of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to divide.
    ///
    /// @return  Matrix containing the quotients of each component.
    Matrix44 Matrix44::DivideComponents( const Matrix44& rMatrix ) const
    {
        Matrix44 result;
        result.DivideComponentsSet( *this, rMatrix );

        return result;
    }

    /// Get the inverse of this matrix.
    ///
    /// @return  Matrix inverse.
    ///
    /// @see Invert()
    Matrix44 Matrix44::GetInverse() const
    {
        Matrix44 result;
        GetInverse( result );

        return result;
    }

    /// Set this matrix to its inverse.
    ///
    /// @see GetInverse()
    void Matrix44::Invert()
    {
        GetInverse( *this );
    }

    /// Get the transpose of this matrix.
    ///
    /// @return  Matrix transpose.
    ///
    /// @see Transpose()
    Matrix44 Matrix44::GetTranspose() const
    {
        Matrix44 result;
        GetTranspose( result );

        return result;
    }

    /// Set this matrix to its transpose.
    ///
    /// @see GetTranspose()
    void Matrix44::Transpose()
    {
        GetTranspose( *this );
    }

    /// Transform a 4-component vector.
    ///
    /// Note that transformation takes into account the vector w-component.
    ///
    /// @param[in] rVector  Vector to transform.
    ///
    /// @return  Transformed result.
    ///
    /// @see TransformPoint(), TransformVector()
    Vector4 Matrix44::Transform( const Vector4& rVector ) const
    {
        Vector4 result;
        Transform( rVector, result );

        return result;
    }

    /// Transform a 3-component vector as a point in 3D space.
    ///
    /// This is equivalent to calling Transform() on a Vector4 filled with the same values as the given vector, with the
    /// w-component set to 1.
    ///
    /// @param[in] rVector  Vector to transform.
    ///
    /// @return  Transformed result.
    ///
    /// @see TransformVector(), Transform()
    Vector3 Matrix44::TransformPoint( const Vector3& rVector ) const
    {
        Vector3 result;
        TransformPoint( rVector, result );

        return result;
    }

    /// Transform a 3-component vector as a directional vector in 3D space.
    ///
    /// This is equivalent to calling Transform() on a Vector4 filled with the same values as the given vector, with the
    /// w-component set to 0.
    ///
    /// @param[in] rVector  Vector to transform.
    ///
    /// @return  Transformed result.
    ///
    /// @see TransformPoint(), Transform()
    Vector3 Matrix44::TransformVector( const Vector3& rVector ) const
    {
        Vector3 result;
        TransformVector( rVector, result );

        return result;
    }

    /// Perform a component-wise addition of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Matrix containing the sums of each component.
    Matrix44 Matrix44::operator+( const Matrix44& rMatrix ) const
    {
        return Add( rMatrix );
    }

    /// Perform a component-wise subtraction of the given matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Matrix containing the differences of each component.
    Matrix44 Matrix44::operator-( const Matrix44& rMatrix ) const
    {
        return Subtract( rMatrix );
    }

    /// Perform a matrix multiplication between this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Product matrix.
    Matrix44 Matrix44::operator*( const Matrix44& rMatrix ) const
    {
        return Multiply( rMatrix );
    }

    /// Perform a component-wise in-place addition of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Reference to this matrix.
    Matrix44& Matrix44::operator+=( const Matrix44& rMatrix )
    {
        AddSet( *this, rMatrix );

        return *this;
    }

    /// Perform a component-wise in-place subtraction of the given matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Reference to this matrix.
    Matrix44& Matrix44::operator-=( const Matrix44& rMatrix )
    {
        SubtractSet( *this, rMatrix );

        return *this;
    }

    /// Perform in-place multiplication of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Reference to this matrix.
    Matrix44& Matrix44::operator*=( const Matrix44& rMatrix )
    {
        MultiplySet( *this, rMatrix );

        return *this;
    }

    /// Test whether each component in this matrix is equal to the corresponding component in another matrix within a
    /// default threshold.
    ///
    /// @param[in] rMatrix  Matrix.
    ///
    /// @return  True if this matrix and the given matrix are equal within the given threshold, false if not.
    bool Matrix44::operator==( const Matrix44& rMatrix ) const
    {
        return Equals( rMatrix );
    }

    /// Test whether any component in this matrix is not equal to the corresponding component in another matrix within a
    /// default threshold.
    ///
    /// @param[in] rMatrix  Matrix.
    ///
    /// @return  True if this matrix and the given matrix are not equal, false if they are or are nearly equal.
    bool Matrix44::operator!=( const Matrix44& rMatrix ) const
    {
        return !Equals( rMatrix );
    }
}
