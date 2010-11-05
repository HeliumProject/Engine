//----------------------------------------------------------------------------------------------------------------------
// MathCommon.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// IsPowerOfTwo() implementation for signed integer types.
    ///
    /// @param[in] rValue     Signed integer value to test.
    /// @param[in] rIsSigned  boost::true_type.
    ///
    /// @return  True if the value is a power of two, false if not.
    template< typename T >
    bool _IsPowerOfTwo( const T& rValue, const boost::true_type& /*rIsSigned*/ )
    {
        T absValue = Abs( rValue );

        return ( ( absValue & ( absValue - 1 ) ) == 0 );
    }

    /// IsPowerOfTwo() implementation for unsigned integer types.
    ///
    /// @param[in] rValue     Unsigned integer value to test.
    /// @param[in] rIsSigned  boost::false_type.
    ///
    /// @return  True if the value is a power of two, false if not.
    template< typename T >
    bool _IsPowerOfTwo( const T& rValue, const boost::false_type& /*rIsSigned*/ )
    {
        return ( ( rValue & ( rValue - 1 ) ) == 0 );
    }

    /// Get the minimum of two values.
    ///
    /// @param[in] rA  First value.
    /// @param[in] rB  Second value.
    ///
    /// @return  Minimum value.
    ///
    /// @see Max()
    template< typename T >
    T& Min( T& rA, T& rB )
    {
        return( rA < rB ? rA : rB );
    }

    /// Get the minimum of two values.
    ///
    /// @param[in] rA  First value.
    /// @param[in] rB  Second value.
    ///
    /// @return  Minimum value.
    ///
    /// @see Max()
    template< typename T >
    const T& Min( const T& rA, const T& rB )
    {
        return( rA < rB ? rA : rB );
    }

    /// Get the maximum of two values.
    ///
    /// @param[in] rA  First value.
    /// @param[in] rB  Second value.
    ///
    /// @return  Maximum value.
    ///
    /// @see Min()
    template< typename T >
    T& Max( T& rA, T& rB )
    {
        return( rA > rB ? rA : rB );
    }

    /// Get the maximum of two values.
    ///
    /// @param[in] rA  First value.
    /// @param[in] rB  Second value.
    ///
    /// @return  Maximum value.
    ///
    /// @see Min()
    template< typename T >
    const T& Max( const T& rA, const T& rB )
    {
        return( rA > rB ? rA : rB );
    }

    /// Clamp a value to a given range.
    ///
    /// @param[in] rValue  Value to clamp.
    /// @param[in] rMin    Minimum range value.
    /// @param[in] rMax    Maximum range value.
    ///
    /// @return  Clamped value.
    template< typename T >
    T& Clamp( T& rValue, T& rMin, T& rMax )
    {
        return ( rValue < rMin ? rMin : ( rValue > rMax ? rMax : rValue ) );
    }

    /// Clamp a value to a given range.
    ///
    /// @param[in] rValue  Value to clamp.
    /// @param[in] rMin    Minimum range value.
    /// @param[in] rMax    Maximum range value.
    ///
    /// @return  Clamped value.
    template< typename T >
    const T& Clamp( const T& rValue, const T& rMin, const T& rMax )
    {
        return ( rValue < rMin ? rMin : ( rValue > rMax ? rMax : rValue ) );
    }

    /// Get the absolute value of a signed integer (other than int64_t).
    ///
    /// @param[in] rValue  Value.
    ///
    /// @return  Absolute value of the given value.
    template< typename T >
    T Abs( const T& rValue )
    {
        return ::abs( rValue );
    }

    /// Get the absolute value of a signed 64-bit integer.
    ///
    /// @param[in] value  Signed 64-bit integer value.
    ///
    /// @return  Absolute value of the given value.
    int64_t Abs( int64_t value )
    {
#if HELIUM_OS_WIN
#if HELIUM_CC_MSC
        return ::_abs64( value );
#else
        return llabs( value );
#endif
#else
#error TODO: Implement 64-bit Abs() on this platform/compiler.
#endif
    }

    /// Get the absolute value of a single-precision floating-point value.
    ///
    /// @param[in] value  Floating-point value.
    ///
    /// @return  Absolute value of the given value.
    float32_t Abs( float32_t value )
    {
        return ::fabsf( value );
    }

    /// Get the absolute value of a double-precision floating-point value.
    ///
    /// @param[in] value  Floating-point value.
    ///
    /// @return  Absolute value of the given value.
    float64_t Abs( float64_t value )
    {
        return ::fabs( value );
    }

    /// Compute the square of a value.
    ///
    /// @param[in] rValue  Value.
    ///
    /// @return  Square of the given value.
    template< typename T >
    T Square( const T& rValue )
    {
        return ( rValue * rValue );
    }

    /// Compute the square root of a single-precision floating-point value.
    ///
    /// @param[in] value  Floating-point value.
    ///
    /// @return  Square root of the given value.
    float32_t Sqrt( float32_t value )
    {
        return ::sqrtf( value );
    }

    /// Compute the square root of a double-precision floating-point value.
    ///
    /// @param[in] value  Floating-point value.
    ///
    /// @return  Square root of the given value.
    float64_t Sqrt( float64_t value )
    {
        return ::sqrt( value );
    }

    /// Test whether an integer value is a power of two.
    ///
    /// @param[in] rValue  Value to test.
    ///
    /// @return  True if the value is a power of two, false if not.
    template< typename T >
    bool IsPowerOfTwo( const T& rValue )
    {
        return _IsPowerOfTwo( rValue, boost::is_signed< T >() );
    }

    /// Compute the base-2 logarithm of an unsigned 32-bit integer.
    ///
    /// @param[in] value  Unsigned 32-bit integer.
    ///
    /// @return  Base-2 logarithm.
    ///
    /// @see Log2( uint64_t )
    size_t Log2( uint32_t value )
    {
        HELIUM_ASSERT( value != 0 );

#if HELIUM_CC_MSC
        unsigned long bitIndex = 0;
        HELIUM_VERIFY( _BitScanReverse( &bitIndex, value ) );

        return bitIndex;
#elif HELIUM_CC_GCC
        return ( 31 - __builtin_clz( value ) );
#else
#warning Compiling unoptimized Log2() implementation.  Please evaluate the availability of more optimal implementations for the current platform/compiler.
        size_t bitIndex = sizeof( value ) * 8 - 1;
        uint32_t mask = ( 1 << bitIndex );
        while( !( value & mask ) )
        {
            if( bitIndex == 0 )
            {
                break;
            }

            --bitIndex;
            mask >>= 1;
        }

        return bitIndex;
#endif
    }

    /// Compute the base-2 logarithm of an unsigned 64-bit integer.
    ///
    /// @param[in] value  Unsigned 64-bit integer.
    ///
    /// @return  Base-2 logarithm.
    ///
    /// @see Log2( uint32_t )
    size_t Log2( uint64_t value )
    {
        HELIUM_ASSERT( value );

#if HELIUM_CC_MSC
        unsigned long bitIndex = 0;

#if HELIUM_WORDSIZE == 64
        HELIUM_VERIFY( _BitScanReverse64( &bitIndex, value ) );
#else
        if( _BitScanReverse( &bitIndex, static_cast< uint32_t >( value >> 32 ) ) )
        {
            bitIndex += 32;
        }
        else
        {
            HELIUM_VERIFY( _BitScanReverse( &bitIndex, static_cast< uint32_t >( value ) ) );
        }
#endif

        return bitIndex;
#elif HELIUM_CC_GCC
        HELIUM_COMPILE_ASSERT( sizeof( long long ) == 64 );

        return ( 63 - __builtin_clzll( static_cast< unsigned long long >( value ) ) );
#else
#warning Compiling unoptimized Log2() implementation.  Please evaluate the availability of more optimal implementations for the current platform/compiler.
        size_t bitIndex = sizeof( value ) * 8 - 1;
        uint64_t mask = ( 1 << bitIndex );
        while( !( value & mask ) )
        {
            if( bitIndex == 0 )
            {
                break;
            }

            --bitIndex;
            mask >>= 1;
        }

        return bitIndex;
#endif
    }

    /// Compute the sine of an angle.
    ///
    /// @param[in] radians  Angle, in radians.
    ///
    /// @return  Sine of the given angle.
    ///
    /// @see Cos(), Tan(), Asin(), Acos(), Atan(), Atan2()
    float32_t Sin( float32_t radians )
    {
        return sinf( radians );
    }

    /// Compute the cosine of an angle.
    ///
    /// @param[in] radians  Angle, in radians.
    ///
    /// @return  Cosine of the given angle.
    ///
    /// @see Sin(), Tan(), Asin(), Acos(), Atan(), Atan2()
    float32_t Cos( float32_t radians )
    {
        return cosf( radians );
    }

    /// Compute the tangent of an angle.
    ///
    /// @param[in] radians  Angle, in radians.
    ///
    /// @return  Tangent of the given angle.
    ///
    /// @see Sin(), Cos(), Asin(), Acos(), Atan(), Atan2()
    float32_t Tan( float32_t radians )
    {
        return tanf( radians );
    }

    /// Compute the arcsine of a value.
    ///
    /// @param[in] value  Value.
    ///
    /// @return  Arcsine of the given value, in radians.
    ///
    /// @see Acos(), Atan(), Atan2(), Sin(), Cos(), Tan()
    float32_t Asin( float32_t value )
    {
        return asinf( value );
    }

    /// Compute the arccosine of a value.
    ///
    /// @param[in] value  Value.
    ///
    /// @return  Arccosine of the given value, in radians.
    ///
    /// @see Asin(), Atan(), Atan2(), Sin(), Cos(), Tan()
    float32_t Acos( float32_t value )
    {
        return acosf( value );
    }

    /// Compute the arctangent of a value.
    ///
    /// @param[in] value  Value.
    ///
    /// @return  Arctangent of the given value, in radians.
    ///
    /// @see Asin(), Acos(), Atan2(), Sin(), Cos(), Tan()
    float32_t Atan( float32_t value )
    {
        return atanf( value );
    }

    /// Compute the arctangent of the specified slope.
    ///
    /// @param[in] y  Rate of change along the y-axis.
    /// @param[in] x  Rate of change along the x-axis.
    ///
    /// @return  Arctangent of the given slope.
    float32_t Atan2( float32_t y, float32_t x )
    {
        return atan2f( y, x );
    }
}
