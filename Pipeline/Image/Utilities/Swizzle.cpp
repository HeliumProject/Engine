////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Swizzle.cpp
//
//  Written by: Rob Wyatt
//
//  General swizzle code for any bit depth, this can be made much faster but as this it tools code
//  its not important. If we need to swizzle on the console then this code should NOT be used, it
//  is very hard on the cache.
//
////////////////////////////////////////////////////////////////////////////////////////////////

#include "swizzle.h"

////////////////////////////////////////////////////////////////////////////////////////////////
void Helium::SwizzleBox(u8* src, u8* dst, u32 Width, u32 Height, u32 Depth, u32 BytesPerPixel)
{
  //PERF: swizzling/unswizzling 3D when Width==1 || Height==1 || Depth==1 is the same as 2d 
  //PERF: if 2 of the following are true: Width==2, Height==1, Depth==1: use memcpy

  if (BytesPerPixel==8)
  {
    BytesPerPixel = 4;
    Width*=2;
  }
  else if (BytesPerPixel==16)
  {
    BytesPerPixel = 4;
    Width*=4;
  }

  u32 RWidth = Width;
  u32 RHeight = Height;
  u32 RDepth = Depth;

  u32 UOffset = 0;
  u32 VOffset = 0;
  u32 WOffset = 0;

  u32 RowPitch = Width * BytesPerPixel;
  u32 SlicePitch = RowPitch * Height;

  unsigned int u, v, w;
  Swizzler swiz(Width, Height, Depth);

  u32 SwizU = swiz.SwizzleU(UOffset);
  u32 SwizV = swiz.SwizzleV(VOffset);
  u32 SwizW = swiz.SwizzleW(WOffset);

  swiz.SetW(SwizW);

  u32 RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
  u32 SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

  for (w = RDepth; w--;) 
  {
    swiz.SetV(SwizV);

    for (v = RHeight; v--;) 
    {
      swiz.SetU(SwizU);

      for (u = RWidth; u--;) 
      {
        memcpy(dst+(swiz.Get3D()*BytesPerPixel), src, BytesPerPixel);
        src+=BytesPerPixel;
        swiz.IncU();
      }

      src += RowPitchAdjust;
      swiz.IncV();
    }

    src += SlicePitchAdjust;
    swiz.IncW();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Unswizzle a box from a texture into a buffer.
void Helium::UnswizzleBox(u8* src, u8* dst, u32 Width, u32 Height, u32 Depth, u32 BytesPerPixel)
{
  //swizzling/unswizzling 3D when Width==1 || Height==1 || Depth==1 is the same as 2d 
  //if 2 of the following are true: Width==2, Height==1, Depth==1: use memcpy

  if (BytesPerPixel==8)
  {
    BytesPerPixel = 4;
    Width*=2;
  }
  else if (BytesPerPixel==16)
  {
    BytesPerPixel = 4;
    Width*=4;
  }

  u32 RWidth=Width;
  u32 RHeight=Height;
  u32 RDepth=Depth;

  u32 UOffset=0;
  u32 VOffset=0;
  u32 WOffset=0;

  u32 RowPitch = Width * BytesPerPixel;
  u32 SlicePitch = RowPitch * Height;

  unsigned int u, v, w;
  Swizzler swiz(Width, Height, Depth);

  u32 SwizU = swiz.SwizzleU(UOffset);
  u32 SwizV = swiz.SwizzleV(VOffset);
  u32 SwizW = swiz.SwizzleW(WOffset);

  swiz.SetW(SwizW);

  u32 RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
  u32 SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

  for (w = RDepth; w--;) 
  {
    swiz.SetV(SwizV);

    for (v = RHeight; v--;) 
    {
      swiz.SetU(SwizU);

      for (u = RWidth; u--;) 
      {
        memcpy(dst,src+(swiz.Get3D()*BytesPerPixel),BytesPerPixel);
        dst+=BytesPerPixel;
        swiz.IncU();
      }

      dst += RowPitchAdjust;
      swiz.IncV();
    }

    dst += SlicePitchAdjust;
    swiz.IncW();
  }
}
