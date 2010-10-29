#include "AlignedBox.h"

#include "Matrix4.h"
#include "Frustum.h"

using namespace Helium;

const AlignedBox AlignedBox::Unit = AlignedBox ( Vector3 (-0.5f, -0.5f, -0.5f),
                                                Vector3 ( 0.5f,  0.5f,  0.5f) );

const AlignedBox AlignedBox::Singular = AlignedBox ( Vector3::Zero,
                                                    Vector3::Zero );

Vector3 AlignedBox::ClosestCorner( const Vector3& v ) const
{
    Vector3 winner;
    f32 winnerDist = v.LengthSquared();
    Vector3 current;
    f32 currentDist;

    current.Set(maximum.x, maximum.y, maximum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(maximum.x, minimum.y, maximum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(minimum.x, minimum.y, maximum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(minimum.x, maximum.y, maximum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(maximum.x, maximum.y, minimum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(maximum.x, minimum.y, minimum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(minimum.x, minimum.y, minimum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    current.Set(minimum.x, maximum.y, minimum.z);
    currentDist = (current - v).LengthSquared();
    if ( currentDist < winnerDist )
    {
        winner = current;
        winnerDist = currentDist;
    }

    return winner;
}

Vector3 AlignedBox::Test(Vector3 vertex)
{
    if (!seeded)
    {
        seeded = true;
        minimum = vertex;
        maximum = vertex;
        return vertex;
    }

    if (vertex.x > maximum.x)
        maximum.x = vertex.x;

    if (vertex.x < minimum.x)
        minimum.x = vertex.x;

    if (vertex.y > maximum.y)
        maximum.y = vertex.y;

    if (vertex.y < minimum.y)
        minimum.y = vertex.y;

    if (vertex.z > maximum.z)
        maximum.z = vertex.z;

    if (vertex.z < minimum.z)
        minimum.z = vertex.z;

    return vertex;
}

void AlignedBox::Merge(const AlignedBox& box)
{
    if (!seeded)
    {
        *this = box;
        seeded = true;
    }

    if (box.maximum.x > maximum.x)
        maximum.x = box.maximum.x;

    if (box.minimum.x < minimum.x)
        minimum.x = box.minimum.x;

    if (box.maximum.y > maximum.y)
        maximum.y = box.maximum.y;

    if (box.minimum.y < minimum.y)
        minimum.y = box.minimum.y;

    if (box.maximum.z > maximum.z)
        maximum.z = box.maximum.z;

    if (box.minimum.z < minimum.z)
        minimum.z = box.minimum.z;
}

void AlignedBox::Merge(const Vector3& position)
{
    AlignedBox box = AlignedBox::Unit;
    box.minimum += position;
    box.maximum += position;

    if (!seeded)
    {
        *this = box;
        seeded = true;
    }

    if (box.maximum.x > maximum.x)
        maximum.x = box.maximum.x;

    if (box.minimum.x < minimum.x)
        minimum.x = box.minimum.x;

    if (box.maximum.y > maximum.y)
        maximum.y = box.maximum.y;

    if (box.minimum.y < minimum.y)
        minimum.y = box.minimum.y;

    if (box.maximum.z > maximum.z)
        maximum.z = box.maximum.z;

    if (box.minimum.z < minimum.z)
        minimum.z = box.minimum.z;
}

void AlignedBox::Transform(const Matrix4& matrix)
{
    // get the currents sample bounds
    V_Vector3 vertices;
    GetVertices( vertices );

    // reseed this box
    Reset();

    // iterate and resample the bounds
    V_Vector3::iterator itr = vertices.begin();
    V_Vector3::iterator end = vertices.end();
    for ( ; itr != end; ++itr )
    {
        // transform the sample
        matrix.TransformVertex( *itr );

        // test the sample
        Test( *itr );
    }
}

void AlignedBox::GetVertices(V_Vector3& vertices) const
{
    vertices.resize(8);
    vertices[0] = Vector3 (maximum.x, maximum.y, maximum.z);
    vertices[1] = Vector3 (maximum.x, minimum.y, maximum.z);
    vertices[2] = Vector3 (minimum.x, minimum.y, maximum.z);
    vertices[3] = Vector3 (minimum.x, maximum.y, maximum.z);
    vertices[4] = Vector3 (maximum.x, maximum.y, minimum.z);
    vertices[5] = Vector3 (maximum.x, minimum.y, minimum.z);
    vertices[6] = Vector3 (minimum.x, minimum.y, minimum.z);
    vertices[7] = Vector3 (minimum.x, maximum.y, minimum.z);
}

void AlignedBox::GetWireframe(const V_Vector3& vertices, V_Vector3& lineList, bool clear)
{
    if (clear)
    {
        lineList.clear();
    }

    //front 2103
    lineList.push_back(vertices[2]);
    lineList.push_back(vertices[1]);

    lineList.push_back(vertices[1]);
    lineList.push_back(vertices[0]);

    lineList.push_back(vertices[0]);
    lineList.push_back(vertices[3]);

    lineList.push_back(vertices[3]);
    lineList.push_back(vertices[2]);

    //back 5674
    lineList.push_back(vertices[5]);
    lineList.push_back(vertices[6]);

    lineList.push_back(vertices[6]);
    lineList.push_back(vertices[7]);

    lineList.push_back(vertices[7]);
    lineList.push_back(vertices[4]);

    lineList.push_back(vertices[4]);
    lineList.push_back(vertices[5]);

    //middle
    lineList.push_back(vertices[1]);
    lineList.push_back(vertices[5]);

    lineList.push_back(vertices[3]);
    lineList.push_back(vertices[7]);

    lineList.push_back(vertices[0]);
    lineList.push_back(vertices[4]);

    lineList.push_back(vertices[6]);
    lineList.push_back(vertices[2]);
}

void AlignedBox::GetTriangulated(const V_Vector3& vertices, V_Vector3& triangleList, bool clear)
{
    if (clear)
    {
        triangleList.clear();
    }

    //front 2103
    triangleList.push_back(vertices[2]);
    triangleList.push_back(vertices[1]);
    triangleList.push_back(vertices[0]);

    triangleList.push_back(vertices[0]);
    triangleList.push_back(vertices[3]);
    triangleList.push_back(vertices[2]);

    //right 1540
    triangleList.push_back(vertices[1]);
    triangleList.push_back(vertices[5]);
    triangleList.push_back(vertices[4]);

    triangleList.push_back(vertices[4]);
    triangleList.push_back(vertices[0]);
    triangleList.push_back(vertices[1]);

    //back 5674
    triangleList.push_back(vertices[5]);
    triangleList.push_back(vertices[6]);
    triangleList.push_back(vertices[7]);

    triangleList.push_back(vertices[7]);
    triangleList.push_back(vertices[4]);
    triangleList.push_back(vertices[5]);

    //left 6237
    triangleList.push_back(vertices[6]);
    triangleList.push_back(vertices[2]);
    triangleList.push_back(vertices[3]);

    triangleList.push_back(vertices[3]);
    triangleList.push_back(vertices[7]);
    triangleList.push_back(vertices[6]);

    //top 3047
    triangleList.push_back(vertices[3]);
    triangleList.push_back(vertices[0]);
    triangleList.push_back(vertices[4]);

    triangleList.push_back(vertices[4]);
    triangleList.push_back(vertices[7]);
    triangleList.push_back(vertices[3]);

    //bottom 6512
    triangleList.push_back(vertices[6]);
    triangleList.push_back(vertices[5]);
    triangleList.push_back(vertices[1]);

    triangleList.push_back(vertices[1]);
    triangleList.push_back(vertices[2]);
    triangleList.push_back(vertices[6]);
}

//http://www.geometrictools.com/LibFoundation/Intersection/Intersection.html
bool AlignedBox::IntersectsSphere( const Vector3& pos, const f32 radius ) const
{
    Vector3 center;
    Vector3 axis[3];
    f32 extent[3];

    V_Vector3 vertices;
    GetVertices( vertices );
    axis[0] = vertices[0] - vertices[3];
    axis[1] = vertices[0] - vertices[1];
    axis[2] = vertices[0] - vertices[4];

    extent[0] = axis[0].Length() * 0.5f;
    extent[1] = axis[1].Length() * 0.5f;
    extent[2] = axis[2].Length() * 0.5f;

    axis[0].Normalize();
    axis[1].Normalize();
    axis[2].Normalize();


    center = ( (maximum - minimum) * 0.5f ) + minimum;

    // Test for intersection in the coordinate system of the box by
    // transforming the sphere into that coordinate system.
    Vector3 kCDiff = pos - center;

    f32 fAx = fabs(kCDiff.Dot(axis[0]));
    f32 fAy = fabs(kCDiff.Dot(axis[1]));
    f32 fAz = fabs(kCDiff.Dot(axis[2]));
    f32 fDx = fAx - extent[0];
    f32 fDy = fAy - extent[1];
    f32 fDz = fAz - extent[2];

    if (fAx <= extent[0])
    {
        if (fAy <= extent[1])
        {
            if (fAz <= extent[2])
            {
                // sphere center inside box
                return true;
            }
            else
            {
                // potential sphere-face intersection with face z
                return fDz <= radius;
            }
        }
        else
        {
            if (fAz <= extent[2])
            {
                // potential sphere-face intersection with face y
                return fDy <= radius;
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces y and z
                f32 fRSqr = radius*radius;
                return fDy*fDy + fDz*fDz <= fRSqr;
            }
        }
    }
    else
    {
        if (fAy <= extent[1])
        {
            if (fAz <= extent[2])
            {
                // potential sphere-face intersection with face x
                return fDx <= radius;
            }
            else
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and z
                f32 fRSqr = radius*radius;
                return fDx*fDx + fDz*fDz <= fRSqr;
            }
        }
        else
        {
            if (fAz <= extent[2])
            {
                // potential sphere-edge intersection with edge formed
                // by faces x and y
                f32 fRSqr = radius*radius;
                return fDx*fDx + fDy*fDy <= fRSqr;
            }
            else
            {
                // potential sphere-vertex intersection at corner formed
                // by faces x,y,z
                f32 fRSqr = radius*radius;
                return fDx*fDx + fDy*fDy + fDz*fDz <= fRSqr;
            }
        }
    }
}

bool AlignedBox::IntersectsBox( const AlignedBox& box ) const
{
#pragma TODO("This is cheesy, there must be a better test")
    return Frustum ( box ).IntersectsBox( *this );
}
