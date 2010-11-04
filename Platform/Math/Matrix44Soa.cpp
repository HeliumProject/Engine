//----------------------------------------------------------------------------------------------------------------------
// Matrix44Soa.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

//#include "CorePch.h"
#include "Platform/Math/Matrix44Soa.h"

#include "Platform/Math/QuatSoa.h"

namespace Helium
{
    const Matrix44Soa Matrix44Soa::IDENTITY(
        Simd::SetSplatF32( 1.0f ), Simd::LoadZeros(),         Simd::LoadZeros(),         Simd::LoadZeros(),
        Simd::LoadZeros(),         Simd::SetSplatF32( 1.0f ), Simd::LoadZeros(),         Simd::LoadZeros(),
        Simd::LoadZeros(),         Simd::LoadZeros(),         Simd::SetSplatF32( 1.0f ), Simd::LoadZeros(),
        Simd::LoadZeros(),         Simd::LoadZeros(),         Simd::LoadZeros(),         Simd::SetSplatF32( 1.0f ) );

    struct Determinant22Cache
    {
        SimdVector det01;
        SimdVector det02;
        SimdVector det03;
        SimdVector det12;
        SimdVector det13;
        SimdVector det23;
    };

    struct Determinant33Cache
    {
        SimdVector detSubmat0;
        SimdVector detSubmat1;
        SimdVector detSubmat2;
        SimdVector detSubmat3;
    };

