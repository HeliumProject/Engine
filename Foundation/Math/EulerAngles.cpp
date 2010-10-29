#include "EulerAngles.h"

#include "Matrix3.h"

using namespace Helium::Math;

const EulerAngles EulerAngles::Zero ( 0, 0, 0 );

EulerAngles::EulerAngles(const Matrix3& v)
{
    order = EulerOrders::XYZs;
    (*this) = v;
}

EulerAngles::EulerAngles(const Matrix3& v, const EulerOrder& o)
{
    order = o; (*this) = v;
}

EulerAngles& EulerAngles::operator=(const Matrix3& m)
{
    uint32_t i, j, k, h, n, s, f;
    float32_t ri, rj, rh;
    float32_t sy, cy;

    EulGetOrd (order.o,i,j,k,h,n,s,f);

    if (s == EulRepYes) 
    {
        sy = sqrt( (float32_t) (m[j][i]*m[j][i] + m[k][i]*m[k][i]) );

        if (sy > 16*FLT_EPSILON)
        {
            ri = atan2((float32_t)m[j][i],  (float32_t)m[k][i]);
            rj = atan2(sy, (float32_t)m[i][i]);
            rh = atan2((float32_t)m[i][j],  (float32_t)-m[i][k]);
        } 
        else
        {
            ri = atan2((float32_t)-m[k][j], (float32_t)m[j][j]);
            rj = atan2(sy, (float32_t)m[i][i]);
            rh = 0;
        }
    } 
    else 
    {
        cy = sqrt( (float32_t) (m[i][i]*m[i][i] + m[i][j]*m[i][j]) );

        if (cy > 16*FLT_EPSILON) 
        {
            ri = atan2((float32_t)m[j][k], (float32_t)m[k][k]);
            rj = atan2((float32_t)-m[i][k], cy);
            rh = atan2((float32_t)m[i][j], (float32_t)m[i][i]);
        } 
        else 
        {
            ri = atan2((float32_t)-m[k][j], (float32_t)m[j][j]);
            rj = atan2((float32_t)-m[i][k], cy);
            rh = 0;
        }
    }

    if (n == EulParOdd) 
    {
        ri = -ri; 
        rj = -rj;
        rh = -rh;
    }

    // Keep within [-PI, PI]
    ClampAngle(ri);
    ClampAngle(rj);
    ClampAngle(rh);

    if (f == EulFrmR) 
    {
        angles.x = rh;
        angles.y = rj;
        angles.z = ri;
    } 
    else
    {
        angles.x = ri;
        angles.y = rj;
        angles.z = rh;
    }

    return *this;
}

EulerAngles::EulerAngles(const Matrix4& v)
{
    (*this) = v;
}

EulerAngles::EulerAngles(const Matrix4& v, const EulerOrder& o)
{
    order = o; (*this) = v;
}

EulerAngles& EulerAngles::operator=(const Matrix4& m)
{
    Matrix3 matrix (m);
    return this->operator=(matrix);
}

EulerAngles::EulerAngles(const AngleAxis& v)
{
    order = EulerOrders::XYZs;
    (*this) = v;
}

EulerAngles::EulerAngles(const AngleAxis& v, const EulerOrder& o)
{
    order = o;
    (*this) = v;
}

EulerAngles& EulerAngles::operator=(const AngleAxis& v)
{
    return *this = Matrix3(v);
}

EulerAngles::EulerAngles(const Quaternion& v)
{
    order = EulerOrders::XYZs;
    (*this) = v;
}

EulerAngles::EulerAngles(const Quaternion& v, const EulerOrder& o)
{
    order = o;
    (*this) = v;
}

EulerAngles& EulerAngles::operator=(const Quaternion& v)
{
    return *this = Matrix3(v);
}

EulerAngles& EulerAngles::Reorder(EulerOrder v)
{
    Matrix3 m (*this);
    order = v;
    return *this = m;
}
