#include "FoundationPch.h"
#include "Frustum.h"
#include "Polygon.h"
#include "Line.h"

using namespace Helium;

namespace ClipCodes
{
    enum ClipCode
    {
        None    = 0,
        Top     = 1 << 0,
        Bottom  = 1 << 1,
        Left    = 1 << 2,
        Right   = 1 << 3,
        Near    = 1 << 4,
        Far     = 1 << 5,
        All     = 0x3f,
    };
}

typedef ClipCodes::ClipCode ClipCode;

// Cohen-Sutherland clipping code
inline static ClipCode GetClipCode(const Frustum& f, const Vector3& p)
{
    int32_t c = ClipCodes::None;

    if (f.top.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Top;
    else if (f.bottom.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Bottom;

    if (f.left.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Left;
    else if (f.right.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Right;

    if (f.front.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Near;
    else if (f.back.DistanceAbove(p) < -HELIUM_POINT_ON_PLANE_ERROR)
        c = c | ClipCodes::Far;

    return static_cast<ClipCode>(c);
}

Frustum::Frustum(const Matrix4& m)
{
    if (m == Matrix4::Zero)
    {
        return;
    }

    Plane p;

    // Extract the LEFT plane
    p.A(m.x.w + m.x.x);
    p.B(m.y.w + m.y.x);
    p.C(m.z.w + m.z.x);
    p.D(m.t.w + m.t.x);
    p.Normalize();
    left = p;

    // Extract the RIGHT plane
    p.A(m.x.w - m.x.x);
    p.B(m.y.w - m.y.x);
    p.C(m.z.w - m.z.x);
    p.D(m.t.w - m.t.x);
    p.Normalize();
    right = p;

    // Extract the BOTTOM plane
    p.A(m.x.w + m.x.y);
    p.B(m.y.w + m.y.y);
    p.C(m.z.w + m.z.y);
    p.D(m.t.w + m.t.y);
    p.Normalize();
    bottom = p;

    // Extract the TOP plane
    p.A(m.x.w - m.x.y);
    p.B(m.y.w - m.y.y);
    p.C(m.z.w - m.z.y);
    p.D(m.t.w - m.t.y);
    p.Normalize();
    top = p;

    // Extract the NEAR plane
    p.A(m.x.w + m.x.z);
    p.B(m.y.w + m.y.z);
    p.C(m.z.w + m.z.z);
    p.D(m.t.w + m.t.z);
    p.Normalize();
    front = p;

    // Extract the FAR plane
    p.A(m.x.w - m.x.z);
    p.B(m.y.w - m.y.z);
    p.C(m.z.w - m.z.z);
    p.D(m.t.w - m.t.z);
    p.Normalize();
    back = p;
}

Frustum::Frustum(const AlignedBox& b)
{
    V_Vector3 v;
    b.GetVertices(v);

    //top 3047
    top = Plane(v[3], (v[3] - v[0]).Cross(v[4] - v[0]));

    //bottom 6512
    bottom = Plane(v[6], (v[6] - v[5]).Vector3::Cross(v[1] - v[5]));

    //left 6237
    left = Plane(v[6], (v[6] - v[2]).Vector3::Cross(v[3] - v[2]));

    //right 1540
    right = Plane(v[1], (v[1] - v[5]).Cross(v[4] - v[5]));

    //front 2103
    front = Plane(v[2], (v[2] - v[1]).Cross(v[0] - v[1]));

    //back 5674
    back = Plane(v[5], (v[5] - v[6]).Cross(v[7] - v[6]));
}

void Frustum::Transform(const Matrix4& m)
{
    MATH_FUNCTION_TIMER();

    Matrix4 inv = m.Inverted();
    inv.Transpose();

    top.Transform(inv);
    bottom.Transform(inv);
    left.Transform(inv);
    right.Transform(inv);
    front.Transform(inv);
    back.Transform(inv);
}

bool Frustum::IntersectsPoint(const Vector3& p, const float32_t distanceFromPlane) const
{
    MATH_FUNCTION_TIMER();

    // we assume its above each plane
    bool above = true;

    // for each plane
    for (int32_t j=0; j<6 && above; j++)
    {
        // we need this vert to be above each plane
        above &= (*this)[j].DistanceAbove(p) >= -distanceFromPlane;
    }

    // if it was above, we found an interior vertex
    return above;
}

//
// 3D version of the Cohen-Sutherland line clipping algorithm
//

bool Frustum::IntersectsSegment(const Vector3& point1, const Vector3& point2) const
{
    MATH_FUNCTION_TIMER();

    ClipCode c1, c2;
    Vector3 p1 (point1), p2 (point2);

    c1 = GetClipCode (*this, p1);
    c2 = GetClipCode (*this, p2);

    int32_t count = 0;
    while (++count < 16)
    {
        // if both points are not clipped by any planes
        if (c1 == ClipCodes::None && c2 == ClipCodes::None)
        {
            // intersection exit condition
            return true;
        }
        // if both points are not clipped by any of the same planes
        else if ((c1 & c2) != ClipCodes::None)
        {
            // non-intersection exit contition
            return false;
        }
        else
        {
            //
            // Failed both exit tests, so calculate the line segment to clip
            //

            ClipCode c = ClipCodes::None;

            Vector3 v = Vector3::Zero;

            // pick the point outside the clipping region
            if (c1 != ClipCodes::None)
            {
                c = c1;
                v = p1;
            }
            else
            {
                c = c2;
                v = p2;
            }

            // if either are not above
            Line line = Line (p1, p2);

            const Plane* clipPlane = NULL;

            // Now find intersection point
            if ((c & ClipCodes::Top) != ClipCodes::None)
                clipPlane = &top;
            else if ((c & ClipCodes::Bottom) != ClipCodes::None)
                clipPlane = &bottom;
            else if ((c & ClipCodes::Left) != ClipCodes::None)
                clipPlane = &left;
            else if ((c & ClipCodes::Right) != ClipCodes::None)
                clipPlane = &right;
            else if ((c & ClipCodes::Near) != ClipCodes::None)
                clipPlane = &front;
            else if ((c & ClipCodes::Far) != ClipCodes::None)
                clipPlane = &back;

            // Clip end point into v... Note that if this intersection test fails, v is the point in question
            if ( !line.IntersectsPlane( *clipPlane, &v ) )
            {
                return false;
            }

            // Now we move outside point to intersection point to clip, and get ready for next pass.
            if (c == c1)
            {
                p1 = v;
                c1 = GetClipCode(*this, p1);
            }
            else
            {
                p2 = v;
                c2 = GetClipCode(*this, p2);
            }
        }
    }

    return false;
}

//
// 3D version of the Sutherland-Hodgman polygon clipping algorithm
//

bool Frustum::IntersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2) const
{
    MATH_FUNCTION_TIMER();

    // this is the source polygon and it will glow as we clip each plane in the frustum
    StaticPolygon<Vector3, 9> polygon (v0, v1, v2);

    // for each clipping plane (the frustum)
    for (int32_t i=0; i<6; i++)
    {
        // the clipping plane to use for this iteration
        const Plane& clipPlane = (*this)[i];

        // this is the clipping target for this plane
        StaticPolygon<Vector3, 9> clipped;

        // first edge vert
        Vector3 p;

        // second edge vert, start with the last one
        Vector3 s = polygon.m_Vertices[polygon.m_Size-1];

        // for each edge in polygon
        for (int32_t j=0; j<polygon.m_Size; j++)
        {
            // get next edge vert
            p = polygon.m_Vertices[j];

            // if p is above the plane
            if (clipPlane.DistanceAbove(p) > -HELIUM_POINT_ON_PLANE_ERROR)
            {
                // and s is above the plane
                if (clipPlane.DistanceAbove(s) > -HELIUM_POINT_ON_PLANE_ERROR)
                {
                    // just take p, its a natural
                    clipped.Append(p);
                }
                // else we intersect, compute it and add the intersection and p
                else
                {
                    Vector3 i;
                    bool intersects = Line (p, s).IntersectsPlane( clipPlane, &i );
                    HELIUM_ASSERT(intersects);

                    // since our previous one (s) is outside, appen it first
                    clipped.Append( i );

                    // then append p, because it above the clipping plane
                    clipped.Append( p );
                }
            }
            // else p is below the plane
            else
            {
                // if s is above the plane, compute the intersection and append it
                if (clipPlane.DistanceAbove(s) > -HELIUM_POINT_ON_PLANE_ERROR)
                {
                    Vector3 i;
                    bool intersects = Line (p, s).IntersectsPlane( clipPlane, &i );
                    HELIUM_ASSERT(intersects);

                    // p is not above the plane, it will be clipped, so just append the intersection
                    //  because since s is the previous one, it will have been added in the previous iteration
                    clipped.Append( i );
                }
                else
                {
                    // no action, its below/below
                }
            }

            // move edge forward
            s = p;
        }

        // copy latest iteration into our result
        polygon = clipped;
    }

    return polygon.m_Size != 0;
}

bool Frustum::IntersectsBox(const AlignedBox& box, bool precise) const
{
    MATH_FUNCTION_TIMER();

    if ( fabs((box.maximum - box.minimum).Length()) < HELIUM_LINEAR_INTERSECTION_ERROR )
    {
        return IntersectsPoint( box.Center() );
    }
    else
    {
        float32_t m, n;
        Vector3 c = box.Center();
        Vector3 d = box.maximum - c;

        for (int32_t i=0; i<6; i++)
        {
            const Plane& p = (*this)[i];

            m = (c.x * p.A()) + (c.y * p.B()) + (c.z * p.C()) + p.D();
            n = (d.x * abs(p.A())) + (d.y * abs(p.B())) + (d.z * abs(p.C()));

            if (m + n < 0)
            {
                return false;
            }
        }

        if ( precise )
        {
#pragma TODO("This precise test should be using separated axis testing instead of triangle decimation")
            V_Vector3 vertices;
            vertices.reserve(8);
            box.GetVertices(vertices);

            V_Vector3 triangleList;
            triangleList.reserve(36);
            AlignedBox::GetTriangulated(vertices, triangleList);

            for ( int i=0; i<36; i+=3 )
            {
                if ( IntersectsTriangle( triangleList[i], triangleList[i+1], triangleList[i+2] ) )
                {
                    return true;
                }
            }

            return false;
        }
    }

    return true;
}

bool Frustum::Contains(const AlignedBox& box) const
{
    MATH_FUNCTION_TIMER();

    V_Vector3 verts;
    box.GetVertices(verts);

    for (size_t i=0; i<verts.size(); i++)
    {
        // for each plane
        for (int32_t j=0; j<6; j++)
        {
            const Plane& p = (*this)[j];

            // we need this vert to be above each plane
            if ((verts[i].x * p.A()) + (verts[i].y * p.B()) + (verts[i].z * p.C()) + p.D() < 0)
            {
                return false;
            }
        }
    }

    return true;
}
