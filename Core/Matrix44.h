//----------------------------------------------------------------------------------------------------------------------
// Matrix44.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_MATRIX44_H
#define LUNAR_CORE_MATRIX44_H

#include "Core/Vector3.h"
#include "Core/Vector4.h"

namespace Lunar
{
    class Quat;

    /// Single-precision floating-point 4x4 matrix.  Note that this is SIMD aligned.
    HELIUM_SIMD_ALIGN_PRE class LUNAR_CORE_API Matrix44
    {
    public:
        /// Identity matrix.
        static const Matrix44 IDENTITY;

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

        /// Perspective projection matrix initialization identifier.
        enum EPerspectiveProjection
        {
            INIT_PERSPECTIVE_PROJECTION
        };

        /// Orthogonal projection matrix initialization identifier.
        enum EOrthogonalProjection
        {
            INIT_ORTHOGONAL_PROJECTION
        };

        /// @name Construction/Destruction
        //@{
        inline Matrix44();
        inline Matrix44(
            float32_t xAxisX, float32_t xAxisY, float32_t xAxisZ, float32_t xAxisW,
            float32_t yAxisX, float32_t yAxisY, float32_t yAxisZ, float32_t yAxisW,
            float32_t zAxisX, float32_t zAxisY, float32_t zAxisZ, float32_t zAxisW,
            float32_t translateX, float32_t translateY, float32_t translateZ, float32_t translateW );
        inline Matrix44(
            const Vector4& rXAxis, const Vector4& rYAxis, const Vector4& rZAxis, const Vector4& rTranslate );
        inline Matrix44( EInitRotation, const Quat& rRotation );
        inline Matrix44( EInitTranslation, const Vector3& rTranslation );
        inline Matrix44( EInitTranslation, const Vector4& rTranslation );
        inline Matrix44( EInitScaling, float32_t scaling );
        inline Matrix44( EInitScaling, const Vector3& rScaling );
        inline Matrix44( EInitRotationTranslation, const Quat& rRotation, const Vector3& rTranslation );
        inline Matrix44( EInitRotationTranslation, const Quat& rRotation, const Vector4& rTranslation );
        inline Matrix44(
            EInitRotationTranslationScaling, const Quat& rRotation, const Vector3& rTranslation, float32_t scaling );
        inline Matrix44(
            EInitRotationTranslationScaling, const Quat& rRotation, const Vector4& rTranslation, float32_t scaling );
        inline Matrix44(
            EInitRotationTranslationScaling, const Quat& rRotation, const Vector3& rTranslation,
            const Vector3& rScaling );
        inline Matrix44(
            EInitRotationTranslationScaling, const Quat& rRotation, const Vector4& rTranslation,
            const Vector3& rScaling );
        inline Matrix44(
            EPerspectiveProjection, float32_t horizontalFovRadians, float32_t aspectRatio, float32_t nearClip,
            float32_t farClip );
        inline Matrix44(
            EPerspectiveProjection, float32_t horizontalFovRadians, float32_t aspectRatio, float32_t nearClip );
        inline Matrix44(
            EOrthogonalProjection, float32_t width, float32_t height, float32_t nearClip, float32_t farClip );
#if HELIUM_SIMD_SIZE == 16
        inline Matrix44(
            const Helium::SimdVector& rXAxis, const Helium::SimdVector& rYAxis, const Helium::SimdVector& rZAxis,
            const Helium::SimdVector& rTranslate );
#elif HELIUM_SIMD_SIZE == 64
        inline explicit Matrix44( const Helium::SimdVector& rMatrix );
#endif
        //@}

        /// @name Data Access
        //@{
#if HELIUM_SIMD_SIZE == 16 || HELIUM_SIMD_SIZE == 64
        inline Helium::SimdVector& GetSimdVector( size_t index );
        inline const Helium::SimdVector& GetSimdVector( size_t index ) const;
        inline void SetSimdVector( size_t index, const Helium::SimdVector& rVector );
#endif

        inline float32_t& GetElement( size_t index );
        inline float32_t GetElement( size_t index ) const;
        inline void SetElement( size_t index, float32_t value );

        inline void GetRow( size_t index, Vector4& rRow ) const;
        inline Vector4 GetRow( size_t index ) const;
        inline void SetRow( size_t index, const Vector4& rRow );
        //@}

        /// @name Component Initialization
        //@{
        void SetRotation( const Quat& rRotation );

        void SetTranslation( const Vector3& rTranslation );
        void SetTranslation( const Vector4& rTranslation );

        void SetScaling( float32_t scaling );
        void SetScaling( const Vector3& rScaling );

        void SetRotationTranslation( const Quat& rRotation, const Vector3& rTranslation );
        void SetRotationTranslation( const Quat& rRotation, const Vector4& rTranslation );

        void SetRotationTranslationScaling( const Quat& rRotation, const Vector3& rTranslation, float32_t scaling );
        void SetRotationTranslationScaling( const Quat& rRotation, const Vector4& rTranslation, float32_t scaling );
        void SetRotationTranslationScaling(
            const Quat& rRotation, const Vector3& rTranslation, const Vector3& rScaling );
        void SetRotationTranslationScaling(
            const Quat& rRotation, const Vector4& rTranslation, const Vector3& rScaling );

        void SetRotationOnly( const Quat& rRotation );
        void SetTranslationOnly( const Vector3& rTranslation );
        void SetTranslationOnly( const Vector4& rTranslation );

        void TranslateWorld( const Vector3& rTranslation );
        void TranslateLocal( const Vector3& rTranslation );
        void ScaleWorld( float32_t scaling );
        void ScaleWorld( const Vector3& rScaling );
        void ScaleLocal( float32_t scaling );
        void ScaleLocal( const Vector3& rScaling );

        void SetPerspectiveProjection(
            float32_t horizontalFovRadians, float32_t aspectRatio, float32_t nearClip, float32_t farClip );
        void SetPerspectiveProjection( float32_t horizontalFovRadians, float32_t aspectRatio, float32_t nearClip );
        void SetOrthogonalProjection( float32_t width, float32_t height, float32_t nearClip, float32_t farClip );
        //@}

        /// @name Math
        //@{
        inline Matrix44 Add( const Matrix44& rMatrix ) const;
        inline Matrix44 Subtract( const Matrix44& rMatrix ) const;
        inline Matrix44 Multiply( const Matrix44& rMatrix ) const;

        inline Matrix44 MultiplyComponents( const Matrix44& rMatrix ) const;
        inline Matrix44 DivideComponents( const Matrix44& rMatrix ) const;

        inline void AddSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 );
        inline void SubtractSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 );
        void MultiplySet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 );

        inline void MultiplyComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 );
        inline void DivideComponentsSet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 );

        float32_t GetDeterminant() const;

        void GetInverse( Matrix44& rMatrix ) const;
        inline Matrix44 GetInverse() const;
        inline void Invert();

        void GetTranspose( Matrix44& rMatrix ) const;
        inline Matrix44 GetTranspose() const;
        inline void Transpose();
        //@}

        /// @name Transformation
        //@{
        inline void Transform( const Vector4& rVector, Vector4& rResult ) const;
        inline Vector4 Transform( const Vector4& rVector ) const;

        inline void TransformPoint( const Vector3& rVector, Vector3& rResult ) const;
        inline Vector3 TransformPoint( const Vector3& rVector ) const;

        inline void TransformVector( const Vector3& rVector, Vector3& rResult ) const;
        inline Vector3 TransformVector( const Vector3& rVector ) const;
        //@}

        /// @name Comparison
        //@{
        inline bool Equals( const Matrix44& rMatrix, float32_t epsilon = L_EPSILON ) const;
        //@}

        /// @name Overloaded Operators
        //@{
        inline Matrix44 operator+( const Matrix44& rMatrix ) const;
        inline Matrix44 operator-( const Matrix44& rMatrix ) const;
        inline Matrix44 operator*( const Matrix44& rMatrix ) const;

        inline Matrix44& operator+=( const Matrix44& rMatrix );
        inline Matrix44& operator-=( const Matrix44& rMatrix );
        inline Matrix44& operator*=( const Matrix44& rMatrix );

        inline bool operator==( const Matrix44& rMatrix ) const;
        inline bool operator!=( const Matrix44& rMatrix ) const;
        //@}

    private:
#if HELIUM_SIMD_SIZE == 16
        /// SIMD vectors containing the matrix values.
        Helium::SimdVector m_matrix[ 4 ];
#elif HELIUM_SIMD_SIZE == 64
        /// SIMD vector containing the matrix values.
        Helium::SimdVector m_matrix;
#else
        /// Matrix values.
        float32_t m_matrix[ 4 ][ 4 ];
#endif
    } HELIUM_SIMD_ALIGN_POST;
}

#include "Core/Matrix44.inl"

#if HELIUM_SIMD_LRBNI
#include "Core/Matrix44Lrbni.inl"
#elif HELIUM_SIMD_SSE
#include "Core/Matrix44Sse.inl"
#endif

#endif  // LUNAR_CORE_MATRIX44_H
