#pragma once

#include "Plane.h"
#include "AlignedBox.h"

namespace Math
{
    class FOUNDATION_API Frustum
    {
    public:
        Plane top;
        Plane bottom;
        Plane left;
        Plane right;
        Plane front;
        Plane back;

        Frustum       ()
        {

        }

        Frustum       (const Matrix4& m);

        Frustum       (const AlignedBox& b);

        Plane&        operator[] (const u32 index)
        {
            switch (index)
            {
            case 0:
                return top;
            case 1:
                return bottom;
            case 2:
                return left;
            case 3:
                return right;
            case 4:
                return front;
            case 5:
                return back;
            }

            static Plane null;
            return null;
        }

        const Plane&  operator[] (const u32 index) const
        {
            switch (index)
            {
            case 0:
                return top;
            case 1:
                return bottom;
            case 2:
                return left;
            case 3:
                return right;
            case 4:
                return front;
            case 5:
                return back;
            }

            return Plane::Null;
        }

        void Transform(const Matrix4& m);

        bool IntersectsPoint(const Vector3& p, const f32 distanceFromPlane = PointOnPlaneError) const;
        bool IntersectsSegment(const Vector3& point1, const Vector3& point2) const;
        bool IntersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) const;
        bool IntersectsBox(const AlignedBox& box, bool precise = false) const;
        bool Contains(const AlignedBox& box) const;
    };
}
