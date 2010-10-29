#include "Line.h"

#include "AlignedBox.h"
#include "Matrix4.h"
#include "Plane.h"

using namespace Helium;

#define EPSILON (0.00001f)

void Line::Transform(const Matrix4& m)
{
    MATH_FUNCTION_TIMER();

    m.TransformVertex(m_Origin);
    m.TransformVertex(m_Point);
}

// http://astronomy.swin.edu.au/~pbourke/geometry/pointline/
bool Line::IntersectsPoint(const Vector3& pos, const f32 err, f32* mu, Vector3* offset) const
{
    MATH_FUNCTION_TIMER();

    f32 length = (m_Origin - m_Point).Length();

    if( fabs(length) < EPSILON )
    {
        return false;
    }

    f32 u = (((pos.x - m_Origin.x) * (m_Point.x - m_Origin.x)) + ((pos.y - m_Origin.y) * (m_Point.y - m_Origin.y)) + ((pos.z - m_Origin.z) * (m_Point.z - m_Origin.z))) / (length * length);

    // closest m_Point does not fall within the line segment
    if(u < 0.0f || u > 1.0f)
    {
        return false;
    }

    Vector3 intersection;
    intersection.x = m_Origin.x + u * (m_Point.x - m_Origin.x);
    intersection.y = m_Origin.y + u * (m_Point.y - m_Origin.y);
    intersection.z = m_Origin.z + u * (m_Point.z - m_Origin.z);

    Vector3 o = pos - intersection;

    if (o.Length() <= err)
    {
        if (mu)
        {
            (*mu) = u;
        }

        if (offset)
        {
            (*offset) = o;
        }

        return true;
    }
    else
    {
        return false;
    }
}

// http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
bool Line::IntersectsLine(const Vector3& p1, const Vector3& p2, Vector3* location) const
{
    MATH_FUNCTION_TIMER();

    Vector3 p3 = m_Origin;
    Vector3 p4 = m_Point;

    Vector3 p13, p43, p21;
    f32 d1343, d4321, d1321, d4343, d2121;
    f32 numer, denom, mua, mub;

    p13.x = p1.x - p3.x;
    p13.y = p1.y - p3.y;
    p13.z = p1.z - p3.z;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;
    p43.z = p4.z - p3.z;

    if (abs(p43.x) < EPSILON && abs(p43.y) < EPSILON && abs(p43.z) < EPSILON)
    {
        return false;
    }

    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p21.z = p2.z - p1.z;

    if (abs(p21.x) < EPSILON && abs(p21.y) < EPSILON && abs(p21.z) < EPSILON)
    {
        return false;
    }

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;

    if (abs(denom) < EPSILON)
    {
        return false;
    }

    numer = d1343 * d4321 - d1321 * d4343;

    mua = numer / denom;
    mub = (d1343 + d4321 * (mua)) / d4343;

    if (location)
    {
        (*location) = Vector3 (p1.x + mua * p21.x, p1.y + mua * p21.y, p1.z + mua * p21.z);
    }

    return true;
}

