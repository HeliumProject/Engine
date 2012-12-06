#pragma once

#include "Math/API.h"
#include "Math/Matrix4.h"

namespace Helium
{
    class HELIUM_MATH_API Plane
    {
    public:
        Vector4 p;

        const static Plane Null;

        Plane       ()
            : p (Vector4::Zero)
        {

        }

        Plane       (const Vector3& point, const Vector3& normal)
        {
            p.x = normal.x;
            p.y = normal.y;
            p.z = normal.z;
            p.w = -(normal.Dot(point));
        }

        float32_t         A () const      { return p.x; }
        void        A (float32_t value)   { p.x = value; }

        float32_t         B () const      { return p.y; }
        void        B (float32_t value)   { p.y = value; }

        float32_t         C () const      { return p.z; }
        void        C (float32_t value)   { p.z = value; }

        float32_t         D () const      { return p.w; }
        void        D (float32_t value)   { p.w = value; }

        float32_t&        operator[] ( const uint32_t index )
        {
            return p[index];
        }

        const float32_t&  operator[] ( const uint32_t index ) const
        {
            return p[index];
        }

        void        GetPoint (Vector3& point) const
        {
            point.x = p.x;
            point.y = p.y;
            point.z = p.z;
            point *= p.w;
        }

        void        GetNormal (Vector3& normal) const
        {
            normal.x = p.x;
            normal.y = p.y;
            normal.z = p.z;
        }

        void        Normalize ()
        {
            p.Normalize();
        }

        void        Transform (const Matrix4& m)
        {
            p = m * p;
        }

        float32_t         DistanceAbove (const Vector3& p) const
        {
            return (p.x*A()) + (p.y*B()) + (p.z*C()) + D();
        }
    };
}