#pragma once

#include "Platform/Types.h"

namespace Helium
{
  #pragma pack(push,1)
  struct TGAHeader
  {
      uint8_t  idLength;
      uint8_t  colormapType;
      uint8_t  imageType;

      uint16_t colorMapIndex;
      uint16_t colorMapLength;
      uint8_t  colorMapBits;

      uint16_t xOrigin;
      uint16_t yOrigin;
      uint16_t width;
      uint16_t height;
      uint8_t  pixelDepth;
      uint8_t  imageDescriptor;
  };
  #pragma pack(pop)
}