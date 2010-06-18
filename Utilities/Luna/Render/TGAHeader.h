#pragma once

#include "Platform/Types.h"

#pragma pack(push,1)
namespace Render
{
  struct TGAHeader
  {
      u8  idLength;
      u8  colormapType;
      u8  imageType;

      u16 colorMapIndex;
      u16 colorMapLength;
      u8  colorMapBits;

      u16 xOrigin;
      u16 yOrigin;
      u16 width;
      u16 height;
      u8  pixelDepth;
      u8  imageDescriptor;
  };
}
#pragma pack(pop)