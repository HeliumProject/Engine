//----------------------------------------------------------------------------------------------------------------------
// Matrix44Soa.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_MATRIX44_SOA_H
#define LUNAR_CORE_MATRIX44_SOA_H

#include "Core/Matrix44.h"
#include "Core/Vector3Soa.h"
#include "Core/Vector4Soa.h"

namespace Lunar
{
    class QuatSoa;

    /// SIMD-optimized structure-of-arrays 4x4 matrix.
    HELIUM_SIMD_ALIGN_PRE class LUNAR_CORE_API Matrix44Soa
    {
    public:
        /// Identity matrix.
        static const Matrix44Soa IDENTITY;

        /// Rotation matrix initialization identifier.
        enum EInitRotation
        {
            INIT_ROTATION
        };

        /// Translation matrix initialization identifier.
        enum EInitTranslation
        {
            INIT_TRANSLATION
        };

        /// Scaling matrix initialization identifier.
        enum EInitScaling
        {
            INIT_SCALING
        };

        /// Rotation and translation matrix initialization identifier.
        enum EInitRotationTranslation
        {
            INIT_ROTATION_TRANSLATION
        };

        /// Rotation, translation, and scaling matrix identifier.
        enum EInitRotationTranslationScaling
        {
            INIT_ROTATION_TRANSLATION_SCALING
        };

        /// Matrix components.
        Helium::SimdVector m_matrix[ 4 ][ 4 ];

        /// @name Construction/Destruction
        //@{
        inline Matrix44Soa();
        inline Matrix44Soa(
            const Helium::SimdVector& rXAxisX, const Helium::SimdVector& rXAxisY, const Helium::SimdVector& rXAxisZ, const Helium::SimdVector& rXAxisW,
            const Helium::SimdVector& rYAxisX, const Helium::SimdVector& rYAxisY, const Helium::SimdVector& rYAxisZ, const Helium::SimdVector& rYAxisW,
            const Helium::SimdVector& rZAxisX, const Helium::SimdVector& rZAxisY, const Helium::SimdVector& rZAxisZ, const Helium::SimdVector& rZAxisW,
            const Helium::SimdVector& rTranslateX, const Helium::SimdVector& rTranslateY, const Helium::SimdVector& rTranslateZ,
            const Helium::SimdVector& rTranslateW );
        inline Matrix44Soa(
            const Vector4Soa& rXAxis, const Vector4Soa& rYAxis, const Vector4Soa& rZAxis,
            const Vector4Soa& rTranslate );
        inline Matrix44Soa( EInitRotation, const QuatSoa& rRotation );
        inline Matrix44Soa( EInitTranslation, const Vector3Soa& rTranslation );
        inline Matrix44Soa( EInitTranslation, const Vector4Soa& rTranslation );
        inline Matrix44Soa( EInitScaling, const Helium::SimdVector& rScaling );
        inline Matrix44Soa( EInitScaling, const Vector3Soa& rScaling );
        inline Matrix44Soa( EInitRotationTranslation, const QuatSoa& rRotation, const Vector3Soa& rTranslation );
        inline Matrix44Soa( EInitRotationTranslation, const QuatSoa& rRotation, const Vector4Soa& rTranslation );
        inline Matrix44Soa(
            EInitRotationTranslationScaling, const QuatSoa& rRotation, const Vector3Soa& rTranslation,
            const Helium::SimdVector& rScaling );
        inline Matrix44Soa(
            EInitRotationTranslationScaling, const QuatSoa& rRotation, const Vector4Soa& rTranslation,
            const Helium::SimdVector& rScaling );
        inline Matrix44Soa(
            EInitRotationTranslationScaling, const QuatSoa& rRotation, const Vector3Soa& rTranslation,
            const Vector3Soa& rScaling );
        inline Matrix44Soa(
            EInitRotationTranslationScaling, const QuatSoa& rRotation, const Vector4Soa& rTranslation,
            const Vector3Soa& rScaling );
        inline Matrix44Soa(
            const float32_t* pXAxisX, const float32_t* pXAxisY, const float32_t* pXAxisZ, const float32_t* pXAxisW,
            const float32_t* pYAxisX, const float32_t* pYAxisY, const float32_t* pYAxisZ, const float32_t* pYAxisW,
            const float32_t* pZAxisX, const float32_t* pZAxisY, const float32_t* pZAxisZ, const float32_t* pZAxisW,
            const float32_t* pTranslateX, const float32_t* pTranslateY, const float32_t* pTranslateZ,
            const float32_t* pTranslateW );
        inline explicit Matrix44Soa( const Matrix44& rMatrix );
        //@}

        /// @name Loading and Storage
        //@{
        inline void Load(
            const float32_t* pXAxisX, const float32_t* pXAxisY, const float32_t* pXAxisZ, const float32_t* pXAxisW,
            const float32_t* pYAxisX, const float32_t* pYAxisY, const float32_t* pYAxisZ, const float32_t* pYAxisW,
            const float32_t* pZAxisX, const float32_t* pZAxisY, const float32_t* pZAxisZ, const float32_t* pZAxisW,
            const float32_t* pTranslateX, const float32_t* pTranslateY, const float32_t* pTranslateZ,
            const float32_t* pTranslateW );
        inline void Load4Splat(
            const float32_t* pXAxisX, const float32_t* pXAxisY, const float32_t* pXAxisZ, const float32_t* pXAxisW,
            const float32_t* pYAxisX, const float32_t* pYAxisY, const float32_t* pYAxisZ, const float32_t* pYAxisW,
            const float32_t* pZAxisX, const float32_t* pZAxisY, const float32_t* pZAxisZ, const float32_t* pZAxisW,
            const float32_t* pTranslateX, const float32_t* pTranslateY, const float32_t* pTranslateZ,
            const float32_t* pTranslateW );
        inline void Load1Splat(
            const float32_t* pXAxisX, const float32_t* pXAxisY, const float32_t* pXAxisZ, const float32_t* pXAxisW,
            const float32_t* pYAxisX, const float32_t* pYAxisY, const float32_t* pYAxisZ, const float32_t* pYAxisW,
            const float32_t* pZAxisX, const float32_t* pZAxisY, const float32_t* pZAxisZ, const float32_t* pZAxisW,
            const float32_t* pTranslateX, const float32_t* pTranslateY, const float32_t* pTranslateZ,
            const float32_t* pTranslateW );

        inline void Store(
            float32_t* pXAxisX, float32_t* pXAxisY, float32_t* pXAxisZ, float32_t* pXAxisW,
            float32_t* pYAxisX, float32_t* pYAxisY, float32_t* pYAxisZ, float32_t* pYAxisW,
            float32_t* pZAxisX, float32_t* pZAxisY, float32_t* pZAxisZ, float32_t* pZAxisW,
            float32_t* pTranslateX, float32_t* pTranslateY, float32_t* pTranslateZ, float32_t* pTranslateW ) const;
        inline void Store4(
            float32_t* pXAxisX, float32_t* pXAxisY, float32_t* pXAxisZ, float32_t* pXAxisW,
            float32_t* pYAxisX, float32_t* pYAxisY, float32_t* pYAxisZ, float32_t* pYAxisW,
            float32_t* pZAxisX, float32_t* pZAxisY, float32_t* pZAxisZ, float32_t* pZAxisW,
            float32_t* pTranslateX, float32_t* pTranslateY, float32_t* pTranslateZ, float32_t* pTranslateW ) const;
        inline void Store1(
            float32_t* pXAxisX, float32_t* pXAxisY, float32_t* pXAxisZ, float32_t* pXAxisW,
            float32_t* pYAxisX, float32_t* pYAxisY, float32_t* pYAxisZ, float32_t* pYAxisW,
            float32_t* pZAxisX, float32_t* pZAxisY, float32_t* pZAxisZ, float32_t* pZAxisW,
            float32_t* pTranslateX, float32_t* pTranslateY, float32_t* pTranslateZ, float32_t* pTranslateW ) const;

        inline void Splat( const Matrix44& rMatrix );
        //@}

        /// @name Data Access
        //@{
        inline void GetRow( size_t index, Vector4Soa& rRow ) const;
        inline Vector4Soa GetRow( size_t index ) const;
        inline void SetRow( size_t index, const Vector4Soa& rRow );
        //@}

        /// @name Component Initialization
        //@{
        void SetRotation( const QuatSoa& rRotation );

        void SetTranslation( const Vector3Soa& rTranslation );
        void SetTranslation( const Vector4Soa& rTranslation );

        void SetScaling( const Helium::SimdVector& rScaling );
        void SetScaling( const Vector3Soa& rScaling );

        void SetRotationTranslation( const QuatSoa& rRotation, const Vector3Soa& rTranslation );
        void SetRotationTranslation( const QuatSoa& rRotation, const Vector4Soa& rTranslation );

        void SetRotationTranslationScaling(
            const QuatSoa& rRotation, const Vector3Soa& rTranslation, const Helium::SimdVector& rScaling );
        void SetRotationTranslationScaling(
            const QuatSoa& rRotation, const Vector4Soa& rTranslation, const Helium::SimdVector& rScaling );
        void SetRotationTranslationScaling(
            const QuatSoa& rRotation, const Vector3Soa& rTranslation, const Vector3Soa& rScaling );
        void SetRotationTranslationScaling(
            const QuatSoa& rRotation, const Vector4Soa& rTranslation, const Vector3Soa& rScaling );

        void SetRotationOnly( const QuatSoa& rRotation );
        void SetTranslationOnly( const Vector3Soa& rTranslation );
        void SetTranslationOnly( const Vector4Soa& rTranslation );

        void TranslateWorld( const Vector3Soa& rTranslation );
        void TranslateLocal( const Vector3Soa& rTranslation );
        void ScaleWorld( const Helium::SimdVector& rScaling );
        void ScaleWorld( const Vector3Soa& rScaling );
        void ScaleLocal( const Helium::SimdVector& rScaling );
        void ScaleLocal( const Vector3Soa& rScaling );
        //@}

        /// @name Math
        //@{
        inline Matrix44Soa Add( const Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa Subtract( const Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa Multiply( const Matrix44Soa& rMatrix ) const;

        inline Matrix44Soa MultiplyComponents( const Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa DivideComponents( const Matrix44Soa& rMatrix ) const;

        inline void AddSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 );
        inline void SubtractSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 );
        void MultiplySet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 );

        inline void MultiplyComponentsSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 );
        inline void DivideComponentsSet( const Matrix44Soa& rMatrix0, const Matrix44Soa& rMatrix1 );

