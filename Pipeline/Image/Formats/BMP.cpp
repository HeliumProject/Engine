#include "Pipeline/Image/Image.h"
#include "BMP.h"

using namespace Nocturnal;

Image* Image::LoadBMP(const void* filedata, bool convert_to_linear)
{
  BMPHeader* bmp = (BMPHeader*)((u8*)filedata+sizeof(BMPFileHeader));

  // validate the bmp
  if ((bmp->compression != BMP_COMPRESSION_RGB) || (bmp->planes  != 1))
  {
    return 0;
  }

  BMPRGB*                      palette_data;
  u8*                          bits;      // pointer to the start of the picture bits
  ColorFormat                  format;
  i32                          line_stride;
  bool                         palette;
  u32                          bitsperpixel;

  palette_data = (BMPRGB*)((u8*)bmp+(u32)(bmp->size));

  // Get the source bit depth
  switch (bmp->bitCount)
  {
  case 8:
    palette         = true;
    format          = CF_ARGB8888;
    bitsperpixel    = 8;
    bits            = ( ((u8*)bmp->colors)+((bmp->clrUsed==0?256:bmp->clrUsed)*sizeof(BMPRGB)) );
    line_stride     = (bmp->width + 3) & ~0x00000003;
    break;

  case 16:
    palette         = false;
    format          = CF_ARGB1555;
    bitsperpixel    = 16;
    bits            = ( ((u8*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = ((bmp->width*2) + 3) & ~0x00000003;       // A BMP line is always a multiple of a 32 bit dword long
    break;

  case 24:
    palette         = false;
    format          = CF_ARGB8888;
    bitsperpixel    = 32;
    bits            = ( ((u8*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = ((bmp->width*3) + 3) & ~0x00000003;
    break;

  case 32:
    palette         = false;
    format          = CF_ARGB8888;
    bitsperpixel    = 32;
    bits            = ( ((u8*)bmp->colors)+(bmp->clrUsed*sizeof(BMPRGB)) );
    line_stride     = bmp->width*4;
    break;

  default:
    return 0;
  }

  //Bottom up BMP
  u32 height = (bmp->height<0)?(u32)(-bmp->height):(u32)bmp->height;
  if (bmp->height>0)
  {
    bits += ((height-1)*line_stride);
    line_stride = -line_stride;
  }

  u32      read_width  = bmp->width;
  u32      read_height = height;
  u32      dest_stride = (read_width*(bitsperpixel/8));
  u8*      dest_mem    = new u8[read_width*read_height*4];
  u8*      dest_line   = dest_mem;

  // copy and convert the palette
  if (palette)
  {
    u32    pal_data[256];

    // Allocate the memory for the palette
    u32 p_entries = bmp->clrUsed==0?256:bmp->clrUsed;

    // copy the color entries and convert to the correct palette entry pixel format
    for (u32 ce = 0;ce<p_entries;ce++)
    {
      u32 col = *(u32*)(palette_data+ce);
      pal_data[ce] = col | 0xff000000;
    }

    // used when copying RGB 24 bit to 32 bit
    // read the source bitmap
    for (u32 y=0;y<read_height;y++)
    {
      u8* src = bits;
      for (u32 x=0;x<read_width;x++)
      {
        ((u32*)dest_line)[x] = pal_data[*src];
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
    for (u32 y=0;y<read_height;y++)
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
    for (u32 y=0;y<read_height;y++)
    {
      u8* src = bits;
      for (u32 x=0;x<read_width;x++)
      {
        ((u32*)dest_line)[x]= (*(u32*)src) | 0xff000000;
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

