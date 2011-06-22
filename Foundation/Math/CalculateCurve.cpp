#include "FoundationPch.h"
#include "CalculateCurve.h"
#include "Matrix4.h"

using namespace Helium;

const static Matrix4 CatmullRomBasis ( Vector4 (0.0f, -1.0f,  2.0f, -1.0f) / 2.0f,
                                      Vector4 (2.0f,  0.0f, -5.0f,  3.0f) / 2.0f,
                                      Vector4 (0.0f,  1.0f,  4.0f, -3.0f) / 2.0f,
                                      Vector4 (0.0f,  0.0f, -1.0f,  1.0f) / 2.0f );

const static Matrix4 BSplineBasis ( Vector4 (-1.0f,  3.0f, -3.0f,  1.0f) / 6.0f,
                                   Vector4 ( 3.0f, -6.0f,  0.0f,  4.0f) / 6.0f,
                                   Vector4 (-3.0f,  3.0f,  3.0f,  1.0f) / 6.0f,
                                   Vector4 ( 1.0f,  0.0f,  0.0f,  0.0f) / 6.0f );

// computes the vector parameter factor
static Vector4 ComputeParam(float32_t t, const CurveGenerator::Type type);

// retrieves the basis matrix for curve type
static const Matrix4& ComputeBasis( const CurveGenerator::Type type );

// computes a curve point
static Vector3 ComputePoint(float32_t param, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const CurveGenerator::Type type );

// compputes B and Catmull Rom splines, respectively
static void ComputeBSpline( V_Vector3& controlPoints, const uint32_t resolution, const bool closed, V_Vector3& points  );
static void ComputeCatmullRom( V_Vector3& controlPoints, const uint32_t resolution, const bool closed, V_Vector3& points  );

static void MakeContinuous(V_Vector3& cvs);
static void MakeClosed(V_Vector3& cvs);

bool CurveGenerator::ComputeCurve( const V_Vector3& controlPoints, const uint32_t resolution, const bool closed, const Type type, V_Vector3& points )
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
    case CurveGenerator::kLinear:
        {
            points = tempControlPoints;
            break;
        }

    case CurveGenerator::kBSpline:
        {
            ComputeBSpline( tempControlPoints, resolution, closed, points );
            break;
        }

    case CurveGenerator::kCatmullRom:
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

Vector4 ComputeParam(float32_t t, const CurveGenerator::Type type )
{
    switch (type)
    {
    case CurveGenerator::kLinear:
        break;

    case CurveGenerator::kBSpline:
        return Vector4 (t*t*t, t*t, t, 1);
        break;

    case CurveGenerator::kCatmullRom:
        return Vector4 (1, t, t*t, t*t*t);
        break;

    default:
        HELIUM_ASSERT(false);
    }

    return Vector4::Zero;
}

const Matrix4& ComputeBasis( const CurveGenerator::Type type )
{
    switch (type)
    {
    case CurveGenerator::kLinear:
        break;

    case CurveGenerator::kBSpline:
        return BSplineBasis;
        break;

    case CurveGenerator::kCatmullRom:
        return CatmullRomBasis;
        break;

    default:
        HELIUM_ASSERT(false);
    }

    return Matrix4::Zero;
}

Vector3 ComputePoint(float32_t param, const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const CurveGenerator::Type type )
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



void ComputeBSpline( V_Vector3& controlPoints, const uint32_t resolution, const bool closed, V_Vector3& points  )
{
    float32_t t = 0.0f;
    uint32_t start = 0;
    uint32_t end   = resolution;

    uint32_t countControlPoints = (uint32_t)( controlPoints.size( ) - 3 );

    float32_t step = 1.0f/static_cast<float32_t>( resolution );

    for( uint32_t i = 0; i < countControlPoints; ++i )
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

        for( uint32_t j = start; j < end; ++j )
        {
            points.push_back( ComputePoint( t, cp0, cp1, cp2, cp3, CurveGenerator::kBSpline ) );
            t += step;
        }
    }
}

void ComputeCatmullRom( V_Vector3& controlPoints, const uint32_t resolution, const bool closed, V_Vector3& points  )
{
    float32_t t = 0.0f;
    uint32_t start = 0;
    uint32_t end   = resolution;// + 1;//for Catmull Rom

    uint32_t countControlPoints = uint32_t( controlPoints.size( ) - 3 );

    float32_t step = 1.0f/static_cast<float32_t>( resolution );

    for( uint32_t i = 0; i < countControlPoints; ++i )
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

        for( uint32_t j = start; j < end; ++j )
        {
            points.push_back( ComputePoint( t, cp0, cp1, cp2, cp3, CurveGenerator::kCatmullRom ) );
            t += step;
        }
    }
}