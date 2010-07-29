#include "Pipeline/Image/Image.h"
#include "TGA.h"

using namespace Helium;

//
// see http://astronomy.swin.edu.au/~pbourke/dataformats/tga/
//
Image* Image::LoadTGA(const void* filedata, bool convert_to_linear)
{
  // Validate header.  TGA files don't seem to have any sort of magic number
  // to identify them.  Therefore, we will proceed as if this is a real TGA
  // file, until we see something we don't understand.

  // Now load the TGA into the surface
  TGAHeader* tga    = (TGAHeader*)filedata;
  u8* data = (u8*)filedata;

  if( tga->colormapType & ~0x01 )
  {
    return 0;
  }

  if( tga->imageType & ~0x0b )
  {
    return 0;
  }

  if( !tga->width || !tga->height )
  {
    return 0;
  }

  // Assume it is a good TGA
  bool                palette=false;
  u32                 palette_data[256];
  ColorFormat         pixelfmt=CF_ARGB8888;
  ColorFormat         palfmt;

  switch(tga->imageType & 0x03)
  {
  case 1:         // paletted map
    if(!tga->colormapType)
    {
      return 0;
    }

    switch(tga->pixelDepth)
    {
    case 8:  // 8 bit palletted tga
      if(tga->colormapType)
      {
        switch(tga->colorMapBits)
        {
        case 15:
        case 16:
          palfmt = CF_ARGB1555;
          break;

        case 24:
        case 32:
          palfmt = CF_ARGB8888;
          break;

        default:
          return false;
        }
        palette = true;
      }
      break;

    default:
      // NON 8 BIT PALETTE
      return 0;
    }
    break;

  case 2:         // rgb
    palette = false;
    switch(tga->pixelDepth)
    {
    case 15:
    case 16:
      pixelfmt = CF_ARGB1555;
      break;

    case 24:
    case 32:
      pixelfmt = CF_ARGB8888;
      break;

    default:
      return 0;
    }
    break;

  case 3:
    switch(tga->pixelDepth)
    {
    case 15:
    case 16:
      pixelfmt = CF_ARGB1555;
      break;

    case 24:
    case 32:
      pixelfmt = CF_ARGB8888;
      break;

    case 8:
      pixelfmt = CF_L8;
    }
    break;

  default:
    return 0;
  }

  u32 color_map_bytes;
  u32 file_pixel_bytes;

  if (pixelfmt==CF_L8) // gray TGAs do not set nice numbers in the header of the gray foramt
  {
    color_map_bytes = 0;
    file_pixel_bytes = 1;
  }
  else
  {
    color_map_bytes = ((u32) tga->colorMapBits + 7) >> 3;
    file_pixel_bytes = ((u32) tga->pixelDepth + 7) >> 3;
  }

  // i32 line_stride     = tga->width * file_pixel_bytes;

  bool rle           = (tga->imageType & 0x08) != 0;
  bool top_to_bottom = 0x20 == (tga->imageDescriptor & 0x20);
  bool left_to_right = 0x10 != (tga->imageDescriptor & 0x10);

  data += sizeof(TGAHeader);


  data += tga->idLength;

  // Color map
  u32 color_map_size = (u32) tga->colorMapLength * color_map_bytes;

  if (palette)
  {
    if(tga->colorMapIndex + tga->colorMapLength > 256)
    {
      return 0;
    }

    u8 *pb = data;
    u32 *pColor = palette_data + tga->colorMapIndex;
    u32 *pColorLim = pColor + tga->colorMapLength;

    while(pColor < pColorLim)
    {
      u32 u=0xff00ff00;

      switch(tga->colorMapBits)
      {
      case 15:
        u = *((u16*) pb);     // need to convert 16bit color to 32 bit
        pb += 2;
        {
          u32 r;
          u32 g;
          u32 b;

          // 555 RGB, set dest alpha to 0xff
          b=u&0xf;
          b=b|(b<<4);
          g=(u&0xf0)>>4;
          g=g|(g<<4);
          r=(u&0xf00)>>8;
          r=r|(r<<4);

          u=b|(g<<8)|(r<<16)|(0xff<<24);
        }
        break;

      case 16:
        u = *((u16*) pb);     // need to convert 16bit color to 32 bit
        pb += 2;
        {
          u32 r;
          u32 g;
          u32 b;
          u32 a;

          // 1555 RGB, set dest alpha to 0 or 0xff
          // TODO: Is there any change a 16bit TGA is 565 with no alpha??
          b=u&0xf;
          b=b|(b<<4);
          g=(u&0xf0)>>4;
          g=g|(g<<4);
          r=(u&0xf00)>>8;
          r=r|(r<<4);

          a=u&8000;
          if (a)
            a=0xff;

          u=b|(g<<8)|(r<<16)|(a<<24);
        }
        break;

      case 24:
        u = *((u32*) pb);
        u|=0xff000000;        // set alpha to be solid
        pb += 3;
        break;

      case 32:
        u = *((u32*) pb);
        pb += 4;
        break;
      }
      *pColor = u;
      pColor++;
    }
  }

  data += color_map_size;

  // required bytes of image data
  //u32 cbImage = (u32) tga->width * (u32) tga->height * file_pixel_bytes;

  u32 stride    = tga->width*(ColorFormatBits(pixelfmt)>>3);
  u8* tga_data  = new u8[tga->width*tga->height*4];
  u8* dest_line;

  dest_line = top_to_bottom ? tga_data : (tga_data + (tga->height - 1) * stride);

  for(u32 y = 0; y < tga->height; y++)
  {
    u8* dest_x = left_to_right ? dest_line : (dest_line + stride - (ColorFormatBits(pixelfmt)>>3));

    for(u32 x = 0; x < tga->width; )
    {
      bool run_length;
      u32 uCount;

      if (rle)
      {
        run_length = (*data & 0x80)!=0;
        uCount = (*data & 0x7f) + 1;

        data++;
      }
      else
      {
        run_length = false;
        uCount = tga->width;
      }

      x += uCount;

      while(uCount--)
      {
        if (file_pixel_bytes == 1)
        {
          if (palette)
          {
            *((u32*)dest_x) = palette_data[ *(u8*)data ];
          }
          else
          {
            *((u8*)dest_x) = *(u8*)data;
          }
        }
        else if (file_pixel_bytes == 2)
        {
          *((u16*)dest_x) = *(u16*)data;
        }
        else if (file_pixel_bytes == 3)
        {
          *((u32*)dest_x) = (*(u32*)data) | 0xff000000;
        }
        else
        {
          *((u32*)dest_x) = *(u32*)data;
        }

        if(!run_length)
        {
          data += file_pixel_bytes;
        }

        dest_x = left_to_right ? (dest_x + (ColorFormatBits(pixelfmt)>>3)) : (dest_x - (ColorFormatBits(pixelfmt)>>3));
      }

      if(run_length)
      {
        data += file_pixel_bytes;
      }
    }

    dest_line = top_to_bottom ? (dest_line + stride) : (dest_line - stride);
  }

  Image* surface = new Image(tga->width,tga->height, pixelfmt);
  surface->FillFaceData(0, pixelfmt, tga_data);

  //Clean up temp data
  delete[] tga_data;

  if (surface == 0)
  {
    return 0;
  }

  if(convert_to_linear)
  {
    surface->ConvertSrgbToLinear();
  }

  return surface;
}


