#pragma once

#include "Matrix4.h"

namespace Helium
{
    namespace Math
    {
        class FOUNDATION_API Plane
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

            f32         A () const      { return p.x; }
            void        A (f32 value)   { p.x = value; }

            f32         B () const      { return p.y; }
            void        B (f32 value)   { p.y = value; }

            f32         C () const      { return p.z; }
            void        C (f32 value)   { p.z = value; }

            f32         D () const      { return p.w; }
            void        D (f32 value)   { p.w = value; }

            f32&        operator[] ( const u32 index )
            {
                return p[index];
            }

            const f32&  operator[] ( const u32 index ) const
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

            f32         DistanceAbove (const Vector3& p) const
            {
                return (p.x*A()) + (p.y*B()) + (p.z*C()) + D();
            }
        };
    }
}