#pragma once

#include "Vector3.h"

namespace Math
{
  class MATH_API Curve
  {
  public:
    enum Type
    {
      kLinear,
      kBSpline,
      kCatmullRom,
    };

   static bool ComputeCurve( const V_Vector3& controlPoints, const u32 resolution, const bool closed, const Type type, V_Vector3& points );
  };
}
