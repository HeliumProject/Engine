#pragma once

#include <cmath>
#include "Utils.h"

#include "Vector3.h"

namespace Helium
{
    class FOUNDATION_API Scale
    {
    public:
        f32 x, y, z;

        const static Scale Identity;

        Scale           () : x(1), y(1), z(1) {}
        explicit Scale  (f32 vx, f32 vy, f32 vz) : x(vx), y(vy), z(vz) {}
        explicit Scale  (Vector3 v) : x(v.x), y(v.y), z(v.z) {}

        Scale&          operator= (const Scale& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Scale&          operator= (const Vector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Scale&          operator*= (const Scale& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
        Scale&          operator*= (const f32 v) { x *= v; y *= v; z *= v; return *this; }
        Scale&          operator/= (const Scale& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
        Scale&          operator/= (const f32 v) { x /= v; y /= v; z /= v; return *this; }

        Scale           operator* (const Scale& v) const { return Scale (x * v.x, y * v.y, z * v.z); }
        Scale           operator* (const f32 v) const { return Scale (x * v, y * v, z * v); }
        Scale           operator/ (const Scale& v) const { return Scale (x / v.x, y / v.y, z / v.z); }
        Scale           operator/ (const f32 v) const { return Scale (x / v, y / v, z / v); }

        f32&            operator[] (const u32 i) {  return (&x)[i]; }
        const f32&      operator[] (const u32 i) const {  return (&x)[i]; }

        bool            operator== (const Scale& v) const { return (x == v.x && y == v.y && z == v.z); }
        bool            operator!= (const Scale& v) const { return !(x == v.x && y == v.y && z == v.z); }
        bool            Equal (const Scale& v, f32 error = 0) const;
        bool            Valid() { return IsValid(x) && IsValid(y) && IsValid(z); }

        Scale&          Invert()
        {
            x = 1.0f/x;
            y = 1.0f/y;
            z = 1.0f/z;
            return *this;
        }

        Scale           Inverted() const
        {
            Scale s (*this);
            return s.Invert();
        }
    };

    inline bool Scale::Equal(const Scale& v, f32 error) const
    {
        return (fabs(x - v.x) < error && fabs(y - v.y) < error && fabs(z - v.z) < error);
    }
}
