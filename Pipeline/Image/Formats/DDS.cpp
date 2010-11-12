#include "Pipeline/Image/Image.h"
#include "DDS.h"

#include <squish.h>

using namespace Helium;

static inline uint32_t GetLineStride(uint32_t bits_per_pixel, uint32_t width)
{
#if ALIGN_DDS_STRIDE_TO_4_BYTE_BOUNDARIES
  return (((bits_per_pixel * width) >> 3) + 3) & 0xfffffffc;	// align up to 4 byte boundary
#else
  return (bits_per_pixel * width) >> 3;		// no alignment
#endif
}

static uint32_t  GetFourCC(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_DXT1:     return  DDS_CC_D3DFMT_DXT1;
    case  Helium::OUTPUT_CF_DXT3:     return  DDS_CC_D3DFMT_DXT3;
    case  Helium::OUTPUT_CF_DXT5:     return  DDS_CC_D3DFMT_DXT5;
    case  Helium::OUTPUT_CF_DUDV:     return  117;
    case  Helium::OUTPUT_CF_F32:      return  DDS_CC_D3DFMT_R32F;
    case  Helium::OUTPUT_CF_F32F32:   return  DDS_CC_D3DFMT_G32R32F;
    case  Helium::OUTPUT_CF_FLOATMAP: return  DDS_CC_D3DFMT_A32B32G32R32F;
    case  Helium::OUTPUT_CF_F16:      return  DDS_CC_D3DFMT_R16F;
    case  Helium::OUTPUT_CF_F16F16:   return  DDS_CC_D3DFMT_G16R16F;
    case  Helium::OUTPUT_CF_HALFMAP:  return  DDS_CC_D3DFMT_A16B16G16R16F;
  }

  return 0;
}

static uint32_t  GetLinearSize(Helium::OutputColorFormat format, uint32_t width, uint32_t height)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_DXT1:     return  (width*height)>>1;
    case  Helium::OUTPUT_CF_DXT3:     return  (width*height);
    case  Helium::OUTPUT_CF_DXT5:     return  (width*height);

    case  Helium::OUTPUT_CF_ARGB8888: return  (width*height)*4;
    case  Helium::OUTPUT_CF_ARGB4444: return  (width*height)*2;
    case  Helium::OUTPUT_CF_ARGB1555: return  (width*height)*2;
    case  Helium::OUTPUT_CF_RGB565:   return  (width*height)*2;
    case  Helium::OUTPUT_CF_A8:       return  (width*height);
    case  Helium::OUTPUT_CF_L8:       return  (width*height);
    case  Helium::OUTPUT_CF_AL88:     return  (width*height)*2;
    case  Helium::OUTPUT_CF_DUDV:     return  (width*height)*2;
    case  Helium::OUTPUT_CF_F32:      return  (width*height)*4;
    case  Helium::OUTPUT_CF_F32F32:   return  (width*height)*8;
    case  Helium::OUTPUT_CF_FLOATMAP: return  (width*height)*16;
    case  Helium::OUTPUT_CF_F16:      return  (width*height)*2;
    case  Helium::OUTPUT_CF_F16F16:   return  (width*height)*4;
    case  Helium::OUTPUT_CF_HALFMAP:  return  (width*height)*8;
    case  Helium::OUTPUT_CF_RGBE:     return  (width*height)*4;
  }
  HELIUM_ASSERT(!"WTF");
  return (width*height);
}

static uint32_t  GetPitchOrLinearSize(Helium::OutputColorFormat format, uint32_t width, uint32_t height)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_DXT1:     return  (width*height)>>1;
    case  Helium::OUTPUT_CF_DXT3:     return  width*height;
    case  Helium::OUTPUT_CF_DXT5:     return  width*height;

    case  Helium::OUTPUT_CF_F32:      return  width*4;
    case  Helium::OUTPUT_CF_F32F32:   return  width*8;
    case  Helium::OUTPUT_CF_FLOATMAP: return  width*16;
    case  Helium::OUTPUT_CF_F16:      return  width*2;
    case  Helium::OUTPUT_CF_F16F16:   return  width*4;
    case  Helium::OUTPUT_CF_HALFMAP:  return  width*8;
  }

  return 0;
}

