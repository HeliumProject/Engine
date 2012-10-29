/// Get the minimum of two values.
///
/// @param[in] rA  First value.
/// @param[in] rB  Second value.
///
/// @return  Minimum value.
///
/// @see Max()
template< typename T >
T& Helium::Min( T& rA, T& rB )
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
const T& Helium::Min( const T& rA, const T& rB )
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
T& Helium::Max( T& rA, T& rB )
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
const T& Helium::Max( const T& rA, const T& rB )
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
T& Helium::Clamp( T& rValue, T& rMin, T& rMax )
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
const T& Helium::Clamp( const T& rValue, const T& rMin, const T& rMax )
{
    return ( rValue < rMin ? rMin : ( rValue > rMax ? rMax : rValue ) );
}

/// Get the absolute value of a signed integer (other than int64_t).
///
/// @param[in] rValue  Value.
///
/// @return  Absolute value of the given value.
template< typename T >
T Helium::Abs( const T& rValue )
{
    return ::abs( rValue );
}

/// Get the absolute value of a signed 64-bit integer.
///
/// @param[in] value  Signed 64-bit integer value.
///
/// @return  Absolute value of the given value.
int64_t Helium::Abs( int64_t value )
{
#if HELIUM_OS_WIN
#if HELIUM_CC_CL
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
float32_t Helium::Abs( float32_t value )
{
    return ::fabsf( value );
}

/// Get the absolute value of a double-precision floating-point value.
///
/// @param[in] value  Floating-point value.
///
/// @return  Absolute value of the given value.
float64_t Helium::Abs( float64_t value )
{
    return ::fabs( value );
}

/// Compute the square of a value.
///
/// @param[in] rValue  Value.
///
/// @return  Square of the given value.
template< typename T >
T Helium::Square( const T& rValue )
{
    return ( rValue * rValue );
}

/// Compute the square root of a single-precision floating-point value.
///
/// @param[in] value  Floating-point value.
///
/// @return  Square root of the given value.
float32_t Helium::Sqrt( float32_t value )
{
    return ::sqrtf( value );
}

/// Compute the square root of a double-precision floating-point value.
///
/// @param[in] value  Floating-point value.
///
/// @return  Square root of the given value.
float64_t Helium::Sqrt( float64_t value )
{
    return ::sqrt( value );
}

/// IsPowerOfTwo() implementation for signed integer types.
///
/// @param[in] rValue     Signed integer value to test.
/// @param[in] rIsSigned  std::true_type.
///
/// @return  True if the value is a power of two, false if not.
template< typename T >
bool _IsPowerOfTwo( const T& rValue, const std::true_type& /*rIsSigned*/ )
{
    T absValue = Abs( rValue );

    return ( ( absValue & ( absValue - 1 ) ) == 0 );
}

/// IsPowerOfTwo() implementation for unsigned integer types.
///
/// @param[in] rValue     Unsigned integer value to test.
/// @param[in] rIsSigned  std::false_type.
///
/// @return  True if the value is a power of two, false if not.
template< typename T >
bool _IsPowerOfTwo( const T& rValue, const std::false_type& /*rIsSigned*/ )
{
    return ( ( rValue & ( rValue - 1 ) ) == 0 );
}

/// Test whether an integer value is a power of two.
///
/// @param[in] rValue  Value to test.
///
/// @return  True if the value is a power of two, false if not.
template< typename T >
bool Helium::IsPowerOfTwo( const T& rValue )
{
    return _IsPowerOfTwo( rValue, std::is_signed< T >() );
}

/// Compute the base-2 logarithm of an unsigned 32-bit integer.
///
/// @param[in] value  Unsigned 32-bit integer.
///
/// @return  Base-2 logarithm.
///
/// @see Log2( uint64_t )
size_t Helium::Log2( uint32_t value )
{
    HELIUM_ASSERT( value != 0 );

#if HELIUM_CC_CL
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
size_t Helium::Log2( uint64_t value )
{
    HELIUM_ASSERT( value );

#if HELIUM_CC_CL
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

/// Round a floating-point value down to the largest integral value less than or equal to it.
///
/// @param[in] value  Floating-point value.
///
/// @return  Largest integral floating-point value less than or equal to the given value.
///
/// @see Ceil()
float32_t Helium::Floor( float32_t value )
{
    return floorf( value );
}

/// Round a floating-point value down to the largest integral value less than or equal to it.
///
/// @param[in] value  Floating-point value.
///
/// @return  Largest integral floating-point value less than or equal to the given value.
///
/// @see Ceil()
float64_t Helium::Floor( float64_t value )
{
    return floor( value );
}

/// Round a floating-point value up to the smallest integral value greater than or equal to it.
///
/// @param[in] value  Floating-point value.
///
/// @return  Smallest integral floating-point value greater than or equal to the given value.
///
/// @see Floor()
float32_t Helium::Ceil( float32_t value )
{
    return ceilf( value );
}

/// Round a floating-point value up to the smallest integral value greater than or equal to it.
///
/// @param[in] value  Floating-point value.
///
/// @return  Smallest integral floating-point value greater than or equal to the given value.
///
/// @see Floor()
float64_t Helium::Ceil( float64_t value )
{
    return ceil( value );
}

/// Compute the remainder of a floating-point division operation.
///
/// @param[in] x  Dividend of the operation.
/// @param[in] y  Divisor of the operation.
///
/// @return  The remainder of the first parameter divided by the second, with the same sign as the first parameter.
///
/// @see Modf()
float32_t Helium::Fmod( float32_t x, float32_t y )
{
    return fmodf( x, y );
}

/// Compute the remainder of a floating-point division operation.
///
/// @param[in] x  Dividend of the operation.
/// @param[in] y  Divisor of the operation.
///
/// @return  The remainder of the first parameter divided by the second, with the same sign as the first parameter.
///
/// @see Modf()
float64_t Helium::Fmod( float64_t x, float64_t y )
{
    return fmod( x, y );
}

/// Separate the integer and fractional parts of a floating-point value.
///
/// @param[in]  value     Floating-point value.
/// @param[out] rInteger  Integer component of the given floating-point value, with the same sign as that value.
///
/// @return  Fractional component of the given floating-point value, with the same sign as that value.
///
/// @see Fmod()
float32_t Helium::Modf( float32_t value, float32_t& rInteger )
{
    return modff( value, &rInteger );
}

/// Separate the integer and fractional parts of a floating-point value.
///
/// @param[in]  value     Floating-point value.
/// @param[out] rInteger  Integer component of the given floating-point value, with the same sign as that value.
///
/// @return  Fractional component of the given floating-point value, with the same sign as that value.
///
/// @see Fmod()
float64_t Helium::Modf( float64_t value, float64_t& rInteger )
{
    return modf( value, &rInteger );
}

/// Compute the sine of an angle.
///
/// @param[in] radians  Angle, in radians.
///
/// @return  Sine of the given angle.
///
/// @see Cos(), Tan(), Asin(), Acos(), Atan(), Atan2()
float32_t Helium::Sin( float32_t radians )
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
float32_t Helium::Cos( float32_t radians )
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
float32_t Helium::Tan( float32_t radians )
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
float32_t Helium::Asin( float32_t value )
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
float32_t Helium::Acos( float32_t value )
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
float32_t Helium::Atan( float32_t value )
{
    return atanf( value );
}

/// Compute the arctangent of the specified slope.
///
/// @param[in] y  Rate of change along the y-axis.
/// @param[in] x  Rate of change along the x-axis.
///
/// @return  Arctangent of the given slope.
float32_t Helium::Atan2( float32_t y, float32_t x )
{
    return atan2f( y, x );
}

//
// Valid
//

inline bool Helium::IsFinite(float32_t val)
{
    return _finite(val) != 0;
}

inline bool Helium::IsFinite(float64_t val)
{
    return _finite(val) != 0;
}

//
// Clamp
//
inline int32_t Helium::Clamp(int32_t& val, int32_t min, int32_t max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

//
// Clamp
//
inline uint32_t Helium::Clamp(uint32_t& val, uint32_t min, uint32_t max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

//
// Clamp
//
inline float32_t Helium::Clamp(float32_t& val, float32_t min, float32_t max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

//
// Clamp
//
inline float64_t Helium::Clamp(float64_t& val, float64_t min, float64_t max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

//
// ClampAngle
//
inline float32_t Helium::ClampAngle(float32_t& v)
{
    while( v < -static_cast< float32_t >( HELIUM_PI ) )
        v += static_cast< float32_t >( HELIUM_TWOPI );
    while( v > static_cast< float32_t >( HELIUM_PI ) )
        v -= static_cast< float32_t >( HELIUM_TWOPI );
    return v;
}

//
// Limit (non ref clamp)
//
inline int32_t Helium::Limit(int32_t min, int32_t val, int32_t max)
{
    if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

//
// LimitAngle
//
inline float32_t Helium::LimitAngle(float32_t v, float32_t low, float32_t high)
{
    if (v < low)
        v += (high - low);
    else if (v > high)
        v -= (high - low);

    return v;
}

//
// Round
//
inline float32_t Helium::Round(float32_t d)
{
    return floor(d + 0.5f);
}
inline float64_t Helium::Round(float64_t d)
{
    return floor(d + 0.5);
}

//
// Ran
//
inline int32_t Helium::Ran(int32_t low, int32_t high)
{
    return (int32_t)Round((((float64_t)rand() / (float64_t) RAND_MAX) * (float64_t)(high - low)) + low);
}

//
// Ran
//
inline float64_t Helium::Ran(float64_t low, float64_t high)
{
    return (((float64_t)rand() / (float64_t) RAND_MAX) * (high - low)) + low;
}

//
// LogBase2
//
inline float64_t Helium::LogBase2(float64_t v)
{
    v = log10(v);
    v = v * 3.3219282;
    return v;
}

//
// NextPowerOfTwo
//
// Return the next power of two, if the number is already a power of two then
// the input is returned.
//
inline uint32_t Helium::NextPowerOfTwo(uint32_t in)
{
    in -= 1;

    in |= in >> 16;
    in |= in >> 8;
    in |= in >> 4;
    in |= in >> 2;
    in |= in >> 1;

    return in + 1;
}

//
// PreviousPowerOfTwo
//
// Return the number rounded down to the previous power of two, if the input is already a power
// of two it is returned unmodified.
//
inline uint32_t Helium::PreviousPowerOfTwo(uint32_t in)
{
    return 1<<Log2(in);
}

//
// IsPowerOfTwo
//
// Returns true if the input is a power of 2
//
inline bool Helium::IsPowerOfTwo(uint32_t in)
{
    return (in & (in-1))==0;
}

//
// IsWholeNumber
//
inline bool Helium::IsWholeNumber(float64_t d, float64_t error)
{
    float64_t i = Round(d);
    if (fabs(d - i) <= error)
        return true;
    return false;
}

//
// Equal
// 
inline bool Helium::Equal( float32_t a, float32_t b, float32_t err )
{
    return ( fabs( a - b ) <= err ); 
}
