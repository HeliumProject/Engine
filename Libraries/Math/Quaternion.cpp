#include "Quaternion.h"

#include "EulerAngles.h"
#include "AngleAxis.h"
#include "Matrix3.h"

using namespace Math;

const Quaternion Quaternion::Zero ( 0.0f, 0.0f, 0.0f, 0.0f );
const Quaternion Quaternion::Identity ( 0.0f, 0.0f, 0.0f, 1.0f );

Quaternion::Quaternion(const Matrix3& v)
{
  (*this) = v;
}

Quaternion::Quaternion(const Matrix4& v)
{
  (*this) = v;
}

Quaternion Quaternion::operator- () const
{
  return Quaternion( -values.x, -values.y, -values.z, -values.w );
}

Quaternion& Quaternion::operator=(const Matrix3& m)
{
  static i32 next[3] = { QY,QZ,QX };
  Quaternion &q = *this;
  
  f32 tr,s;
  register i32 i,j,k;

  tr = m[0][0] + m[1][1] + m[2][2];
    
  if (tr > 0.0)
  {
    s = (f32)sqrt(tr + 1.0f);
    q[QW] = s * 0.5f;

    s = 0.5f / s;
    q[QX] = (m[1][2] - m[2][1]) * s;
    q[QY] = (m[2][0] - m[0][2]) * s;
    q[QZ] = (m[0][1] - m[1][0]) * s;
  }
  else
  {
    i = QX;
    
    if (m[QY][QY] > m[QX][QX])
      i = QY;

    if (m[QZ][QZ] > m[i][i])
      i = QZ;
    
    j = next[i];
    k = next[j];
      
    s = sqrt((f32) ( (m[i][i] - (m[j][j]+m[k][k])) + 1.0) );
    q[i] = s * 0.5f;

    s = 0.5f / s;
    q[QW] = (m[j][k] - m[k][j]) * s;
    q[j] = (m[i][j] + m[j][i]) * s;
    q[k] = (m[i][k] + m[k][i]) * s;
  }
  
  return *this;
}

Quaternion& Quaternion::operator=(const Matrix4& m)
{
  return operator=(Matrix3 (m));
}

Quaternion::Quaternion(const EulerAngles& v)
{
  (*this) = v;
}

Quaternion& Quaternion::operator=(const EulerAngles& v)
{
  Vector3  a;
  f32 ti, tj, th, ci, cj, ch, si, sj, sh, cc, cs, sc, ss;
  u32 i, j, k, h, n, s, f;

  EulGetOrd(v.order.o,i,j,k,h,n,s,f);

  if (f == EulFrmR) 
  {
    ti = (f32)v.angles.z * 0.5f; 
    tj = (f32)v.angles.y * 0.5f; 
    th = (f32)v.angles.x * 0.5f;
  }
  else
  {
    ti = (f32)v.angles.x * 0.5f; 
    tj = (f32)v.angles.y * 0.5f; 
    th = (f32)v.angles.z * 0.5f;
  }

  if (n==EulParOdd) 
    tj = -tj;

  ci = cos(ti); cj = cos(tj); ch = cos(th);
  si = sin(ti); sj = sin(tj); sh = sin(th);
  cc = ci*ch;   cs = ci*sh;   
  sc = si*ch;   ss = si*sh;

  if (s==EulRepYes)
  {
    a[i] = (cj*(cs + sc));
    a[j] = (sj*(cc + ss));
    a[k] = (sj*(cs - sc));
    values.w = (cj*(cc - ss));
  } 
  else 
  {
    a[i] = (cj*sc - sj*cs);
    a[j] = (cj*ss + sj*cc);
    a[k] = (cj*cs - sj*sc);
    values.w = (cj*cc + sj*ss);
  }

  if (n==EulParOdd) a[j] = -a[j];

  values.x = a[QX];
  values.y = a[QY];
  values.z = a[QZ];

  return *this;
}

Quaternion::Quaternion(const AngleAxis& v)
{
  (*this) = v;
}

Quaternion& Quaternion::operator=(const AngleAxis& v)
{
  f32 theta = ((f32)v.angle)/2.0f;
  f32 sinTheta = sin(theta);
  Vector3 axis(v.axis);

  axis.Normalize();
  
  values.x = axis.x * sinTheta;
  values.y = axis.y * sinTheta;
  values.z = axis.z * sinTheta;
  values.w = cos(theta);

  return *this;
}