    static HELIUM_FORCEINLINE void MultiplyResultRow(
        SimdVector ( &rResultRow )[ 4 ],
        const SimdVector ( &rMatrix0Row )[ 4 ],
        const Matrix44Soa& rMatrix1 )
    {
        SimdVector temp;
        SimdVector res0, res1, res2, res3;

        temp = rMatrix0Row[ 0 ];
        res0 = Simd::MultiplyF32( temp, rMatrix1.m_matrix[ 0 ][ 0 ] );
        res1 = Simd::MultiplyF32( temp, rMatrix1.m_matrix[ 0 ][ 1 ] );
        res2 = Simd::MultiplyF32( temp, rMatrix1.m_matrix[ 0 ][ 2 ] );
        res3 = Simd::MultiplyF32( temp, rMatrix1.m_matrix[ 0 ][ 3 ] );
        temp = rMatrix0Row[ 1 ];
        res0 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 1 ][ 0 ], res0 );
        res1 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 1 ][ 1 ], res1 );
        res2 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 1 ][ 2 ], res2 );
        res3 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 1 ][ 3 ], res3 );
        temp = rMatrix0Row[ 2 ];
        res0 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 2 ][ 0 ], res0 );
        res1 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 2 ][ 1 ], res1 );
        res2 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 2 ][ 2 ], res2 );
        res3 = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 2 ][ 3 ], res3 );
        temp = rMatrix0Row[ 3 ];
        rResultRow[ 0 ] = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 3 ][ 0 ], res0 );
        rResultRow[ 1 ] = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 3 ][ 1 ], res1 );
        rResultRow[ 2 ] = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 3 ][ 2 ], res2 );
        rResultRow[ 3 ] = Simd::MultiplyAddF32( temp, rMatrix1.m_matrix[ 3 ][ 3 ], res3 );
    }

    static HELIUM_FORCEINLINE void ComputeDet22Helper(
        const SimdVector ( &rRow0 )[ 4 ],
        const SimdVector ( &rRow1 )[ 4 ],
        Determinant22Cache& rCache )
    {
        rCache.det01 = Simd::MultiplyF32( rRow0[ 0 ], rRow1[ 1 ] );
        rCache.det01 = Simd::MultiplySubtractReverseF32( rRow0[ 1 ], rRow1[ 0 ], rCache.det01 );

        rCache.det02 = Simd::MultiplyF32( rRow0[ 0 ], rRow1[ 2 ] );
        rCache.det02 = Simd::MultiplySubtractReverseF32( rRow0[ 2 ], rRow1[ 0 ], rCache.det02 );

        rCache.det03 = Simd::MultiplyF32( rRow0[ 0 ], rRow1[ 3 ] );
        rCache.det03 = Simd::MultiplySubtractReverseF32( rRow0[ 3 ], rRow1[ 0 ], rCache.det03 );

        rCache.det12 = Simd::MultiplyF32( rRow0[ 1 ], rRow1[ 2 ] );
        rCache.det12 = Simd::MultiplySubtractReverseF32( rRow0[ 2 ], rRow1[ 1 ], rCache.det12 );

        rCache.det13 = Simd::MultiplyF32( rRow0[ 1 ], rRow1[ 3 ] );
        rCache.det13 = Simd::MultiplySubtractReverseF32( rRow0[ 3 ], rRow1[ 1 ], rCache.det13 );

        rCache.det23 = Simd::MultiplyF32( rRow0[ 2 ], rRow1[ 3 ] );
        rCache.det23 = Simd::MultiplySubtractReverseF32( rRow0[ 3 ], rRow1[ 2 ], rCache.det23 );
    }

    static HELIUM_FORCEINLINE void ComputeDet33Helper(
        const SimdVector ( &rRow )[ 4 ],
        const Determinant22Cache& rCache22,
        Determinant33Cache& rCache33 )
    {
        rCache33.detSubmat0 = Simd::MultiplyF32( rRow[ 1 ], rCache22.det23 );
        rCache33.detSubmat0 = Simd::MultiplySubtractReverseF32( rRow[ 2 ], rCache22.det13, rCache33.detSubmat0 );
        rCache33.detSubmat0 = Simd::MultiplyAddF32( rRow[ 3 ], rCache22.det12, rCache33.detSubmat0 );

        rCache33.detSubmat1 = Simd::MultiplyF32( rRow[ 0 ], rCache22.det23 );
        rCache33.detSubmat1 = Simd::MultiplySubtractReverseF32( rRow[ 2 ], rCache22.det03, rCache33.detSubmat1 );
        rCache33.detSubmat1 = Simd::MultiplyAddF32( rRow[ 3 ], rCache22.det02, rCache33.detSubmat1 );

        rCache33.detSubmat2 = Simd::MultiplyF32( rRow[ 0 ], rCache22.det13 );
        rCache33.detSubmat2 = Simd::MultiplySubtractReverseF32( rRow[ 1 ], rCache22.det03, rCache33.detSubmat2 );
        rCache33.detSubmat2 = Simd::MultiplyAddF32( rRow[ 3 ], rCache22.det01, rCache33.detSubmat2 );

        rCache33.detSubmat3 = Simd::MultiplyF32( rRow[ 0 ], rCache22.det12 );
        rCache33.detSubmat3 = Simd::MultiplySubtractReverseF32( rRow[ 1 ], rCache22.det02, rCache33.detSubmat3 );
        rCache33.detSubmat3 = Simd::MultiplyAddF32( rRow[ 2 ], rCache22.det01, rCache33.detSubmat3 );
    }

    static HELIUM_FORCEINLINE SimdVector CalculateDeterminant(
        const Matrix44Soa& rMatrix,
        Determinant22Cache& rRow23Cache,
        Determinant33Cache& rRow0SubmatCache )
    {
        ComputeDet22Helper( rMatrix.m_matrix[ 2 ], rMatrix.m_matrix[ 3 ], rRow23Cache );
        ComputeDet33Helper( rMatrix.m_matrix[ 1 ], rRow23Cache, rRow0SubmatCache );

        SimdVector determinant = Simd::MultiplyF32( rMatrix.m_matrix[ 0 ][ 0 ], rRow0SubmatCache.detSubmat0 );
        determinant = Simd::MultiplySubtractReverseF32(
            rMatrix.m_matrix[ 0 ][ 1 ],
            rRow0SubmatCache.detSubmat1,
            determinant );
        determinant = Simd::MultiplyAddF32( rMatrix.m_matrix[ 0 ][ 2 ], rRow0SubmatCache.detSubmat2, determinant );
        determinant = Simd::MultiplySubtractReverseF32(
            rMatrix.m_matrix[ 0 ][ 3 ],
            rRow0SubmatCache.detSubmat3,
            determinant );

        return determinant;
    }

    static HELIUM_FORCEINLINE void CopyTransposedVectors(
        const Matrix44Soa& rSource,
        Matrix44Soa& rDest,
        size_t indexA,
        size_t indexB )
    {
        SimdVector temp = rSource.m_matrix[ indexA ][ indexB ];
        rDest.m_matrix[ indexA ][ indexB ] = rSource.m_matrix[ indexB ][ indexA ];
        rDest.m_matrix[ indexB ][ indexA ] = temp;
    }

    /// Set this matrix to a rotation matrix.
    ///
    /// Any translation or scaling in this matrix will be reset.
    ///
    /// @param[in] rRotation  Rotation to set.
    ///
    /// @see SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44Soa::SetRotation( const QuatSoa& rRotation )
    {
        SetRotationOnly( rRotation );

        SimdVector zeroVec = Simd::LoadZeros();
        m_matrix[ 3 ][ 0 ] = zeroVec;
        m_matrix[ 3 ][ 1 ] = zeroVec;
        m_matrix[ 3 ][ 2 ] = zeroVec;
        m_matrix[ 3 ][ 3 ] = Simd::SetSplatF32( 1.0f );
    }

    /// Set this matrix to a translation matrix.
    ///
    /// Any rotation or scaling in this matrix will be reset.
    ///
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetScaling(), SetRotationTranslation(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44Soa::SetTranslation( const Vector3Soa& rTranslation )
    {
        SimdVector zeroVec = Simd::LoadZeros();
        SimdVector oneVec = Simd::SetSplatF32( 1.0f );
        m_matrix[ 0 ][ 0 ] = oneVec;
        m_matrix[ 0 ][ 1 ] = zeroVec;
        m_matrix[ 0 ][ 2 ] = zeroVec;
        m_matrix[ 0 ][ 3 ] = zeroVec;
        m_matrix[ 1 ][ 0 ] = zeroVec;
        m_matrix[ 1 ][ 1 ] = oneVec;
        m_matrix[ 1 ][ 2 ] = zeroVec;
        m_matrix[ 1 ][ 3 ] = zeroVec;
        m_matrix[ 2 ][ 0 ] = zeroVec;
        m_matrix[ 2 ][ 1 ] = zeroVec;
        m_matrix[ 2 ][ 2 ] = oneVec;
        m_matrix[ 2 ][ 3 ] = zeroVec;

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
    void Matrix44Soa::SetTranslation( const Vector4Soa& rTranslation )
    {
        SimdVector zeroVec = Simd::LoadZeros();
        SimdVector oneVec = Simd::SetSplatF32( 1.0f );
        m_matrix[ 0 ][ 0 ] = oneVec;
        m_matrix[ 0 ][ 1 ] = zeroVec;
        m_matrix[ 0 ][ 2 ] = zeroVec;
        m_matrix[ 0 ][ 3 ] = zeroVec;
        m_matrix[ 1 ][ 0 ] = zeroVec;
        m_matrix[ 1 ][ 1 ] = oneVec;
        m_matrix[ 1 ][ 2 ] = zeroVec;
        m_matrix[ 1 ][ 3 ] = zeroVec;
        m_matrix[ 2 ][ 0 ] = zeroVec;
        m_matrix[ 2 ][ 1 ] = zeroVec;
        m_matrix[ 2 ][ 2 ] = oneVec;
        m_matrix[ 2 ][ 3 ] = zeroVec;

        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a uniform scaling matrix.
    ///
    /// Any rotation or translation in this matrix will be reset.
    ///
    /// @param[in] rScaling  Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44Soa::SetScaling( const SimdVector& rScaling )
    {
        SimdVector zeroVec = Simd::LoadZeros();

        m_matrix[ 0 ][ 0 ] = rScaling;
        m_matrix[ 0 ][ 1 ] = zeroVec;
        m_matrix[ 0 ][ 2 ] = zeroVec;
        m_matrix[ 0 ][ 3 ] = zeroVec;
        m_matrix[ 1 ][ 0 ] = zeroVec;
        m_matrix[ 1 ][ 1 ] = rScaling;
        m_matrix[ 1 ][ 2 ] = zeroVec;
        m_matrix[ 1 ][ 3 ] = zeroVec;
        m_matrix[ 2 ][ 0 ] = zeroVec;
        m_matrix[ 2 ][ 1 ] = zeroVec;
        m_matrix[ 2 ][ 2 ] = rScaling;
        m_matrix[ 2 ][ 3 ] = zeroVec;
        m_matrix[ 3 ][ 0 ] = zeroVec;
        m_matrix[ 3 ][ 1 ] = zeroVec;
        m_matrix[ 3 ][ 2 ] = zeroVec;
        m_matrix[ 3 ][ 3 ] = Simd::SetSplatF32( 1.0f );
    }

    /// Set this matrix to a non-uniform scaling matrix.
    ///
    /// Any rotation or translation in this matrix will be reset.
    ///
    /// @param[in] rScaling  Vector specifying the scaling factors along each axis.
    ///
    /// @see SetRotation(), SetTranslation(), SetRotationTranslation(), SetRotationTranslationScaling(),
    ///      SetRotationOnly(), SetTranslationOnly()
    void Matrix44Soa::SetScaling( const Vector3Soa& rScaling )
    {
        SimdVector zeroVec = Simd::LoadZeros();

        m_matrix[ 0 ][ 0 ] = rScaling.m_x;
        m_matrix[ 0 ][ 1 ] = zeroVec;
        m_matrix[ 0 ][ 2 ] = zeroVec;
        m_matrix[ 0 ][ 3 ] = zeroVec;
        m_matrix[ 1 ][ 0 ] = zeroVec;
        m_matrix[ 1 ][ 1 ] = rScaling.m_y;
        m_matrix[ 1 ][ 2 ] = zeroVec;
        m_matrix[ 1 ][ 3 ] = zeroVec;
        m_matrix[ 2 ][ 0 ] = zeroVec;
        m_matrix[ 2 ][ 1 ] = zeroVec;
        m_matrix[ 2 ][ 2 ] = rScaling.m_z;
        m_matrix[ 2 ][ 3 ] = zeroVec;
        m_matrix[ 3 ][ 0 ] = zeroVec;
        m_matrix[ 3 ][ 1 ] = zeroVec;
        m_matrix[ 3 ][ 2 ] = zeroVec;
        m_matrix[ 3 ][ 3 ] = Simd::SetSplatF32( 1.0f );
    }

    /// Set this matrix to a rotation/translation matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslationScaling(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44Soa::SetRotationTranslation( const QuatSoa& rRotation, const Vector3Soa& rTranslation )
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
    void Matrix44Soa::SetRotationTranslation( const QuatSoa& rRotation, const Vector4Soa& rTranslation )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44Soa::SetRotationTranslationScaling(
        const QuatSoa& rRotation,
        const Vector3Soa& rTranslation,
        const SimdVector& rScaling )
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
    /// @param[in] rScaling      Scaling factor.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44Soa::SetRotationTranslationScaling(
        const QuatSoa& rRotation,
        const Vector4Soa& rTranslation,
        const SimdVector& rScaling )
    {
        SetRotationOnly( rRotation );
        SetTranslationOnly( rTranslation );

        ScaleLocal( rScaling );
    }

    /// Set this matrix to a rotation/translation/scaling matrix.
    ///
    /// @param[in] rRotation     Rotation to set.
    /// @param[in] rTranslation  Translation to set.
    /// @param[in] rScaling      Vector specifying the scaling factors along each axis.
    ///
    /// @see SetRotation(), SetTranslation(), SetScaling(), SetRotationTranslation(), SetRotationOnly(),
    ///      SetTranslationOnly()
    void Matrix44Soa::SetRotationTranslationScaling(
        const QuatSoa& rRotation,
        const Vector3Soa& rTranslation,
        const Vector3Soa& rScaling )
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
    void Matrix44Soa::SetRotationTranslationScaling(
        const QuatSoa& rRotation,
        const Vector4Soa& rTranslation,
        const Vector3Soa& rScaling )
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
    void Matrix44Soa::SetRotationOnly( const QuatSoa& rRotation )
    {
        SimdVector xx = Simd::MultiplyF32( rRotation.m_x, rRotation.m_x );
        SimdVector xy = Simd::MultiplyF32( rRotation.m_x, rRotation.m_y );
        SimdVector xz = Simd::MultiplyF32( rRotation.m_x, rRotation.m_z );
        SimdVector xw = Simd::MultiplyF32( rRotation.m_x, rRotation.m_w );
        SimdVector yy = Simd::MultiplyF32( rRotation.m_y, rRotation.m_y );
        SimdVector yz = Simd::MultiplyF32( rRotation.m_y, rRotation.m_z );
        SimdVector yw = Simd::MultiplyF32( rRotation.m_y, rRotation.m_w );
        SimdVector zz = Simd::MultiplyF32( rRotation.m_z, rRotation.m_z );
        SimdVector zw = Simd::MultiplyF32( rRotation.m_z, rRotation.m_w );

        SimdVector oneVec = Simd::SetSplatF32( 1.0f );

        SimdVector temp;

        temp = Simd::AddF32( yy, zz );
        m_matrix[ 0 ][ 0 ] = Simd::SubtractF32( oneVec, Simd::AddF32( temp, temp ) );

        temp = Simd::AddF32( xy, zw );
        m_matrix[ 0 ][ 1 ] = Simd::AddF32( temp, temp );

        temp = Simd::SubtractF32( xz, yw );
        m_matrix[ 0 ][ 2 ] = Simd::AddF32( temp, temp );

        temp = Simd::SubtractF32( xy, zw );
        m_matrix[ 1 ][ 0 ] = Simd::AddF32( temp, temp );

        temp = Simd::AddF32( xx, zz );
        m_matrix[ 1 ][ 1 ] = Simd::SubtractF32( oneVec, Simd::AddF32( temp, temp ) );

        temp = Simd::AddF32( yz, xw );
        m_matrix[ 1 ][ 2 ] = Simd::AddF32( temp, temp );

        temp = Simd::AddF32( xz, yw );
        m_matrix[ 2 ][ 0 ] = Simd::AddF32( temp, temp );

        temp = Simd::SubtractF32( yz, xw );
        m_matrix[ 2 ][ 1 ] = Simd::AddF32( temp, temp );

        temp = Simd::AddF32( xx, yy );
        m_matrix[ 2 ][ 2 ] = Simd::SubtractF32( oneVec, Simd::AddF32( temp, temp ) );

        SimdVector zeroVec = Simd::LoadZeros();
        m_matrix[ 0 ][ 3 ] = zeroVec;
        m_matrix[ 1 ][ 3 ] = zeroVec;
        m_matrix[ 2 ][ 3 ] = zeroVec;
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
    void Matrix44Soa::SetTranslationOnly( const Vector3Soa& rTranslation )
    {
        m_matrix[ 3 ][ 0 ] = rTranslation.m_x;
        m_matrix[ 3 ][ 1 ] = rTranslation.m_y;
        m_matrix[ 3 ][ 2 ] = rTranslation.m_z;
        m_matrix[ 3 ][ 3 ] = Simd::SetSplatF32( 1.0f );
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
    void Matrix44Soa::SetTranslationOnly( const Vector4Soa& rTranslation )
    {
        m_matrix[ 3 ][ 0 ] = rTranslation.m_x;
        m_matrix[ 3 ][ 1 ] = rTranslation.m_y;
        m_matrix[ 3 ][ 2 ] = rTranslation.m_z;
        m_matrix[ 3 ][ 3 ] = rTranslation.m_w;
    }

    /// Translate this matrix in world-space (post-multiply).
    ///
    /// This operates under the assumption that the last element of each matrix axis (the first three rows) is 0, and
    /// the last element of the matrix translation component (the last row) is 1.
    ///
    /// @param[in] rTranslation  Amount by which to translate.
    ///
    /// @see TranslateLocal(), ScaleWorld(), ScaleLocal()
    void Matrix44Soa::TranslateWorld( const Vector3Soa& rTranslation )
    {
        m_matrix[ 3 ][ 0 ] = Simd::AddF32( m_matrix[ 3 ][ 0 ], rTranslation.m_x );
        m_matrix[ 3 ][ 1 ] = Simd::AddF32( m_matrix[ 3 ][ 1 ], rTranslation.m_y );
        m_matrix[ 3 ][ 2 ] = Simd::AddF32( m_matrix[ 3 ][ 2 ], rTranslation.m_z );
    }

    /// Translate this matrix in local-space (pre-multiply).
    ///
    /// @param[in] rTranslation  Amount by which to translate.
    ///
    /// @see TranslateWorld(), ScaleWorld(), ScaleLocal()
    void Matrix44Soa::TranslateLocal( const Vector3Soa& rTranslation )
    {
        SimdVector temp;

        temp = Simd::MultiplyAddF32( m_matrix[ 0 ][ 0 ], rTranslation.m_x, m_matrix[ 3 ][ 0 ] );
        temp = Simd::MultiplyAddF32( m_matrix[ 1 ][ 0 ], rTranslation.m_y, temp );
        m_matrix[ 3 ][ 0 ] = Simd::MultiplyAddF32( m_matrix[ 2 ][ 0 ], rTranslation.m_z, temp );

        temp = Simd::MultiplyAddF32( m_matrix[ 0 ][ 1 ], rTranslation.m_x, m_matrix[ 3 ][ 1 ] );
        temp = Simd::MultiplyAddF32( m_matrix[ 1 ][ 1 ], rTranslation.m_y, temp );
        m_matrix[ 3 ][ 1 ] = Simd::MultiplyAddF32( m_matrix[ 2 ][ 1 ], rTranslation.m_z, temp );

        temp = Simd::MultiplyAddF32( m_matrix[ 0 ][ 2 ], rTranslation.m_x, m_matrix[ 3 ][ 2 ] );
        temp = Simd::MultiplyAddF32( m_matrix[ 1 ][ 2 ], rTranslation.m_y, temp );
        m_matrix[ 3 ][ 2 ] = Simd::MultiplyAddF32( m_matrix[ 2 ][ 2 ], rTranslation.m_z, temp );
    }

    /// Scale this matrix in world-space (post-multiply).
    ///
    /// @param[in] rScaling  Amount by which to scale.
    ///
    /// @see ScaleLocal(), TranslateWorld(), TranslateLocal()
    void Matrix44Soa::ScaleWorld( const SimdVector& rScaling )
    {
        m_matrix[ 0 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 0 ], rScaling );
        m_matrix[ 0 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 1 ], rScaling );
        m_matrix[ 0 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 2 ], rScaling );
        m_matrix[ 1 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 0 ], rScaling );
        m_matrix[ 1 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 1 ], rScaling );
        m_matrix[ 1 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 2 ], rScaling );
        m_matrix[ 2 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 0 ], rScaling );
        m_matrix[ 2 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 1 ], rScaling );
        m_matrix[ 2 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 2 ], rScaling );
        m_matrix[ 3 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 0 ], rScaling );
        m_matrix[ 3 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 1 ], rScaling );
        m_matrix[ 3 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 2 ], rScaling );
    }

    /// Scale this matrix in world-space (post-multiply).
    ///
    /// @param[in] rScaling  Vector specifying the amount by which to scale along each axis.
    ///
    /// @see ScaleLocal(), TranslateWorld(), TranslateLocal()
    void Matrix44Soa::ScaleWorld( const Vector3Soa& rScaling )
    {
        m_matrix[ 0 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 0 ], rScaling.m_x );
        m_matrix[ 0 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 1 ], rScaling.m_y );
        m_matrix[ 0 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 2 ], rScaling.m_z );
        m_matrix[ 1 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 0 ], rScaling.m_x );
        m_matrix[ 1 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 1 ], rScaling.m_y );
        m_matrix[ 1 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 2 ], rScaling.m_z );
        m_matrix[ 2 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 0 ], rScaling.m_x );
        m_matrix[ 2 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 1 ], rScaling.m_y );
        m_matrix[ 2 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 2 ], rScaling.m_z );
        m_matrix[ 3 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 0 ], rScaling.m_x );
        m_matrix[ 3 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 1 ], rScaling.m_y );
        m_matrix[ 3 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 3 ][ 2 ], rScaling.m_z );
    }

    /// Scale this matrix in local-space (pre-multiply).
    ///
    /// @param[in] rScaling  Amount by which to scale.
    ///
    /// @see ScaleWorld(), TranslateWorld(), TranslateLocal()
    void Matrix44Soa::ScaleLocal( const SimdVector& rScaling )
    {
        m_matrix[ 0 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 0 ], rScaling );
        m_matrix[ 0 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 1 ], rScaling );
        m_matrix[ 0 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 2 ], rScaling );
        m_matrix[ 0 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 3 ], rScaling );
        m_matrix[ 1 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 0 ], rScaling );
        m_matrix[ 1 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 1 ], rScaling );
        m_matrix[ 1 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 2 ], rScaling );
        m_matrix[ 1 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 3 ], rScaling );
        m_matrix[ 2 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 0 ], rScaling );
        m_matrix[ 2 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 1 ], rScaling );
        m_matrix[ 2 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 2 ], rScaling );
        m_matrix[ 2 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 3 ], rScaling );
    }

    /// Scale this matrix in local-space (pre-multiply).
    ///
    /// @param[in] rScaling  Vector specifying the amount by which to scale along each axis.
    ///
    /// @see ScaleWorld(), TranslateWorld(), TranslateLocal()
    void Matrix44Soa::ScaleLocal( const Vector3Soa& rScaling )
    {
        m_matrix[ 0 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 0 ], rScaling.m_x );
        m_matrix[ 0 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 1 ], rScaling.m_x );
        m_matrix[ 0 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 2 ], rScaling.m_x );
        m_matrix[ 0 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 0 ][ 3 ], rScaling.m_x );
        m_matrix[ 1 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 0 ], rScaling.m_y );
        m_matrix[ 1 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 1 ], rScaling.m_y );
        m_matrix[ 1 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 2 ], rScaling.m_y );
        m_matrix[ 1 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 1 ][ 3 ], rScaling.m_y );
        m_matrix[ 2 ][ 0 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 0 ], rScaling.m_z );
        m_matrix[ 2 ][ 1 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 1 ], rScaling.m_z );
        m_matrix[ 2 ][ 2 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 2 ], rScaling.m_z );
        m_matrix[ 2 ][ 3 ] = Simd::MultiplyF32( m_matrix[ 2 ][ 3 ], rScaling.m_z );
    }

    /// Set this matrix to the product of two matrices.
    ///
    /// @param[in] rMatrix0  First matrix.
    /// @param[in] rMatrix1  Second matrix.
    void Matrix44Soa::MultiplySet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 )
    {
        Matrix44Soa result;

        MultiplyResultRow( result.m_matrix[ 0 ], rMatrix0.m_matrix[ 0 ], rMatrix1 );
        MultiplyResultRow( result.m_matrix[ 1 ], rMatrix0.m_matrix[ 1 ], rMatrix1 );
        MultiplyResultRow( result.m_matrix[ 2 ], rMatrix0.m_matrix[ 2 ], rMatrix1 );
        MultiplyResultRow( result.m_matrix[ 3 ], rMatrix0.m_matrix[ 3 ], rMatrix1 );

        *this = result;
    }

    /// Compute the determinant of this matrix.
    ///
    /// @return  Matrix determinant.
    SimdVector Matrix44Soa::GetDeterminant() const
    {
        Determinant22Cache det22Cache;
        Determinant33Cache det33Cache;
        SimdVector determinant = CalculateDeterminant( *this, det22Cache, det33Cache );

        return determinant;
    }

    /// Get the inverse of this matrix.
    ///
    /// @param[out] rMatrix  Matrix inverse.
    ///
    /// @see Invert()
    void Matrix44Soa::GetInverse( Matrix44Soa& rMatrix ) const
    {
        Determinant22Cache det22Cache;
        Determinant33Cache det33Cache;
        SimdVector determinant = CalculateDeterminant( *this, det22Cache, det33Cache );

        SimdVector invDeterminantEven = Simd::InverseF32( determinant );
        SimdVector invDeterminantOdd = Simd::Xor( invDeterminantEven, Simd::SetSplatU32( 0x80000000 ) );

        SimdVector result00 = Simd::MultiplyF32( det33Cache.detSubmat0, invDeterminantEven );
        SimdVector result10 = Simd::MultiplyF32( det33Cache.detSubmat1, invDeterminantOdd );
        SimdVector result20 = Simd::MultiplyF32( det33Cache.detSubmat2, invDeterminantEven );
        SimdVector result30 = Simd::MultiplyF32( det33Cache.detSubmat3, invDeterminantOdd );

        ComputeDet33Helper( m_matrix[ 0 ], det22Cache, det33Cache );

        SimdVector result01 = Simd::MultiplyF32( det33Cache.detSubmat0, invDeterminantOdd );
        SimdVector result11 = Simd::MultiplyF32( det33Cache.detSubmat1, invDeterminantEven );
        SimdVector result21 = Simd::MultiplyF32( det33Cache.detSubmat2, invDeterminantOdd );
        SimdVector result31 = Simd::MultiplyF32( det33Cache.detSubmat3, invDeterminantEven );

        ComputeDet22Helper( m_matrix[ 0 ], m_matrix[ 1 ], det22Cache );

        ComputeDet33Helper( m_matrix[ 3 ], det22Cache, det33Cache );

        SimdVector result02 = Simd::MultiplyF32( det33Cache.detSubmat0, invDeterminantEven );
        SimdVector result12 = Simd::MultiplyF32( det33Cache.detSubmat1, invDeterminantOdd );
        SimdVector result22 = Simd::MultiplyF32( det33Cache.detSubmat2, invDeterminantEven );
        SimdVector result32 = Simd::MultiplyF32( det33Cache.detSubmat3, invDeterminantOdd );

        ComputeDet33Helper( m_matrix[ 2 ], det22Cache, det33Cache );

        SimdVector result03 = Simd::MultiplyF32( det33Cache.detSubmat0, invDeterminantOdd );
        SimdVector result13 = Simd::MultiplyF32( det33Cache.detSubmat1, invDeterminantEven );
        SimdVector result23 = Simd::MultiplyF32( det33Cache.detSubmat2, invDeterminantOdd );
        SimdVector result33 = Simd::MultiplyF32( det33Cache.detSubmat3, invDeterminantEven );

        rMatrix.m_matrix[ 0 ][ 0 ] = result00;
        rMatrix.m_matrix[ 0 ][ 1 ] = result01;
        rMatrix.m_matrix[ 0 ][ 2 ] = result02;
        rMatrix.m_matrix[ 0 ][ 3 ] = result03;
        rMatrix.m_matrix[ 1 ][ 0 ] = result10;
        rMatrix.m_matrix[ 1 ][ 1 ] = result11;
        rMatrix.m_matrix[ 1 ][ 2 ] = result12;
        rMatrix.m_matrix[ 1 ][ 3 ] = result13;
        rMatrix.m_matrix[ 2 ][ 0 ] = result20;
        rMatrix.m_matrix[ 2 ][ 1 ] = result21;
        rMatrix.m_matrix[ 2 ][ 2 ] = result22;
        rMatrix.m_matrix[ 2 ][ 3 ] = result23;
        rMatrix.m_matrix[ 3 ][ 0 ] = result30;
        rMatrix.m_matrix[ 3 ][ 1 ] = result31;
        rMatrix.m_matrix[ 3 ][ 2 ] = result32;
        rMatrix.m_matrix[ 3 ][ 3 ] = result33;
    }

    /// Get the transpose of this matrix.
    ///
    /// @param[out] rMatrix  Matrix transpose.
    ///
    /// @see Transpose()
    void Matrix44Soa::GetTranspose( Matrix44Soa& rMatrix ) const
    {
        rMatrix.m_matrix[ 0 ][ 0 ] = m_matrix[ 0 ][ 0 ];
        rMatrix.m_matrix[ 1 ][ 1 ] = m_matrix[ 1 ][ 1 ];
        rMatrix.m_matrix[ 2 ][ 2 ] = m_matrix[ 2 ][ 2 ];
        rMatrix.m_matrix[ 3 ][ 3 ] = m_matrix[ 3 ][ 3 ];

        CopyTransposedVectors( *this, rMatrix, 0, 1 );
        CopyTransposedVectors( *this, rMatrix, 0, 2 );
        CopyTransposedVectors( *this, rMatrix, 0, 3 );
        CopyTransposedVectors( *this, rMatrix, 1, 2 );
        CopyTransposedVectors( *this, rMatrix, 1, 3 );
        CopyTransposedVectors( *this, rMatrix, 2, 3 );
    }
}
