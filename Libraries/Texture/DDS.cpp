#include "Texture.h"
#include "DDS.h"

#include <squish.h>

using namespace IG;

static inline u32 GetLineStride(u32 bits_per_pixel, u32 width)
{
#if 0
  return (((bits_per_pixel * width) >> 3) + 3) & 0xfffffffc;	// align up to 4 byte boundary
#else
  return (bits_per_pixel * width) >> 3;		// no alignment
#endif
}

static u32  GetFourCC(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_DXT1:     return  DDS_CC_D3DFMT_DXT1;
    case  IG::OUTPUT_CF_DXT3:     return  DDS_CC_D3DFMT_DXT3;
    case  IG::OUTPUT_CF_DXT5:     return  DDS_CC_D3DFMT_DXT5;
    case  IG::OUTPUT_CF_DUDV:     return  117;
    case  IG::OUTPUT_CF_F32:      return  DDS_CC_D3DFMT_R32F;
    case  IG::OUTPUT_CF_F32F32:   return  DDS_CC_D3DFMT_G32R32F;
    case  IG::OUTPUT_CF_FLOATMAP: return  DDS_CC_D3DFMT_A32B32G32R32F;
    case  IG::OUTPUT_CF_F16:      return  DDS_CC_D3DFMT_R16F;
    case  IG::OUTPUT_CF_F16F16:   return  DDS_CC_D3DFMT_G16R16F;
    case  IG::OUTPUT_CF_HALFMAP:  return  DDS_CC_D3DFMT_A16B16G16R16F;
  }

  return 0;
}

static u32  GetLinearSize(IG::OutputColorFormat format, u32 width, u32 height)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_DXT1:     return  (width*height)>>1;
    case  IG::OUTPUT_CF_DXT3:     return  (width*height);
    case  IG::OUTPUT_CF_DXT5:     return  (width*height);

    case  IG::OUTPUT_CF_ARGB8888: return  (width*height)*4;
    case  IG::OUTPUT_CF_ARGB4444: return  (width*height)*2;
    case  IG::OUTPUT_CF_ARGB1555: return  (width*height)*2;
    case  IG::OUTPUT_CF_RGB565:   return  (width*height)*2;
    case  IG::OUTPUT_CF_A8:       return  (width*height);
    case  IG::OUTPUT_CF_L8:       return  (width*height);
    case  IG::OUTPUT_CF_AL88:     return  (width*height)*2;
    case  IG::OUTPUT_CF_DUDV:     return  (width*height)*2;
    case  IG::OUTPUT_CF_F32:      return  (width*height)*4;
    case  IG::OUTPUT_CF_F32F32:   return  (width*height)*8;
    case  IG::OUTPUT_CF_FLOATMAP: return  (width*height)*16;
    case  IG::OUTPUT_CF_F16:      return  (width*height)*2;
    case  IG::OUTPUT_CF_F16F16:   return  (width*height)*4;
    case  IG::OUTPUT_CF_HALFMAP:  return  (width*height)*8;
    case  IG::OUTPUT_CF_RGBE:     return  (width*height)*4;
  }
  NOC_ASSERT(!"WTF");
  return (width*height);
}

static u32  GetPitchOrLinearSize(IG::OutputColorFormat format, u32 width, u32 height)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_DXT1:     return  (width*height)>>1;
    case  IG::OUTPUT_CF_DXT3:     return  width*height;
    case  IG::OUTPUT_CF_DXT5:     return  width*height;

    case  IG::OUTPUT_CF_F32:      return  width*4;
    case  IG::OUTPUT_CF_F32F32:   return  width*8;
    case  IG::OUTPUT_CF_FLOATMAP: return  width*16;
    case  IG::OUTPUT_CF_F16:      return  width*2;
    case  IG::OUTPUT_CF_F16F16:   return  width*4;
    case  IG::OUTPUT_CF_HALFMAP:  return  width*8;
  }

  return 0;
}

