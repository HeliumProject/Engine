//----------------------------------------------------------------------------------------------------------------------
// MathCommon.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_CORE_MATH_COMMON_H
#define HELIUM_CORE_MATH_COMMON_H

#include "Platform/Types.h"
#include "Platform/Assert.h"

#if HELIUM_CC_MSC
#pragma warning( push )
#pragma warning( disable : 4530 )  // C++ exception handler used, but unwind semantics are not enabled. Specify /EHsc
// Temporary workaround for bug in Visual C++ 2008 with including intrin.h and math.h simultaneously
// (see http://connect.microsoft.com/VisualStudio/feedback/details/381422/warning-of-attributes-not-present-on-previous-declaration-on-ceil-using-both-math-h-and-intrin-h).
#pragma warning( disable : 4985 )  // 'symbol name': attributes not present on previous declaration
#endif

#include <type_traits>
#include <cmath>

#if HELIUM_CC_MSC
#pragma warning( pop )
#endif

/// @defgroup mathconstants Standard Math Constants
//@{
/// Pi.
#define HELIUM_PI 3.14159265358979323846
/// 2 * Pi.
#define HELIUM_TWOPI ( HELIUM_PI * 2.0 )
/// Pi / 2.
#define HELIUM_PI_2 1.57079632679489661923
/// Pi / 4.
#define HELIUM_PI_4 0.78539816339744830962
/// 3 * Pi / 4.
#define HELIUM_3PI_4 2.3561944901923448370E0
/// Square root of Pi.
#define HELIUM_SQRTPI 1.77245385090551602792981
/// 1 / Pi.
#define HELIUM_1_PI 0.31830988618379067154
/// 2 / Pi.
#define HELIUM_2_PI 0.63661977236758134308
/// 2 / (square root of Pi).
#define HELIUM_2_SQRTPI 1.12837916709551257390

/// Degrees-to-radians scale.
#define HELIUM_DEG_TO_RAD ( HELIUM_PI / 180.0f )
/// Radians-to-degrees scale.
#define HELIUM_RAD_TO_DEG ( 180.0f / HELIUM_PI )

/// Generic single-precision floating-point epsilon value.
#define HELIUM_EPSILON ( 1.0e-8f )

#define HELIUM_VALUE_NEAR_ZERO 1e-20f
#define HELIUM_DIVISOR_NEAR_ZERO 1e-15f//(0.00005f)
#define HELIUM_ANGLE_NEAR_ZERO 1e-7f

#define HELIUM_CRITICAL_DOT_PRODUCT 0.98f
#define HELIUM_POINT_ON_PLANE_ERROR 0.00001f
#define HELIUM_LINEAR_INTERSECTION_ERROR 0.05f

//
// from http://en.wikipedia.org/wiki/Luminance_(relative)
// these luminance weights assume the input color is linearly encoded
//
#define HELIUM_LUMINANCE_R 0.2126f
#define HELIUM_LUMINANCE_G 0.7152f
#define HELIUM_LUMINANCE_B 0.0722f
#define HELIUM_INVERSE_LUMINANCE_R ( 1.0f / HELIUM_LUMINANCE_R )
#define HELIUM_INVERSE_LUMINANCE_G ( 1.0f / HELIUM_LUMINANCE_G )
#define HELIUM_INVERSE_LUMINANCE_B ( 1.0f / HELIUM_LUMINANCE_B )
//@}

namespace Helium
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

    inline float32_t Floor( float32_t value );
    inline float64_t Floor( float64_t value );
    inline float32_t Ceil( float32_t value );
    inline float64_t Ceil( float64_t value );

    inline float32_t Fmod( float32_t x, float32_t y );
    inline float64_t Fmod( float64_t x, float64_t y );
    inline float32_t Modf( float32_t value, float32_t& rInteger );
    inline float64_t Modf( float64_t value, float64_t& rInteger );
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
    //@}
}

#include "Platform/Math/MathCommon.inl"

#endif  // HELIUM_CORE_MATH_COMMON_H
