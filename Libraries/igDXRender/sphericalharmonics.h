////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Spherical Harmionic Helpers
//
////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "platform.h"
#include "d3dx9.h"

#define fconst1 (4.0f/17.0f)
#define fconst2 (8.0f/17.0f)
#define fconst3 (15.0f/17.0f)
#define fconst4 (5.0f/68.0f)
#define fconst5 (15.0f/68.0f)

////////////////////////////////////////////////////////////////////////////////////////////////
inline void AddLightToSH(D3DXVECTOR4* sh, const D3DXVECTOR4& col, const D3DXVECTOR4& dirn)
{
  sh[0]+=(col*fconst1);
  sh[1]+=(col*fconst2*dirn.x);
  sh[2]+=(col*fconst2*dirn.y);
  sh[3]+=(col*fconst2*dirn.z);
  sh[4]+=(col*fconst3*(dirn.x*dirn.z));
  sh[5]+=(col*fconst3*(dirn.z*dirn.y));
  sh[6]+=(col*fconst3*(dirn.y*dirn.x));
  sh[7]+=(col*fconst4*(3.0f * dirn.z * dirn.z - 1.0f));
  sh[8]+=(col*fconst5 * (dirn.x * dirn.x - dirn.y * dirn.y));
}


////////////////////////////////////////////////////////////////////////////////////////////////
inline void AddAmbientToSH(D3DXVECTOR4* sh, const D3DXVECTOR4& col)
{
  sh[0]+=col;
}

////////////////////////////////////////////////////////////////////////////////////////////////
inline void FinalizeSH(D3DXVECTOR4* sh)
{  
  // To compute the lighting the following is used with constant 7:
  //
  // col += sh[7] * (3.0f * norm.z*norm.z - 1.0f);
  //
  // this can be simplied to  'col += sh[7] * norm.z*norm.z;'
  // by adjusting constant 0 and constant 7 before uploading to the gpu.
  // The optimized GPU code to compute the final color looks like this:
  // half3 norm2 = ws_normal*ws_normal;
  // half3 normd = ws_normal.xzy*ws_normal.zyx;
  // half3 col = g_sh0;
  // col += g_sh1 * ws_normal.x;
  // col += g_sh2 * ws_normal.y;
  // col += g_sh3 * ws_normal.z;
  // col += g_sh4 * normd.x;
  // col += g_sh5 * normd.y;
  // col += g_sh6 * normd.z;  
  // col += g_sh7 * norm2.z;
  // col += g_sh8 * (norm2.x - norm2.y);	

  //
  // NOTE: once this has been called you cannot accumulate more lights

  sh[0]=sh[0]-sh[7];
  sh[7]*=3.0f;
}


#undef fconst1
#undef fconst2
#undef fconst3
#undef fconst4
#undef fconst5