// http://astronomy.swin.edu.au/~pbourke/geometry/lineline3d/
bool Line::IntersectsSegment(const Vector3& p1, const Vector3& p2, const f32 err, f32* mu, Vector3* offset) const
{
    MATH_FUNCTION_TIMER();

    Vector3 p3 = m_Origin;
    Vector3 p4 = m_Point;

    Vector3 p13, p43, p21;
    f32 d1343, d4321, d1321, d4343, d2121;
    f32 numer, denom, ua, ub;

    p13.x = p1.x - p3.x;
    p13.y = p1.y - p3.y;
    p13.z = p1.z - p3.z;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;
    p43.z = p4.z - p3.z;

    if (abs(p43.x) < EPSILON && abs(p43.y) < EPSILON && abs(p43.z) < EPSILON)
    {
        return false;
    }

    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p21.z = p2.z - p1.z;

    if (abs(p21.x) < EPSILON && abs(p21.y) < EPSILON && abs(p21.z) < EPSILON)
    {
        return false;
    }

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;

    if (abs(denom) < EPSILON)
    {
        return false;
    }

    numer = d1343 * d4321 - d1321 * d4343;

    ua = numer / denom;
    ub = (d1343 + d4321 * (ua)) / d4343;

    if (ua >= 0.0f && ua <= 1.0f)
    {
        Vector3 pa, pb;
        pa.x = p1.x + ua * p21.x;
        pa.y = p1.y + ua * p21.y;
        pa.z = p1.z + ua * p21.z;
        pb.x = p3.x + ub * p43.x;
        pb.y = p3.y + ub * p43.y;
        pb.z = p3.z + ub * p43.z;

        Vector3 o (pb - pa);

        if ( err < 0.0f || o.Length() < err )
        {
            if (mu)
            {
                (*mu) = ua;
            }

            if (offset)
            {
                (*offset) = o;
            }

            return true;
        }
    }

    return false;
}
//http://www.geometrictools.com/LibFoundation/Intersection/Intersection.html
bool Line::IntersectSegmentTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const
{
    Vector3 Origin = ( m_Origin + m_Point ) * 0.5f;

    // compute the offset origin, edges, and normal
    Vector3 kDiff = Origin - v0;
    Vector3 kEdge1 = v1 - v0;
    Vector3 kEdge2 = v2 - v0;
    Vector3 kNormal = kEdge1.Cross(kEdge2);
    Vector3 direction = m_Point - m_Origin;

    f32 extent = direction.Length() * 0.5f;
    direction.Normalize();

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = segment direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    f32 fDdN = direction.Dot(kNormal);
    f32 fSign;
    if (fDdN > EPSILON)
    {
        fSign = 1.0f;
    }
    else if (fDdN < -EPSILON)
    {
        fSign = -1.0f;
        fDdN = -fDdN;
    }
    else
    {
        // Segment and triangle are parallel, call it a "no intersection"
        // even if the segment does intersect.
        return false;
    }

    f32 fDdQxE2 = fSign*direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= 0.0f)
    {
        f32 fDdE1xQ = fSign*direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= 0.0f)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle, check if segment does
                f32 fQdN = -fSign*kDiff.Dot(kNormal);
                f32 fExtDdN = extent*fDdN;
                if (-fExtDdN <= fQdN && fQdN <= fExtDdN)
                {
                    // segment intersects triangle
                    return true;
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}

bool Line::IntersectRayTriangle( const Vector3& v0, const Vector3& v1, const Vector3& v2 ) const
{
    Vector3 Origin = ( m_Origin + m_Point ) * 0.5f;
    // compute the offset origin, edges, and normal
    Vector3 kDiff = Origin - v0;
    Vector3 kEdge1 = v1 - v0;
    Vector3 kEdge2 = v2 - v0;
    Vector3 kNormal = kEdge1.Cross(kEdge2);
    Vector3 direction = m_Point - m_Origin;
    direction.Normalize();

    // Solve Q + t*D = b1*E1 + b2*E2 (Q = kDiff, D = ray direction,
    // E1 = kEdge1, E2 = kEdge2, N = Cross(E1,E2)) by
    //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
    //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
    //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
    f32 fDdN = direction.Dot(kNormal);
    f32 fSign;
    if (fDdN > EPSILON)
    {
        fSign = (f32)1.0;
    }
    else if (fDdN < -EPSILON)
    {
        fSign = (f32)-1.0;
        fDdN = -fDdN;
    }
    else
    {
        // Ray and triangle are parallel, call it a "no intersection"
        // even if the ray does intersect.
        return false;
    }

    f32 fDdQxE2 = fSign*direction.Dot(kDiff.Cross(kEdge2));
    if (fDdQxE2 >= (f32)0.0)
    {
        f32 fDdE1xQ = fSign*direction.Dot(kEdge1.Cross(kDiff));
        if (fDdE1xQ >= (f32)0.0)
        {
            if (fDdQxE2 + fDdE1xQ <= fDdN)
            {
                // line intersects triangle, check if ray does
                f32 fQdN = -fSign*kDiff.Dot(kNormal);
                if (fQdN >= (f32)0.0)
                {
                    // ray intersects triangle
                    return true;
                }
                // else: t < 0, no intersection
            }
            // else: b1+b2 > 1, no intersection
        }
        // else: b2 < 0, no intersection
    }
    // else: b1 < 0, no intersection

    return false;
}

