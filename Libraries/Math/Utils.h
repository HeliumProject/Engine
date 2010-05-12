#pragma once

#include <cmath>
#include <iostream>
#include <intrin.h>
#include <float.h>

#include "Common/Types.h"
#include "Common/Assert.h"

#include "API.h"
#include "Macros.h"
#include "Constants.h"

namespace Math
{
  //
  // Valid
  //

  inline bool IsValid(f64 val)
  {
    return _finite(val) != 0;
  }

  //
  // Clamp
  //
  inline i32 Clamp(i32& val, i32 min, i32 max)
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
  inline u32 Clamp(u32& val, u32 min, u32 max)
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
  inline f32 Clamp(f32& val, f32 min, f32 max)
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
  inline f64 Clamp(f64& val, f64 min, f64 max)
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
  inline f32 ClampAngle(f32& v)
  {
    while (v < -Pi)
      v += TwoPi;
    while (v > Pi)
      v -= TwoPi;
    return v;
  }

  //
  // Limit (non ref clamp)
  //
  inline i32 Limit(i32 min, i32 val, i32 max)
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
  inline f32 LimitAngle(f32 v, f32 low, f32 high)
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
  inline f32 Round(f32 d)
  {
    return floor(d + 0.5f);
  }
  inline f64 Round(f64 d)
  {
    return floor(d + 0.5);
  }

  //
  // Ran
  //
  inline i32 Ran(i32 low, i32 high)
  {
    return (i32)Round((((f64)rand() / (f64) RAND_MAX) * (f64)(high - low)) + low);
  }

  //
  // Ran
  //
  inline f64 Ran(f64 low, f64 high)
  {
    return (((f64)rand() / (f64) RAND_MAX) * (high - low)) + low;
  }

  //
  // Log2
  //
  // Return the log2 of the input, effectively this is the position of the highest bit set (signed)
  //
  inline i32 Log2(i32 val)
  {
    i32 log = 0;
    for (; val > 1; log++)
      val = val >> 1;
    return (log);
  }

  //
  // Log2
  //
  // Return the log2 of the input, effectively this is the position of the highest bit set
  //
  inline u32 Log2(u32 v)
  {
#if defined(_M_IX86)
    _asm
    {
      bsr eax,v
    }
#else
    u32 result = 0;
    _BitScanReverse((unsigned long*)&result, v);
    return result;
#endif
  }

  //
  // LogBase2
  //
  inline f64 LogBase2(f64 v)
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
  inline u32 NextPowerOfTwo(u32 in)
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
  inline u32 PreviousPowerOfTwo(u32 in)
  {
    return 1<<Log2(in);
  }

  //
  // IsPowerOfTwo
  //
  // Returns true if the input is a power of 2
  //
  inline bool IsPowerOfTwo(u32 in)
  {
    return (in & (in-1))==0;
  }

  //
  // IsWholeNumber
  //
  inline bool IsWholeNumber(f64 d, f64 error)
  {
    f64 i = Round(d);
    if (fabs(d - i) <= error)
      return true;
    return false;
  }

  //
  // Equal
  // 
  inline bool Equal( f32 a, f32 b, f32 err = ValueNearZero )
  {
    return ( fabs( a - b ) <= err ); 
  }

}