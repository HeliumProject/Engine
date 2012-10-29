#pragma once

#include <iostream>
#include <vector>

#include "Math/API.h"
#include "Foundation/Math.h"

namespace Helium
{
    class HELIUM_MATH_API Vector2
    {
    public:
        float32_t x, y;

        const static Vector2 Zero;
        const static Vector2 BasisX;
        const static Vector2 BasisY;

        Vector2           () : x(0), y(0) {}
        explicit Vector2  ( float32_t vx, float32_t vy )  : x(vx), y(vy) {}

        Vector2&          operator= (const Vector2& v) { x = v.x; y = v.y; return *this; }
        Vector2&          operator+= (const Vector2& v) { x += v.x; y += v.y; return *this; }
        Vector2&          operator-= (const Vector2& v) { x -= v.x; y -= v.y; return *this; }
        Vector2&          operator*= (const Vector2& v) { x *= v.x; y *= v.y; return *this; }
        Vector2&          operator*= (const float32_t v) { x *= v; y *= v; return *this; }
        Vector2&          operator/= (const Vector2& v) { x /= v.x; y /= v.y; return *this; }
        Vector2&          operator/= (const float32_t v) { x /= v; y /= v; return *this; }

        Vector2           operator+ (const Vector2& v) const { return Vector2 (x + v.x, y + v.y); }
        Vector2           operator- (const Vector2& v) const { return Vector2 (x - v.x, y - v.y); }
        Vector2           operator* (const Vector2& v) const { return Vector2 (x * v.x, y * v.y); }
        Vector2           operator* (const float32_t v) const { return Vector2 (x * v, y * v); }
        Vector2           operator/ (const Vector2& v) const { return Vector2 (x / v.x, y / v.y); }
        Vector2           operator/ (const float32_t v) const { return Vector2 (x / v, y / v); }

        // unary negation
        Vector2           operator- () const { return Vector2( -x, -y ); }

        float32_t&              operator[] (const uint32_t i) { HELIUM_ASSERT(i < 2); return (&x)[i]; }
        const float32_t&        operator[] (const uint32_t i) const { HELIUM_ASSERT(i < 2); return (&x)[i]; }

        bool              operator== (const Vector2& v) const { return (x == v.x && y == v.y); }
        bool              operator!= (const Vector2& v) const { return !(x == v.x && y == v.y); }
        bool              Equal (const Vector2& v, float32_t error = 0) const;
        bool              Finite() { return IsFinite(x) && IsFinite(y); }

        float32_t               LengthSquared () const { return x * x + y * y; }
        float32_t               Length () const;

        Vector2&          Normalize ();
        Vector2           Normalized () const;

        float32_t               Dot (const Vector2& v) const { return (x * v.x + y * v.y); }

        friend HELIUM_MATH_API tostream& operator<<(tostream& outStream, const Vector2& v);
        friend HELIUM_MATH_API tistream& operator>>(tistream& inStream, Vector2& v);
    };

    typedef std::vector< Vector2 > V_Vector2;

    inline bool Vector2::Equal(const Vector2& v, float32_t error) const
    {
        return (fabs(x - v.x) <= error && fabs(y - v.y) <= error);
    }

    inline float32_t Vector2::Length() const
    {
        float32_t lenSqr = this->LengthSquared();

        if (lenSqr <= 0)
            return 0;

        return sqrt((float32_t)lenSqr);
    }

    inline Vector2& Vector2::Normalize()
    { 
        float32_t len = this->Length();

        if (len > HELIUM_DIVISOR_NEAR_ZERO)
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

    inline tostream& operator<<(tostream& outStream, const Vector2& vector)
    {
        outStream << vector.x << ", " << vector.y;

        return outStream;
    }

    inline tistream& operator>>(tistream& inStream, Vector2& vector)
    {
        inStream >> vector.x;
        inStream.ignore();

        inStream >> vector.y;

        return inStream;
    }
}