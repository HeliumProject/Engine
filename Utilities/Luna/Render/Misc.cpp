#include "Precompile.h"
#include "Misc.h"

const u8 u16_to_rgb_map[] = { 15, 23, 31, 14, 22, 30, 13, 21, 29, 12, 20, 28, 11, 19, 27, 10 };

void ExtractFromViewProj( D3DXVECTOR4 &center_of_projection, D3DXVECTOR4 *frustum_vertices, D3DXMATRIX &viewproj_matrix )
{
  D3DXMATRIX Pinv,V,Vinv,Minv;
  D3DXMATRIX &M = viewproj_matrix;

  // get the elements of the view proj matrix (_YX ordered)
  float m00 = M._11;
  float m01 = M._12;
  float m02 = M._13;
  float m03 = M._14;

  float m10 = M._21;
  float m11 = M._22;
  float m12 = M._23;
  float m13 = M._24;

  float m20 = M._31;
  float m21 = M._32;
  float m22 = M._33;
  float m23 = M._34;

  float m32 = M._43;
  float m33 = M._44;

  // use biggest element out of m03,m13,m23 for accuracy
  float mk2=m02, mk3=m03;
  if (m13*m13 > mk3*mk3)
  {
    mk2=m12, mk3=m13;
  }
  if (m23*m23 > mk3*mk3)
  {
    mk2=m22, mk3=m23;
  }

  // some helper vars
  float p = mk2*m33 - mk3*m32;
  float q = 1.0f / p;
  float n = p / mk2;
  float f = p / (mk2 - mk3);

  float s00 = m00*m00 + m10*m10 + m20*m20;
  float s11 = m01*m01 + m11*m11 + m21*m21;
  float s30 = m03*m00 + m13*m10 + m23*m20;
  float s31 = m03*m01 + m13*m11 + m23*m21;

  float rx = 1.0f/sqrt(s00 - s30*s30);
  float ry = 1.0f/sqrt(s11 - s31*s31);

  // reconstruct inverse of proj matrix
  Pinv = D3DXMATRIX(   -rx,    0.0f, 0.0f,   0.0f,
             0.0f,      ry, 0.0f,   0.0f,
             0.0f,    0.0f, 0.0f, -mk3*q,
           s30*rx, -s31*ry, 1.0f,  mk2*q);

  // calculate view matrix
  V=M*Pinv;

  // invert view matrix
  float det;
  D3DXMatrixInverse(&Vinv,&det,&V);

  // invert view proj matrix
  Minv = Pinv*Vinv;
  
  // transform points from clip space back to world space
  D3DXVECTOR4 aa = D3DXVECTOR4(0, 0, -p/mk3, 0);
  D3DXVECTOR4 bb = D3DXVECTOR4(-n, -n, 0, n);
  D3DXVECTOR4 cc = D3DXVECTOR4( n, -n, 0, n);
  D3DXVECTOR4 dd = D3DXVECTOR4(-n,  n, 0, n);
  D3DXVECTOR4 ee = D3DXVECTOR4( n,  n, 0, n);
  D3DXVECTOR4 ff = D3DXVECTOR4(-f, -f, f, f);
  D3DXVECTOR4 gg = D3DXVECTOR4( f, -f, f, f);
  D3DXVECTOR4 hh = D3DXVECTOR4(-f,  f, f, f);
  D3DXVECTOR4 ii = D3DXVECTOR4( f,  f, f, f);

  D3DXVec4Transform(&center_of_projection, &aa,&Minv);
  D3DXVec4Transform(&frustum_vertices[0], &bb ,&Minv);
  D3DXVec4Transform(&frustum_vertices[1], &cc ,&Minv);
  D3DXVec4Transform(&frustum_vertices[2], &dd ,&Minv);
  D3DXVec4Transform(&frustum_vertices[3], &ee ,&Minv);
  D3DXVec4Transform(&frustum_vertices[4], &ff ,&Minv);
  D3DXVec4Transform(&frustum_vertices[5], &gg ,&Minv);
  D3DXVec4Transform(&frustum_vertices[6], &hh ,&Minv);
  D3DXVec4Transform(&frustum_vertices[7], &ii ,&Minv);
}
