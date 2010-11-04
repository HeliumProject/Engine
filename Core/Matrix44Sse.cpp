//----------------------------------------------------------------------------------------------------------------------
// Matrix44Sse.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Platform/Simd.h"

#if HELIUM_SIMD_SSE

#include "Core/Matrix44.h"

#include "Core/Quat.h"

namespace Lunar
{
    static HELIUM_FORCEINLINE Helium::SimdVector MultiplyResultRow(
        const Helium::SimdVector& rMatrix0Row,
        const Helium::SimdVector ( &rMatrix1Rows )[ 4 ] )
    {
        Helium::SimdVector x = _mm_shuffle_ps( rMatrix0Row, rMatrix0Row, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( rMatrix0Row, rMatrix0Row, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( rMatrix0Row, rMatrix0Row, _MM_SHUFFLE( 2, 2, 2, 2 ) );
        Helium::SimdVector w = _mm_shuffle_ps( rMatrix0Row, rMatrix0Row, _MM_SHUFFLE( 3, 3, 3, 3 ) );

        x = Helium::Simd::MultiplyF32( x, rMatrix1Rows[ 0 ] );
        y = Helium::Simd::MultiplyF32( y, rMatrix1Rows[ 1 ] );
        z = Helium::Simd::MultiplyF32( z, rMatrix1Rows[ 2 ] );
        w = Helium::Simd::MultiplyF32( w, rMatrix1Rows[ 3 ] );

        Helium::SimdVector result = Helium::Simd::AddF32( x, y );
        result = Helium::Simd::AddF32( result, z );
        result = Helium::Simd::AddF32( result, w );

        return result;
    }

    static HELIUM_FORCEINLINE void ComputeDet22Helper(
        const Helium::SimdVector& rRow0,
        const Helium::SimdVector& rRow1,
        Helium::SimdVector& rDet22Adj,
        Helium::SimdVector& rDet22Opp )
    {
        Helium::SimdVector row1Shift1 = _mm_shuffle_ps( rRow1, rRow1, _MM_SHUFFLE( 0, 3, 2, 1 ) );
        Helium::SimdVector row1Shift2 = _mm_shuffle_ps( rRow1, rRow1, _MM_SHUFFLE( 1, 0, 3, 2 ) );
        Helium::SimdVector row1Shift3 = _mm_shuffle_ps( rRow1, rRow1, _MM_SHUFFLE( 2, 1, 0, 3 ) );

        Helium::SimdVector prod01 = Helium::Simd::MultiplyF32( rRow0, row1Shift1 );
        Helium::SimdVector prod02 = Helium::Simd::MultiplyF32( rRow0, row1Shift2 );
        Helium::SimdVector prod03 = Helium::Simd::MultiplyF32( rRow0, row1Shift3 );

        rDet22Adj = Helium::Simd::SubtractF32( prod01, _mm_shuffle_ps( prod03, prod03, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        rDet22Opp = Helium::Simd::SubtractF32( prod02, _mm_shuffle_ps( prod02, prod02, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
    }

    static HELIUM_FORCEINLINE void ComputeDet33PartsHelper(
        const Helium::SimdVector& rBaseRow,
        const Helium::SimdVector& rDet22Adj,
        const Helium::SimdVector& rDet22Opp,
        Helium::SimdVector& rDet33Pre,
        Helium::SimdVector& rDet33Split,
        Helium::SimdVector& rDet33Post )
    {
        Helium::SimdVector baseRowShift3 = _mm_shuffle_ps( rBaseRow, rBaseRow, _MM_SHUFFLE( 2, 1, 0, 3 ) );

        rDet33Pre = Helium::Simd::MultiplyF32( baseRowShift3, rDet22Adj );
        rDet33Split = Helium::Simd::MultiplyF32(
            baseRowShift3,
            _mm_shuffle_ps( rDet22Opp, rDet22Opp, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
        rDet33Post = Helium::Simd::MultiplyF32(
            baseRowShift3,
            _mm_shuffle_ps( rDet22Adj, rDet22Adj, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
    }

    static HELIUM_FORCEINLINE void DeterminantHelper(
        const Helium::SimdVector ( &rMatrixRows )[ 4 ],
        Helium::SimdVector& rDeterminant,
        Helium::SimdVector& rLastRowsDet22Adj,
        Helium::SimdVector& rLastRowsDet22Opp,
        Helium::SimdVector& rLastRowsDet33Pre,
        Helium::SimdVector& rLastRowsDet33Split,
        Helium::SimdVector& rLastRowsDet33Post )
    {
        ComputeDet22Helper( rMatrixRows[ 2 ], rMatrixRows[ 3 ], rLastRowsDet22Adj, rLastRowsDet22Opp );

        ComputeDet33PartsHelper(
            rMatrixRows[ 1 ],
            rLastRowsDet22Adj,
            rLastRowsDet22Opp,
            rLastRowsDet33Pre,
            rLastRowsDet33Split,
            rLastRowsDet33Post );

        Helium::SimdVector row0Shift2 = _mm_shuffle_ps( rMatrixRows[ 0 ], rMatrixRows[ 0 ], _MM_SHUFFLE( 1, 0, 3, 2 ) );

        rDeterminant = Helium::Simd::SubtractF32(
            rLastRowsDet33Pre,
            _mm_shuffle_ps( rLastRowsDet33Split, rLastRowsDet33Split, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
        rDeterminant = Helium::Simd::AddF32(
            rDeterminant,
            _mm_shuffle_ps( rLastRowsDet33Post, rLastRowsDet33Post, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
        rDeterminant = Helium::Simd::MultiplyF32( row0Shift2, rDeterminant );

        rDeterminant = Helium::Simd::AddF32(
            rDeterminant,
            _mm_shuffle_ps( rDeterminant, rDeterminant, _MM_SHUFFLE( 1, 0, 3, 2 ) ) );
        rDeterminant = Helium::Simd::SubtractF32(
            rDeterminant,
            _mm_shuffle_ps( rDeterminant, rDeterminant, _MM_SHUFFLE( 0, 3, 2, 1 ) ) );
    }

    static HELIUM_FORCEINLINE Helium::SimdVector InverseRowHelper(
        const Helium::SimdVector& rDet33Pre,
        const Helium::SimdVector& rDet33Split,
        const Helium::SimdVector& rDet33Post,
        const Helium::SimdVector& rInvDeterminantScaler )
    {
        Helium::SimdVector result = _mm_shuffle_ps( rDet33Pre, rDet33Pre, _MM_SHUFFLE( 1, 0, 3, 2 ) );
        result = Helium::Simd::SubtractF32( result, _mm_shuffle_ps( rDet33Split, rDet33Split, _MM_SHUFFLE( 2, 1, 0, 3 ) ) );
        result = Helium::Simd::AddF32( result, rDet33Post );
        result = Helium::Simd::MultiplyF32( result, rInvDeterminantScaler );

        return result;
    }

    /// Set this matrix to a rotation matrix.
    ///
    /// Any translation or scaling in this matrix will be reset.
    ///
    /// @param[in] rRotation  Rotation to set.
    ///
    /// @see SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44::SetRotation( const Quat& rRotation )
    {
        SetRotationOnly( rRotation );
        m_matrix[ 3 ] = IDENTITY.m_matrix[ 3 ];
    }

    /// Set this matrix to a translation matrix.
    ///
    /// Any rotation or scaling in this matrix will be reset.
    ///
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetScaling(), SetRotationTranslation(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetTranslation( const Vector3& rTranslation )
    {
        m_matrix[ 0 ] = IDENTITY.m_matrix[ 0 ];
        m_matrix[ 1 ] = IDENTITY.m_matrix[ 1 ];
        m_matrix[ 2 ] = IDENTITY.m_matrix[ 2 ];

        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a translation matrix.
    ///
    /// Any rotation or scaling in this matrix will be reset.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetScaling(), SetRotationTranslation(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetTranslation( const Vector4& rTranslation )
    {
        m_matrix[ 0 ] = IDENTITY.m_matrix[ 0 ];
        m_matrix[ 1 ] = IDENTITY.m_matrix[ 1 ];
        m_matrix[ 2 ] = IDENTITY.m_matrix[ 2 ];

        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a uniform scaling matrix.
    ///
    /// Any rotation or translation in this matrix will be reset.
    ///
    /// @param[in] scaling  Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44::SetScaling( float32_t scaling )
    {
        Helium::SimdVector scalingVec = _mm_set_ps1( scaling );

        Helium::SimdVector x = IDENTITY.m_matrix[ 0 ];
        Helium::SimdVector y = IDENTITY.m_matrix[ 1 ];
        Helium::SimdVector z = IDENTITY.m_matrix[ 2 ];

        m_matrix[ 3 ] = IDENTITY.m_matrix[ 3 ];

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( x, scalingVec );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( y, scalingVec );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( z, scalingVec );
    }

    /// Set this matrix to a non-uniform scaling matrix.
    ///
    /// Any rotation or translation in this matrix will be reset.
    ///
    /// @param[in] rScaling  Vector specifying the scaling factors along each axis.
    ///
    /// @see SetRotation(), SetTranslation(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44::SetScaling( const Vector3& rScaling )
    {
        Helium::SimdVector x = IDENTITY.m_matrix[ 0 ];
        Helium::SimdVector y = IDENTITY.m_matrix[ 1 ];
        Helium::SimdVector z = IDENTITY.m_matrix[ 2 ];

        m_matrix[ 3 ] = IDENTITY.m_matrix[ 3 ];

        Helium::SimdVector scalingVec = rScaling.GetSimdVector();

        Helium::SimdVector scaleX = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector scaleY = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector scaleZ = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( x, scaleX );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( y, scaleY );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( z, scaleZ );
    }

    /// Set this matrix to a rotation/translation matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslation( const Quat& rRotation, const Vector3& rTranslation )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a rotation/translation matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslation( const Quat& rRotation, const Vector4& rTranslation )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] scaling       Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslationScaling(
        const Quat& rRotation,
        const Vector3& rTranslation,
        float32_t scaling )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );

        ScaleLocal( scaling );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] scaling       Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslationScaling(
        const Quat& rRotation,
        const Vector4& rTranslation,
        float32_t scaling )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );

        ScaleLocal( scaling );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslationScaling(
        const Quat& rRotation,
        const Vector3& rTranslation,
        const Vector3& rScaling )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );

        ScaleLocal( rScaling );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// Note that the w-component of the given translation will be stored in this matrix as well.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44::SetRotationTranslationScaling(
        const Quat& rRotation,
        const Vector4& rTranslation,
        const Vector3& rScaling )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );

        ScaleLocal( rScaling );
    }

    /// Set the rotation component of this matrix.
    ///
    /// This will only affect the values in the first three rows of this matrix.  Any translation values (those in the
    /// last row) will be left intact.
    ///
    /// @param[in] rRotation  Rotation to set.
    ///
    /// @see SetTranslationOnly(), SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(),
    ///      SetRotationTranslationScaling()
    void Matrix44::SetRotationOnly( const Quat& rRotation )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        Helium::SimdVector oneVec = _mm_set_ps1( 1.0f );

        Helium::SimdVector xyz = rRotation.GetSimdVector();
        Helium::SimdVector yzx = _mm_shuffle_ps( xyz, xyz, _MM_SHUFFLE( 3, 0, 2, 1 ) );
        Helium::SimdVector zxy = _mm_shuffle_ps( xyz, xyz, _MM_SHUFFLE( 3, 1, 0, 2 ) );
        Helium::SimdVector www = _mm_shuffle_ps( xyz, xyz, _MM_SHUFFLE( 3, 3, 3, 3 ) );

        Helium::SimdVector product0, product1;

        product0 = Helium::Simd::MultiplyF32( yzx, yzx );
        product1 = Helium::Simd::MultiplyF32( zxy, zxy );
        Helium::SimdVector valuesA = Helium::Simd::AddF32( product0, product1 );
        valuesA = Helium::Simd::AddF32( valuesA, valuesA );
        valuesA = Helium::Simd::SubtractF32( oneVec, valuesA );

        product0 = Helium::Simd::MultiplyF32( xyz, yzx );
        product1 = Helium::Simd::MultiplyF32( zxy, www );
        Helium::SimdVector valuesB = Helium::Simd::AddF32( product0, product1 );
        valuesB = Helium::Simd::AddF32( valuesB, valuesB );

        Helium::SimdVector loAB = _mm_unpacklo_ps( valuesA, valuesB );
        Helium::SimdVector hiAB = _mm_unpackhi_ps( valuesA, valuesB );

        product0 = Helium::Simd::MultiplyF32( xyz, zxy );
        product1 = Helium::Simd::MultiplyF32( yzx, www );
        Helium::SimdVector valuesC = Helium::Simd::SubtractF32( product0, product1 );
        valuesC = Helium::Simd::AddF32( valuesC, valuesC );

        m_matrix[ 0 ] = _mm_movelh_ps( loAB, valuesC );

        m_matrix[ 1 ] = _mm_shuffle_ps( loAB, valuesC, _MM_SHUFFLE( 3, 1, 3, 2 ) );
        m_matrix[ 1 ] = _mm_shuffle_ps( m_matrix[ 1 ], m_matrix[ 1 ], _MM_SHUFFLE( 3, 1, 0, 2 ) );

        m_matrix[ 2 ] = _mm_shuffle_ps( hiAB, valuesC, _MM_SHUFFLE( 3, 2, 1, 0 ) );
        m_matrix[ 2 ] = _mm_shuffle_ps( m_matrix[ 2 ], m_matrix[ 2 ], _MM_SHUFFLE( 3, 0, 2, 1 ) );

        Helium::SimdVector componentMaskVec = Helium::Simd::LoadAligned( componentMask );
        m_matrix[ 0 ] = Helium::Simd::And( m_matrix[ 0 ], componentMaskVec );
        m_matrix[ 1 ] = Helium::Simd::And( m_matrix[ 1 ], componentMaskVec );
        m_matrix[ 2 ] = Helium::Simd::And( m_matrix[ 2 ], componentMaskVec );
    }

    /// Set the translation component of this matrix.
    ///
    /// This will only affect the last row of this matrix.  Any rotation or scaling values (those in the first three
    /// row) will be left intact.
    ///
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotationOnly(), SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(),
    ///      SetRotationTranslationScaling()
    void Matrix44::SetTranslationOnly( const Vector3& rTranslation )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        Helium::SimdVector identityTranslation = IDENTITY.m_matrix[ 3 ];
        Helium::SimdVector componentMaskVec = Helium::Simd::LoadAligned( componentMask );

        Helium::SimdVector translationVec = rTranslation.GetSimdVector();

        m_matrix[ 3 ] = Helium::Simd::Or( Helium::Simd::And( componentMaskVec, translationVec ), identityTranslation );
    }

    /// Set the translation component of this matrix.
    ///
    /// This will only affect the last row of this matrix.  Any rotation or scaling values (those in the first three
    /// row) will be left intact.
    ///
    /// Note that the w-component of the given vector will be stored in this matrix as well.
    ///
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotationOnly(), SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(),
    ///      SetRotationTranslationScaling()
    void Matrix44::SetTranslationOnly( const Vector4& rTranslation )
    {
        m_matrix[ 3 ] = rTranslation.GetSimdVector();
    }

    /// Translate this matrix in world-space (post-multiply).
    ///
    /// This operates under the assumption that the last element of each matrix axis (the first three rows) is 0, and
    /// the last element of the matrix translation component (the last row) is 1.
    ///
    /// @param[in] rTranslation  Amount by which to translate.
    ///
    /// @see TranslateLocal(), ScaleWorld(), ScaleLocal()
    void Matrix44::TranslateWorld( const Vector3& rTranslation )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        Helium::SimdVector componentMaskVec = Helium::Simd::LoadAligned( componentMask );

        m_matrix[ 3 ] = Helium::Simd::AddF32( Helium::Simd::And( componentMaskVec, rTranslation.GetSimdVector() ), m_matrix[ 3 ] );
    }

    /// Translate this matrix in local-space (pre-multiply).
    ///
    /// @param[in] rTranslation  Amount by which to translate.
    ///
    /// @see TranslateWorld(), ScaleWorld(), ScaleLocal()
    void Matrix44::TranslateLocal( const Vector3& rTranslation )
    {
        Helium::SimdVector translationVec = rTranslation.GetSimdVector();

        Helium::SimdVector x = _mm_shuffle_ps( translationVec, translationVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( translationVec, translationVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( translationVec, translationVec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

        x = Helium::Simd::MultiplyF32( x, m_matrix[ 0 ] );
        y = Helium::Simd::MultiplyF32( y, m_matrix[ 1 ] );
        z = Helium::Simd::MultiplyF32( z, m_matrix[ 2 ] );

        Helium::SimdVector result = Helium::Simd::AddF32( x, y );
        result = Helium::Simd::AddF32( result, z );
        result = Helium::Simd::AddF32( result, m_matrix[ 3 ] );

        m_matrix[ 3 ] = result;
    }

    /// Scale this matrix in world-space (post-multiply).
    ///
    /// @param[in] scaling  Amount by which to scale.
    ///
    /// @see ScaleLocal(), TranslateWorld(), TranslateLocal()
    void Matrix44::ScaleWorld( float32_t scaling )
    {
        Helium::SimdVector scalingVec = _mm_set_ps( 1.0f, scaling, scaling, scaling );

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( m_matrix[ 0 ], scalingVec );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( m_matrix[ 1 ], scalingVec );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( m_matrix[ 2 ], scalingVec );
        m_matrix[ 3 ] = Helium::Simd::MultiplyF32( m_matrix[ 3 ], scalingVec );
    }

    /// Scale this matrix in world-space (post-multiply).
    ///
    /// @param[in] rScaling  Vector specifying the amount by which to scale along each axis.
    ///
    /// @see ScaleLocal(), TranslateWorld(), TranslateLocal()
    void Matrix44::ScaleWorld( const Vector3& rScaling )
    {
        HELIUM_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] HELIUM_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        Helium::SimdVector componentMaskVec = Helium::Simd::LoadAligned( componentMask );

        Helium::SimdVector scalingVec = Helium::Simd::Or(
            Helium::Simd::And( rScaling.GetSimdVector(), componentMaskVec ),
            IDENTITY.m_matrix[ 3 ] );

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( m_matrix[ 0 ], scalingVec );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( m_matrix[ 1 ], scalingVec );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( m_matrix[ 2 ], scalingVec );
        m_matrix[ 3 ] = Helium::Simd::MultiplyF32( m_matrix[ 3 ], scalingVec );
    }

    /// Scale this matrix in local-space (pre-multiply).
    ///
    /// @param[in] scaling  Amount by which to scale.
    ///
    /// @see ScaleWorld(), TranslateWorld(), TranslateLocal()
    void Matrix44::ScaleLocal( float32_t scaling )
    {
        Helium::SimdVector scalingVec = _mm_set_ps1( scaling );

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( m_matrix[ 0 ], scalingVec );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( m_matrix[ 1 ], scalingVec );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( m_matrix[ 2 ], scalingVec );
    }

    /// Scale this matrix in local-space (pre-multiply).
    ///
    /// @param[in] rScaling  Vector specifying the amount by which to scale along each axis.
    ///
    /// @see ScaleWorld(), TranslateWorld(), TranslateLocal()
    void Matrix44::ScaleLocal( const Vector3& rScaling )
    {
        Helium::SimdVector scalingVec = rScaling.GetSimdVector();

        Helium::SimdVector x = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        Helium::SimdVector y = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        Helium::SimdVector z = _mm_shuffle_ps( scalingVec, scalingVec, _MM_SHUFFLE( 2, 2, 2, 2 ) );

        m_matrix[ 0 ] = Helium::Simd::MultiplyF32( m_matrix[ 0 ], x );
        m_matrix[ 1 ] = Helium::Simd::MultiplyF32( m_matrix[ 1 ], z );
        m_matrix[ 2 ] = Helium::Simd::MultiplyF32( m_matrix[ 2 ], y );
    }

    /// Set this matrix to the product of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44::MultiplySet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
    {
        Matrix44 result;
        result.m_matrix[ 0 ] = MultiplyResultRow( rMatrix0.m_matrix[ 0 ], rMatrix1.m_matrix );
        result.m_matrix[ 1 ] = MultiplyResultRow( rMatrix0.m_matrix[ 1 ], rMatrix1.m_matrix );
        result.m_matrix[ 2 ] = MultiplyResultRow( rMatrix0.m_matrix[ 2 ], rMatrix1.m_matrix );
        result.m_matrix[ 3 ] = MultiplyResultRow( rMatrix0.m_matrix[ 3 ], rMatrix1.m_matrix );

        *this = result;
    }

    /// Compute the determinant of this matrix.
    ///
    /// @return  Matrix determinant.
    float32_t Matrix44::GetDeterminant() const
    {
        Helium::SimdVector determinant, det22Adj, det22Opp, det33Pre, det33Split, det33Post;
        DeterminantHelper( m_matrix, determinant, det22Adj, det22Opp, det33Pre, det33Split, det33Post );

        return reinterpret_cast< const float32_t* >( &determinant )[ 0 ];
    }

    /// Get the inverse of this matrix.
    ///
    /// @param[out] rMatrix  Matrix inverse.
    ///
    /// @see Invert()
    void Matrix44::GetInverse( Matrix44& rMatrix ) const
    {
        Helium::SimdVector invDeterminantEven, det22Adj, det22Opp, det33Pre, det33Split, det33Post;
        DeterminantHelper( m_matrix, invDeterminantEven, det22Adj, det22Opp, det33Pre, det33Split, det33Post );

        invDeterminantEven = Helium::Simd::InverseF32( invDeterminantEven );

        Helium::SimdVector invDeterminantOdd = _mm_shuffle_ps(
            invDeterminantEven,
            invDeterminantEven,
            _MM_SHUFFLE( 0, 3, 2, 1 ) );

        Helium::SimdVector row0 = InverseRowHelper( det33Pre, det33Split, det33Post, invDeterminantEven );

        ComputeDet33PartsHelper( m_matrix[ 0 ], det22Adj, det22Opp, det33Pre, det33Split, det33Post );

        Helium::SimdVector row1 = InverseRowHelper( det33Pre, det33Split, det33Post, invDeterminantOdd );

        ComputeDet22Helper( m_matrix[ 0 ], m_matrix[ 1 ], det22Adj, det22Opp );

        rMatrix.m_matrix[ 0 ] = row0;
        rMatrix.m_matrix[ 1 ] = row1;

        ComputeDet33PartsHelper( m_matrix[ 3 ], det22Adj, det22Opp, det33Pre, det33Split, det33Post );

        row0 = InverseRowHelper( det33Pre, det33Split, det33Post, invDeterminantEven );

        ComputeDet33PartsHelper( m_matrix[ 2 ], det22Adj, det22Opp, det33Pre, det33Split, det33Post );

        rMatrix.m_matrix[ 2 ] = row0;
        rMatrix.m_matrix[ 3 ] = InverseRowHelper( det33Pre, det33Split, det33Post, invDeterminantOdd );

        rMatrix.Transpose();
    }

    /// Get the transpose of this matrix.
    ///
    /// @param[out] rMatrix  Matrix transpose.
    ///
    /// @see Transpose()
    void Matrix44::GetTranspose( Matrix44& rMatrix ) const
    {
        Helium::SimdVector xyxy = _mm_unpacklo_ps( m_matrix[ 0 ], m_matrix[ 1 ] );
        Helium::SimdVector xyzw = _mm_unpackhi_ps( m_matrix[ 0 ], m_matrix[ 1 ] );
        Helium::SimdVector zwxy = _mm_unpacklo_ps( m_matrix[ 2 ], m_matrix[ 3 ] );
        Helium::SimdVector zwzw = _mm_unpackhi_ps( m_matrix[ 2 ], m_matrix[ 3 ] );

        rMatrix.m_matrix[ 0 ] = _mm_movelh_ps( xyxy, zwxy );
        rMatrix.m_matrix[ 1 ] = _mm_movehl_ps( zwxy, xyxy );
        rMatrix.m_matrix[ 2 ] = _mm_movelh_ps( xyzw, zwzw );
        rMatrix.m_matrix[ 3 ] = _mm_movehl_ps( zwzw, xyzw );
    }
}

#endif  // HELIUM_SIMD_SSE
