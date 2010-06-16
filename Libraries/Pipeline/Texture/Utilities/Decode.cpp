#include "Decode.h"
#include "Pipeline/Texture/Image/Formats/DXT.h"

#include "Foundation/Exception.h"

#include <squish.h>

using namespace Nocturnal;

DecodeMips::DecodeMips(MipSet* mips)
{
  m_levels = 0;
  for (u32 i=0;i<MAX_TEXTURE_MIPS;i++)
  {
    m_images[i] = 0;
  }

  // decode the specified mips set
  Decode(mips);
}

DecodeMips::~DecodeMips()
{
  for (u32 i=0;i<m_levels;i++)
  {
    delete m_images[i];
  }
}

void DecodeMips::Decode(MipSet* mips)
{
  m_levels = mips->m_levels_used;

  for (u32 i=0;i<m_levels;i++)
  {
    if (mips->m_levels[0][i].m_data)
    {
      u32 width = mips->m_levels[0][i].m_width;
      u32 height = mips->m_levels[0][i].m_height;
      m_images[i] = new Texture( width,height,CF_ARGB8888);

      ColorFormat cf = CompatibleColorFormat(mips->m_format);
      if (cf==CF_UNKNOWN)
      {
        if (mips->m_format == OUTPUT_CF_DUDV)
        {
          // we need to convert DUDV back into a 3 byte vector
          i8*   src_data  = (i8*)mips->m_levels[0][i].m_data;
          f32*  r_ptr     = m_images[i]->m_Channels[0][Nocturnal::Texture::R];
          f32*  g_ptr     = m_images[i]->m_Channels[0][Nocturnal::Texture::G];
          f32*  b_ptr     = m_images[i]->m_Channels[0][Nocturnal::Texture::B];
          f32*  a_ptr     = m_images[i]->m_Channels[0][Nocturnal::Texture::A];

          for (u32 p=0;p<width*height;p++)
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
          u32 flags = squish::kDxt1;
          if (mips->m_format == OUTPUT_CF_DXT3)
          {
            flags = squish::kDxt3;
          }
          else if (mips->m_format == OUTPUT_CF_DXT5)
          {
            flags = squish::kDxt5;
          }

          u8* src_data  = (u8*)mips->m_levels[0][i].m_data;
          u8* src_rgba  = new u8[m_images[i]->m_Width*m_images[i]->m_Height*4];
          squish::DecompressImage(src_rgba, m_images[i]->m_Width,  m_images[i]->m_Height,  src_data, flags);
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