static u32  GetPixelFormatFlag(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_F32:
    case  IG::OUTPUT_CF_F32F32:
    case  IG::OUTPUT_CF_FLOATMAP:
    case  IG::OUTPUT_CF_F16:
    case  IG::OUTPUT_CF_F16F16:
    case  IG::OUTPUT_CF_HALFMAP:
    case  IG::OUTPUT_CF_DXT1:
    case  IG::OUTPUT_CF_DXT3:
    case  IG::OUTPUT_CF_DXT5:
      return IG::DDS_PF_FLAGS_FOURCC;

    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_ARGB4444:
    case  IG::OUTPUT_CF_ARGB1555:
    case  IG::OUTPUT_CF_RGBE:
      return  IG::DDS_PF_FLAGS_RGB | IG::DDS_PF_FLAGS_ALPHA;

    case  IG::OUTPUT_CF_RGB565:
      return  IG::DDS_PF_FLAGS_RGB;

    case  IG::OUTPUT_CF_A8:
      return IG::DDS_PF_FLAGS_ALPHA_ONLY;

    case  IG::OUTPUT_CF_L8:
      return IG::DDS_PF_LUMINANCE;

    case  IG::OUTPUT_CF_AL88:
      return IG::DDS_PF_LUMINANCE |DDS_PF_FLAGS_ALPHA;
  }

  return 0;
}

static u32  GetRedMask(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_RGBE:
      return  0x00FF0000;

    case  IG::OUTPUT_CF_ARGB4444:
      return  0x00000F00;

    case  IG::OUTPUT_CF_ARGB1555:
      return  0x00007C00;

    case  IG::OUTPUT_CF_RGB565:
      return  0x0000F800;

    case  IG::OUTPUT_CF_L8:
    case  IG::OUTPUT_CF_AL88:
     return  0x000000FF;
  }

  return 0;
}

static u32  GetGreenMask(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_RGBE:
      return  0x0000FF00;

    case  IG::OUTPUT_CF_ARGB4444:
      return  0x000000F0;

    case  IG::OUTPUT_CF_ARGB1555:
      return  0x000003E0;

    case  IG::OUTPUT_CF_RGB565:
      return  0x000007e0;
  }

  return 0;
}

static u32  GetBlueMask(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_RGBE:
      return  0x000000FF;

    case  IG::OUTPUT_CF_ARGB4444:
      return  0x0000000F;

    case  IG::OUTPUT_CF_ARGB1555:
      return  0x0000001F;

    case  IG::OUTPUT_CF_RGB565:
      return  0x0000001F;
  }

  return 0;
}

static u32  GetAlphaMask(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_RGBE:
      return  0xFF000000;

    case  IG::OUTPUT_CF_ARGB4444:
      return  0x0000F000;

    case  IG::OUTPUT_CF_ARGB1555:
      return  0x00008000;

    case  IG::OUTPUT_CF_AL88:
      return  0x0000FF00;

    case  IG::OUTPUT_CF_A8:
      return  0x000000FF;
  }

  return 0;
}

