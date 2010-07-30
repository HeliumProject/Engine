#pragma once

#include "Vector3.h"

namespace Helium
{
    namespace Math
    {
        class AlignedBox;
        class Matrix4;
        class Plane;

        class FOUNDATION_API Line
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

            bool IntersectsPoint(const Vector3& pos, const f32 err = LinearIntersectionError, f32* mu = NULL, Vector3* offset = NULL) const;
            bool IntersectsLine(const Vector3& p1, const Vector3& p2, Vector3* location = NULL) const;
            bool IntersectsSegment(const Vector3& p1, const Vector3& p2, const f32 err = LinearIntersectionError, f32* mu = NULL, Vector3* offset = NULL) const;
            bool IntersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, f32* bary0 = NULL, f32* bary1 = NULL, f32* scale = NULL) const;
            bool IntersectsSphere(const Vector3& pos, const f32 radius, V_Vector3* intersections = NULL) const;
            bool IntersectsPlane(const Plane& plane, Vector3* intersection = NULL) const;
            bool IntersectsBox(const AlignedBox& box, Vector3* intersection = NULL) const;
            bool IntersectSegmentTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const;
            bool IntersectRayTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const;

            void ProjectPointOnSegment( const Vector3& point, Vector3& projectedPoint ) const;
            f32 DistanceSqrToPoint( const Vector3& point ) const;
            f32 DistanceToPoint( const Vector3& point ) const;
        };
    }
}