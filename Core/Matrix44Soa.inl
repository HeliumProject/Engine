//----------------------------------------------------------------------------------------------------------------------
// Matrix44Soa.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a matrix with all components uninitialized.  Values should be assigned before use.
    Matrix44Soa::Matrix44Soa()
    {
    }

    /// Constructor.
    ///
    /// @param[in] rXAxisX      X-axis, x components.
    /// @param[in] rXAxisY      X-axis, y components.
    /// @param[in] rXAxisZ      X-axis, z components.
    /// @param[in] rXAxisW      X-axis, w components.
    /// @param[in] rYAxisX      Y-axis, x components.
    /// @param[in] rYAxisY      Y-axis, y components.
    /// @param[in] rYAxisZ      Y-axis, z components.
    /// @param[in] rYAxisW      Y-axis, w components.
    /// @param[in] rZAxisX      Z-axis, x components.
    /// @param[in] rZAxisY      Z-axis, y components.
    /// @param[in] rZAxisZ      Z-axis, z components.
    /// @param[in] rZAxisW      Z-axis, w components.
    /// @param[in] rTranslateX  Translation, x components.
    /// @param[in] rTranslateY  Translation, y components.
    /// @param[in] rTranslateZ  Translation, z components.
    /// @param[in] rTranslateW  Translation, w components.
    Matrix44Soa::Matrix44Soa(
        const SimdVector& rXAxisX,
        const SimdVector& rXAxisY,
        const SimdVector& rXAxisZ,
        const SimdVector& rXAxisW,
        const SimdVector& rYAxisX,
        const SimdVector& rYAxisY,
        const SimdVector& rYAxisZ,
        const SimdVector& rYAxisW,
        const SimdVector& rZAxisX,
        const SimdVector& rZAxisY,
        const SimdVector& rZAxisZ,
        const SimdVector& rZAxisW,
        const SimdVector& rTranslateX,
        const SimdVector& rTranslateY,
        const SimdVector& rTranslateZ,
        const SimdVector& rTranslateW )
    {
        m_matrix[ 0 ][ 0 ] = rXAxisX;
        m_matrix[ 0 ][ 1 ] = rXAxisY;
        m_matrix[ 0 ][ 2 ] = rXAxisZ;
        m_matrix[ 0 ][ 3 ] = rXAxisW;
        m_matrix[ 1 ][ 0 ] = rYAxisX;
        m_matrix[ 1 ][ 1 ] = rYAxisY;
        m_matrix[ 1 ][ 2 ] = rYAxisZ;
        m_matrix[ 1 ][ 3 ] = rYAxisW;
        m_matrix[ 2 ][ 0 ] = rZAxisX;
        m_matrix[ 2 ][ 1 ] = rZAxisY;
        m_matrix[ 2 ][ 2 ] = rZAxisZ;
        m_matrix[ 2 ][ 3 ] = rZAxisW;
        m_matrix[ 3 ][ 0 ] = rTranslateX;
        m_matrix[ 3 ][ 1 ] = rTranslateY;
        m_matrix[ 3 ][ 2 ] = rTranslateZ;
        m_matrix[ 3 ][ 3 ] = rTranslateW;
    }

    /// Constructor.
    ///
    /// @param[in] rXAxis      X-axis values.
    /// @param[in] rYAxis      Y-axis values.
    /// @param[in] rZAxis      Z-axis values.
    /// @param[in] rTranslate  Translation values.
    Matrix44Soa::Matrix44Soa(
        const Vector4Soa& rXAxis,
        const Vector4Soa& rYAxis,
        const Vector4Soa& rZAxis,
        const Vector4Soa& rTranslate )
    {
        m_matrix[ 0 ][ 0 ] = rXAxis.m_x;
        m_matrix[ 0 ][ 1 ] = rXAxis.m_y;
        m_matrix[ 0 ][ 2 ] = rXAxis.m_z;
        m_matrix[ 0 ][ 3 ] = rXAxis.m_w;
        m_matrix[ 1 ][ 0 ] = rYAxis.m_x;
        m_matrix[ 1 ][ 1 ] = rYAxis.m_y;
        m_matrix[ 1 ][ 2 ] = rYAxis.m_z;
        m_matrix[ 1 ][ 3 ] = rYAxis.m_w;
        m_matrix[ 2 ][ 0 ] = rZAxis.m_x;
        m_matrix[ 2 ][ 1 ] = rZAxis.m_y;
        m_matrix[ 2 ][ 2 ] = rZAxis.m_z;
        m_matrix[ 2 ][ 3 ] = rZAxis.m_w;
        m_matrix[ 3 ][ 0 ] = rTranslate.m_x;
        m_matrix[ 3 ][ 1 ] = rTranslate.m_y;
        m_matrix[ 3 ][ 2 ] = rTranslate.m_z;
        m_matrix[ 3 ][ 3 ] = rTranslate.m_w;
    }

    /// Constructor.
    ///
    /// Initializes to a rotation matrix.
    ///
    /// @param[in] rRotation  Rotation to set.
    Matrix44Soa::Matrix44Soa( EInitRotation, const QuatSoa& rRotation )
    {
        SetRotation( rRotation );
    }

    /// Constructor.
    ///
    /// Initializes to a translation matrix.
    ///
    /// @param[in] rTranslation  Translation to set.
    Matrix44Soa::Matrix44Soa( EInitTranslation, const Vector3Soa& rTranslation )
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
    Matrix44Soa::Matrix44Soa( EInitTranslation, const Vector4Soa& rTranslation )
    {
        SetTranslation( rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a uniform scaling matrix.
    ///
    /// @param[in] rScaling  Scaling factor.
    Matrix44Soa::Matrix44Soa( EInitScaling, const SimdVector& rScaling )
    {
        SetScaling( rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a non-uniform scaling matrix.
    ///
    /// @param[in] rScaling  Vector specifying the scaling factors along each axis.
    Matrix44Soa::Matrix44Soa( EInitScaling, const Vector3Soa& rScaling )
    {
        SetScaling( rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    Matrix44Soa::Matrix44Soa( EInitRotationTranslation, const QuatSoa& rRotation, const Vector3Soa& rTranslation )
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
    Matrix44Soa::Matrix44Soa( EInitRotationTranslation, const QuatSoa& rRotation, const Vector4Soa& rTranslation )
    {
        SetRotationTranslation( rRotation, rTranslation );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Scaling to set.
    Matrix44Soa::Matrix44Soa(
        EInitRotationTranslationScaling,
        const QuatSoa& rRotation,
        const Vector3Soa& rTranslation,
        const SimdVector& rScaling )
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
    /// @param[in] rScaling      Scaling to set.
    Matrix44Soa::Matrix44Soa(
        EInitRotationTranslationScaling,
        const QuatSoa& rRotation,
        const Vector4Soa& rTranslation,
        const SimdVector& rScaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, rScaling );
    }

    /// Constructor.
    ///
    /// Initializes to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    Matrix44Soa::Matrix44Soa(
        EInitRotationTranslationScaling,
        const QuatSoa& rRotation,
        const Vector3Soa& rTranslation,
        const Vector3Soa& rScaling )
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
    Matrix44Soa::Matrix44Soa(
        EInitRotationTranslationScaling,
        const QuatSoa& rRotation,
        const Vector4Soa& rTranslation,
        const Vector3Soa& rScaling )
    {
        SetRotationTranslationScaling( rRotation, rTranslation, rScaling );
    }

    /// Constructor.
    ///
    /// This will fully load the SIMD vectors for each component from the given addresses.
    ///
    /// @param[in] pXAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pXAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pXAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pXAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pYAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pYAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pYAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pYAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pZAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pZAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pZAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pZAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pTranslateX  Translation, x component values (must be SIMD aligned).
    /// @param[in] pTranslateY  Translation, y component values (must be SIMD aligned).
    /// @param[in] pTranslateZ  Translation, z component values (must be SIMD aligned).
    /// @param[in] pTranslateW  Translation, w component values (must be SIMD aligned).
    Matrix44Soa::Matrix44Soa(
        const float32_t* pXAxisX,
        const float32_t* pXAxisY,
        const float32_t* pXAxisZ,
        const float32_t* pXAxisW,
        const float32_t* pYAxisX,
        const float32_t* pYAxisY,
        const float32_t* pYAxisZ,
        const float32_t* pYAxisW,
        const float32_t* pZAxisX,
        const float32_t* pZAxisY,
        const float32_t* pZAxisZ,
        const float32_t* pZAxisW,
        const float32_t* pTranslateX,
        const float32_t* pTranslateY,
        const float32_t* pTranslateZ,
        const float32_t* pTranslateW )
    {
        Load(
            pXAxisX,
            pXAxisY,
            pXAxisZ,
            pXAxisW,
            pYAxisX,
            pYAxisY,
            pYAxisZ,
            pYAxisW,
            pZAxisX,
            pZAxisY,
            pZAxisZ,
            pZAxisW,
            pTranslateX,
            pTranslateY,
            pTranslateZ,
            pTranslateW );
    }

    /// Constructor.
    ///
    /// This will initialize this matrix by splatting each component of the given matrix across a full SIMD vector.
    ///
    /// @param[in] rMatrix  Matrix from which to initialize this matrix.
    Matrix44Soa::Matrix44Soa( const Matrix44& rMatrix )
    {
        Splat( rMatrix );
    }

    /// Fully load the SIMD vectors for each matrix component.
    ///
    /// @param[in] pXAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pXAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pXAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pXAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pYAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pYAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pYAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pYAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pZAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[in] pZAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[in] pZAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[in] pZAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[in] pTranslateX  Translation, x component values (must be SIMD aligned).
    /// @param[in] pTranslateY  Translation, y component values (must be SIMD aligned).
    /// @param[in] pTranslateZ  Translation, z component values (must be SIMD aligned).
    /// @param[in] pTranslateW  Translation, w component values (must be SIMD aligned).
    void Matrix44Soa::Load(
        const float32_t* pXAxisX,
        const float32_t* pXAxisY,
        const float32_t* pXAxisZ,
        const float32_t* pXAxisW,
        const float32_t* pYAxisX,
        const float32_t* pYAxisY,
        const float32_t* pYAxisZ,
        const float32_t* pYAxisW,
        const float32_t* pZAxisX,
        const float32_t* pZAxisY,
        const float32_t* pZAxisZ,
        const float32_t* pZAxisW,
        const float32_t* pTranslateX,
        const float32_t* pTranslateY,
        const float32_t* pTranslateZ,
        const float32_t* pTranslateW )
    {
        m_matrix[ 0 ][ 0 ] = Simd::LoadAligned( pXAxisX );
        m_matrix[ 0 ][ 1 ] = Simd::LoadAligned( pXAxisY );
        m_matrix[ 0 ][ 2 ] = Simd::LoadAligned( pXAxisZ );
        m_matrix[ 0 ][ 3 ] = Simd::LoadAligned( pXAxisW );
        m_matrix[ 1 ][ 0 ] = Simd::LoadAligned( pYAxisX );
        m_matrix[ 1 ][ 1 ] = Simd::LoadAligned( pYAxisY );
        m_matrix[ 1 ][ 2 ] = Simd::LoadAligned( pYAxisZ );
        m_matrix[ 1 ][ 3 ] = Simd::LoadAligned( pYAxisW );
        m_matrix[ 2 ][ 0 ] = Simd::LoadAligned( pZAxisX );
        m_matrix[ 2 ][ 1 ] = Simd::LoadAligned( pZAxisY );
        m_matrix[ 2 ][ 2 ] = Simd::LoadAligned( pZAxisZ );
        m_matrix[ 2 ][ 3 ] = Simd::LoadAligned( pZAxisW );
        m_matrix[ 3 ][ 0 ] = Simd::LoadAligned( pTranslateX );
        m_matrix[ 3 ][ 1 ] = Simd::LoadAligned( pTranslateY );
        m_matrix[ 3 ][ 2 ] = Simd::LoadAligned( pTranslateZ );
        m_matrix[ 3 ][ 3 ] = Simd::LoadAligned( pTranslateW );
    }

    /// Load 4 single-precision floating-point values for each matrix component, splatting the values to fill.
    ///
    /// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
    /// same effect as Load().
    ///
    /// @param[in] pXAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[in] pXAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[in] pXAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[in] pXAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[in] pYAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[in] pYAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[in] pYAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[in] pYAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[in] pZAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[in] pZAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[in] pZAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[in] pZAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[in] pTranslateX  Translation, x component values (must be aligned to a 16-byte boundary).
    /// @param[in] pTranslateY  Translation, y component values (must be aligned to a 16-byte boundary).
    /// @param[in] pTranslateZ  Translation, z component values (must be aligned to a 16-byte boundary).
    /// @param[in] pTranslateW  Translation, w component values (must be aligned to a 16-byte boundary).
    void Matrix44Soa::Load4Splat(
        const float32_t* pXAxisX,
        const float32_t* pXAxisY,
        const float32_t* pXAxisZ,
        const float32_t* pXAxisW,
        const float32_t* pYAxisX,
        const float32_t* pYAxisY,
        const float32_t* pYAxisZ,
        const float32_t* pYAxisW,
        const float32_t* pZAxisX,
        const float32_t* pZAxisY,
        const float32_t* pZAxisZ,
        const float32_t* pZAxisW,
        const float32_t* pTranslateX,
        const float32_t* pTranslateY,
        const float32_t* pTranslateZ,
        const float32_t* pTranslateW )
    {
        m_matrix[ 0 ][ 0 ] = Simd::LoadSplat128( pXAxisX );
        m_matrix[ 0 ][ 1 ] = Simd::LoadSplat128( pXAxisY );
        m_matrix[ 0 ][ 2 ] = Simd::LoadSplat128( pXAxisZ );
        m_matrix[ 0 ][ 3 ] = Simd::LoadSplat128( pXAxisW );
        m_matrix[ 1 ][ 0 ] = Simd::LoadSplat128( pYAxisX );
        m_matrix[ 1 ][ 1 ] = Simd::LoadSplat128( pYAxisY );
        m_matrix[ 1 ][ 2 ] = Simd::LoadSplat128( pYAxisZ );
        m_matrix[ 1 ][ 3 ] = Simd::LoadSplat128( pYAxisW );
        m_matrix[ 2 ][ 0 ] = Simd::LoadSplat128( pZAxisX );
        m_matrix[ 2 ][ 1 ] = Simd::LoadSplat128( pZAxisY );
        m_matrix[ 2 ][ 2 ] = Simd::LoadSplat128( pZAxisZ );
        m_matrix[ 2 ][ 3 ] = Simd::LoadSplat128( pZAxisW );
        m_matrix[ 3 ][ 0 ] = Simd::LoadSplat128( pTranslateX );
        m_matrix[ 3 ][ 1 ] = Simd::LoadSplat128( pTranslateY );
        m_matrix[ 3 ][ 2 ] = Simd::LoadSplat128( pTranslateZ );
        m_matrix[ 3 ][ 3 ] = Simd::LoadSplat128( pTranslateW );
    }

    /// Load 1 single-precision floating-point value for each matrix component, splatting the value to fill.
    ///
    /// @param[in] pXAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[in] pXAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[in] pXAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[in] pXAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[in] pYAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[in] pYAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[in] pYAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[in] pYAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[in] pZAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[in] pZAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[in] pZAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[in] pZAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[in] pTranslateX  Translation, x component values (must be aligned to a 4-byte boundary).
    /// @param[in] pTranslateY  Translation, y component values (must be aligned to a 4-byte boundary).
    /// @param[in] pTranslateZ  Translation, z component values (must be aligned to a 4-byte boundary).
    /// @param[in] pTranslateW  Translation, w component values (must be aligned to a 4-byte boundary).
    void Matrix44Soa::Load1Splat(
        const float32_t* pXAxisX,
        const float32_t* pXAxisY,
        const float32_t* pXAxisZ,
        const float32_t* pXAxisW,
        const float32_t* pYAxisX,
        const float32_t* pYAxisY,
        const float32_t* pYAxisZ,
        const float32_t* pYAxisW,
        const float32_t* pZAxisX,
        const float32_t* pZAxisY,
        const float32_t* pZAxisZ,
        const float32_t* pZAxisW,
        const float32_t* pTranslateX,
        const float32_t* pTranslateY,
        const float32_t* pTranslateZ,
        const float32_t* pTranslateW )
    {
        m_matrix[ 0 ][ 0 ] = Simd::LoadSplat32( pXAxisX );
        m_matrix[ 0 ][ 1 ] = Simd::LoadSplat32( pXAxisY );
        m_matrix[ 0 ][ 2 ] = Simd::LoadSplat32( pXAxisZ );
        m_matrix[ 0 ][ 3 ] = Simd::LoadSplat32( pXAxisW );
        m_matrix[ 1 ][ 0 ] = Simd::LoadSplat32( pYAxisX );
        m_matrix[ 1 ][ 1 ] = Simd::LoadSplat32( pYAxisY );
        m_matrix[ 1 ][ 2 ] = Simd::LoadSplat32( pYAxisZ );
        m_matrix[ 1 ][ 3 ] = Simd::LoadSplat32( pYAxisW );
        m_matrix[ 2 ][ 0 ] = Simd::LoadSplat32( pZAxisX );
        m_matrix[ 2 ][ 1 ] = Simd::LoadSplat32( pZAxisY );
        m_matrix[ 2 ][ 2 ] = Simd::LoadSplat32( pZAxisZ );
        m_matrix[ 2 ][ 3 ] = Simd::LoadSplat32( pZAxisW );
        m_matrix[ 3 ][ 0 ] = Simd::LoadSplat32( pTranslateX );
        m_matrix[ 3 ][ 1 ] = Simd::LoadSplat32( pTranslateY );
        m_matrix[ 3 ][ 2 ] = Simd::LoadSplat32( pTranslateZ );
        m_matrix[ 3 ][ 3 ] = Simd::LoadSplat32( pTranslateW );
    }

    /// Fully store the SIMD vectors from each matrix component into memory.
    ///
    /// @param[out] pXAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[out] pXAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[out] pXAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[out] pXAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[out] pYAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[out] pYAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[out] pYAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[out] pYAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[out] pZAxisX      X-axis, x component values (must be SIMD aligned).
    /// @param[out] pZAxisY      X-axis, y component values (must be SIMD aligned).
    /// @param[out] pZAxisZ      X-axis, z component values (must be SIMD aligned).
    /// @param[out] pZAxisW      X-axis, w component values (must be SIMD aligned).
    /// @param[out] pTranslateX  Translation, x component values (must be SIMD aligned).
    /// @param[out] pTranslateY  Translation, y component values (must be SIMD aligned).
    /// @param[out] pTranslateZ  Translation, z component values (must be SIMD aligned).
    /// @param[out] pTranslateW  Translation, w component values (must be SIMD aligned).
    void Matrix44Soa::Store(
        float32_t* pXAxisX,
        float32_t* pXAxisY,
        float32_t* pXAxisZ,
        float32_t* pXAxisW,
        float32_t* pYAxisX,
        float32_t* pYAxisY,
        float32_t* pYAxisZ,
        float32_t* pYAxisW,
        float32_t* pZAxisX,
        float32_t* pZAxisY,
        float32_t* pZAxisZ,
        float32_t* pZAxisW,
        float32_t* pTranslateX,
        float32_t* pTranslateY,
        float32_t* pTranslateZ,
        float32_t* pTranslateW ) const
    {
        Simd::StoreAligned( pXAxisX, m_matrix[ 0 ][ 0 ] );
        Simd::StoreAligned( pXAxisY, m_matrix[ 0 ][ 1 ] );
        Simd::StoreAligned( pXAxisZ, m_matrix[ 0 ][ 2 ] );
        Simd::StoreAligned( pXAxisW, m_matrix[ 0 ][ 3 ] );
        Simd::StoreAligned( pYAxisX, m_matrix[ 1 ][ 0 ] );
        Simd::StoreAligned( pYAxisY, m_matrix[ 1 ][ 1 ] );
        Simd::StoreAligned( pYAxisZ, m_matrix[ 1 ][ 2 ] );
        Simd::StoreAligned( pYAxisW, m_matrix[ 1 ][ 3 ] );
        Simd::StoreAligned( pZAxisX, m_matrix[ 2 ][ 0 ] );
        Simd::StoreAligned( pZAxisY, m_matrix[ 2 ][ 1 ] );
        Simd::StoreAligned( pZAxisZ, m_matrix[ 2 ][ 2 ] );
        Simd::StoreAligned( pZAxisW, m_matrix[ 2 ][ 3 ] );
        Simd::StoreAligned( pTranslateX, m_matrix[ 3 ][ 0 ] );
        Simd::StoreAligned( pTranslateY, m_matrix[ 3 ][ 1 ] );
        Simd::StoreAligned( pTranslateZ, m_matrix[ 3 ][ 2 ] );
        Simd::StoreAligned( pTranslateW, m_matrix[ 3 ][ 3 ] );
    }

    /// Store the lowest 4 single-precision floating-point values from each matrix component into memory.
    ///
    /// If the current platform SIMD vector format is only large enough to contain 4 floats, this will have the exact
    /// same effect as Store().
    ///
    /// @param[out] pXAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[out] pXAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[out] pXAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[out] pXAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[out] pYAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[out] pYAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[out] pYAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[out] pYAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[out] pZAxisX      X-axis, x component values (must be aligned to a 16-byte boundary).
    /// @param[out] pZAxisY      X-axis, y component values (must be aligned to a 16-byte boundary).
    /// @param[out] pZAxisZ      X-axis, z component values (must be aligned to a 16-byte boundary).
    /// @param[out] pZAxisW      X-axis, w component values (must be aligned to a 16-byte boundary).
    /// @param[out] pTranslateX  Translation, x component values (must be aligned to a 16-byte boundary).
    /// @param[out] pTranslateY  Translation, y component values (must be aligned to a 16-byte boundary).
    /// @param[out] pTranslateZ  Translation, z component values (must be aligned to a 16-byte boundary).
    /// @param[out] pTranslateW  Translation, w component values (must be aligned to a 16-byte boundary).
    void Matrix44Soa::Store4(
        float32_t* pXAxisX,
        float32_t* pXAxisY,
        float32_t* pXAxisZ,
        float32_t* pXAxisW,
        float32_t* pYAxisX,
        float32_t* pYAxisY,
        float32_t* pYAxisZ,
        float32_t* pYAxisW,
        float32_t* pZAxisX,
        float32_t* pZAxisY,
        float32_t* pZAxisZ,
        float32_t* pZAxisW,
        float32_t* pTranslateX,
        float32_t* pTranslateY,
        float32_t* pTranslateZ,
        float32_t* pTranslateW ) const
    {
        Simd::Store128( pXAxisX, m_matrix[ 0 ][ 0 ] );
        Simd::Store128( pXAxisY, m_matrix[ 0 ][ 1 ] );
        Simd::Store128( pXAxisZ, m_matrix[ 0 ][ 2 ] );
        Simd::Store128( pXAxisW, m_matrix[ 0 ][ 3 ] );
        Simd::Store128( pYAxisX, m_matrix[ 1 ][ 0 ] );
        Simd::Store128( pYAxisY, m_matrix[ 1 ][ 1 ] );
        Simd::Store128( pYAxisZ, m_matrix[ 1 ][ 2 ] );
        Simd::Store128( pYAxisW, m_matrix[ 1 ][ 3 ] );
        Simd::Store128( pZAxisX, m_matrix[ 2 ][ 0 ] );
        Simd::Store128( pZAxisY, m_matrix[ 2 ][ 1 ] );
        Simd::Store128( pZAxisZ, m_matrix[ 2 ][ 2 ] );
        Simd::Store128( pZAxisW, m_matrix[ 2 ][ 3 ] );
        Simd::Store128( pTranslateX, m_matrix[ 3 ][ 0 ] );
        Simd::Store128( pTranslateY, m_matrix[ 3 ][ 1 ] );
        Simd::Store128( pTranslateZ, m_matrix[ 3 ][ 2 ] );
        Simd::Store128( pTranslateW, m_matrix[ 3 ][ 3 ] );
    }

    /// Store the lowest single-precision floating-point value from each matrix component into memory.
    ///
    /// @param[out] pXAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[out] pXAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[out] pXAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[out] pXAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[out] pYAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[out] pYAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[out] pYAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[out] pYAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[out] pZAxisX      X-axis, x component values (must be aligned to a 4-byte boundary).
    /// @param[out] pZAxisY      X-axis, y component values (must be aligned to a 4-byte boundary).
    /// @param[out] pZAxisZ      X-axis, z component values (must be aligned to a 4-byte boundary).
    /// @param[out] pZAxisW      X-axis, w component values (must be aligned to a 4-byte boundary).
    /// @param[out] pTranslateX  Translation, x component values (must be aligned to a 4-byte boundary).
    /// @param[out] pTranslateY  Translation, y component values (must be aligned to a 4-byte boundary).
    /// @param[out] pTranslateZ  Translation, z component values (must be aligned to a 4-byte boundary).
    /// @param[out] pTranslateW  Translation, w component values (must be aligned to a 4-byte boundary).
    void Matrix44Soa::Store1(
        float32_t* pXAxisX,
        float32_t* pXAxisY,
        float32_t* pXAxisZ,
        float32_t* pXAxisW,
        float32_t* pYAxisX,
        float32_t* pYAxisY,
        float32_t* pYAxisZ,
        float32_t* pYAxisW,
        float32_t* pZAxisX,
        float32_t* pZAxisY,
        float32_t* pZAxisZ,
        float32_t* pZAxisW,
        float32_t* pTranslateX,
        float32_t* pTranslateY,
        float32_t* pTranslateZ,
        float32_t* pTranslateW ) const
    {
        Simd::Store32( pXAxisX, m_matrix[ 0 ][ 0 ] );
        Simd::Store32( pXAxisY, m_matrix[ 0 ][ 1 ] );
        Simd::Store32( pXAxisZ, m_matrix[ 0 ][ 2 ] );
        Simd::Store32( pXAxisW, m_matrix[ 0 ][ 3 ] );
        Simd::Store32( pYAxisX, m_matrix[ 1 ][ 0 ] );
        Simd::Store32( pYAxisY, m_matrix[ 1 ][ 1 ] );
        Simd::Store32( pYAxisZ, m_matrix[ 1 ][ 2 ] );
        Simd::Store32( pYAxisW, m_matrix[ 1 ][ 3 ] );
        Simd::Store32( pZAxisX, m_matrix[ 2 ][ 0 ] );
        Simd::Store32( pZAxisY, m_matrix[ 2 ][ 1 ] );
        Simd::Store32( pZAxisZ, m_matrix[ 2 ][ 2 ] );
        Simd::Store32( pZAxisW, m_matrix[ 2 ][ 3 ] );
        Simd::Store32( pTranslateX, m_matrix[ 3 ][ 0 ] );
        Simd::Store32( pTranslateY, m_matrix[ 3 ][ 1 ] );
        Simd::Store32( pTranslateZ, m_matrix[ 3 ][ 2 ] );
        Simd::Store32( pTranslateW, m_matrix[ 3 ][ 3 ] );
    }

    /// Fill out a vector with the values for a given row of this matrix.
    ///
    /// @param[in]  index  Row index (less than 4).
    /// @param[out] rRow   Vector filled with the row values.
    ///
    /// @see SetRow()
    void Matrix44Soa::GetRow( size_t index, Vector4Soa& rRow ) const
    {
        L_ASSERT( index < 4 );

        const SimdVector* pRow = m_matrix[ index ];

        rRow.m_x = pRow[ 0 ];
        rRow.m_y = pRow[ 1 ];
        rRow.m_z = pRow[ 2 ];
        rRow.m_w = pRow[ 3 ];
    }

    /// Retrieve a vector containing the values for a given row of this matrix.
    ///
    /// @param[in] index  Row index (less than 4).
    ///
    /// @return  Vector containing the row values.
    ///
    /// @see SetRow()
    Vector4Soa Matrix44Soa::GetRow( size_t index ) const
    {
        L_ASSERT( index < 4 );

        const SimdVector* pRow = m_matrix[ index ];

        return Vector4Soa( pRow[ 0 ], pRow[ 1 ], pRow[ 2 ], pRow[ 3 ] );
    }

    /// Set the values for a given row of this matrix.
    ///
    /// @param[in] index  Row index (less than 4).
    /// @param[in] rRow   Row values.
    ///
    /// @see GetRow()
    void Matrix44Soa::SetRow( size_t index, const Vector4Soa& rRow )
    {
        L_ASSERT( index < 4 );

        SimdVector* pRow = m_matrix[ index ];

        pRow[ 0 ] = rRow.m_x;
        pRow[ 1 ] = rRow.m_y;
        pRow[ 2 ] = rRow.m_z;
        pRow[ 3 ] = rRow.m_w;
    }

    /// Perform a component-wise addition of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Matrix containing the sums of each component.
    Matrix44Soa Matrix44Soa::Add( const Matrix44Soa& rMatrix ) const
    {
        Matrix44Soa result;
        result.AddSet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise subtraction of a matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Matrix containing the differences of each component.
    Matrix44Soa Matrix44Soa::Subtract( const Matrix44Soa& rMatrix ) const
    {
        Matrix44Soa result;
        result.SubtractSet( *this, rMatrix );

        return result;
    }

    /// Perform a matrix multiplication between this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Product matrix.
    Matrix44Soa Matrix44Soa::Multiply( const Matrix44Soa& rMatrix ) const
    {
        Matrix44Soa result;
        result.MultiplySet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise multiplication of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Matrix containing the products of each component.
    Matrix44Soa Matrix44Soa::MultiplyComponents( const Matrix44Soa& rMatrix ) const
    {
        Matrix44Soa result;
        result.MultiplyComponentsSet( *this, rMatrix );

        return result;
    }

    /// Perform a component-wise division of this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to divide.
    ///
    /// @return  Matrix containing the quotients of each component.
    Matrix44Soa Matrix44Soa::DivideComponents( const Matrix44Soa& rMatrix ) const
    {
        Matrix44Soa result;
        result.DivideComponentsSet( *this, rMatrix );

        return result;
    }

    /// Set this matrix to the component-wise sum of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44Soa::AddSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 )
    {
        m_matrix[ 0 ][ 0 ] = Simd::AddF32( rMatrix0.m_matrix[ 0 ][ 0 ], rMatrix1.m_matrix[ 0 ][ 0 ] );
        m_matrix[ 0 ][ 1 ] = Simd::AddF32( rMatrix0.m_matrix[ 0 ][ 1 ], rMatrix1.m_matrix[ 0 ][ 1 ] );
        m_matrix[ 0 ][ 2 ] = Simd::AddF32( rMatrix0.m_matrix[ 0 ][ 2 ], rMatrix1.m_matrix[ 0 ][ 2 ] );
        m_matrix[ 0 ][ 3 ] = Simd::AddF32( rMatrix0.m_matrix[ 0 ][ 3 ], rMatrix1.m_matrix[ 0 ][ 3 ] );
        m_matrix[ 1 ][ 0 ] = Simd::AddF32( rMatrix0.m_matrix[ 1 ][ 0 ], rMatrix1.m_matrix[ 1 ][ 0 ] );
        m_matrix[ 1 ][ 1 ] = Simd::AddF32( rMatrix0.m_matrix[ 1 ][ 1 ], rMatrix1.m_matrix[ 1 ][ 1 ] );
        m_matrix[ 1 ][ 2 ] = Simd::AddF32( rMatrix0.m_matrix[ 1 ][ 2 ], rMatrix1.m_matrix[ 1 ][ 2 ] );
        m_matrix[ 1 ][ 3 ] = Simd::AddF32( rMatrix0.m_matrix[ 1 ][ 3 ], rMatrix1.m_matrix[ 1 ][ 3 ] );
        m_matrix[ 2 ][ 0 ] = Simd::AddF32( rMatrix0.m_matrix[ 2 ][ 0 ], rMatrix1.m_matrix[ 2 ][ 0 ] );
        m_matrix[ 2 ][ 1 ] = Simd::AddF32( rMatrix0.m_matrix[ 2 ][ 1 ], rMatrix1.m_matrix[ 2 ][ 1 ] );
        m_matrix[ 2 ][ 2 ] = Simd::AddF32( rMatrix0.m_matrix[ 2 ][ 2 ], rMatrix1.m_matrix[ 2 ][ 2 ] );
        m_matrix[ 2 ][ 3 ] = Simd::AddF32( rMatrix0.m_matrix[ 2 ][ 3 ], rMatrix1.m_matrix[ 2 ][ 3 ] );
        m_matrix[ 3 ][ 0 ] = Simd::AddF32( rMatrix0.m_matrix[ 3 ][ 0 ], rMatrix1.m_matrix[ 3 ][ 0 ] );
        m_matrix[ 3 ][ 1 ] = Simd::AddF32( rMatrix0.m_matrix[ 3 ][ 1 ], rMatrix1.m_matrix[ 3 ][ 1 ] );
        m_matrix[ 3 ][ 2 ] = Simd::AddF32( rMatrix0.m_matrix[ 3 ][ 2 ], rMatrix1.m_matrix[ 3 ][ 2 ] );
        m_matrix[ 3 ][ 3 ] = Simd::AddF32( rMatrix0.m_matrix[ 3 ][ 3 ], rMatrix1.m_matrix[ 3 ][ 3 ] );
    }

    /// Set this matrix to the component-wise difference of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44Soa::SubtractSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 )
    {
        m_matrix[ 0 ][ 0 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 0 ][ 0 ], rMatrix1.m_matrix[ 0 ][ 0 ] );
        m_matrix[ 0 ][ 1 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 0 ][ 1 ], rMatrix1.m_matrix[ 0 ][ 1 ] );
        m_matrix[ 0 ][ 2 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 0 ][ 2 ], rMatrix1.m_matrix[ 0 ][ 2 ] );
        m_matrix[ 0 ][ 3 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 0 ][ 3 ], rMatrix1.m_matrix[ 0 ][ 3 ] );
        m_matrix[ 1 ][ 0 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 1 ][ 0 ], rMatrix1.m_matrix[ 1 ][ 0 ] );
        m_matrix[ 1 ][ 1 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 1 ][ 1 ], rMatrix1.m_matrix[ 1 ][ 1 ] );
        m_matrix[ 1 ][ 2 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 1 ][ 2 ], rMatrix1.m_matrix[ 1 ][ 2 ] );
        m_matrix[ 1 ][ 3 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 1 ][ 3 ], rMatrix1.m_matrix[ 1 ][ 3 ] );
        m_matrix[ 2 ][ 0 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 2 ][ 0 ], rMatrix1.m_matrix[ 2 ][ 0 ] );
        m_matrix[ 2 ][ 1 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 2 ][ 1 ], rMatrix1.m_matrix[ 2 ][ 1 ] );
        m_matrix[ 2 ][ 2 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 2 ][ 2 ], rMatrix1.m_matrix[ 2 ][ 2 ] );
        m_matrix[ 2 ][ 3 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 2 ][ 3 ], rMatrix1.m_matrix[ 2 ][ 3 ] );
        m_matrix[ 3 ][ 0 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 3 ][ 0 ], rMatrix1.m_matrix[ 3 ][ 0 ] );
        m_matrix[ 3 ][ 1 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 3 ][ 1 ], rMatrix1.m_matrix[ 3 ][ 1 ] );
        m_matrix[ 3 ][ 2 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 3 ][ 2 ], rMatrix1.m_matrix[ 3 ][ 2 ] );
        m_matrix[ 3 ][ 3 ] = Simd::SubtractF32( rMatrix0.m_matrix[ 3 ][ 3 ], rMatrix1.m_matrix[ 3 ][ 3 ] );
    }

    /// Set this matrix to the component-wise product of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44Soa::MultiplyComponentsSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 )
    {
        m_matrix[ 0 ][ 0 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ][ 0 ], rMatrix1.m_matrix[ 0 ][ 0 ] );
        m_matrix[ 0 ][ 1 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ][ 1 ], rMatrix1.m_matrix[ 0 ][ 1 ] );
        m_matrix[ 0 ][ 2 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ][ 2 ], rMatrix1.m_matrix[ 0 ][ 2 ] );
        m_matrix[ 0 ][ 3 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 0 ][ 3 ], rMatrix1.m_matrix[ 0 ][ 3 ] );
        m_matrix[ 1 ][ 0 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ][ 0 ], rMatrix1.m_matrix[ 1 ][ 0 ] );
        m_matrix[ 1 ][ 1 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ][ 1 ], rMatrix1.m_matrix[ 1 ][ 1 ] );
        m_matrix[ 1 ][ 2 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ][ 2 ], rMatrix1.m_matrix[ 1 ][ 2 ] );
        m_matrix[ 1 ][ 3 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 1 ][ 3 ], rMatrix1.m_matrix[ 1 ][ 3 ] );
        m_matrix[ 2 ][ 0 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ][ 0 ], rMatrix1.m_matrix[ 2 ][ 0 ] );
        m_matrix[ 2 ][ 1 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ][ 1 ], rMatrix1.m_matrix[ 2 ][ 1 ] );
        m_matrix[ 2 ][ 2 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ][ 2 ], rMatrix1.m_matrix[ 2 ][ 2 ] );
        m_matrix[ 2 ][ 3 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 2 ][ 3 ], rMatrix1.m_matrix[ 2 ][ 3 ] );
        m_matrix[ 3 ][ 0 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ][ 0 ], rMatrix1.m_matrix[ 3 ][ 0 ] );
        m_matrix[ 3 ][ 1 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ][ 1 ], rMatrix1.m_matrix[ 3 ][ 1 ] );
        m_matrix[ 3 ][ 2 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ][ 2 ], rMatrix1.m_matrix[ 3 ][ 2 ] );
        m_matrix[ 3 ][ 3 ] = Simd::MultiplyF32( rMatrix0.m_matrix[ 3 ][ 3 ], rMatrix1.m_matrix[ 3 ][ 3 ] );
    }

    /// Set this matrix to the component-wise quotient of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44Soa::DivideComponentsSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 )
    {
        m_matrix[ 0 ][ 0 ] = Simd::DivideF32( rMatrix0.m_matrix[ 0 ][ 0 ], rMatrix1.m_matrix[ 0 ][ 0 ] );
        m_matrix[ 0 ][ 1 ] = Simd::DivideF32( rMatrix0.m_matrix[ 0 ][ 1 ], rMatrix1.m_matrix[ 0 ][ 1 ] );
        m_matrix[ 0 ][ 2 ] = Simd::DivideF32( rMatrix0.m_matrix[ 0 ][ 2 ], rMatrix1.m_matrix[ 0 ][ 2 ] );
        m_matrix[ 0 ][ 3 ] = Simd::DivideF32( rMatrix0.m_matrix[ 0 ][ 3 ], rMatrix1.m_matrix[ 0 ][ 3 ] );
        m_matrix[ 1 ][ 0 ] = Simd::DivideF32( rMatrix0.m_matrix[ 1 ][ 0 ], rMatrix1.m_matrix[ 1 ][ 0 ] );
        m_matrix[ 1 ][ 1 ] = Simd::DivideF32( rMatrix0.m_matrix[ 1 ][ 1 ], rMatrix1.m_matrix[ 1 ][ 1 ] );
        m_matrix[ 1 ][ 2 ] = Simd::DivideF32( rMatrix0.m_matrix[ 1 ][ 2 ], rMatrix1.m_matrix[ 1 ][ 2 ] );
        m_matrix[ 1 ][ 3 ] = Simd::DivideF32( rMatrix0.m_matrix[ 1 ][ 3 ], rMatrix1.m_matrix[ 1 ][ 3 ] );
        m_matrix[ 2 ][ 0 ] = Simd::DivideF32( rMatrix0.m_matrix[ 2 ][ 0 ], rMatrix1.m_matrix[ 2 ][ 0 ] );
        m_matrix[ 2 ][ 1 ] = Simd::DivideF32( rMatrix0.m_matrix[ 2 ][ 1 ], rMatrix1.m_matrix[ 2 ][ 1 ] );
        m_matrix[ 2 ][ 2 ] = Simd::DivideF32( rMatrix0.m_matrix[ 2 ][ 2 ], rMatrix1.m_matrix[ 2 ][ 2 ] );
        m_matrix[ 2 ][ 3 ] = Simd::DivideF32( rMatrix0.m_matrix[ 2 ][ 3 ], rMatrix1.m_matrix[ 2 ][ 3 ] );
        m_matrix[ 3 ][ 0 ] = Simd::DivideF32( rMatrix0.m_matrix[ 3 ][ 0 ], rMatrix1.m_matrix[ 3 ][ 0 ] );
        m_matrix[ 3 ][ 1 ] = Simd::DivideF32( rMatrix0.m_matrix[ 3 ][ 1 ], rMatrix1.m_matrix[ 3 ][ 1 ] );
        m_matrix[ 3 ][ 2 ] = Simd::DivideF32( rMatrix0.m_matrix[ 3 ][ 2 ], rMatrix1.m_matrix[ 3 ][ 2 ] );
        m_matrix[ 3 ][ 3 ] = Simd::DivideF32( rMatrix0.m_matrix[ 3 ][ 3 ], rMatrix1.m_matrix[ 3 ][ 3 ] );
    }

    /// Get the inverse of this matrix.
    ///
    /// @return  Matrix inverse.
    ///
    /// @see Invert()
    Matrix44Soa Matrix44Soa::GetInverse() const
    {
        Matrix44Soa result;
        GetInverse( result );

        return result;
    }

    /// Set this matrix to its inverse.
    ///
    /// @see GetInverse()
    void Matrix44Soa::Invert()
    {
        GetInverse( *this );
    }

    /// Get the transpose of this matrix.
    ///
    /// @return  Matrix transpose.
    ///
    /// @see Transpose()
    Matrix44Soa Matrix44Soa::GetTranspose() const
    {
        Matrix44Soa result;
        GetTranspose( result );

        return result;
    }

    /// Set this matrix to its transpose.
    ///
    /// @see GetTranspose()
    void Matrix44Soa::Transpose()
    {
        GetTranspose( *this );
    }

    /// Transform a 4-component vector.
    ///
    /// Note that transformation takes into account the vector w-component.
    ///
    /// @param[in]  rVector  Vector to transform.
    /// @param[out] rResult  Transformed result.
    ///
    /// @see TransformPoint(), TransformVector()
    void Matrix44Soa::Transform( const Vector4Soa& rVector, Vector4Soa& rResult ) const
    {
        SimdVector x = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 0 ] );
        SimdVector y = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 1 ] );
        SimdVector z = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 2 ] );
        SimdVector w = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 3 ] );

        x = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 2 ], z );
        w = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 3 ], w );

        x = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 2 ], z );
        w = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 3 ], w );

        x = Simd::MultiplyAddF32( rVector.m_w, m_matrix[ 3 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_w, m_matrix[ 3 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_w, m_matrix[ 3 ][ 2 ], z );
        w = Simd::MultiplyAddF32( rVector.m_w, m_matrix[ 3 ][ 3 ], w );

        rResult.m_x = x;
        rResult.m_y = y;
        rResult.m_z = z;
        rResult.m_w = w;
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
    Vector4Soa Matrix44Soa::Transform( const Vector4Soa& rVector ) const
    {
        Vector4Soa result;
        Transform( rVector, result );

        return result;
    }

    /// Transform a 3-component vector as a point in 3D space.
    ///
    /// This is equivalent to calling Transform() on a Vector4Soa filled with the same values as the given vector, with
    /// the w-component set to 1.
    ///
    /// @param[in]  rVector  Vector to transform.
    /// @param[out] rResult  Transformed result.
    ///
    /// @see TransformVector(), Transform()
    void Matrix44Soa::TransformPoint( const Vector3Soa& rVector, Vector3Soa& rResult ) const
    {
        SimdVector x = Simd::MultiplyAddF32( rVector.m_x, m_matrix[ 0 ][ 0 ], m_matrix[ 3 ][ 0 ] );
        SimdVector y = Simd::MultiplyAddF32( rVector.m_x, m_matrix[ 0 ][ 1 ], m_matrix[ 3 ][ 1 ] );
        SimdVector z = Simd::MultiplyAddF32( rVector.m_x, m_matrix[ 0 ][ 2 ], m_matrix[ 3 ][ 2 ] );

        x = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 2 ], z );

        x = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 2 ], z );

        rResult.m_x = x;
        rResult.m_y = y;
        rResult.m_z = z;
    }

    /// Transform a 3-component vector as a point in 3D space.
    ///
    /// This is equivalent to calling Transform() on a Vector4Soa filled with the same values as the given vector, with
    /// the w-component set to 1.
    ///
    /// @param[in] rVector  Vector to transform.
    ///
    /// @return  Transformed result.
    ///
    /// @see TransformVector(), Transform()
    Vector3Soa Matrix44Soa::TransformPoint( const Vector3Soa& rVector ) const
    {
        Vector3Soa result;
        TransformPoint( rVector, result );

        return result;
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
    void Matrix44Soa::TransformVector( const Vector3Soa& rVector, Vector3Soa& rResult ) const
    {
        SimdVector x = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 0 ] );
        SimdVector y = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 1 ] );
        SimdVector z = Simd::MultiplyF32( rVector.m_x, m_matrix[ 0 ][ 2 ] );

        x = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_y, m_matrix[ 1 ][ 2 ], z );

        x = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 0 ], x );
        y = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 1 ], y );
        z = Simd::MultiplyAddF32( rVector.m_z, m_matrix[ 2 ][ 2 ], z );

        rResult.m_x = x;
        rResult.m_y = y;
        rResult.m_z = z;
    }

    /// Transform a 3-component vector as a directional vector in 3D space.
    ///
    /// This is equivalent to calling Transform() on a Vector4Soa filled with the same values as the given vector, with
    /// the w-component set to 0.
    ///
    /// @param[in] rVector  Vector to transform.
    ///
    /// @return  Transformed result.
    ///
    /// @see TransformPoint(), Transform()
    Vector3Soa Matrix44Soa::TransformVector( const Vector3Soa& rVector ) const
    {
        Vector3Soa result;
        TransformVector( rVector, result );

        return result;
    }

    /// Test whether each component in this matrix is equal to the corresponding component in another matrix within a
    /// given threshold.
    ///
    /// @param[in] rMatrix   Matrix.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for matrices that are equal within the given threshold.
    SimdMask Matrix44Soa::Equals( const Matrix44Soa& rMatrix, const SimdVector& rEpsilon ) const
    {
        SimdVector absMask = Simd::SetSplatU32( 0x7fffffff );

        SimdMask result;

        SimdVector difference0, difference1, difference2, difference3;
        SimdMask testResult0, testResult1, testResult2, testResult3;

        difference0 = Simd::SubtractF32( m_matrix[ 0 ][ 0 ], rMatrix.m_matrix[ 0 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 0 ][ 1 ], rMatrix.m_matrix[ 0 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 0 ][ 2 ], rMatrix.m_matrix[ 0 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 0 ][ 3 ], rMatrix.m_matrix[ 0 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::LessEqualsF32( difference0, rEpsilon );
        testResult1 = Simd::LessEqualsF32( difference1, rEpsilon );
        testResult2 = Simd::LessEqualsF32( difference2, rEpsilon );
        testResult3 = Simd::LessEqualsF32( difference3, rEpsilon );
        result = testResult0;
        result = Simd::And( result, testResult1 );
        result = Simd::And( result, testResult2 );
        result = Simd::And( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 1 ][ 0 ], rMatrix.m_matrix[ 1 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 1 ][ 1 ], rMatrix.m_matrix[ 1 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 1 ][ 2 ], rMatrix.m_matrix[ 1 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 1 ][ 3 ], rMatrix.m_matrix[ 1 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::LessEqualsF32( difference0, rEpsilon );
        testResult1 = Simd::LessEqualsF32( difference1, rEpsilon );
        testResult2 = Simd::LessEqualsF32( difference2, rEpsilon );
        testResult3 = Simd::LessEqualsF32( difference3, rEpsilon );
        result = Simd::And( result, testResult0 );
        result = Simd::And( result, testResult1 );
        result = Simd::And( result, testResult2 );
        result = Simd::And( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 2 ][ 0 ], rMatrix.m_matrix[ 2 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 2 ][ 1 ], rMatrix.m_matrix[ 2 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 2 ][ 2 ], rMatrix.m_matrix[ 2 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 2 ][ 3 ], rMatrix.m_matrix[ 2 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::LessEqualsF32( difference0, rEpsilon );
        testResult1 = Simd::LessEqualsF32( difference1, rEpsilon );
        testResult2 = Simd::LessEqualsF32( difference2, rEpsilon );
        testResult3 = Simd::LessEqualsF32( difference3, rEpsilon );
        result = Simd::And( result, testResult0 );
        result = Simd::And( result, testResult1 );
        result = Simd::And( result, testResult2 );
        result = Simd::And( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 3 ][ 0 ], rMatrix.m_matrix[ 3 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 3 ][ 1 ], rMatrix.m_matrix[ 3 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 3 ][ 2 ], rMatrix.m_matrix[ 3 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 3 ][ 3 ], rMatrix.m_matrix[ 3 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::LessEqualsF32( difference0, rEpsilon );
        testResult1 = Simd::LessEqualsF32( difference1, rEpsilon );
        testResult2 = Simd::LessEqualsF32( difference2, rEpsilon );
        testResult3 = Simd::LessEqualsF32( difference3, rEpsilon );
        result = Simd::And( result, testResult0 );
        result = Simd::And( result, testResult1 );
        result = Simd::And( result, testResult2 );
        result = Simd::And( result, testResult3 );

        return result;
    }

    /// Test whether any component in this matrix is not equal to the corresponding component in another matrix within a
    /// given threshold.
    ///
    /// @param[in] rMatrix   Matrix.
    /// @param[in] rEpsilon  Comparison threshold.
    ///
    /// @return  SIMD mask with bits set for matrices that are not equal within the given threshold.
    SimdMask Matrix44Soa::NotEquals( const Matrix44Soa& rMatrix, const SimdVector& rEpsilon ) const
    {
        SimdVector absMask = Simd::SetSplatU32( 0x7fffffff );

        SimdMask result;

        SimdVector difference0, difference1, difference2, difference3;
        SimdMask testResult0, testResult1, testResult2, testResult3;

        difference0 = Simd::SubtractF32( m_matrix[ 0 ][ 0 ], rMatrix.m_matrix[ 0 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 0 ][ 1 ], rMatrix.m_matrix[ 0 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 0 ][ 2 ], rMatrix.m_matrix[ 0 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 0 ][ 3 ], rMatrix.m_matrix[ 0 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::GreaterF32( difference0, rEpsilon );
        testResult1 = Simd::GreaterF32( difference1, rEpsilon );
        testResult2 = Simd::GreaterF32( difference2, rEpsilon );
        testResult3 = Simd::GreaterF32( difference3, rEpsilon );
        result = testResult0;
        result = Simd::Or( result, testResult1 );
        result = Simd::Or( result, testResult2 );
        result = Simd::Or( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 1 ][ 0 ], rMatrix.m_matrix[ 1 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 1 ][ 1 ], rMatrix.m_matrix[ 1 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 1 ][ 2 ], rMatrix.m_matrix[ 1 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 1 ][ 3 ], rMatrix.m_matrix[ 1 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::GreaterF32( difference0, rEpsilon );
        testResult1 = Simd::GreaterF32( difference1, rEpsilon );
        testResult2 = Simd::GreaterF32( difference2, rEpsilon );
        testResult3 = Simd::GreaterF32( difference3, rEpsilon );
        result = Simd::Or( result, testResult0 );
        result = Simd::Or( result, testResult1 );
        result = Simd::Or( result, testResult2 );
        result = Simd::Or( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 2 ][ 0 ], rMatrix.m_matrix[ 2 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 2 ][ 1 ], rMatrix.m_matrix[ 2 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 2 ][ 2 ], rMatrix.m_matrix[ 2 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 2 ][ 3 ], rMatrix.m_matrix[ 2 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::GreaterF32( difference0, rEpsilon );
        testResult1 = Simd::GreaterF32( difference1, rEpsilon );
        testResult2 = Simd::GreaterF32( difference2, rEpsilon );
        testResult3 = Simd::GreaterF32( difference3, rEpsilon );
        result = Simd::Or( result, testResult0 );
        result = Simd::Or( result, testResult1 );
        result = Simd::Or( result, testResult2 );
        result = Simd::Or( result, testResult3 );

        difference0 = Simd::SubtractF32( m_matrix[ 3 ][ 0 ], rMatrix.m_matrix[ 3 ][ 0 ] );
        difference1 = Simd::SubtractF32( m_matrix[ 3 ][ 1 ], rMatrix.m_matrix[ 3 ][ 1 ] );
        difference2 = Simd::SubtractF32( m_matrix[ 3 ][ 2 ], rMatrix.m_matrix[ 3 ][ 2 ] );
        difference3 = Simd::SubtractF32( m_matrix[ 3 ][ 3 ], rMatrix.m_matrix[ 3 ][ 3 ] );
        difference0 = Simd::And( difference0, absMask );
        difference1 = Simd::And( difference1, absMask );
        difference2 = Simd::And( difference2, absMask );
        difference3 = Simd::And( difference3, absMask );
        testResult0 = Simd::GreaterF32( difference0, rEpsilon );
        testResult1 = Simd::GreaterF32( difference1, rEpsilon );
        testResult2 = Simd::GreaterF32( difference2, rEpsilon );
        testResult3 = Simd::GreaterF32( difference3, rEpsilon );
        result = Simd::Or( result, testResult0 );
        result = Simd::Or( result, testResult1 );
        result = Simd::Or( result, testResult2 );
        result = Simd::Or( result, testResult3 );

        return result;
    }

    /// Perform a component-wise addition of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Matrix containing the sums of each component.
    Matrix44Soa Matrix44Soa::operator+( const Matrix44Soa& rMatrix ) const
    {
        return Add( rMatrix );
    }

    /// Perform a component-wise subtraction of the given matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Matrix containing the differences of each component.
    Matrix44Soa Matrix44Soa::operator-( const Matrix44Soa& rMatrix ) const
    {
        return Subtract( rMatrix );
    }

    /// Perform a matrix multiplication between this matrix and another matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Product matrix.
    Matrix44Soa Matrix44Soa::operator*( const Matrix44Soa& rMatrix ) const
    {
        return Multiply( rMatrix );
    }

    /// Perform a component-wise in-place addition of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix to add.
    ///
    /// @return  Reference to this matrix.
    Matrix44Soa& Matrix44Soa::operator+=( const Matrix44Soa& rMatrix )
    {
        AddSet( *this, rMatrix );

        return *this;
    }

    /// Perform a component-wise in-place subtraction of the given matrix from this matrix.
    ///
    /// @param[in] rMatrix  Matrix to subtract.
    ///
    /// @return  Reference to this matrix.
    Matrix44Soa& Matrix44Soa::operator-=( const Matrix44Soa& rMatrix )
    {
        SubtractSet( *this, rMatrix );

        return *this;
    }

    /// Perform in-place multiplication of this matrix and the given matrix.
    ///
    /// @param[in] rMatrix  Matrix with which to multiply.
    ///
    /// @return  Reference to this matrix.
    Matrix44Soa& Matrix44Soa::operator*=( const Matrix44Soa& rMatrix )
    {
        MultiplySet( *this, rMatrix );

        return *this;
    }

    /// Test whether each component in this matrix is equal to the corresponding component in another matrix within a
    /// default threshold.
    ///
    /// @param[in] rMatrix  Matrix.
    ///
    /// @return  SIMD mask with bits set for matrices that are equal within the given threshold.
    SimdMask Matrix44Soa::operator==( const Matrix44Soa& rMatrix ) const
    {
        return Equals( rMatrix );
    }

    /// Test whether any component in this matrix is not equal to the corresponding component in another matrix within a
    /// default threshold.
    ///
    /// @param[in] rMatrix  Matrix.
    ///
    /// @return  SIMD mask with bits set for matrices that are not equal within the given threshold.
    SimdMask Matrix44Soa::operator!=( const Matrix44Soa& rMatrix ) const
    {
        return NotEquals( rMatrix );
    }
}