static uint32_t  GetPixelFormatFlag(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_F32:
    case  Helium::OUTPUT_CF_F32F32:
    case  Helium::OUTPUT_CF_FLOATMAP:
    case  Helium::OUTPUT_CF_F16:
    case  Helium::OUTPUT_CF_F16F16:
    case  Helium::OUTPUT_CF_HALFMAP:
    case  Helium::OUTPUT_CF_DXT1:
    case  Helium::OUTPUT_CF_DXT3:
    case  Helium::OUTPUT_CF_DXT5:
      return Helium::DDS_PF_FLAGS_FOURCC;

    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_ARGB4444:
    case  Helium::OUTPUT_CF_ARGB1555:
    case  Helium::OUTPUT_CF_RGBE:
      return  Helium::DDS_PF_FLAGS_RGB | Helium::DDS_PF_FLAGS_ALPHA;

    case  Helium::OUTPUT_CF_RGB565:
      return  Helium::DDS_PF_FLAGS_RGB;

    case  Helium::OUTPUT_CF_A8:
      return Helium::DDS_PF_FLAGS_ALPHA_ONLY;

    case  Helium::OUTPUT_CF_L8:
      return Helium::DDS_PF_LUMINANCE;

    case  Helium::OUTPUT_CF_AL88:
      return Helium::DDS_PF_LUMINANCE |DDS_PF_FLAGS_ALPHA;
  }

  return 0;
}

static uint32_t  GetRedMask(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_RGBE:
      return  0x00FF0000;

    case  Helium::OUTPUT_CF_ARGB4444:
      return  0x00000F00;

    case  Helium::OUTPUT_CF_ARGB1555:
      return  0x00007C00;

    case  Helium::OUTPUT_CF_RGB565:
      return  0x0000F800;

    case  Helium::OUTPUT_CF_L8:
    case  Helium::OUTPUT_CF_AL88:
     return  0x000000FF;
  }

  return 0;
}

static uint32_t  GetGreenMask(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_RGBE:
      return  0x0000FF00;

    case  Helium::OUTPUT_CF_ARGB4444:
      return  0x000000F0;

    case  Helium::OUTPUT_CF_ARGB1555:
      return  0x000003E0;

    case  Helium::OUTPUT_CF_RGB565:
      return  0x000007e0;
  }

  return 0;
}

static uint32_t  GetBlueMask(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_RGBE:
      return  0x000000FF;

    case  Helium::OUTPUT_CF_ARGB4444:
      return  0x0000000F;

    case  Helium::OUTPUT_CF_ARGB1555:
      return  0x0000001F;

    case  Helium::OUTPUT_CF_RGB565:
      return  0x0000001F;
  }

  return 0;
}

static uint32_t  GetAlphaMask(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_RGBE:
      return  0xFF000000;

    case  Helium::OUTPUT_CF_ARGB4444:
      return  0x0000F000;

    case  Helium::OUTPUT_CF_ARGB1555:
      return  0x00008000;

    case  Helium::OUTPUT_CF_AL88:
      return  0x0000FF00;

    case  Helium::OUTPUT_CF_A8:
      return  0x000000FF;
  }

  return 0;
}

