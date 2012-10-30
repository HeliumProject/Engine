#include "ImagePch.h"
#include "Decode.h"
#include "Image/Formats/DXT.h"

#include "Platform/Exception.h"

#include <squish.h>

using namespace Helium;

DecodeMips::DecodeMips(MipSet* mips)
{
  m_levels = 0;
  for (uint32_t i=0;i<MAX_TEXTURE_MIPS;i++)
  {
    m_images[i] = 0;
  }

  // decode the specified mips set
  Decode(mips);
}

DecodeMips::~DecodeMips()
{
  for (uint32_t i=0;i<m_levels;i++)
  {
    delete m_images[i];
  }
}

void DecodeMips::Decode(MipSet* mips)
{
  m_levels = mips->m_levels_used;

  for (uint32_t i=0;i<m_levels;i++)
  {
    if (mips->m_levels[0][i].m_data)
    {
      uint32_t width = mips->m_levels[0][i].m_width;
      uint32_t height = mips->m_levels[0][i].m_height;
      m_images[i] = new Image( width,height,CF_ARGB8888);

      ColorFormat cf = CompatibleColorFormat(mips->m_format);
      if (cf==CF_UNKNOWN)
      {
        if (mips->m_format == OUTPUT_CF_DUDV)
        {
          // we need to convert DUDV back into a 3 byte vector
          int8_t*   src_data  = (int8_t*)mips->m_levels[0][i].m_data;
          float32_t*  r_ptr     = m_images[i]->m_Channels[0][Helium::Image::R];
          float32_t*  g_ptr     = m_images[i]->m_Channels[0][Helium::Image::G];
          float32_t*  b_ptr     = m_images[i]->m_Channels[0][Helium::Image::B];
          float32_t*  a_ptr     = m_images[i]->m_Channels[0][Helium::Image::A];

          for (uint32_t p=0;p<width*height;p++)
          {
            float nx = ((float)src_data[0])/128.0f;
            float ny = ((float)src_data[1])/128.0f;
            float nz = 1.0f-sqrtf(nx*nx+ny*ny);
            src_data+=2;

            *r_ptr++ = ((nz+1.0f)*0.5f);      // b = z
            *g_ptr++ = ((ny+1.0f)*0.5f);      // g = y
            *b_ptr++ = ((nx+1.0f)*0.5f);      // r = x
            *a_ptr++ = 1.0f;
          }
        }
        else
        {
          uint32_t flags = nvsquish::kDxt1;
          if (mips->m_format == OUTPUT_CF_DXT3)
          {
            flags = nvsquish::kDxt3;
          }
          else if (mips->m_format == OUTPUT_CF_DXT5)
          {
            flags = nvsquish::kDxt5;
          }

          uint8_t* src_data  = (uint8_t*)mips->m_levels[0][i].m_data;
          uint8_t* src_rgba  = new uint8_t[m_images[i]->m_Width*m_images[i]->m_Height*4];
          nvsquish::DecompressImage(src_rgba, m_images[i]->m_Width,  m_images[i]->m_Height,  src_data, flags);
          m_images[i]->FillFaceData(0, CF_ARGB8888, src_rgba);
          delete[] src_rgba;
        }
      }
      else
      {
        // all the other output formats are available via the color format conversions and thus we can directly
        // convert to RBGA
        m_images[i]->FillFaceData(0, cf, mips->m_levels[0][i].m_data);
      }
    }    
  }
}
