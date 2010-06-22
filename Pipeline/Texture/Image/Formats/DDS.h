#pragma once

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Nocturnal
{
  const u32 DDS_MAGIC                   =0x20534444;

  const u32 DDS_FLAGS_CAPS              =0x00000001;
  const u32 DDS_FLAGS_HEIGHT            =0x00000002;
  const u32 DDS_FLAGS_WIDTH             =0x00000004;
  const u32 DDS_FLAGS_PITCH             =0x00000008;
  const u32 DDS_FLAGS_ALPHABITDEPTH     =0x00000080;
  const u32 DDS_FLAGS_PIXELFORMAT       =0x00001000;
  const u32 DDS_FLAGS_MIPMAPCOUNT       =0x00020000;
  const u32 DDS_FLAGS_LINEARSIZE        =0x00080000;
  const u32 DDS_FLAGS_DEPTH             =0x00800000;

  const u32 DDS_FLAGS_REQUIRED          =0x00000007;

  const u32 DDS_PF_FLAGS_ALPHA          =0x00000001;
  const u32 DDS_PF_FLAGS_ALPHA_ONLY     =0x00000002;
  const u32 DDS_PF_FLAGS_FOURCC         =0x00000004;
  const u32 DDS_PF_FLAGS_RGB            =0x00000040;
  const u32 DDS_PF_LUMINANCE            =0x00020000;

  const u32 DDS_CAPS1_TEXTURE           =0x00001000;
  const u32 DDS_CAPS1_MIPMAPS           =0x00400000;
  const u32 DDS_CAPS1_COMPLEX						=0x00000008;

  const u32 DDS_CAPS2_CUBEMAP           =0x00000200;
  const u32 DDS_CAPS2_CUBEMAP_POSX      =0x00000400;
  const u32 DDS_CAPS2_CUBEMAP_NEGX      =0x00000800;
  const u32 DDS_CAPS2_CUBEMAP_POSY      =0x00001000;
  const u32 DDS_CAPS2_CUBEMAP_NEGY      =0x00002000;
  const u32 DDS_CAPS2_CUBEMAP_POSZ      =0x00004000;
  const u32 DDS_CAPS2_CUBEMAP_NEGZ      =0x00008000;
  const u32 DDS_CAPS2_VOLUME            =0x00200000;

  // These 4cc codes match the D3D format ID
  const u32 DDS_CC_D3DFMT_R16F          = 111;
  const u32 DDS_CC_D3DFMT_G16R16F       = 112;
  const u32 DDS_CC_D3DFMT_A16B16G16R16F = 113;
  const u32 DDS_CC_D3DFMT_R32F          = 114;
  const u32 DDS_CC_D3DFMT_G32R32F       = 115;
  const u32 DDS_CC_D3DFMT_A32B32G32R32F = 116;
  const u32 DDS_CC_D3DFMT_DXT1          = 0x31545844;   //'DXT1'
  const u32 DDS_CC_D3DFMT_DXT2          = 0x32545844;   //'DXT2'
  const u32 DDS_CC_D3DFMT_DXT3          = 0x33545844;   //'DXT3'
  const u32 DDS_CC_D3DFMT_DXT4          = 0x34545844;   //'DXT4'
  const u32 DDS_CC_D3DFMT_DXT5          = 0x35545844;   //'DXT5'

  struct DDSPixelFormat
  {
    u32                       m_size;
    u32                       m_flags;
    u32                       m_fourcc;
    u32                       m_bit_count;
    u32                       m_red_mask;
    u32                       m_green_mask;
    u32                       m_blue_mask;
    u32                       m_alpha_mask;
  };

  struct DDSCaps
  {
    u32                       m_caps1;
    u32                       m_caps2;
    u32                       m_reserved[2];
  };

  struct DDSHeader
  {
    u32                       m_magic;
    u32                       m_size;
    u32                       m_flags;
    u32                       m_height;
    u32                       m_width;
    union
    {
      u32                     m_pitch;
      u32                     m_data_size;
    };
    u32                       m_depth;
    u32                       m_mip_count;
    u32                       m_unused[11];
    DDSPixelFormat            m_pixel_format;
    DDSCaps                   m_caps;
    u32                       m_reserved;
  };

  struct DDSMipMap
  {
    u32                       m_width;
    u32                       m_height;
    std::vector<u8>           m_data;

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