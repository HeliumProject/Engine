#include "Curve.h"
#include "Matrix4.h"

using namespace Helium::Math;

const static Matrix4 CatmullRomBasis ( Vector4 (0.0f, -1.0f,  2.0f, -1.0f) / 2.0f,
                                      Vector4 (2.0f,  0.0f, -5.0f,  3.0f) / 2.0f,
                                      Vector4 (0.0f,  1.0f,  4.0f, -3.0f) / 2.0f,
                                      Vector4 (0.0f,  0.0f, -1.0f,  1.0f) / 2.0f );

const static Matrix4 BSplineBasis (    Vector4 (-1.0f,  3.0f, -3.0f,  1.0f) / 6.0f,
                                   Vector4 ( 3.0f, -6.0f,  0.0f,  4.0f) / 6.0f,
                                   Vector4 (-3.0f,  3.0f,  3.0f,  1.0f) / 6.0f,
                                   Vector4 ( 1.0f,  0.0f,  0.0f,  0.0f) / 6.0f );

// computes the vector parameter factor
static Vector4 ComputeParam(f32 t, const Curve::Type type);

// retrieves the basis matrix for curve type
static const Matrix4& ComputeBasis( const Curve::Type type );

// computes a curve point
static Vector3 ComputePoint(f32 param, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const Curve::Type type );

// compputes B and Catmull Rom splines, respectively
static void ComputeBSpline( V_Vector3& controlPoints, const u32 resolution, const bool closed, V_Vector3& points  );
static void ComputeCatmullRom( V_Vector3& controlPoints, const u32 resolution, const bool closed, V_Vector3& points  );

static void MakeContinuous(V_Vector3& cvs);
static void MakeClosed(V_Vector3& cvs);

bool Curve::ComputeCurve( const V_Vector3& controlPoints, const u32 resolution, const bool closed, const Type type, V_Vector3& points )
{
    bool success = false;

    if (resolution == 0 || controlPoints.size()<4)
        return success;

    success = true;

    points.clear( );

    V_Vector3 tempControlPoints( controlPoints );

    if( closed )
    {
        MakeClosed( tempControlPoints );    
    }
    else
    {
        MakeContinuous( tempControlPoints );
    }

    switch( type )
    {
    case Curve::kLinear:
        {
            points = tempControlPoints;
            break;
        }

    case Curve::kBSpline:
        {
            ComputeBSpline( tempControlPoints, resolution, closed, points );
            break;
        }

    case Curve::kCatmullRom:
        {
            ComputeCatmullRom( tempControlPoints, resolution, closed, points );
            break;
        }

    default:
        {
            points = tempControlPoints;
            break;
        }
    }

    return success;
}


static void MakeContinuous(V_Vector3& cvs)
{
    // ensure continuity through to first and last cvs by creating new begin and end tagent cvs
    Vector3 ab = cvs[0] - cvs[1];
    cvs.insert(cvs.begin(), cvs[0] + ab);

    Vector3 cd = cvs[ cvs.size() - 1 ] - cvs[ cvs.size() - 2 ];
    cvs.push_back(cvs[ cvs.size() - 1 ] + cd);
}

static void MakeClosed(V_Vector3& cvs)
{
    // synthisize a new knot from the last two and first two cvs
    cvs.insert(cvs.begin(), cvs[cvs.size()-1]);
    cvs.insert(cvs.begin(), cvs[cvs.size()-2]);

    // this is 2 and 3 since we inserted @ 0 above
    cvs.push_back(cvs[2]);
    cvs.push_back(cvs[3]);
}

Vector4 ComputeParam(f32 t, const Curve::Type type )
{
    switch (type)
    {
    case Curve::kLinear:
        break;

    case Curve::kBSpline:
        return Vector4 (t*t*t, t*t, t, 1);
        break;

    case Curve::kCatmullRom:
        return Vector4 (1, t, t*t, t*t*t);
        break;

    default:
        HELIUM_ASSERT(false);
    }

    return Vector4::Zero;
}

const Matrix4& ComputeBasis( const Curve::Type type )
{
    switch (type)
    {
    case Curve::kLinear:
        break;

    case Curve::kBSpline:
        return BSplineBasis;
        break;

    case Curve::kCatmullRom:
        return CatmullRomBasis;
        break;

    default:
        HELIUM_ASSERT(false);
    }

    return Matrix4::Zero;
}

Vector3 ComputePoint(f32 param, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const Curve::Type type )
{
    Vector4 v = ComputeParam( param, type );
    const Matrix4& curveBasis = ComputeBasis( type );

    Vector4 x (a.x, b.x, c.x, d.x);
    x = curveBasis * x;

    Vector4 y (a.y, b.y, c.y, d.y);
    y = curveBasis * y;

    Vector4 z (a.z, b.z, c.z, d.z);
    z = curveBasis * z;

    return Vector3 (v.Dot(x), v.Dot(y), v.Dot(z));
}



void ComputeBSpline( V_Vector3& controlPoints, const u32 resolution, const bool closed, V_Vector3& points  )
{
    f32 t = 0.0f;
    u32 start = 0;
    u32 end   = resolution;

    u32 countControlPoints = (u32)( controlPoints.size( ) - 3 );

    f32 step = 1.0f/static_cast<f32>( resolution );

    for( u32 i = 0; i < countControlPoints; ++i )
    {
        t = 0.0f;
        end = resolution;

        //push end forward to get the last point
        if( i == countControlPoints - 1 )
        {
            if( closed )
                break;

            end++;
        }

        //only incur the cost of copying from the STL vector once
        const Vector3& cp0 = controlPoints[ i ];
        const Vector3& cp1 = controlPoints[ i + 1 ];
        const Vector3& cp2 = controlPoints[ i + 2 ];
        const Vector3& cp3 = controlPoints[ i + 3 ];

        for( u32 j = start; j < end; ++j )
        {
            points.push_back( ComputePoint( t, cp0, cp1, cp2, cp3, Curve::kBSpline ) );
            t += step;
        }
    }
}

void ComputeCatmullRom( V_Vector3& controlPoints, const u32 resolution, const bool closed, V_Vector3& points  )
{
    f32 t = 0.0f;
    u32 start = 0;
    u32 end   = resolution;// + 1;//for Catmull Rom

    u32 countControlPoints = u32( controlPoints.size( ) - 3 );

    f32 step = 1.0f/static_cast<f32>( resolution );

    for( u32 i = 0; i < countControlPoints; ++i )
    {
        t = 0.0f;
        end = resolution; 

        //if( i != 0 ) //for Catmull Rom
        //  start = 1;

        if( i == countControlPoints - 1 )
        {
            if( closed )
                break;

            end++;
        }

        //only incur the cost of copying from the STL vector once
        const Vector3& cp0 = controlPoints[ i ];
        const Vector3& cp1 = controlPoints[ i + 1 ];
        const Vector3& cp2 = controlPoints[ i + 2 ];
        const Vector3& cp3 = controlPoints[ i + 3 ];

        for( u32 j = start; j < end; ++j )
        {
            points.push_back( ComputePoint( t, cp0, cp1, cp2, cp3, Curve::kCatmullRom ) );
            t += step;
        }
    }
}