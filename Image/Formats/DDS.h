#pragma once

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
  const uint32_t DDS_MAGIC                   =0x20534444;

  const uint32_t DDS_FLAGS_CAPS              =0x00000001;
  const uint32_t DDS_FLAGS_HEIGHT            =0x00000002;
  const uint32_t DDS_FLAGS_WIDTH             =0x00000004;
  const uint32_t DDS_FLAGS_PITCH             =0x00000008;
  const uint32_t DDS_FLAGS_ALPHABITDEPTH     =0x00000080;
  const uint32_t DDS_FLAGS_PIXELFORMAT       =0x00001000;
  const uint32_t DDS_FLAGS_MIPMAPCOUNT       =0x00020000;
  const uint32_t DDS_FLAGS_LINEARSIZE        =0x00080000;
  const uint32_t DDS_FLAGS_DEPTH             =0x00800000;

  const uint32_t DDS_FLAGS_REQUIRED          =0x00000007;

  const uint32_t DDS_PF_FLAGS_ALPHA          =0x00000001;
  const uint32_t DDS_PF_FLAGS_ALPHA_ONLY     =0x00000002;
  const uint32_t DDS_PF_FLAGS_FOURCC         =0x00000004;
  const uint32_t DDS_PF_FLAGS_RGB            =0x00000040;
  const uint32_t DDS_PF_LUMINANCE            =0x00020000;

  const uint32_t DDS_CAPS1_TEXTURE           =0x00001000;
  const uint32_t DDS_CAPS1_MIPMAPS           =0x00400000;
  const uint32_t DDS_CAPS1_COMPLEX						=0x00000008;

  const uint32_t DDS_CAPS2_CUBEMAP           =0x00000200;
  const uint32_t DDS_CAPS2_CUBEMAP_POSX      =0x00000400;
  const uint32_t DDS_CAPS2_CUBEMAP_NEGX      =0x00000800;
  const uint32_t DDS_CAPS2_CUBEMAP_POSY      =0x00001000;
  const uint32_t DDS_CAPS2_CUBEMAP_NEGY      =0x00002000;
  const uint32_t DDS_CAPS2_CUBEMAP_POSZ      =0x00004000;
  const uint32_t DDS_CAPS2_CUBEMAP_NEGZ      =0x00008000;
  const uint32_t DDS_CAPS2_VOLUME            =0x00200000;

  // These 4cc codes match the D3D format ID
  const uint32_t DDS_CC_D3DFMT_R16F          = 111;
  const uint32_t DDS_CC_D3DFMT_G16R16F       = 112;
  const uint32_t DDS_CC_D3DFMT_A16B16G16R16F = 113;
  const uint32_t DDS_CC_D3DFMT_R32F          = 114;
  const uint32_t DDS_CC_D3DFMT_G32R32F       = 115;
  const uint32_t DDS_CC_D3DFMT_A32B32G32R32F = 116;
  const uint32_t DDS_CC_D3DFMT_DXT1          = 0x31545844;   //'DXT1'
  const uint32_t DDS_CC_D3DFMT_DXT2          = 0x32545844;   //'DXT2'
  const uint32_t DDS_CC_D3DFMT_DXT3          = 0x33545844;   //'DXT3'
  const uint32_t DDS_CC_D3DFMT_DXT4          = 0x34545844;   //'DXT4'
  const uint32_t DDS_CC_D3DFMT_DXT5          = 0x35545844;   //'DXT5'

  struct DDSPixelFormat
  {
    uint32_t                       m_size;
    uint32_t                       m_flags;
    uint32_t                       m_fourcc;
    uint32_t                       m_bit_count;
    uint32_t                       m_red_mask;
    uint32_t                       m_green_mask;
    uint32_t                       m_blue_mask;
    uint32_t                       m_alpha_mask;
  };

  struct DDSCaps
  {
    uint32_t                       m_caps1;
    uint32_t                       m_caps2;
    uint32_t                       m_reserved[2];
  };

  struct DDSHeader
  {
    uint32_t                       m_magic;
    uint32_t                       m_size;
    uint32_t                       m_flags;
    uint32_t                       m_height;
    uint32_t                       m_width;
    union
    {
      uint32_t                     m_pitch;
      uint32_t                     m_data_size;
    };
    uint32_t                       m_depth;
    uint32_t                       m_mip_count;
    uint32_t                       m_unused[11];
    DDSPixelFormat            m_pixel_format;
    DDSCaps                   m_caps;
    uint32_t                       m_reserved;
  };

  struct DDSMipMap
  {
    uint32_t                       m_width;
    uint32_t                       m_height;
    std::vector<uint8_t>           m_data;

    DDSMipMap()
      : m_width (0)
      , m_height (0)
    {

    }
  };

  struct DDSFace
  {
    std::vector<DDSMipMap>    m_mip_maps;
  };

  struct DDSFile
  {
    DDSHeader                 m_header;
    std::vector<DDSFace>      m_faces;

    DDSFile()
    {
      m_header.m_magic = DDS_MAGIC;
    }
  };
}