// From Microsoft's Pick example...
//  sf is the scaling of dir to intersection m_Point
//  u and v are v0's and v1's barycentric coordinates
bool Line::IntersectsTriangle(const Vector3& v0, const Vector3& v1, const Vector3& v2, f32* bary0, f32* bary1, f32* scale) const
{
    MATH_FUNCTION_TIMER();

    f32 u, v;

    // transform and copy m_Origin
    Vector3 orig = m_Origin;

    // transform m_Point and build direction vector
    Vector3 direction = m_Point - orig;

    // Find vectors for two edges sharing vert0
    Vector3 edge1 = v1 - v0;
    Vector3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    Vector3 pvec = direction.Cross(edge2);

    // If determinant is near zero, ray lies in plane of triangle
    f32 det = edge1.Dot(pvec);

    Vector3 tvec;

    if (det > 0)
    {
        tvec = orig - v0;
    }
    else
    {
        tvec = v0 - orig;
        det = -det;
    }

    if (det < EPSILON)
    {
        return false;
    }

    // Calculate U parameter and test bounds
    u = tvec.Dot(pvec);

    if (u < 0.0f || u > det)
    {
        return false;
    }

    // Prepare to test V parameter
    Vector3 qvec = tvec.Cross(edge1);

    // Calculate V parameter and test bounds
    v = direction.Dot(qvec);

    if (v < 0.0f || u + v > det)
    {
        return false;
    }

    if (bary0 || bary1 || scale)
    {
        f32 invDet = 1.0f / det;

        if (bary0)
        {
            (*bary0) = u * invDet;
        }

        if (bary1)
        {
            (*bary1) = v * invDet;
        }

        if (scale)
        {
            (*scale) = edge2.Dot(qvec) * invDet;
        }
    }

    return true;
}

// http://astronomy.swin.edu.au/~pbourke/geometry/sphereline/
bool Line::IntersectsSphere(const Vector3& pos, const f32 radius, V_Vector3* intersections) const
{
    MATH_FUNCTION_TIMER();

    // polynomial coefficients
    f32 a = SQR(m_Point.x - m_Origin.x) + SQR(m_Point.y - m_Origin.y) + SQR(m_Point.z - m_Origin.z);
    f32 b = 2 * ((m_Point.x - m_Origin.x)*(m_Origin.x - pos.x) + (m_Point.y - m_Origin.y)*(m_Origin.y - pos.y) + (m_Point.z - m_Origin.z)*(m_Origin.z - pos.z)) ;
    f32 c = SQR(pos.x) + SQR(pos.y) + SQR(pos.z) + SQR(m_Origin.x) + SQR(m_Origin.y) + SQR(m_Origin.z) - 2 * (pos.x*m_Origin.x + pos.y*m_Origin.y + pos.z*m_Origin.z) - SQR(radius);

    // discriminant
    f32 i = b * b - 4 * a * c ;

    if (i == 0.0)
    {
        if (intersections)
        {
            // one intersection
            intersections->resize(1);

            f32 mu = -b/(2*a);

            (*intersections)[0] = Vector3 (m_Origin.x + mu*(m_Point.x-m_Origin.x), m_Origin.y + mu*(m_Point.y-m_Origin.y), m_Origin.z + mu*(m_Point.z-m_Origin.z));
        }

        return true;
    }
    else if (i > 0.0)
    {
        if (intersections)
        {
            // two intersections
            intersections->resize(2);

            // first intersection
            f32 mu = (-b + static_cast<f32>(sqrt(SQR(b) - 4*a*c))) / (2*a);
            (*intersections)[0] = Vector3 (m_Origin.x + mu*(m_Point.x-m_Origin.x), m_Origin.y + mu*(m_Point.y-m_Origin.y), m_Origin.z + mu*(m_Point.z-m_Origin.z));

            // second intersection
            mu = (-b - static_cast<f32>(sqrt(SQR(b) - 4*a*c))) / (2*a);
            (*intersections)[1] = Vector3 (m_Origin.x + mu*(m_Point.x-m_Origin.x), m_Origin.y + mu*(m_Point.y-m_Origin.y), m_Origin.z + mu*(m_Point.z-m_Origin.z));
        }

        return true;
    }

    return false;
}

