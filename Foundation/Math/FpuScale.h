#pragma once

#include "Foundation/API.h"
#include "Foundation/Math/Common.h"
#include "Foundation/Math/FpuVector3.h"

namespace Helium
{
    class HELIUM_FOUNDATION_API Scale
    {
    public:
        float32_t x, y, z;

        const static Scale Identity;

        Scale           () : x(1), y(1), z(1) {}
        explicit Scale  (float32_t vx, float32_t vy, float32_t vz) : x(vx), y(vy), z(vz) {}
        explicit Scale  (Vector3 v) : x(v.x), y(v.y), z(v.z) {}

        Scale&          operator= (const Scale& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Scale&          operator= (const Vector3& v) { x = v.x; y = v.y; z = v.z; return *this; }
        Scale&          operator*= (const Scale& v) { x *= v.x; y *= v.y; z *= v.z; return *this; }
        Scale&          operator*= (const float32_t v) { x *= v; y *= v; z *= v; return *this; }
        Scale&          operator/= (const Scale& v) { x /= v.x; y /= v.y; z /= v.z; return *this; }
        Scale&          operator/= (const float32_t v) { x /= v; y /= v; z /= v; return *this; }

        Scale           operator* (const Scale& v) const { return Scale (x * v.x, y * v.y, z * v.z); }
        Scale           operator* (const float32_t v) const { return Scale (x * v, y * v, z * v); }
        Scale           operator/ (const Scale& v) const { return Scale (x / v.x, y / v.y, z / v.z); }
        Scale           operator/ (const float32_t v) const { return Scale (x / v, y / v, z / v); }

        float32_t&            operator[] (const uint32_t i) {  return (&x)[i]; }
        const float32_t&      operator[] (const uint32_t i) const {  return (&x)[i]; }

        bool            operator== (const Scale& v) const { return (x == v.x && y == v.y && z == v.z); }
        bool            operator!= (const Scale& v) const { return !(x == v.x && y == v.y && z == v.z); }
        bool            Equal (const Scale& v, float32_t error = 0) const;
        bool            Finite() { return IsFinite(x) && IsFinite(y) && IsFinite(z); }

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

    inline bool Scale::Equal(const Scale& v, float32_t error) const
    {
        return (fabs(x - v.x) < error && fabs(y - v.y) < error && fabs(z - v.z) < error);
    }
}