static u32  GetBitCount(IG::OutputColorFormat format)
{
  switch(format)
  {
    case  IG::OUTPUT_CF_DXT1:
    case  IG::OUTPUT_CF_DXT3:
    case  IG::OUTPUT_CF_DXT5:
    case  IG::OUTPUT_CF_ARGB8888:
    case  IG::OUTPUT_CF_F32:
    case  IG::OUTPUT_CF_F16F16:
    case  IG::OUTPUT_CF_RGBE:
      return  32;

    case  IG::OUTPUT_CF_ARGB4444:
    case  IG::OUTPUT_CF_ARGB1555:
    case  IG::OUTPUT_CF_RGB565:
    case  IG::OUTPUT_CF_AL88:
    case  IG::OUTPUT_CF_DUDV:
    case  IG::OUTPUT_CF_F16:
      return 16;

    case  IG::OUTPUT_CF_A8:
    case  IG::OUTPUT_CF_L8:
      return 8;

    case  IG::OUTPUT_CF_F32F32:
    case  IG::OUTPUT_CF_HALFMAP:
      return 64;

    case  IG::OUTPUT_CF_FLOATMAP:
      return  128;
  }
  NOC_ASSERT(!"WTF");
  return 8;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetFourCCPixelSize()
//
// Returns bits per pixel for 4cc pixel formats that can be found in DDS files
//
////////////////////////////////////////////////////////////////////////////////////////////////
static u32 GetFourCCPixelSize(u32 cc)
{
  switch (cc)
  {
    case IG::DDS_CC_D3DFMT_DXT1:
      return 4;

    case IG::DDS_CC_D3DFMT_DXT2:
    case IG::DDS_CC_D3DFMT_DXT3:
    case IG::DDS_CC_D3DFMT_DXT4:
    case IG::DDS_CC_D3DFMT_DXT5:
      return 8;

    case IG::DDS_CC_D3DFMT_R16F:
      return 16;

    case IG::DDS_CC_D3DFMT_G16R16F:
    case IG::DDS_CC_D3DFMT_R32F:
      return 32;

    case IG::DDS_CC_D3DFMT_G32R32F:
    case IG::DDS_CC_D3DFMT_A16B16G16R16F:
      return 64;

    case IG::DDS_CC_D3DFMT_A32B32G32R32F:
      return 128;
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetDirectlyCompatibleColorFormat()
//
// Converts the DDS pixel format to a compatible texture class pixel format. If there is no
// direct mapping this function returns CF_UNKNOWN and the caller can decide what to do.
//
////////////////////////////////////////////////////////////////////////////////////////////////
static IG::ColorFormat GetDirectlyCompatibleColorFormat(IG::DDSPixelFormat* pf)
{
  // if the 4 CC code of pixel format is set then use it to figure out the format.
  // if the 4 CC code is not set then the format has to be an RGB format, use the pixel format
  // to figure the RGB layout
  if (pf->m_flags & IG::DDS_PF_FLAGS_FOURCC)
  {
    switch (pf->m_fourcc)
    {
    case IG::DDS_CC_D3DFMT_A16B16G16R16F:
      return IG::CF_RGBAHALFMAP;
    case IG::DDS_CC_D3DFMT_R32F:
      return IG::CF_F32;
    case IG::DDS_CC_D3DFMT_A32B32G32R32F:
      return IG::CF_RGBAFLOATMAP;
    }
  }
  else if ( (pf->m_flags & IG::DDS_PF_FLAGS_RGB) !=0 )
  {
    // This is an RGB format, may also have alpha
    if (pf->m_bit_count==16)
    {
      if ((pf->m_red_mask == 0x0f00) && (pf->m_green_mask == 0x00f0) && (pf->m_blue_mask == 0x000f))
        return IG::CF_ARGB4444;

      if ((pf->m_red_mask == 0xf800) && (pf->m_green_mask == 0x07e0) && (pf->m_blue_mask == 0x001f))
        return IG::CF_RGB565;

      if ((pf->m_red_mask == 0x7c00) && (pf->m_green_mask == 0x03e0) && (pf->m_blue_mask == 0x001f))
        return IG::CF_ARGB1555;
    }
    else if (pf->m_bit_count==32)
    {
      if (pf->m_red_mask != 0x00ff0000)
        return IG::CF_UNKNOWN;
      if (pf->m_green_mask != 0x0000ff00)
        return IG::CF_UNKNOWN;
      if (pf->m_blue_mask != 0x000000ff)
        return IG::CF_UNKNOWN;

      return IG::CF_ARGB8888;
    }
  }
  else if (pf->m_flags & DDS_PF_FLAGS_ALPHA_ONLY)
  {
    if (pf->m_alpha_mask == 0xff)
      return IG::CF_A8;
  }
  else if (pf->m_flags & DDS_PF_LUMINANCE)
  {
    if (pf->m_bit_count==8)
    {
      if ((pf->m_red_mask == 0xff)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0x00))
        return IG::CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0xff) && (pf->m_blue_mask == 0x00))
        return IG::CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0xff))
        return IG::CF_L8;
    }
    else if (pf->m_bit_count==16)
    {
      if (pf->m_flags & DDS_PF_FLAGS_ALPHA)
      {
        if ((pf->m_red_mask == 0x000000ff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0) && (pf->m_alpha_mask == 0xff00))
          return IG::CF_AL88;
      }

      if ((pf->m_red_mask == 0x0000ffff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0))
        return IG::CF_L16;

      if ((pf->m_red_mask == 0x0000)  && (pf->m_green_mask == 0xffff) && (pf->m_blue_mask == 0x0000))
        return IG::CF_L16;

      if ((pf->m_red_mask == 0x0000)  && (pf->m_green_mask == 0x0000) && (pf->m_blue_mask == 0xffff))
        return IG::CF_L16;
    }
  }

  // do not know what this format is
  return IG::CF_UNKNOWN;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetOutputCompatibleColorFormat()
