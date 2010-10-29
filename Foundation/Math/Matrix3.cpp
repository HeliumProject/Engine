#include "Matrix3.h"

#include "Matrix4.h"
#include "AngleAxis.h"
#include "EulerAngles.h"
#include "Quaternion.h"

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
    f32 ri, rj, rh;
    f32 ci, cj, ch, si, sj, sh; 
    f32 cc, cs, sc, ss;
    u32 i, j, k, h, n, s, f;
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
    f32 v1 = v.axis.x, v2 = v.axis.y, v3 = v.axis.z;
    f32 v1s = v1*v1, v2s = v2*v2, v3s = v3*v3;

    f32 cosTheta  = cos((f32)v.angle), sinTheta  = sin((f32)v.angle);

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

Matrix3::Matrix3(const Quaternion& v)
{
    (*this) = v;
}

Matrix3& Matrix3::operator=(const Quaternion& v)
{
    f32 xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

    f32 xv = v.values.x;
    f32 yv = v.values.y;
    f32 zv = v.values.z;
    f32 wv = v.values.w;

    f32 Nq = (xv * xv) + (yv * yv) + (zv * zv) + (wv * wv);
    f32 s  = (Nq > 0.0f) ? (2.0f / Nq) : 0.0f;

    xs = xv * s;  ys = yv * s;  zs = zv * s;
    wx = wv * xs; wy = wv * ys; wz = wv * zs;
    xx = xv * xs; xy = xv * ys; xz = xv * zs;
    yy = yv * ys; yz = yv * zs; zz = zv * zs;

    x[0] = 1.0f - (yy + zz);
    x[1] = xy + wz;
    x[2] = xz - wy;

    y[0] = xy - wz;
    y[1] = 1.0f - (xx + zz);
    y[2] = yz + wx;

    z[0] = xz + wy;
    z[1] = yz - wx;
    z[2] = 1.0f - (xx + yy);

    return *this;
}

Matrix3 Matrix3::RotateX(f32 theta)
{
    Matrix3 m = Identity;
    m[1][1] = m[2][2] = cos(theta);
    m[1][2] = sin(theta);
    m[2][1] = -m[1][2];
    return m;
}

Matrix3 Matrix3::RotateY(f32 theta)
{
    Matrix3 m = Identity;
    m[0][0] = m[2][2] = cos(theta);
    m[2][0] = sin(theta);
    m[0][2] = -m[2][0];
    return m;
}

Matrix3 Matrix3::RotateZ(f32 theta)
{
    Matrix3 m = Identity;
    m[0][0] = m[1][1] = cos(theta);
    m[1][0] = sin(theta);
    m[0][1] = -m[1][0];
    return m;
}