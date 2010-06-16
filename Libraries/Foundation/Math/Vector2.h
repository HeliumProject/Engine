#pragma once

#include <cmath>
#include <vector>
#include <iostream>
#include "Utils.h"

namespace Math
{
  class FOUNDATION_API Vector2
  {
  public:
    f32 x, y;

    const static Vector2 Zero;
    const static Vector2 BasisX;
    const static Vector2 BasisY;

    Vector2           () : x(0), y(0) {}
    explicit Vector2  ( f32 vx, f32 vy )  : x(vx), y(vy) {}

    Vector2&          operator= (const Vector2& v) { x = v.x; y = v.y; return *this; }
    Vector2&          operator+= (const Vector2& v) { x += v.x; y += v.y; return *this; }
    Vector2&          operator-= (const Vector2& v) { x -= v.x; y -= v.y; return *this; }
    Vector2&          operator*= (const Vector2& v) { x *= v.x; y *= v.y; return *this; }
    Vector2&          operator*= (const f32 v) { x *= v; y *= v; return *this; }
    Vector2&          operator/= (const Vector2& v) { x /= v.x; y /= v.y; return *this; }
    Vector2&          operator/= (const f32 v) { x /= v; y /= v; return *this; }

    Vector2           operator+ (const Vector2& v) const { return Vector2 (x + v.x, y + v.y); }
    Vector2           operator- (const Vector2& v) const { return Vector2 (x - v.x, y - v.y); }
    Vector2           operator* (const Vector2& v) const { return Vector2 (x * v.x, y * v.y); }
    Vector2           operator* (const f32 v) const { return Vector2 (x * v, y * v); }
    Vector2           operator/ (const Vector2& v) const { return Vector2 (x / v.x, y / v.y); }
    Vector2           operator/ (const f32 v) const { return Vector2 (x / v, y / v); }

    // unary negation
    Vector2           operator- () const { return Vector2( -x, -y ); }

    f32&              operator[] (const u32 i) { NOC_ASSERT(i < 2); return (&x)[i]; }
    const f32&        operator[] (const u32 i) const { NOC_ASSERT(i < 2); return (&x)[i]; }

    bool              operator== (const Vector2& v) const { return (x == v.x && y == v.y); }
    bool              operator!= (const Vector2& v) const { return !(x == v.x && y == v.y); }
    bool              Equal (const Vector2& v, f32 error = 0) const;
    bool              Valid() { return IsValid(x) && IsValid(y); }

    f32               LengthSquared () const { return x * x + y * y; }
    f32               Length () const;

    Vector2&          Normalize ();
    Vector2           Normalized () const;

    f32               Dot (const Vector2& v) const { return (x * v.x + y * v.y); }

    friend FOUNDATION_API std::ostream& operator<<(std::ostream& outStream, const Vector2& v);
  	friend FOUNDATION_API std::istream& operator>>(std::istream& inStream, Vector2& v);
  };

  typedef std::vector< Vector2 > V_Vector2;

  inline bool Vector2::Equal(const Vector2& v, f32 error) const
  {
    return (fabs(x - v.x) <= error && fabs(y - v.y) <= error);
  }

  inline f32 Vector2::Length() const
  {
    f32 lenSqr = this->LengthSquared();

    if (lenSqr <= 0)
      return 0;

    return sqrt((f32)lenSqr);
  }

  inline Vector2& Vector2::Normalize()
  { 
    f32 len = this->Length();

    if (len > DivisorNearZero)
    {
      return *this /= len; 
    }
    else
    {
      return *this = Vector2 (0, 0);
    }
  }

  inline Vector2 Vector2::Normalized() const
  {
    Vector2 result = *this;
    return result.Normalize();
  }

  inline std::ostream& operator<<(std::ostream& outStream, const Vector2& vector)
  {
	  outStream << vector.x << ", " << vector.y;

    return outStream;
  }

  inline std::istream& operator>>(std::istream& inStream, Vector2& vector)
  {
    inStream >> vector.x;
    inStream.ignore();

    inStream >> vector.y;

    return inStream;
  }
}