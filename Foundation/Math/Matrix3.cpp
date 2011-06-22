#include "FoundationPch.h"
#include "Matrix3.h"

#include "Matrix4.h"
#include "AngleAxis.h"
#include "EulerAngles.h"

using namespace Helium;

const Matrix3 Matrix3::Identity (Vector3 (1, 0, 0), Vector3 (0, 1, 0), Vector3 (0, 0, 1));
const Matrix3 Matrix3::Zero (Vector3 (0, 0, 0), Vector3 (0, 0, 0), Vector3 (0, 0, 0));

Matrix3::Matrix3(const Matrix4& v)
{
    (*this) = v;
}

Matrix3& Matrix3::operator=(const Matrix4& v)
{
    x = Vector3(v.x.x, v.x.y, v.x.z);
    y = Vector3(v.y.x, v.y.y, v.y.z);
    z = Vector3(v.z.x, v.z.y, v.z.z);

    return *this;
}

Matrix3::Matrix3(const EulerAngles& v)
{
    (*this) = v;
}

Matrix3& Matrix3::operator=(const EulerAngles& v)
{
    float32_t ri, rj, rh;
    float32_t ci, cj, ch, si, sj, sh; 
    float32_t cc, cs, sc, ss;
    uint32_t i, j, k, h, n, s, f;
    Matrix3 &m = *this;

    EulGetOrd(v.order.o,i,j,k,h,n,s,f);

    if (f == EulFrmR) 
    {
        ri = v.angles.z; 
        rj = v.angles.y; 
        rh = v.angles.x;
    }
    else
    {
        ri = v.angles.x; 
        rj = v.angles.y; 
        rh = v.angles.z;
    }

    if (n == EulParOdd) 
    {
        ri = -ri; 
        rj = -rj; 
        rh = -rh;
    }

    ci = cos(ri);  cj = cos(rj);  ch = cos(rh);
    si = sin(ri);  sj = sin(rj);  sh = sin(rh);

    cc = ci*ch;     
    cs = ci*sh; 
    sc = si*ch; 
    ss = si*sh;

    if (s == EulRepYes) 
    {
        m[i][i] = cj;     m[j][i] =  sj*si;    m[k][i] =  sj*ci;
        m[i][j] = sj*sh;  m[j][j] = -cj*ss+cc; m[k][j] = -cj*cs-sc;
        m[i][k] = -sj*ch; m[j][k] =  cj*sc+cs; m[k][k] =  cj*cc-ss;
    } 
    else 
    {
        m[i][i] = cj*ch; m[j][i] = sj*sc-cs; m[k][i] = sj*cc+ss;
        m[i][j] = cj*sh; m[j][j] = sj*ss+cc; m[k][j] = sj*cs-sc;
        m[i][k] = -sj;   m[j][k] = cj*si;    m[k][k] = cj*ci;
    }

    return *this;
}

Matrix3::Matrix3(const AngleAxis& v)
{
    (*this) = v;
}

Matrix3& Matrix3::operator=(const AngleAxis& v)
{
    float32_t v1 = v.axis.x, v2 = v.axis.y, v3 = v.axis.z;
    float32_t v1s = v1*v1, v2s = v2*v2, v3s = v3*v3;

    float32_t cosTheta  = cos((float32_t)v.angle), sinTheta  = sin((float32_t)v.angle);

    x[0] = v1s + ((1.0f-v1s) * cosTheta);
    x[1] = ((v1*v2) * (1.0f-cosTheta))  + (v3*sinTheta);
    x[2] = ((v1*v3) * (1.0f-cosTheta))  - (v2*sinTheta);

    y[0] = ((v1*v2) * (1.0f-cosTheta))  - (v3*sinTheta);
    y[1] = v2s + ((1.0f-v2s) * cosTheta);
    y[2] = ((v2*v3) * (1.0f-cosTheta))  + (v1*sinTheta);

    z[0] = ((v1*v3) * (1.0f-cosTheta))  + (v2*sinTheta);
    z[1] = ((v2*v3) * (1.0f-cosTheta))  - (v1*sinTheta);
    z[2] = v3s + ((1.0f-v3s) * cosTheta);

    return *this;
}

Matrix3 Matrix3::RotateX( float32_t theta )
{
    Matrix3 m = Identity;
    m[1][1] = m[2][2] = cos(theta);
    m[1][2] = sin(theta);
    m[2][1] = -m[1][2];
    return m;
}

Matrix3 Matrix3::RotateY(float32_t theta)
{
    Matrix3 m = Identity;
    m[0][0] = m[2][2] = cos(theta);
    m[2][0] = sin(theta);
    m[0][2] = -m[2][0];
    return m;
}

Matrix3 Matrix3::RotateZ(float32_t theta)
{
    Matrix3 m = Identity;
    m[0][0] = m[1][1] = cos(theta);
    m[1][0] = sin(theta);
    m[0][1] = -m[1][0];
    return m;
}