#pragma once

#include "Foundation/API.h"
#include "Foundation/Math/FpuVector3.h"

namespace Helium
{
    class AlignedBox;
    class Matrix4;
    class Plane;

    class HELIUM_FOUNDATION_API Line
    {
    public:
        Vector3 m_Origin;
        Vector3 m_Point;

        Line        ()
        {

        }

        Line        (const Vector3& o, const Vector3& p)
            : m_Origin (o)
            , m_Point (p)
        {

        }

        void Transform(const Matrix4& m);

        bool IntersectsPoint(const Vector3& pos, const float32_t err = HELIUM_LINEAR_INTERSECTION_ERROR, float32_t* mu = NULL, Vector3* offset = NULL) const;
        bool IntersectsLine(const Vector3& p1, const Vector3& p2, Vector3* location = NULL) const;
        bool IntersectsSegment(const Vector3& p1, const Vector3& p2, const float32_t err = HELIUM_LINEAR_INTERSECTION_ERROR, float32_t* mu = NULL, Vector3* offset = NULL) const;
        bool IntersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, float32_t* bary0 = NULL, float32_t* bary1 = NULL, float32_t* scale = NULL) const;
        bool IntersectsSphere(const Vector3& pos, const float32_t radius, V_Vector3* intersections = NULL) const;
        bool IntersectsPlane(const Plane& plane, Vector3* intersection = NULL) const;
        bool IntersectsBox(const AlignedBox& box, Vector3* intersection = NULL) const;
        bool IntersectSegmentTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const;
        bool IntersectRayTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const;

        void ProjectPointOnSegment( const Vector3& point, Vector3& projectedPoint ) const;
        float32_t DistanceSqrToPoint( const Vector3& point ) const;
        float32_t DistanceToPoint( const Vector3& point ) const;
    };
}