#pragma once

#include "Math/API.h"
#include "Math/Axes.h"
#include "Foundation/Math.h"

namespace Helium
{
    class HELIUM_MATH_API Point
    {
    public:
        int32_t x, y;

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

        Point             operator* (int32_t v) const
        {
            return Point (x * v, y * v);
        }

        Point&            operator*= (int32_t v)
        {
            x *= v;
            y *= v;
            return *this;
        }

        Point             operator/ (int32_t v) const
        {
            return Point (x / v, y / v);
        }

        Point&            operator/= (int32_t v)
        {
            x /= v;
            y /= v;
            return *this;
        }

        int32_t&              operator[] (const unsigned i)
        {
            HELIUM_ASSERT(i < 2);
            return (&x)[i];
        }

        const int32_t&        operator[] (const unsigned i) const
        {
            HELIUM_ASSERT(i < 2);
            return (&x)[i];
        }

        static Axis       FlipAxis(Axis axis)
        {
            return (Axis)((axis + 1) % 2);
        }
    };
}