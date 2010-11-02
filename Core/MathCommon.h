//----------------------------------------------------------------------------------------------------------------------
// MathCommon.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_MATH_COMMON_H
#define LUNAR_CORE_MATH_COMMON_H

#include "Core/Core.h"

/// @defgroup mathconstants Standard Math Constants
//@{
/// Pi.
#define L_PI 3.14159265358979323846
/// 2 * Pi.
#define L_TWOPI ( L_PI * 2.0 )
/// Pi / 2.
#define L_PI_2 1.57079632679489661923
/// Pi / 4.
#define L_PI_4 0.78539816339744830962
/// 3 * Pi / 4.
#define L_3PI_4 2.3561944901923448370E0
/// Square root of Pi.
#define L_SQRTPI 1.77245385090551602792981
/// 1 / Pi.
#define L_1_PI 0.31830988618379067154
/// 2 / Pi.
#define L_2_PI 0.63661977236758134308
/// 2 / (square root of Pi).
#define L_2_SQRTPI 1.12837916709551257390

/// Degrees-to-radians scale.
#define L_DEG_TO_RAD_SCALE ( L_PI / 180.0f )
/// Radians-to-degrees scale.
#define L_RAD_TO_DEG_SCALE ( 180.0f / L_PI )

/// Generic single-precision floating-point epsilon value.
#define L_EPSILON ( 1.0e-8f )
//@}

namespace Lunar
{
    /// @defgroup mathgeneral General Math Functions
    //@{
    template< typename T > T& Min( T& rA, T& rB );
    template< typename T > const T& Min( const T& rA, const T& rB );
    template< typename T > T& Max( T& rA, T& rB );
    template< typename T > const T& Max( const T& rA, const T& rB );
    template< typename T > T& Clamp( T& rValue, T& rMin, T& rMax );
    template< typename T > const T& Clamp( const T& rValue, const T& rMin, const T& rMax );

    template< typename T > T Abs( const T& rValue );
    inline int64_t Abs( int64_t value );
    inline float32_t Abs( float32_t value );
    inline float64_t Abs( float64_t value );

    template< typename T > T Square( const T& rValue );

    inline float32_t Sqrt( float32_t value );
    inline float64_t Sqrt( float64_t value );

    template< typename T > bool IsPowerOfTwo( const T& rValue );

    inline size_t Log2( uint32_t value );
    inline size_t Log2( uint64_t value );
    //@}

    /// @defgroup mathtrig Trigonometric Functions
    //@{
    inline float32_t Sin( float32_t radians );
    inline float32_t Cos( float32_t radians );
    inline float32_t Tan( float32_t radians );
    inline float32_t Asin( float32_t value );
    inline float32_t Acos( float32_t value );
    inline float32_t Atan( float32_t value );
    inline float32_t Atan2( float32_t y, float32_t x );

    inline float32_t DegToRad( float32_t deg );
    inline float32_t RadToDeg( float32_t rad );
    //@}
}

#include "Core/MathCommon.inl"

#endif  // LUNAR_CORE_MATH_COMMON_H