static uint32_t  GetBitCount(Helium::OutputColorFormat format)
{
  switch(format)
  {
    case  Helium::OUTPUT_CF_DXT1:
    case  Helium::OUTPUT_CF_DXT3:
    case  Helium::OUTPUT_CF_DXT5:
    case  Helium::OUTPUT_CF_ARGB8888:
    case  Helium::OUTPUT_CF_F32:
    case  Helium::OUTPUT_CF_F16F16:
    case  Helium::OUTPUT_CF_RGBE:
      return  32;

    case  Helium::OUTPUT_CF_ARGB4444:
    case  Helium::OUTPUT_CF_ARGB1555:
    case  Helium::OUTPUT_CF_RGB565:
    case  Helium::OUTPUT_CF_AL88:
    case  Helium::OUTPUT_CF_DUDV:
    case  Helium::OUTPUT_CF_F16:
      return 16;

    case  Helium::OUTPUT_CF_A8:
    case  Helium::OUTPUT_CF_L8:
      return 8;

    case  Helium::OUTPUT_CF_F32F32:
    case  Helium::OUTPUT_CF_HALFMAP:
      return 64;

    case  Helium::OUTPUT_CF_FLOATMAP:
      return  128;
  }
  HELIUM_ASSERT(!"WTF");
  return 8;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetFourCCPixelSize()
//
// Returns bits per pixel for 4cc pixel formats that can be found in DDS files
//
////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t GetFourCCPixelSize(uint32_t cc)
{
  switch (cc)
  {
    case Helium::DDS_CC_D3DFMT_DXT1:
      return 4;

    case Helium::DDS_CC_D3DFMT_DXT2:
    case Helium::DDS_CC_D3DFMT_DXT3:
    case Helium::DDS_CC_D3DFMT_DXT4:
    case Helium::DDS_CC_D3DFMT_DXT5:
      return 8;

    case Helium::DDS_CC_D3DFMT_R16F:
      return 16;

    case Helium::DDS_CC_D3DFMT_G16R16F:
    case Helium::DDS_CC_D3DFMT_R32F:
      return 32;

    case Helium::DDS_CC_D3DFMT_G32R32F:
    case Helium::DDS_CC_D3DFMT_A16B16G16R16F:
      return 64;

    case Helium::DDS_CC_D3DFMT_A32B32G32R32F:
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
static Helium::ColorFormat GetDirectlyCompatibleColorFormat(Helium::DDSPixelFormat* pf)
{
  // if the 4 CC code of pixel format is set then use it to figure out the format.
  // if the 4 CC code is not set then the format has to be an RGB format, use the pixel format
  // to figure the RGB layout
  if (pf->m_flags & Helium::DDS_PF_FLAGS_FOURCC)
  {
    switch (pf->m_fourcc)
    {
    case Helium::DDS_CC_D3DFMT_A16B16G16R16F:
      return Helium::CF_RGBAHALFMAP;
    case Helium::DDS_CC_D3DFMT_R32F:
      return Helium::CF_F32;
    case Helium::DDS_CC_D3DFMT_A32B32G32R32F:
      return Helium::CF_RGBAFLOATMAP;
    }
  }
  else if ( (pf->m_flags & Helium::DDS_PF_FLAGS_RGB) !=0 )
  {
    // This is an RGB format, may also have alpha
    if (pf->m_bit_count==16)
    {
      if ((pf->m_red_mask == 0x0f00) && (pf->m_green_mask == 0x00f0) && (pf->m_blue_mask == 0x000f))
        return Helium::CF_ARGB4444;

      if ((pf->m_red_mask == 0xf800) && (pf->m_green_mask == 0x07e0) && (pf->m_blue_mask == 0x001f))
        return Helium::CF_RGB565;

      if ((pf->m_red_mask == 0x7c00) && (pf->m_green_mask == 0x03e0) && (pf->m_blue_mask == 0x001f))
        return Helium::CF_ARGB1555;
    }
    else if (pf->m_bit_count==32)
    {
      if (pf->m_red_mask != 0x00ff0000)
        return Helium::CF_UNKNOWN;
      if (pf->m_green_mask != 0x0000ff00)
        return Helium::CF_UNKNOWN;
      if (pf->m_blue_mask != 0x000000ff)
        return Helium::CF_UNKNOWN;

      return Helium::CF_ARGB8888;
    }
  }
  else if (pf->m_flags & DDS_PF_FLAGS_ALPHA_ONLY)
  {
    if (pf->m_alpha_mask == 0xff)
      return Helium::CF_A8;
  }
  else if (pf->m_flags & DDS_PF_LUMINANCE)
  {
    if (pf->m_bit_count==8)
    {
      if ((pf->m_red_mask == 0xff)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0x00))
        return Helium::CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0xff) && (pf->m_blue_mask == 0x00))
        return Helium::CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0xff))
        return Helium::CF_L8;
    }
    else if (pf->m_bit_count==16)
    {
      if (pf->m_flags & DDS_PF_FLAGS_ALPHA)
      {
        if ((pf->m_red_mask == 0x000000ff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0) && (pf->m_alpha_mask == 0xff00))
          return Helium::CF_AL88;
      }

      if ((pf->m_red_mask == 0x0000ffff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0))
        return Helium::CF_L16;

      if ((pf->m_red_mask == 0x0000)  && (pf->m_green_mask == 0xffff) && (pf->m_blue_mask == 0x0000))
        return Helium::CF_L16;

      if ((pf->m_red_mask == 0x0000)  && (pf->m_green_mask == 0x0000) && (pf->m_blue_mask == 0xffff))
        return Helium::CF_L16;
    }
  }

  // do not know what this format is
  return Helium::CF_UNKNOWN;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetOutputCompatibleColorFormat()
//
// Converts the DDS pixel format to a compatible ouput pixel format. If there is no
// direct mapping this function returns CF_UNKNOWN and the caller can decide what to do.
//
////////////////////////////////////////////////////////////////////////////////////////////////
static Helium::OutputColorFormat GetOutputCompatibleColorFormat(Helium::DDSPixelFormat* pf)
{
  // if the 4 CC code of pixel format is set then use it to figure out the format.
  // if the 4 CC code is not set then the format has to be an RGB format, use the pixel format
  // to figure the RGB layout
  if (pf->m_flags & Helium::DDS_PF_FLAGS_FOURCC)
  {
    switch (pf->m_fourcc)
    {
    case Helium::DDS_CC_D3DFMT_A16B16G16R16F:
      return Helium::OUTPUT_CF_HALFMAP;
    case Helium::DDS_CC_D3DFMT_R32F:
      return Helium::OUTPUT_CF_F32;
    case Helium::DDS_CC_D3DFMT_A32B32G32R32F:
      return Helium::OUTPUT_CF_FLOATMAP;
    case Helium::DDS_CC_D3DFMT_DXT1:
      return Helium::OUTPUT_CF_DXT1;
    case Helium::DDS_CC_D3DFMT_DXT3:
      return Helium::OUTPUT_CF_DXT3;
    case Helium::DDS_CC_D3DFMT_DXT5:
      return Helium::OUTPUT_CF_DXT5;
    }
  }
  else if ( (pf->m_flags & Helium::DDS_PF_FLAGS_RGB) !=0 )
  {
    // This is an RGB format, may also have alpha
    if (pf->m_bit_count==16)
    {
      if ((pf->m_red_mask == 0x0f00) && (pf->m_green_mask == 0x00f0) && (pf->m_blue_mask == 0x000f))
        return Helium::OUTPUT_CF_ARGB4444;

      if ((pf->m_red_mask == 0xf800) && (pf->m_green_mask == 0x07e0) && (pf->m_blue_mask == 0x001f))
        return Helium::OUTPUT_CF_RGB565;

      if ((pf->m_red_mask == 0x7c00) && (pf->m_green_mask == 0x03e0) && (pf->m_blue_mask == 0x001f))
        return Helium::OUTPUT_CF_ARGB1555;
    }
    else if (pf->m_bit_count==32)
    {
      if (pf->m_red_mask != 0x00ff0000)
        return Helium::OUTPUT_CF_UNKNOWN;
      if (pf->m_green_mask != 0x0000ff00)
        return Helium::OUTPUT_CF_UNKNOWN;
      if (pf->m_blue_mask != 0x000000ff)
        return Helium::OUTPUT_CF_UNKNOWN;

      return Helium::OUTPUT_CF_ARGB8888;
    }
  }
  else if (pf->m_flags & DDS_PF_FLAGS_ALPHA_ONLY)
  {
    if (pf->m_alpha_mask == 0xff)
      return Helium::OUTPUT_CF_A8;
  }
  else if (pf->m_flags & DDS_PF_LUMINANCE)
  {
    if (pf->m_bit_count==8)
    {
      if ((pf->m_red_mask == 0xff)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0x00))
        return Helium::OUTPUT_CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0xff) && (pf->m_blue_mask == 0x00))
        return Helium::OUTPUT_CF_L8;

      if ((pf->m_red_mask == 0x00)  && (pf->m_green_mask == 0x00) && (pf->m_blue_mask == 0xff))
        return Helium::OUTPUT_CF_L8;
    }
    else if (pf->m_bit_count==16)
    {
      if (pf->m_flags & DDS_PF_FLAGS_ALPHA)
      {
        if ((pf->m_red_mask == 0x000000ff)  && (pf->m_green_mask == 00) && (pf->m_blue_mask == 0x0) && (pf->m_alpha_mask == 0xff00))
          return Helium::OUTPUT_CF_AL88;
      }
    }
  }

  // do not know what this format is
  return Helium::OUTPUT_CF_UNKNOWN;
}




