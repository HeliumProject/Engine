#pragma once

#include "Utils.h"

namespace Math
{
  class MATH_API Point
  {
  public:
    i32 x, y;

    Point ()
      : x(0)
      , y(0)
    {
    
    }

    Point (unsigned px, unsigned py)
      : x(px)
      , y(py)
    {
    
    }

    const static Point Zero;

    bool              operator== (const Point& p) const
    {
      return (x == p.x && y == p.y);
    }

    bool              operator!= (const Point& p) const
    {
      return !(x == p.x && y == p.y);
    }

    Point&            operator= (const Point& p)
    {
      x = p.x; y = p.y;
      return *this;
    }

    Point             operator+ (const Point& p) const
    {
      return Point (x + p.x, y + p.y);
    }

    Point&            operator+= (const Point& p)
    {
      x += p.x; y += p.y; return *this;
    }

    Point             operator- (const Point& p) const
    {
      return Point (x - p.x, y - p.y);
    }

    Point&            operator-= (const Point& p)
    {
      x -= p.x;
      y -= p.y;
      return *this;
    }

    Point             operator* (i32 v) const
    {
      return Point (x * v, y * v);
    }

    Point&            operator*= (i32 v)
    {
      x *= v;
      y *= v;
      return *this;
    }

    Point             operator/ (i32 v) const
    {
      return Point (x / v, y / v);
    }

    Point&            operator/= (i32 v)
    {
      x /= v;
      y /= v;
      return *this;
    }

    i32&              operator[] (const unsigned i)
    {
      NOC_ASSERT(i < 2);
      return (&x)[i];
    }

    const i32&        operator[] (const unsigned i) const
    {
      NOC_ASSERT(i < 2);
      return (&x)[i];
    }

    static Axis       FlipAxis(Axis axis)
    {
      return (Axis)((axis + 1) % 2);
    }
  };
}