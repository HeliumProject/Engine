#include "PipelinePch.h"
#include "Pipeline/Image/Image.h"
#include "BMP.h"

using namespace Helium;

Image* Image::LoadBMP(const void* filedata, bool convert_to_linear)
{
  BMPHeader* bmp = (BMPHeader*)((uint8_t*)filedata+sizeof(BMPFileHeader));

  // validate the bmp
  if ((bmp->compression != BMP_COMPRESSION_RGB) || (bmp->planes  != 1))
  {
    return 0;
  }

  BMPRGB*                      palette_data;
  uint8_t*                          bits;      // pointer to the start of the picture bits
  ColorFormat                  format;
  int32_t                          line_stride;
  bool                         palette;
  uint32_t                          bitsperpixel;

  palette_data = (BMPRGB*)((uint8_t*)bmp+(uint32_t)(bmp->size));

  // Get the source bit depth
  switch (bmp->bitCount)
  {
  case 8:
    palette         = true;
    format          = CF_ARGB8888;
    bitsperpixel    = 8;
    bits            = ( ((uint8_t*)bmp->colors)+((bmp->clrUsed==0?256:bmp->clrUsed)*sizeof(BMPRGB)) );
    line_stride     = (bmp->width + 3) & ~0x00000003;
    break;

  case 16:
    palette         = false;
    format          = CF_ARGB1555;
    bitsperpixel    = 16;
    bits            = ( ((uint8_t*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = ((bmp->width*2) + 3) & ~0x00000003;       // A BMP line is always a multiple of a 32 bit dword long
    break;

  case 24:
    palette         = false;
    format          = CF_ARGB8888;
    bitsperpixel    = 32;
    bits            = ( ((uint8_t*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = ((bmp->width*3) + 3) & ~0x00000003;
    break;

  case 32:
    palette         = false;
    format          = CF_ARGB8888;
    bitsperpixel    = 32;
    bits            = ( ((uint8_t*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = bmp->width*4;
    break;

  default:
    return 0;
  }

  //Bottom up BMP
  uint32_t height = (bmp->height<0)?(uint32_t)(-bmp->height):(uint32_t)bmp->height;
  if (bmp->height>0)
  {
    bits += ((height-1)*line_stride);
    line_stride = -line_stride;
  }

  uint32_t      read_width  = bmp->width;
  uint32_t      read_height = height;
  uint32_t      dest_stride = (read_width*(bitsperpixel/8));
  uint8_t*      dest_mem    = new uint8_t[read_width*read_height*4];
  uint8_t*      dest_line   = dest_mem;

  // copy and convert the palette
  if (palette)
  {
    uint32_t    pal_data[256];

    // Allocate the memory for the palette
    uint32_t p_entries = bmp->clrUsed==0?256:bmp->clrUsed;

    // copy the color entries and convert to the correct palette entry pixel format
    for (uint32_t ce = 0;ce<p_entries;ce++)
    {
      uint32_t col = *(uint32_t*)(palette_data+ce);
      pal_data[ce] = col | 0xff000000;
    }

    // used when copying RGB 24 bit to 32 bit
    // read the source bitmap
    for (uint32_t y=0;y<read_height;y++)
    {
      uint8_t* src = bits;
      for (uint32_t x=0;x<read_width;x++)
      {
        ((uint32_t*)dest_line)[x] = pal_data[*src];
        src++;
      }
      // mode to the next scan line
      bits += line_stride;
      dest_line += dest_stride;
    }
  }
  else if (bmp->bitCount!=24)
  {
    // read the source bitmap
    for (uint32_t y=0;y<read_height;y++)
    {
      memcpy(dest_line,bits,read_width*bmp->bitCount>>3);
      // mode to the next scan line
      bits += line_stride;
      dest_line += dest_stride;
    }
  }
  else
  {
    // used when copying RGB 24 bit to 32 bit
    // read the source bitmap
    for (uint32_t y=0;y<read_height;y++)
    {
      uint8_t* src = bits;
      for (uint32_t x=0;x<read_width;x++)
      {
        ((uint32_t*)dest_line)[x]= (*(uint32_t*)src) | 0xff000000;
        src+=3;
      }
      // mode to the next scan line
      bits += line_stride;
      dest_line += dest_stride;
    }
  }

  Image* surface = new Image(read_width,read_height,format);
  surface->FillFaceData(0, format, dest_mem);

  delete[] dest_mem;

  if(convert_to_linear)
  {
    surface->ConvertSrgbToLinear();
  }

  return surface;
}