//
// Converts the DDS pixel format to a compatible ouput pixel format. If there is no
// direct mapping this function returns CF_UNKNOWN and the caller can decide what to do.
//
////////////////////////////////////////////////////////////////////////////////////////////////
static IG::OutputColorFormat GetOutputCompatibleColorFormat(IG::DDSPixelFormat* pf)
{
  // if the 4 CC code of pixel format is set then use it to figure out the format.
  // if the 4 CC code is not set then the format has to be an RGB format, use the pixel format
  // to figure the RGB layout
  if (pf->m_flags & IG::DDS_PF_FLAGS_FOURCC)
  {
    switch (pf->m_fourcc)
    {
    case IG::DDS_CC_D3DFMT_A16B16G16R16F:
      return IG::OUTPUT_CF_HALFMAP;
    case IG::DDS_CC_D3DFMT_R32F:
      return IG::OUTPUT_CF_F32;
    case IG::DDS_CC_D3DFMT_A32B32G32R32F:
      return IG::OUTPUT_CF_FLOATMAP;
    case IG::DDS_CC_D3DFMT_DXT1:
      return IG::OUTPUT_CF_DXT1;
    case IG::DDS_CC_D3DFMT_DXT3:
      return IG::OUTPUT_CF_DXT3;
    case IG::DDS_CC_D3DFMT_DXT5:
      return IG::OUTPUT_CF_DXT5;
    }
  }
  else if ( (pf->m_flags & IG::DDS_PF_FLAGS_RGB) !=0 )
  {
    // This is an RGB format, may also have alpha
    if (pf->m_bit_count==16)
    {
      if ((pf->m_red_mask == 0x0f00) && (pf->m_green_mask == 0x00f0) && (pf->m_blue_mask == 0x000f))
        return IG::OUTPUT_CF_ARGB4444;

      if ((pf->m_red_mask == 0xf800) && (pf->m_green_mask == 0x07e0) && (pf->m_blue_mask == 0x001f))
        return IG::OUTPUT_CF_RGB565;

      if ((pf->m_red_mask == 0x7c00) && (pf->m_green_mask == 0x03e0) && (pf->m_blue_mask == 0x001f))
        return IG::OUTPUT_CF_ARGB1555;
    }
    else if (pf->m_bit_count==32)
    {
      if (pf->m_red_mask != 0x00ff0000)
        return IG::OUTPUT_CF_UNKNOWN;
      if (pf->m_green_mask != 0x0000ff00)
        return IG::OUTPUT_CF_UNKNOWN;
      if (pf->m_blue_mask != 0x000000ff)
        return IG::OUTPUT_CF_UNKNOWN;

      return IG::OUTPUT_CF_ARGB8888;
    }
  }
  else if (pf->m_flags & DDS_PF_FLAGS_ALPHA_ONLY)
  {
    if (pf->m_alpha_mask == 0xff)
      return IG::OUTPUT_CF_A8;
  }
  else if (pf->m_flags & DDS_PF_LUMINANCE)
  {
    if (pf->m_bit_count==8)
    {
      if ((pf->m_red_mask == 0xff)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0x00))
        return IG::OUTPUT_CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0xff) && (pf->m_blue_mask == 0x00))
        return IG::OUTPUT_CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0xff))
        return IG::OUTPUT_CF_L8;
    }
    else if (pf->m_bit_count==16)
    {
      if (pf->m_flags & DDS_PF_FLAGS_ALPHA)
      {
        if ((pf->m_red_mask == 0x000000ff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0) && (pf->m_alpha_mask == 0xff00))
          return IG::OUTPUT_CF_AL88;
      }
    }
  }

  // do not know what this format is
  return IG::OUTPUT_CF_UNKNOWN;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetBitsPerPixel()
