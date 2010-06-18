#pragma once

#include "Platform.h"
#include <d3dx9.h>

extern const u8 u16_to_rgb_map[];

inline u32 StringHashDJB2( const char* str )
{
  u32 hash = 5381;
  int c;

  while (c = *str++)
  {
    hash = hash * 33 ^ c;
  }

  return hash;
}

inline u32 HashU16ToRGB(u16 index)
{  
  u32 color = 0xff;
  for (u32 i = 0; i < 16; ++i)
  {
    u32 bit = (index >> i) & 0x1;
    color |= bit << u16_to_rgb_map[i];
  }
  return color;
}

inline void HashU16ToRGB(u16 index, float& r, float& g, float& b, float& a)
{
  u32 color = HashU16ToRGB(index);
  r = ((color >> 24) & 0xFF) * (1.0f / 255.0f);
  g = ((color >> 16) & 0xFF) * (1.0f / 255.0f);
  b = ((color >> 8)  & 0xFF) * (1.0f / 255.0f);
  a = 1.0f;
}

inline u16 HashRGBToU16(u32 color)
{
  u16 index = 0;
  for (u32 i = 0; i < 16; i++)
  {
    u32 bit = (color >> u16_to_rgb_map[i]) & 0x1;
    index |= bit << i;
  }
  return index;
}


// extract the world space frustum corners from a viewproj mat
void ExtractFromViewProj( D3DXVECTOR4 &center_of_projection, D3DXVECTOR4 *frustum_vertices, D3DXMATRIX &viewproj_matrix );


