#pragma once

#include "Foundation/API.h"
#include "Platform/Types.h"

//
// Constants
//

namespace Math
{
  //
  // Axes flags
  //

  namespace SingleAxes
  {
    enum Axis
    {
      X = 0,
      Y = 1,
      Z = 2,
    };
  }

  typedef SingleAxes::Axis Axis;

  inline Axis NextAxis(Axis axis)
  {
    return (Axis)( (axis+1) % 3);
  }

  inline Axis PrevAxis(Axis axis)
  {
    return (Axis)( (axis+2) % 3);
  }

  namespace MultipleAxes
  {
    enum AxesFlags
    {
      None  = 0,
      X     = 1 << 0,
      Y     = 1 << 1,
      Z     = 1 << 2,
      All   = X | Y | Z,
    };
  }

  typedef MultipleAxes::AxesFlags AxesFlags;

  //
  // Constants
  //

  const f32 ValueNearZero = 1e-20f;
  const f32 DivisorNearZero = 1e-15f;//(0.00005f);
  const f32 AngleNearZero = 1e-7f;

  const f32 TwoPi = (6.283185307179586476925286766559f);
  const f32 Pi = (3.141592653589793238462643383279f);
  const f32 HalfPi = (1.570796326794896619231321691639f);

  const f32 DegToRad = Pi / 180.0f;
  const f32 RadToDeg = 180.0f / Pi;

  const f32 CentimetersToMeters = 0.01f;
  const f32 MetersToCentimeters = 100.0f;

  const f32 CriticalDotProduct = 0.98f;
  const f32 PointOnPlaneError = 0.00001f;
  const f32 LinearIntersectionError = 0.05f;

  const __int64 BigInt = 0x40000000;
  const double BigFloat = 1000000000000.0;

  //
  // from http://en.wikipedia.org/wiki/Luminance_(relative)
  // these luminance weights assume the input color is linearly encoded
  //
  const f32 LuminanceR = 0.2126f;
  const f32 LuminanceG = 0.7152f;
  const f32 LuminanceB = 0.0722f;
  const f32 InverseLuminanceR = (1.0f / LuminanceR);
  const f32 InverseLuminanceG = (1.0f / LuminanceG);
  const f32 InverseLuminanceB = (1.0f / LuminanceB);
}