//
// Get the bits per pixel of the pixels in the DDS files
//
////////////////////////////////////////////////////////////////////////////////////////////////
static u32 GetBitsPerPixel(IG::DDSPixelFormat* pf)
{
  if (pf->m_flags & DDS_PF_FLAGS_FOURCC)
  {
    // the size is calculated from the size of the 4CC code and width and height
    return GetFourCCPixelSize(pf->m_fourcc);
  }
  else
  {
    return pf->m_bit_count;
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// CalculateImageAndMipSize()
//
////////////////////////////////////////////////////////////////////////////////////////////////
u32 CalculateImageAndMipSize(u32 pixel_size,u32 width,u32 height,u32 depth,u32 mip_count)
{
  if (depth==0)
    depth=1;

  u32 size = 0;
  for (u32 m=0;m<mip_count;m++)
  {
    u32 line_stride = GetLineStride(pixel_size, width);
    size = size + (line_stride*height*depth);
    width = MAX(1,width>>1);
    height = MAX(1,height>>1);
  }

  return size;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// IsDXTC()
//
// Returns true for one of the DXTC formats
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool IsDXTC(IG::DDSPixelFormat* pf)
{
  if (pf->m_flags & DDS_PF_FLAGS_FOURCC)
  {
    if ((pf->m_fourcc==DDS_CC_D3DFMT_DXT1) ||
      (pf->m_fourcc==DDS_CC_D3DFMT_DXT2) ||
      (pf->m_fourcc==DDS_CC_D3DFMT_DXT3) ||
      (pf->m_fourcc==DDS_CC_D3DFMT_DXT4) ||
      (pf->m_fourcc==DDS_CC_D3DFMT_DXT5))
    {
      return true;
    }
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// CalculateImageAndMipSize()
//
// Calculates the number of bytes to the next image in the source data
//
////////////////////////////////////////////////////////////////////////////////////////////////
u32 CalculateImageSize(IG::DDSPixelFormat* pf,u32 width,u32 height,u32 depth)
{
  if (depth==0)
    depth=1;

  u32 bits = GetBitsPerPixel(pf);
  if (IsDXTC(pf))
  {
    return (MAX(1,width>>2)*MAX(1,height>>2)*bits*16)>>3;
  }

  u32 line_stride = GetLineStride(bits, width);

  return (line_stride*height*depth);
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// LoadDDS()
//
// Loads Direct3D .dds files, any uncompressed format any type (2D, cube env, volume etc). The
// mips are not loaded because a texture class does not contain mips.
//
// To direclty load a DDS use LoadDDSToMipSet() functions
//
////////////////////////////////////////////////////////////////////////////////////////////////
Texture* Texture::LoadDDS(const void* ddsadr, bool convert_to_linear)
{
  DDSHeader* header = (DDSHeader*)ddsadr;

  u32 width;
  u32 height;
  u32 depth;
  u32 line_stride;
  u32 levels;
  u32 pixel_size;
  u8* image_data = (u8*)(header+1);

  bool faces[6];

  // check the header
  if (header->m_magic != DDS_MAGIC)
  {
    return 0;
  }

  // check the header size the header(128) - magic number size(4)
  if (header->m_size != 124)
  {
    return 0;
  }

  if ((header->m_flags & DDS_FLAGS_REQUIRED) != DDS_FLAGS_REQUIRED)
  {
    // the flags must specifiy, width, height and the caps bit must be set
    return 0;
  }

  ColorFormat fmt = GetDirectlyCompatibleColorFormat(&header->m_pixel_format);

  width   = header->m_width;
  height  = header->m_height;
  depth   = 1;

  pixel_size = GetBitsPerPixel(&header->m_pixel_format);

  if (header->m_caps.m_caps2 & DDS_CAPS2_CUBEMAP)
  {
    depth = 0;    // signal that this is a cubemap

    // scan the flags to see which cube faces are present
    u32 mask = DDS_CAPS2_CUBEMAP_POSX;
    for (u32 f=0;f<6;f++)
    {
      faces[f] = (header->m_caps.m_caps2 & mask)!=0;
      mask<<=1;
    }
  }
  else if (header->m_caps.m_caps2 & DDS_CAPS2_VOLUME)
  {
    if (header->m_flags & DDS_FLAGS_DEPTH)
    {
      depth = header->m_depth;
    }

    // 2D map
    faces[0] = true;
    faces[1] = false;
    faces[2] = false;
    faces[3] = false;
    faces[4] = false;
    faces[5] = false;
  }
  else
  {
    // 2D map
    faces[0] = true;
    faces[1] = false;
    faces[2] = false;
    faces[3] = false;
    faces[4] = false;
    faces[5] = false;
  }

  if (header->m_flags & DDS_FLAGS_PITCH)
  {
    line_stride = header->m_pitch;                // size of a single line
  }
  else if (header->m_flags & DDS_FLAGS_LINEARSIZE)
  {
    // this should only occur with compressed images
    return 0;
  }
  else
  {
    // neither linear size nor pitch are specified, a 4CC code has to be specified, in this
    // case the 4CC code is the D3DFORMAT id.
    line_stride = GetLineStride(pixel_size, header->m_width);
  }

  // see how many mip levels we have
  if (header->m_caps.m_caps1 & DDS_CAPS1_MIPMAPS)
  {
    levels = header->m_mip_count;
  }
  else
  {
    levels = 1;
  }

  Texture*  result     = new Texture(width,height,depth,fmt);
  u32       image_size = CalculateImageAndMipSize(pixel_size,width,height,depth,levels);
  u32 d = depth;

  if (d==0)
  {
    d = 1;
  }

  for (u32 f=0;f<6;f++)
  {
    if (faces[f])
    {
      u8* face_data = image_data;

      if (fmt == CF_UNKNOWN)
      {
        u32 flags;
        if (header->m_pixel_format.m_fourcc == IG::DDS_CC_D3DFMT_DXT1)
        {
          flags = squish::kDxt1;
        }
        else if (header->m_pixel_format.m_fourcc == IG::DDS_CC_D3DFMT_DXT3)
        {
          flags = squish::kDxt3;
        }
        else if (header->m_pixel_format.m_fourcc == IG::DDS_CC_D3DFMT_DXT5)
        {
          flags = squish::kDxt5;
        }
        else
        {
          //Unsupported format
          delete result;
          return NULL;
        }

        u8* src_rgba  = new u8[width*height*4];
        squish::DecompressImage(src_rgba, width,  height,  face_data, flags);
        result->FillFaceData(f, CF_ARGB8888, src_rgba);
        result->m_NativeFormat  = CF_ARGB8888;
        delete[] src_rgba;
      }
      else
      {
        // all the other output formats are available via the color format conversions and thus we can directly
        // convert to RBGA
        result->FillFaceData(f, fmt, face_data);
      }

      // skip all the mips and go to the next face
      image_data+=image_size;
    }
  }

  if(convert_to_linear)
  {
    result->ConvertSrgbToLinear();
  }

  return result;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// LoadDDSToMipSet()
//
// Loads Direct3D .dds files to directly to a mip set, all the mips are loaded as is along with
// additional faces and layers.
//
////////////////////////////////////////////////////////////////////////////////////////////////
MipSet* Texture::LoadDDSToMipSet(const void* ddsadr)
{
  DDSHeader* header = (DDSHeader*)ddsadr;

  u32 levels=1;
  u32 pixel_size;
  u8* image_data = (u8*)(header+1);

  bool faces[6];

  // check the header
  if (header->m_magic != DDS_MAGIC)
  {
    return 0;
  }

  // check the header size the header(128) - magic number size(4)
  if (header->m_size != 124)
  {
    return 0;
  }

  if ((header->m_flags & DDS_FLAGS_REQUIRED) != DDS_FLAGS_REQUIRED)
  {
    // the flags must specifiy, width, height and the caps bit must be set
    return 0;
  }

  OutputColorFormat fmt = GetOutputCompatibleColorFormat(&header->m_pixel_format);

  // derive the number of mipmaps
  if (header->m_caps.m_caps1 & DDS_CAPS1_MIPMAPS)
  {
    levels = header->m_mip_count;
  }

  MipSet* mips = new MipSet;

  mips->m_width = header->m_width;
  mips->m_height = header->m_width;
  pixel_size = GetBitsPerPixel(&header->m_pixel_format);

  // see how many mip levels we have
  if (header->m_caps.m_caps1 & DDS_CAPS1_MIPMAPS)
  {
    levels = header->m_mip_count;
  }
  else
  {
    levels = 1;
  }

  mips->m_levels_used = levels;
  mips->m_format = fmt;

  if (header->m_caps.m_caps2 & DDS_CAPS2_CUBEMAP)
  {
    mips->m_depth = 0;
    mips->m_texture_type = Texture::CUBE;

    // scan the flags to see which cube faces are present
    u32 mask = DDS_CAPS2_CUBEMAP_POSX;
    for (u32 f=0;f<6;f++)
    {
      faces[f] = (header->m_caps.m_caps2 & mask)!=0;
      mask<<=1;
    }
  }
  else if (header->m_caps.m_caps2 & DDS_CAPS2_VOLUME)
  {
    mips->m_depth = 1;
    mips->m_texture_type = Texture::VOLUME;
    if (header->m_flags & DDS_FLAGS_DEPTH)
    {
      mips->m_depth = header->m_depth;
    }

    // Volume map
    faces[0] = true;
    faces[1] = false;
    faces[2] = false;
    faces[3] = false;
    faces[4] = false;
    faces[5] = false;
  }
  else
  {
    mips->m_depth = 1;
    mips->m_texture_type = Texture::REGULAR;

    // 2D map
    faces[0] = true;
    faces[1] = false;
    faces[2] = false;
    faces[3] = false;
    faces[4] = false;
    faces[5] = false;
  }


  for (u32 f=0;f<6;f++)
  {
    u8* face_data = image_data;
    if (faces[f])
    {
      u32 w  = mips->m_width;
      u32 h  = mips->m_height;
      u32 d = mips->m_depth;
      if (d==0)
        d=1;

      for (u32 level=0;level<levels;level++)
      {
        u32 bytes = CalculateImageSize(&header->m_pixel_format,w,h,d);
        mips->m_datasize[level] = bytes;
        mips->m_levels[f][level].m_width = w;
        mips->m_levels[f][level].m_height = h;
        mips->m_levels[f][level].m_depth = d;
        mips->m_levels[f][level].m_data = new u8[bytes];

        memcpy(mips->m_levels[f][level].m_data,face_data,bytes);
        face_data+=bytes;

        w = MAX(1,w>>1);
        h = MAX(1,h>>1);
        d = MAX(1,d>>1);
      }

      // directly calculate the start of the next face
      image_data+=CalculateImageAndMipSize(pixel_size,mips->m_width,mips->m_height,mips->m_depth,levels);
    }
  }

  return mips;
}

//Write DDS file
bool IG::MipSet::WriteDDS(const char* fname) const
{
  FILE * file = fopen(fname, "wb");
  if (!file)
  {
    return false;
  }

  IG::DDSHeader header;
  memset(&header, 0, sizeof(header));

  header.m_magic  = IG::DDS_MAGIC;
  header.m_size   = 124;

  header.m_flags |= IG::DDS_FLAGS_CAPS        |
                    IG::DDS_FLAGS_HEIGHT      |
                    IG::DDS_FLAGS_WIDTH       |
                    IG::DDS_FLAGS_PIXELFORMAT |
                    IG::DDS_FLAGS_MIPMAPCOUNT;

  header.m_height     = m_height;
  header.m_width      = m_width;
  header.m_data_size  = GetPitchOrLinearSize(m_format, m_width, m_height);

  header.m_depth      = m_depth;
  header.m_mip_count  = m_levels_used;

  //Setup the pixel format
  header.m_pixel_format.m_size        = sizeof(DDSPixelFormat);
  header.m_pixel_format.m_flags       = GetPixelFormatFlag(m_format);
  header.m_pixel_format.m_fourcc      = GetFourCC(m_format);
  header.m_pixel_format.m_bit_count   = GetBitCount(m_format);

  header.m_pixel_format.m_red_mask    = GetRedMask(m_format);
  header.m_pixel_format.m_green_mask  = GetGreenMask(m_format);
  header.m_pixel_format.m_blue_mask   = GetBlueMask(m_format);
  header.m_pixel_format.m_alpha_mask  = GetAlphaMask(m_format);

  //Caps
  header.m_caps.m_caps1 = IG::DDS_CAPS1_TEXTURE;

  if(m_levels_used > 1)
  {
    header.m_caps.m_caps1 |= (IG::DDS_CAPS1_MIPMAPS | IG::DDS_CAPS1_COMPLEX);
  }

  if(m_texture_type != IG::Texture::REGULAR)
  {
    header.m_caps.m_caps1 |= IG::DDS_CAPS1_COMPLEX;
  }

  if(m_texture_type == IG::Texture::CUBE)
  {
    header.m_caps.m_caps2 = IG::DDS_CAPS2_CUBEMAP |
                            IG::DDS_CAPS2_CUBEMAP_POSX |
                            IG::DDS_CAPS2_CUBEMAP_NEGX |
                            IG::DDS_CAPS2_CUBEMAP_POSY |
                            IG::DDS_CAPS2_CUBEMAP_NEGY |
                            IG::DDS_CAPS2_CUBEMAP_POSZ |
                            IG::DDS_CAPS2_CUBEMAP_NEGZ;
  }
  else if(m_texture_type == IG::Texture::VOLUME)
  {
    header.m_caps.m_caps2 = IG::DDS_CAPS2_VOLUME;
  }

  fwrite(&header, sizeof(header), 1, file);

  u32 num_levels = m_levels_used;

  for(u32 face = 0; face < 6; ++face)
  {
    for(u32 level = 0; level < num_levels; ++level)
    {
      const IG::MipSet::MipInfo*  face_data       = &m_levels[face][level];
      const u32                   dds_linear_size = GetLinearSize(m_format, face_data->m_width, face_data->m_height);
      fwrite(face_data->m_data, dds_linear_size, 1, file);
    }

    if(m_texture_type != IG::Texture::CUBE)
    {
      break;
    }
  }

  fclose(file);

  return true;
}

//Write DDS file
bool IG::Texture::WriteDDS(const char* fname, bool convert_to_srgb, IG::OutputColorFormat output_fmt) const
{
  //If we don't sepecify an output format, pick a suitable one
  if(output_fmt == IG::OUTPUT_CF_UNKNOWN)
  {
    switch(m_NativeFormat)
    {
      case  IG::CF_ARGB8888:        output_fmt = IG::OUTPUT_CF_ARGB8888;  break;
      case  IG::CF_ARGB4444:        output_fmt = IG::OUTPUT_CF_ARGB4444;  break;
      case  IG::CF_ARGB1555:        output_fmt = IG::OUTPUT_CF_ARGB1555;  break;
      case  IG::CF_RGB565:          output_fmt = IG::OUTPUT_CF_RGB565;    break;
      case  IG::CF_A8:              output_fmt = IG::OUTPUT_CF_A8;        break;
      case  IG::CF_L8:              output_fmt = IG::OUTPUT_CF_L8;        break;
      case  IG::CF_AL88:            output_fmt = IG::OUTPUT_CF_AL88;      break;
      case  IG::CF_F32:             output_fmt = IG::OUTPUT_CF_F32;       break;
      case  IG::CF_F32F32:          output_fmt = IG::OUTPUT_CF_F32F32;    break;
      case  IG::CF_RGBAFLOATMAP:    output_fmt = IG::OUTPUT_CF_FLOATMAP;  break;
      case  IG::CF_F16:             output_fmt = IG::OUTPUT_CF_F16;       break;
      case  IG::CF_F16F16:          output_fmt = IG::OUTPUT_CF_F16F16;    break;
      case  IG::CF_RGBAHALFMAP:     output_fmt = IG::OUTPUT_CF_HALFMAP;   break;
      case  IG::CF_RGBE:            output_fmt = IG::OUTPUT_CF_RGBE;      break;
    }
  }

  if((output_fmt == IG::OUTPUT_CF_UNKNOWN) || (output_fmt == IG::OUTPUT_CF_DUDV))
  {
    NOC_ASSERT(!"Unkown or unspported DDS output format");
    return false;
  }

  //Reuse some of the tools functionality instead of writing more code!
  IG::MipGenOptions mip_gen_options;
  IG::MipSet::RuntimeSettings runtime;

  mip_gen_options.m_Levels        = 1;                     //We're only interested in the top mip
  mip_gen_options.m_PostFilter    = IG::IMAGE_FILTER_NONE; //Unused
  mip_gen_options.m_Filter        = IG::MIP_FILTER_POINT;  //Unused
  mip_gen_options.m_OutputFormat  = output_fmt;
  mip_gen_options.m_ConvertToSrgb = convert_to_srgb;

  //Generate the mipset
  IG::MipSet* mip_set = GenerateMipSet(mip_gen_options, runtime);

  //Verify the mipset
  if(mip_set)
  {
    //Yatta!
    mip_set->WriteDDS(fname);
    delete mip_set;
    return true;
  }

  //Failed!
  return false;
}