        Helium::SimdVector GetDeterminant() const;

        void GetInverse( Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa GetInverse() const;
        inline void Invert();

        void GetTranspose( Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa GetTranspose() const;
        inline void Transpose();
        //@}

        /// @name Transformation
        //@{
        inline void Transform( const Vector4Soa& rVector, Vector4Soa& rResult ) const;
        inline Vector4Soa Transform( const Vector4Soa& rVector ) const;

        inline void TransformPoint( const Vector3Soa& rVector, Vector3Soa& rResult ) const;
        inline Vector3Soa TransformPoint( const Vector3Soa& rVector ) const;

        inline void TransformVector( const Vector3Soa& rVector, Vector3Soa& rResult ) const;
        inline Vector3Soa TransformVector( const Vector3Soa& rVector ) const;
        //@}

        /// @name Comparison
        //@{
        inline Helium::SimdMask Equals( const Matrix44Soa& rMatrix, const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON ) const;
        inline Helium::SimdMask NotEquals( const Matrix44Soa& rMatrix, const Helium::SimdVector& rEpsilon = Helium::Simd::EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Matrix44Soa operator+( const Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa operator-( const Matrix44Soa& rMatrix ) const;
        inline Matrix44Soa operator*( const Matrix44Soa& rMatrix ) const;

        inline Matrix44Soa& operator+=( const Matrix44Soa& rMatrix );
        inline Matrix44Soa& operator-=( const Matrix44Soa& rMatrix );
        inline Matrix44Soa& operator*=( const Matrix44Soa& rMatrix );

        inline Helium::SimdMask operator==( const Matrix44Soa& rMatrix ) const;
        inline Helium::SimdMask operator!=( const Matrix44Soa& rMatrix ) const;
        //@}
    } HELIUM_SIMD_ALIGN_POST;
}

#include "Core/Matrix44Soa.inl"

#if HELIUM_SIMD_LRBNI
#include "Core/Matrix44SoaLrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Core/Matrix44SoaSse.inl"
#endif

#endif  // LUNAR_CORE_MATRIX44_SOA_H
