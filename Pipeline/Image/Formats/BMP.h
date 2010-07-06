#pragma once

#include "Platform/Types.h"

namespace Nocturnal
{
  const u32 BMP_COMPRESSION_RGB       = 0;
  const u32 BMP_COMPRESSION_RLE8      = 1;
  const u32 BMP_COMPRESSION_RLE4      = 2;
  const u32 BMP_COMPRESSION_BITFIELDS = 3;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // NOTE: The RGB order of a bitmap is different to the ARGB8888 order we use all over the place
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  struct BMPRGB
  {
    u8   blue;
    u8   green;
    u8   red;
    u8   unused;
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  struct BMPHeader
  {
      u32     size;                      // Size of the structure up until the palette info
      i32     width; 
      i32     height;                    // negative height for top down BMPs
      u16     planes;
      u16     bitCount;
      u32     compression;
      u32     sizeImage;
      i32     xPelsPerMeter;
      i32     yPelsPerMeter;
      u32     clrUsed;
      u32     clrImportant;
      BMPRGB  colors[1];
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // We need to change the alignment of this structure because it starts with a
  // 16 bit value followed by a 32 bit value.
  // This structure is directly before the BMPHeader structure above when the
  // BMP file is on disk. The m_Offset member of this structure is the offset
  // to the bits of the picture
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  #pragma pack(push,2)
  struct BMPFileHeader
  {
      u16   type;
      u32   size;
      u16   reserved1;
      u16   reserved2;
      u32   offset;
  };
  #pragma pack(pop)
}


