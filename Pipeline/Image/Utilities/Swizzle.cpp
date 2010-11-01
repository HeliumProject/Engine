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
void Helium::SwizzleBox(uint8_t* src, uint8_t* dst, uint32_t Width, uint32_t Height, uint32_t Depth, uint32_t BytesPerPixel)
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

  uint32_t RWidth = Width;
  uint32_t RHeight = Height;
  uint32_t RDepth = Depth;

  uint32_t UOffset = 0;
  uint32_t VOffset = 0;
  uint32_t WOffset = 0;

  uint32_t RowPitch = Width * BytesPerPixel;
  uint32_t SlicePitch = RowPitch * Height;

  unsigned int u, v, w;
  Swizzler swiz(Width, Height, Depth);

  uint32_t SwizU = swiz.SwizzleU(UOffset);
  uint32_t SwizV = swiz.SwizzleV(VOffset);
  uint32_t SwizW = swiz.SwizzleW(WOffset);

  swiz.SetW(SwizW);

  uint32_t RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
  uint32_t SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

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
void Helium::UnswizzleBox(uint8_t* src, uint8_t* dst, uint32_t Width, uint32_t Height, uint32_t Depth, uint32_t BytesPerPixel)
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

  uint32_t RWidth=Width;
  uint32_t RHeight=Height;
  uint32_t RDepth=Depth;

  uint32_t UOffset=0;
  uint32_t VOffset=0;
  uint32_t WOffset=0;

  uint32_t RowPitch = Width * BytesPerPixel;
  uint32_t SlicePitch = RowPitch * Height;

  unsigned int u, v, w;
  Swizzler swiz(Width, Height, Depth);

  uint32_t SwizU = swiz.SwizzleU(UOffset);
  uint32_t SwizV = swiz.SwizzleV(VOffset);
  uint32_t SwizW = swiz.SwizzleW(WOffset);

  swiz.SetW(SwizW);

  uint32_t RowPitchAdjust = RowPitch - RWidth * BytesPerPixel;
  uint32_t SlicePitchAdjust = SlicePitch - RHeight * RowPitch;

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
