#include "PipelinePch.h"
#include "MipSet.h"

#include "Pipeline/Image/Utilities/Swizzle.h"
#include "Pipeline/Image/Formats/DXT.h"

#include "Platform/Exception.h"

#include "Foundation/Profile.h" 
#include "Foundation/Log.h"
#include "Math/FpuVector4.h"

#include "Pipeline/Image/Formats/DDS.h"

using namespace Helium;

////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
Helium::MipSet::MipSet()
{
  m_levels_used = 0;
  for (int i=0;i<MAX_TEXTURE_MIPS;i++)
  {
    m_levels[0][i].m_data = 0;
    m_levels[1][i].m_data = 0;
    m_levels[2][i].m_data = 0;
    m_levels[3][i].m_data = 0;
    m_levels[4][i].m_data = 0;
    m_levels[5][i].m_data = 0;
    m_datasize[i] = 0;
  }

  m_swizzled = false;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////
Helium::MipSet::~MipSet()
{
  for (uint32_t i=0;i<MAX_TEXTURE_MIPS;i++)
  {
    delete [] m_levels[0][i].m_data;
    delete [] m_levels[1][i].m_data;
    delete [] m_levels[2][i].m_data;
    delete [] m_levels[3][i].m_data;
    delete [] m_levels[4][i].m_data;
    delete [] m_levels[5][i].m_data;
  }
}

/////////////////////////////////////////////////////////
// RuntimeSettings constructor
///////////////////////////////////////////////////////
MipSet::RuntimeSettings::RuntimeSettings()
{
  m_wrap_u = UV_WRAP;
  m_wrap_v = UV_WRAP;
  m_wrap_w = UV_WRAP;
  m_filter = FILTER_LINEAR_LINEAR_MIP;

  m_alpha_channel = COLOR_CHANNEL_DEFAULT;
  m_red_channel = COLOR_CHANNEL_DEFAULT;
  m_green_channel = COLOR_CHANNEL_DEFAULT;
  m_blue_channel = COLOR_CHANNEL_DEFAULT;

  m_alpha_signed = false;
  m_red_signed = false;
  m_green_signed = false;
  m_blue_signed = false;

  m_direct_uvs = false;
  m_expand_range = false;
  m_srgb_expand_a = false;
  m_srgb_expand_r = false;
  m_srgb_expand_g = false;
  m_srgb_expand_b = false;

  m_mip_bias = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Comapre two runtime settings for equality,
////////////////////////////////////////////////////////////////////////////////////////////////
bool MipSet::RuntimeSettings::operator == (const RuntimeSettings& rhs)
{
  if (m_wrap_u != rhs.m_wrap_u)
    return false;
  if (m_wrap_v != rhs.m_wrap_v)
    return false;
  if (m_wrap_w != rhs.m_wrap_w)
    return false;
  if (m_filter != rhs.m_filter)
    return false;

  if (m_alpha_channel != rhs.m_alpha_channel)
    return false;
  if (m_red_channel != rhs.m_red_channel)
    return false;
  if (m_green_channel != rhs.m_green_channel)
    return false;
  if (m_blue_channel != rhs.m_blue_channel)
    return false;

  if (m_alpha_signed != rhs.m_alpha_signed)
    return false;
  if (m_red_signed != rhs.m_red_signed)
    return false;
  if (m_green_signed != rhs.m_green_signed)
    return false;
  if (m_blue_signed != rhs.m_blue_signed)
    return false;

  if (m_direct_uvs != rhs.m_direct_uvs)
    return false;
  if (m_expand_range != rhs.m_expand_range)
    return false;
  if (m_srgb_expand_a != rhs.m_srgb_expand_a)
    return false;
  if (m_srgb_expand_r != rhs.m_srgb_expand_r)
    return false;
  if (m_srgb_expand_g != rhs.m_srgb_expand_g)
    return false;
  if (m_srgb_expand_b != rhs.m_srgb_expand_b)
    return false;

  if (m_mip_bias != rhs.m_mip_bias)
    return false;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// RemoveFromTail
//
// Removes the specified number of mips from the tail of the mip set (removes the N smallest mips).
// There has to be at least a single mip level remaining after levels have been removed.
////////////////////////////////////////////////////////////////////////////////////////////////
bool Helium::MipSet::RemoveFromTail(uint32_t levels)
{
  if (levels>=m_levels_used)
  {
    // cannot remove all the mip levels
    return false;
  }

  // remove the mips
  for (uint32_t l=m_levels_used-levels;l<m_levels_used;l++)
  {
    for (uint32_t m=0;m<6;m++)
    {
      delete [] m_levels[m][l].m_data;
      m_levels[m][l].m_data = 0;
    }
  }

  m_levels_used-=levels;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// RemoveFromHead
//
// Removes the specified number of mips from the head of the mip set (removes the N biggest mips).
// There has to be at least a single mip level remaining after levels have been removed.
////////////////////////////////////////////////////////////////////////////////////////////////
bool Helium::MipSet::RemoveFromHead(uint32_t levels)
{
  if (levels>=m_levels_used)
  {
    // cannot remove all the mip levels
    return false;
  }

  // delete the top mips
  for (uint32_t l=0;l<levels;l++)
  {
    for (uint32_t m=0;m<6;m++)
    {
      delete [] m_levels[m][l].m_data;
    }
  }

  // move the mips up N positions
  for (uint32_t l=levels;l<m_levels_used;l++)
  {
    for (uint32_t m=0;m<6;m++)
    {
      m_levels[m][l-levels] = m_levels[m][l];
      m_levels[m][l].m_data = 0;

      m_datasize[l-levels] = m_datasize[l];
    }
  }

  m_levels_used-=levels;

  // adjust the top size within the class
  m_width>>=levels;
  m_height>>=levels;
  m_depth>>=levels;
  m_width = MAX(m_width,1);
  m_height = MAX(m_height,1);
  m_depth = MAX(m_depth,1);

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Apply the PS3 swizzle to data. The swizzle is format specific and in some cases the format
// does not require swizzling and in these cases this function does nothing.
//
// DO NOT CALL THIS FOR PC TEXTURES
////////////////////////////////////////////////////////////////////////////////////////////////
bool Helium::MipSet::Swizzle()
{
  // do not attempt to swizzle if it already is swizzled
  if (m_swizzled)
    return true;

  // Special handling for volume textures
  if(m_depth > 1)
  {
    // none power of 2 textures cannot be swizzled
    if(!IsPowerOfTwo(m_depth))
    {
      m_swizzled = false;
    }
    // Do VTC swizzling for DXT compressed volume textures
    else if(  (m_format == Helium::OUTPUT_CF_DXT1) ||
              (m_format == Helium::OUTPUT_CF_DXT3) ||
              (m_format == Helium::OUTPUT_CF_DXT5) )
    {
      VTCSwizzle();
      m_swizzled = true;
    }
    else
    {
      m_swizzled = false;
    }

    // DXT textures can't go through normal swizzling
    return m_swizzled;
  }

  if (!IsPowerOfTwo(m_width) || !IsPowerOfTwo(m_height))
  {
    // none power of 2 textures cannot be swizzled
    m_swizzled = false;
    return false;
  }

  uint32_t bytes_per_pixel = (ColorFormatBits( CompatibleColorFormat( m_format ) )>>3);
  if (bytes_per_pixel==0)
  {
    // not a format that has a meaning full bytes per pixel
    m_swizzled = false;
    return false;
  }

  // go through all the mip levels and swizzle them, we might need to adjust the size while we
  // are doing it.
  for (uint32_t l=0;l<m_levels_used;l++)
  {
    for (uint32_t m=0;m<6;m++)
    {
      if (m_levels[m][l].m_data)
      {
        // this face is present, swizzle it
        uint8_t* swizzle_level = new uint8_t[m_datasize[l]];
        SwizzleBox(m_levels[m][l].m_data,swizzle_level,m_levels[m][l].m_width,m_levels[m][l].m_height,m_levels[m][l].m_depth,bytes_per_pixel);
        delete [] m_levels[m][l].m_data;
        m_levels[m][l].m_data = swizzle_level;
      }
    }
  }

  m_swizzled = true;
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool Helium::MipSet::VTCSwizzle()
{
  // Some assumptions made since we should only be called from Swizzle()
  HELIUM_ASSERT(!m_swizzled);
  HELIUM_ASSERT( (m_format == Helium::OUTPUT_CF_DXT1) || (m_format == Helium::OUTPUT_CF_DXT3) || (m_format == Helium::OUTPUT_CF_DXT5) );
  HELIUM_ASSERT(m_depth > 1);
  HELIUM_ASSERT_MSG(m_levels_used == 1, TXT("Mipmap support not yet implimented for volume textures"));

  uint8_t* p_data = m_levels[0][0].m_data;
  if(!p_data)
  {
    return false;
  }

  uint32_t block_width = m_width / 4;
  uint32_t block_height = m_height / 4;
  uint32_t num_layer_blocks = block_width * block_height;

  uint32_t block_size = (m_format == Helium::OUTPUT_CF_DXT1) ? 8 : 16;
  uint32_t layer_size = num_layer_blocks * block_size;

  uint32_t num_layer_sets = m_depth / 4;

  uint8_t* p_vtc = new uint8_t[m_datasize[0]];
  HELIUM_ASSERT(p_vtc);
  uint8_t* p_dest = p_vtc;
  for(uint32_t layer_set = 0; layer_set < num_layer_sets; layer_set++)
  {
    uint8_t* first_layer_offset = p_data + (layer_size * (layer_set * 4));
    uint8_t* p_src_1 = first_layer_offset;
    uint8_t* p_src_2 = first_layer_offset + (layer_size * 1);
    uint8_t* p_src_3 = first_layer_offset + (layer_size * 2);
    uint8_t* p_src_4 = first_layer_offset + (layer_size * 3);

    for(uint32_t block = 0; block < num_layer_blocks; block++)
    {
      memcpy(p_dest+(block_size*0), p_src_1, block_size);
      memcpy(p_dest+(block_size*1), p_src_2, block_size);
      memcpy(p_dest+(block_size*2), p_src_3, block_size);
      memcpy(p_dest+(block_size*3), p_src_4, block_size);
      p_src_1 += block_size;
      p_src_2 += block_size;
      p_src_3 += block_size;
      p_src_4 += block_size;
      p_dest  += (block_size*4);
    }
  }

  // Copy over leftover layers unswizzled
  //if( (num_layer_sets*4) < m_depth )
  uint32_t swizzled_size = (uint32_t)(p_dest - p_vtc);
  HELIUM_ASSERT( swizzled_size == (num_layer_sets * 4 * layer_size) );
  if( swizzled_size < m_datasize[0] )
  {
    uint32_t unswizzled_size = m_datasize[0] - swizzled_size;
    uint8_t* p_unswizzled_start = p_data + swizzled_size;
    memcpy(p_dest, p_unswizzled_start, unswizzled_size);
  }

  delete [] m_levels[0][0].m_data;
  m_levels[0][0].m_data = p_vtc;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool Helium::MipSet::ExtractNonePowerOfTwo(uint32_t width, uint32_t height, uint32_t depth)
{
  // we do not currently support extracting volume data
  if (m_texture_type==Helium::Image::VOLUME)
    return false;

  // we only support compressed
  if (m_format!=OUTPUT_CF_DXT1 && m_format!=OUTPUT_CF_DXT3 && m_format!=OUTPUT_CF_DXT5)
  {
    return false;
  }

  if (m_levels_used > 1)
  {
    return false;
  }

  uint32_t dst_x_blocks = ((width+3)&~3)>>2;
  uint32_t dst_y_blocks = ((height+3)&~3)>>2;
  uint32_t dst_stride = dst_x_blocks*((m_format==OUTPUT_CF_DXT1)?8:16);

  for (uint32_t f=0;f<6;f++)
  {
    if (m_levels[f][0].m_data)
    {
      uint32_t src_x_blocks = m_levels[f][0].m_width>>2;
      uint32_t src_y_blocks = m_levels[f][0].m_height>>2;
      uint32_t src_stride = src_x_blocks*16;
      if (m_format==OUTPUT_CF_DXT1)
        src_stride>>=1;

      uint8_t* dst = m_levels[f][0].m_data + dst_stride;
      uint8_t* src = m_levels[f][0].m_data + src_stride;
      for (uint32_t y=0;y<dst_y_blocks-1;y++)
      {
        memcpy(dst,src,dst_stride);
        dst+=dst_stride;
        src+=src_stride;
      }

      m_levels[f][0].m_width = width;
      m_levels[f][0].m_height = height;
      m_datasize[f] = dst_stride*dst_y_blocks;
    }
  }

  m_width = width;
  m_height = height;

  return true;
}
