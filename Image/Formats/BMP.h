#pragma once

#include "Platform/Types.h"

namespace Helium
{
  const uint32_t BMP_COMPRESSION_RGB       = 0;
  const uint32_t BMP_COMPRESSION_RLE8      = 1;
  const uint32_t BMP_COMPRESSION_RLE4      = 2;
  const uint32_t BMP_COMPRESSION_BITFIELDS = 3;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // NOTE: The RGB order of a bitmap is different to the ARGB8888 order we use all over the place
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  struct BMPRGB
  {
    uint8_t   blue;
    uint8_t   green;
    uint8_t   red;
    uint8_t   unused;
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  struct BMPHeader
  {
      uint32_t     size;                      // Size of the structure up until the palette info
      int32_t     width; 
      int32_t     height;                    // negative height for top down BMPs
      uint16_t     planes;
      uint16_t     bitCount;
      uint32_t     compression;
      uint32_t     sizeImage;
      int32_t     xPelsPerMeter;
      int32_t     yPelsPerMeter;
      uint32_t     clrUsed;
      uint32_t     clrImportant;
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
      uint16_t   type;
      uint32_t   size;
      uint16_t   reserved1;
      uint16_t   reserved2;
      uint32_t   offset;
  };
  #pragma pack(pop)
}