////////////////////////////////////////////////////////////////////////////////////////////////
//
// GetBitsPerPixel()
//
// Get the bits per pixel of the pixels in the DDS files
//
////////////////////////////////////////////////////////////////////////////////////////////////
static uint32_t GetBitsPerPixel(Helium::DDSPixelFormat* pf)
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
uint32_t CalculateImageAndMipSize(uint32_t pixel_size,uint32_t width,uint32_t height,uint32_t depth,uint32_t mip_count)
{
  if (depth==0)
    depth=1;

  uint32_t size = 0;
  for (uint32_t m=0;m<mip_count;m++)
  {
    uint32_t line_stride = GetLineStride(pixel_size, width);
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
bool IsDXTC(Helium::DDSPixelFormat* pf)
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
uint32_t CalculateImageSize(Helium::DDSPixelFormat* pf,uint32_t width,uint32_t height,uint32_t depth)
{
  if (depth==0)
    depth=1;

  uint32_t bits = GetBitsPerPixel(pf);
  if (IsDXTC(pf))
  {
    return (MAX(1,width>>2)*MAX(1,height>>2)*bits*16)>>3;
  }

  uint32_t line_stride = GetLineStride(bits, width);

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
Image* Image::LoadDDS(const void* ddsadr, bool convert_to_linear)
{
  DDSHeader* header = (DDSHeader*)ddsadr;

  uint32_t width;
  uint32_t height;
  uint32_t depth;
  uint32_t line_stride;
  uint32_t levels;
  uint32_t pixel_size;
  uint8_t* image_data = (uint8_t*)(header+1);

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
    uint32_t mask = DDS_CAPS2_CUBEMAP_POSX;
    for (uint32_t f=0;f<6;f++)
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

  Image*  result     = new Image(width,height,depth,fmt);
  uint32_t       image_size = CalculateImageAndMipSize(pixel_size,width,height,depth,levels);
  uint32_t d = depth;

  if (d==0)
  {
    d = 1;
  }

  for (uint32_t f=0;f<6;f++)
  {
    if (faces[f])
    {
      uint8_t* face_data = image_data;

      if (fmt == CF_UNKNOWN)
      {
        uint32_t flags;
        if (header->m_pixel_format.m_fourcc == Helium::DDS_CC_D3DFMT_DXT1)
        {
          flags = squish::kDxt1;
        }
        else if (header->m_pixel_format.m_fourcc == Helium::DDS_CC_D3DFMT_DXT3)
        {
          flags = squish::kDxt3;
        }
        else if (header->m_pixel_format.m_fourcc == Helium::DDS_CC_D3DFMT_DXT5)
        {
          flags = squish::kDxt5;
        }
        else
        {
          //Unsupported format
          delete result;
          return NULL;
        }

        uint8_t* src_rgba  = new uint8_t[width*height*4];
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
MipSet* Image::LoadDDSToMipSet(const void* ddsadr)
{
  DDSHeader* header = (DDSHeader*)ddsadr;

  uint32_t levels=1;
  uint32_t pixel_size;
  uint8_t* image_data = (uint8_t*)(header+1);

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
    mips->m_texture_type = Image::CUBE;

    // scan the flags to see which cube faces are present
    uint32_t mask = DDS_CAPS2_CUBEMAP_POSX;
    for (uint32_t f=0;f<6;f++)
    {
      faces[f] = (header->m_caps.m_caps2 & mask)!=0;
      mask<<=1;
    }
  }
  else if (header->m_caps.m_caps2 & DDS_CAPS2_VOLUME)
  {
    mips->m_depth = 1;
    mips->m_texture_type = Image::VOLUME;
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
    mips->m_texture_type = Image::REGULAR;

    // 2D map
    faces[0] = true;
    faces[1] = false;
    faces[2] = false;
    faces[3] = false;
    faces[4] = false;
    faces[5] = false;
  }


  for (uint32_t f=0;f<6;f++)
  {
    uint8_t* face_data = image_data;
    if (faces[f])
    {
      uint32_t w  = mips->m_width;
      uint32_t h  = mips->m_height;
      uint32_t d = mips->m_depth;
      if (d==0)
        d=1;

      for (uint32_t level=0;level<levels;level++)
      {
        uint32_t bytes = CalculateImageSize(&header->m_pixel_format,w,h,d);
        mips->m_datasize[level] = bytes;
        mips->m_levels[f][level].m_width = w;
        mips->m_levels[f][level].m_height = h;
        mips->m_levels[f][level].m_depth = d;
        mips->m_levels[f][level].m_data = new uint8_t[bytes];

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
bool Helium::MipSet::WriteDDS(const tchar_t* fname) const
{
  FILE * file = _tfopen(fname, TXT( "wb" ));
  if (!file)
  {
    return false;
  }

  Helium::DDSHeader header;
  memset(&header, 0, sizeof(header));

  header.m_magic  = Helium::DDS_MAGIC;
  header.m_size   = 124;

  header.m_flags |= Helium::DDS_FLAGS_CAPS        |
                    Helium::DDS_FLAGS_HEIGHT      |
                    Helium::DDS_FLAGS_WIDTH       |
                    Helium::DDS_FLAGS_PIXELFORMAT |
                    Helium::DDS_FLAGS_MIPMAPCOUNT;

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
  header.m_caps.m_caps1 = Helium::DDS_CAPS1_TEXTURE;

  if(m_levels_used > 1)
  {
    header.m_caps.m_caps1 |= (Helium::DDS_CAPS1_MIPMAPS | Helium::DDS_CAPS1_COMPLEX);
  }

  if(m_texture_type != Helium::Image::REGULAR)
  {
    header.m_caps.m_caps1 |= Helium::DDS_CAPS1_COMPLEX;
  }

  if(m_texture_type == Helium::Image::CUBE)
  {
    header.m_caps.m_caps2 = Helium::DDS_CAPS2_CUBEMAP |
                            Helium::DDS_CAPS2_CUBEMAP_POSX |
                            Helium::DDS_CAPS2_CUBEMAP_NEGX |
                            Helium::DDS_CAPS2_CUBEMAP_POSY |
                            Helium::DDS_CAPS2_CUBEMAP_NEGY |
                            Helium::DDS_CAPS2_CUBEMAP_POSZ |
                            Helium::DDS_CAPS2_CUBEMAP_NEGZ;
  }
  else if(m_texture_type == Helium::Image::VOLUME)
  {
    header.m_caps.m_caps2 = Helium::DDS_CAPS2_VOLUME;
  }

  fwrite(&header, sizeof(header), 1, file);

  uint32_t num_levels = m_levels_used;

  for(uint32_t face = 0; face < 6; ++face)
  {
    for(uint32_t level = 0; level < num_levels; ++level)
    {
      const Helium::MipSet::MipInfo*  face_data       = &m_levels[face][level];
      const uint32_t                   dds_linear_size = GetLinearSize(m_format, face_data->m_width, face_data->m_height);
      fwrite(face_data->m_data, dds_linear_size, 1, file);
    }

    if(m_texture_type != Helium::Image::CUBE)
    {
      break;
    }
  }

  fclose(file);

  return true;
}

//Write DDS file
bool Helium::Image::WriteDDS(const tchar_t* fname, bool convert_to_srgb, Helium::OutputColorFormat output_fmt) const
{
  //If we don't sepecify an output format, pick a suitable one
  if(output_fmt == Helium::OUTPUT_CF_UNKNOWN)
  {
    switch(m_NativeFormat)
    {
      case  Helium::CF_ARGB8888:        output_fmt = Helium::OUTPUT_CF_ARGB8888;  break;
      case  Helium::CF_ARGB4444:        output_fmt = Helium::OUTPUT_CF_ARGB4444;  break;
      case  Helium::CF_ARGB1555:        output_fmt = Helium::OUTPUT_CF_ARGB1555;  break;
      case  Helium::CF_RGB565:          output_fmt = Helium::OUTPUT_CF_RGB565;    break;
      case  Helium::CF_A8:              output_fmt = Helium::OUTPUT_CF_A8;        break;
      case  Helium::CF_L8:              output_fmt = Helium::OUTPUT_CF_L8;        break;
      case  Helium::CF_AL88:            output_fmt = Helium::OUTPUT_CF_AL88;      break;
      case  Helium::CF_F32:             output_fmt = Helium::OUTPUT_CF_F32;       break;
      case  Helium::CF_F32F32:          output_fmt = Helium::OUTPUT_CF_F32F32;    break;
      case  Helium::CF_RGBAFLOATMAP:    output_fmt = Helium::OUTPUT_CF_FLOATMAP;  break;
      case  Helium::CF_F16:             output_fmt = Helium::OUTPUT_CF_F16;       break;
      case  Helium::CF_F16F16:          output_fmt = Helium::OUTPUT_CF_F16F16;    break;
      case  Helium::CF_RGBAHALFMAP:     output_fmt = Helium::OUTPUT_CF_HALFMAP;   break;
      case  Helium::CF_RGBE:            output_fmt = Helium::OUTPUT_CF_RGBE;      break;
    }
  }

  if((output_fmt == Helium::OUTPUT_CF_UNKNOWN) || (output_fmt == Helium::OUTPUT_CF_DUDV))
  {
    HELIUM_ASSERT(!"Unkown or unspported DDS output format");
    return false;
  }

  //Reuse some of the tools functionality instead of writing more code!
  Helium::MipGenOptions mip_gen_options;
  Helium::MipSet::RuntimeSettings runtime;

  mip_gen_options.m_Levels        = 1;                     //We're only interested in the top mip
  mip_gen_options.m_PostFilter    = Helium::IMAGE_FILTER_NONE; //Unused
  mip_gen_options.m_Filter        = Helium::MIP_FILTER_POINT;  //Unused
  mip_gen_options.m_OutputFormat  = output_fmt;
  mip_gen_options.m_ConvertToSrgb = convert_to_srgb;

  //Generate the mipset
  Helium::MipSet* mip_set = GenerateMipSet(mip_gen_options, runtime);

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