// http://astronomy.swin.edu.au/~pbourke/geometry/planeline/
bool Line::IntersectsPlane(const Plane& plane, Vector3* intersection) const
{
    MATH_FUNCTION_TIMER();

    f32 den = (plane.A() * (m_Origin.x - m_Point.x)) + (plane.B() * (m_Origin.y - m_Point.y)) + (plane.C() * (m_Origin.z - m_Point.z));

    if (abs(den) < ValueNearZero)
    {
        return false;
    }
    else
    {
        if (intersection)
        {
            f32 mu = ( (plane.A() * m_Origin.x) + (plane.B() * m_Origin.y) + (plane.C() * m_Origin.z) + plane.D() ) / den;

            (*intersection) = m_Origin + (m_Point - m_Origin) * mu;
        }

        return true;
    }
}

bool Line::IntersectsBox(const AlignedBox& box, Vector3* intersection) const
{
    MATH_FUNCTION_TIMER();

    bool inside = true;
    i32 i;

    // Plane selection
    i32 whichPlane;

    // Plane division values
    const i32 LEFT = 0;
    const i32 RIGHT = 1;
    const i32 MIDDLE = 2;

    // AABB values
    const Vector3& min = box.minimum;
    const Vector3& max = box.maximum;

    // Ray values
    Vector3 dir = m_Point - m_Origin;

    // Temp Values
    Vector3 quadrant;
    Vector3 distance;
    Vector3 candidatePlane;
    Vector3 intersect;

    // Find candidate planes; this loop can be avoided if
    //  rays cast all from the eye(assume perpsective view)
    for (i=0; i<3; i++)
    {
        if(m_Origin[i] < min[i])
        {
            quadrant[i] = (f32)LEFT;
            candidatePlane[i] = min[i];
            inside = false;
        }
        else if (m_Origin[i] > max[i])
        {
            quadrant[i] = (f32)RIGHT;
            candidatePlane[i] = max[i];
            inside = false;
        }
        else
        {
            quadrant[i] = (f32)MIDDLE;
        }
    }

    // Ray m_Origin inside bounding box
    if(inside)
    {
        if (intersection)
        {
            (*intersection) = m_Origin;
        }

        return true;
    }

    // Calculate T distances to candidate planes
    for (i = 0; i < 3; i++)
    {
        if (quadrant[i] != MIDDLE && dir[i] !=0)
        {
            distance[i] = (candidatePlane[i]-m_Origin[i]) / dir[i];
        }
        else
        {
            distance[i] = -1;
        }
    }

    // Get largest of the distance's for final choice of intersection
    whichPlane = 0;
    for (i = 1; i < 3; i++)
    {
        if (distance[whichPlane] < distance[i])
        {
            whichPlane = i;
        }
    }

    // Check final candidate actually inside box
    if (distance[whichPlane] < 0)
    {
        return false;
    }

    for (i = 0; i < 3; i++)
    {
        if (whichPlane != i)
        {
            intersect[i] = m_Origin[i] + distance[whichPlane] *dir[i];

            if (intersect[i] < min[i] || intersect[i] > max[i])
            {
                return false;
            }
        }
        else
        {
            intersect[i] = candidatePlane[i];
        }
    }

    if (intersection)
    {
        (*intersection) = intersect;
    }

    return true;
}

//http://www.geometrictools.com/LibFoundation/Distance/Distance.html
void Line::ProjectPointOnSegment( const Vector3& point, Vector3& projectedPoint ) const
{
    Vector3 diff = point - m_Origin;
    Vector3 direction = m_Point - m_Origin;
    f32 extent = direction.Length();
    direction.Normalize();

    f32 segmentParam = direction.Dot(diff);

    if ( -extent < segmentParam )
    {
        if ( segmentParam < extent )
        {
            projectedPoint = m_Origin + direction * segmentParam;
        }
        else
        {
            projectedPoint = m_Origin + direction * extent;
        }
    }
    else
    {
        projectedPoint = m_Origin - direction * extent;
    }
}


f32 Line::DistanceSqrToPoint( const Vector3& point ) const
{
    Vector3 projectedPoint;

    ProjectPointOnSegment( point, projectedPoint );

    return (projectedPoint - point).LengthSquared();
}

f32 Line::DistanceToPoint( const Vector3& point ) const
{
    return sqrt( DistanceSqrToPoint( point ) );
}