//-----------------------------------------------------------------------------
bool Image::WriteTGA(const tchar* fname, u32 face, bool convert_to_srgb) const
{
  const f32* r = GetFacePtr(face, R);

  if (r == 0)
  {
    return false;
  }

  FILE * textureFile = _tfopen(fname, TXT( "wb" ));
  if (!textureFile)
  {
    return false;
  }

  //Verify first that we have a valid TGA format
  ColorFormat valid_format = CF_ARGB8888;

  switch (m_NativeFormat)
  {
    case CF_L8:
    case CF_L16:
    case CF_ARGB8888:
    case CF_ARGB1555:
      valid_format = m_NativeFormat;
  }

  // write header
  TGAHeader   tga;
  tga.idLength = 0;
  tga.colormapType = 0;         // u8 : 1 = palette included, 0 = no palette
  if (valid_format==CF_L8)
  {
    tga.imageType = 0x03;
    tga.imageDescriptor = 0x20;
    tga.pixelDepth = 0x08;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (u16)m_Width;
    tga.height = (u16)m_Height;
  }
  else if (valid_format==CF_L16)  // not many apps will load this one!
  {
    tga.imageType = 0x03;
    tga.imageDescriptor = 0x20;
    tga.pixelDepth = 0x10;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (u16)m_Width;
    tga.height = (u16)m_Height;
  }
  else if (valid_format==CF_ARGB8888)
  {
    tga.imageType = 0x02;
    tga.imageDescriptor = 0x28;
    tga.pixelDepth = 32;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (u16)m_Width;
    tga.height = (u16)m_Height;
  }
  else if (valid_format==CF_ARGB1555)
  {
    tga.imageType = 0x02;
    tga.imageDescriptor = 0x21;
    tga.pixelDepth = 16;
    tga.colorMapIndex = 0;
    tga.colorMapLength = 0;
    tga.colorMapBits = 0;
    tga.xOrigin = 0;
    tga.yOrigin = 0;
    tga.width = (u16)m_Width;
    tga.height = (u16)m_Height;
  }
  else
  {
    fclose(textureFile);
    return false;
  }


  fwrite(&tga,sizeof(tga),1,textureFile);

  //Generate the native data
  u8* native_data = Image::GenerateFormatData(this, valid_format, face, convert_to_srgb);
  if(native_data)
  {
    // write image bits
    fwrite(native_data, m_Width*m_Height*(ColorFormatBits(valid_format)>>3),1,textureFile);
    // clean up native data
    delete[] native_data;
  }
  fclose(textureFile);

  return true;
}