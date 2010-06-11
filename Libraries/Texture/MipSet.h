#pragma once

#include <math.h>
#include <string>
#include <vector>
#include <list>

#include "Platform/Types.h"

#include "API.h"
#include "ColorFormats.h"

namespace IG
{
  enum
  {
    MAX_TEXTURE_MIPS = 12,
  };

  enum
  {
    TEXTURE_CHANNEL_RED,
    TEXTURE_CHANNEL_GREEN,
    TEXTURE_CHANNEL_BLUE,
    TEXTURE_CHANNEL_ALPHA,

    TEXTURE_CHANNEL_NUM,
  };

  typedef float TexelType;

  const TexelType COLOR_MAP_DEFAULT_RED     = 0.5f;
  const TexelType COLOR_MAP_DEFAULT_GREEN   = 0.5f;
  const TexelType COLOR_MAP_DEFAULT_BLUE    = 0.5f;
  const TexelType COLOR_MAP_DEFAULT_ALPHA   = 1.0f;

  const TexelType NORMAL_MAP_DEFAULT_RED     = 0.5f;
  const TexelType NORMAL_MAP_DEFAULT_GREEN   = 0.5f;
  const TexelType NORMAL_MAP_DEFAULT_BLUE    = 1.0f;
  const TexelType NORMAL_MAP_DEFAULT_ALPHA   = 1.0f;

  const TexelType GPI_MAP_DEFAULT_RED     = 0.0f;
  const TexelType GPI_MAP_DEFAULT_GREEN   = 0.0f;
  const TexelType GPI_MAP_DEFAULT_BLUE    = 0.0f;
  const TexelType GPI_MAP_DEFAULT_ALPHA   = 1.0f;

  const TexelType D_MAP_DEFAULT_RED     = 0.5f;
  const TexelType D_MAP_DEFAULT_GREEN   = 0.5f;
  const TexelType D_MAP_DEFAULT_BLUE    = 0.5f;
  const TexelType D_MAP_DEFAULT_ALPHA   = 1.0f;

  const IG::ColorFormat TEXTURE_PROCESS_FORMAT = IG::CF_RGBAFLOATMAP;
  const TexelType BLACK_TEXEL = 0.0f;
  const TexelType WHITE_TEXEL = 1.0f;
  const TexelType COLOR_MAP_ALPHA_THRESHOLD = 0.95f;
  const TexelType AMBIENT_OCC_THRESHOLD = 0.95f;
  const TexelType INCAN_THRESHOLD = 0.05f;
  const TexelType PARALLAX_THRESHOLD = 0.05f;
  const TexelType TEX_CONST_THRESHOLD = 0.001f;

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Enum of filter types than can be used when generating mip levels or rescaling images
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  enum FilterType
  {
    MIP_FILTER_NONE,
    MIP_FILTER_POINT,
    MIP_FILTER_BOX,
    MIP_FILTER_TRIANGLE,
    MIP_FILTER_QUADRATIC,
    MIP_FILTER_CUBIC,
    MIP_FILTER_MITCHELL,
    MIP_FILTER_GAUSSIAN,
    MIP_FILTER_SINC,
    MIP_FILTER_KAISER,
    MIP_FILTER_POINT_COMPOSITE,
    MIP_FILTER_COUNT
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Enum of image processing operations that can be used after the mip maps are generated
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  enum PostMipImageFilter
  {
    IMAGE_FILTER_NONE,
    IMAGE_FILTER_LIGHTER,
    IMAGE_FILTER_DARKER,
    IMAGE_FILTER_MORE_CONTRAST,
    IMAGE_FILTER_LESS_CONTRAST,
    IMAGE_FILTER_SMOOTH,
    IMAGE_FILTER_SHARPEN_GRADUAL,
    IMAGE_FILTER_SHARPEN1X,
    IMAGE_FILTER_SHARPEN2X,
    IMAGE_FILTER_SHARPEN3X,
    IMAGE_FILTER_HIGH_PASS,
    IMAGE_FILTER_COUNT
  };

  // generic uv addressing modes
  enum UVAddressMode
  {
    UV_WRAP,
    UV_MIRROR,
    UV_CLAMP,
    UV_BORDER,
    UV_COUNT,
  };

  // generic runtime filtering modes
  enum TextureFilter
  {
    FILTER_POINT_SELECT_MIP,
    FILTER_LINEAR_SELECT_MIP,
    FILTER_LINEAR_LINEAR_MIP,
    FILTER_ANISO_2_SELECT_MIP,
    FILTER_ANISO_2_LINEAR_MIP,
    FILTER_ANISO_4_SELECT_MIP,
    FILTER_ANISO_4_LINEAR_MIP,
    FILTER_COUNT
  };

  enum ColorChannelMode
  {
    COLOR_CHANNEL_DEFAULT,        // the default is whatever the texture requires
    COLOR_CHANNEL_FORCE_ONE,      // force the channel to 1.0
    COLOR_CHANNEL_FORCE_ZERO,     // force the channel to 0.0
    COLOR_CHANNEL_GET_FROM_A,     // get the channel from alpha
    COLOR_CHANNEL_GET_FROM_R,     // get the channel from red
    COLOR_CHANNEL_GET_FROM_G,     // get the channel from green
    COLOR_CHANNEL_GET_FROM_B,     // get the channel from blue
  };
  ////////////////////////////////////////////////////////////////////////////////////////////////

  struct TEXTURE_API TextureGenerationSettings
  {
    //
    // Channels could be partitioned arbitrarily and processed separately before combining them for compression
    //
    TextureGenerationSettings() 
    : m_output_format(OUTPUT_CF_ARGB8888)
    , m_max_size(2048)
    , m_scale(1.0f) 
    , m_generate_mips(true)
    {
      //Defaults
      for(u32 c = 0; c < TEXTURE_CHANNEL_NUM; ++c)
      {
        m_image_filter[c]   = IMAGE_FILTER_NONE;
        m_mip_filter[c]     = MIP_FILTER_POINT;

        m_ifilter_cnt[c][0] = 0;
        for (u32 i=1;i<MAX_TEXTURE_MIPS;i++)
        {
          m_ifilter_cnt[c][i] = 1;
        }
      }
    }

    // The Filter to use when generating the mip maps (for now 2 are used since separate for color+alpha, AG and IP)
    IG::FilterType           m_mip_filter[TEXTURE_CHANNEL_NUM];

    // The image filter to apply after the mips have been generated (for now 2 are used since sepearate for color+alpha, AG and IP)
    IG::PostMipImageFilter   m_image_filter[TEXTURE_CHANNEL_NUM];

    // The number of times to apply the image filter to each mip level (for now 2 are used since sepearate for color+alpha, AG and IP)
    u32                      m_ifilter_cnt[TEXTURE_CHANNEL_NUM][MAX_TEXTURE_MIPS];

    // Output format for this texture
    IG::OutputColorFormat    m_output_format;

    // Tool time scale factor
    float                    m_scale;

    // Maximum size of the output texture, after the output scale factor has been applied
    u32                      m_max_size;

    bool                     m_generate_mips;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // CompareMipSettings()
    //
    // - runs through each channels mipsettings and calculates unique ones to process them together
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool CompareMipSettings(u32 id1, u32 id2) const
    {
      if (m_mip_filter[id1] != m_mip_filter[id2])
      {
        return false;
      }
      if (m_image_filter[id1] != m_image_filter[id2])
      {
        return false;
      }
      for (u32 i=0; i<MAX_TEXTURE_MIPS; i++)
      {
        if (m_ifilter_cnt[id1][i] != m_ifilter_cnt[id2][i])
        {
          return false;
        }
      }
      return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // AreMipSettingsEqual()
    //
    // - runs through each mipsettings and checks equality
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool AreMipSettingsEqual() const
    {
      u32 num_unique = 0;
      for (u32 i=1; i<TEXTURE_CHANNEL_NUM; i++)
      {
        for (u32 j=0; j<i; j++)
        {
          if (!CompareMipSettings(i, j))
          {
            return false;
          }
        }
      }
      return true;
    }
  };

  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  // Options to use when generating a mipset from a texture
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  struct TEXTURE_API MipGenOptions
  {
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructor to set suitable defaults
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipGenOptions()
      : m_Levels(0)
      , m_PostFilter(IMAGE_FILTER_NONE)
      , m_Filter(MIP_FILTER_TRIANGLE)
      , m_ConvertToSrgb(false)
      , m_OutputFormat(OUTPUT_CF_ARGB8888)
      , m_UAddressMode(UV_WRAP)
      , m_VAddressMode(UV_WRAP)
    {
      m_ApplyPostFilter[0]  = 0;
      for (u32 i=1;i<MAX_TEXTURE_MIPS;i++)
      {
        m_ApplyPostFilter[i]=1;
      }
    }

    u32                 m_Levels;
    OutputColorFormat   m_OutputFormat;
    PostMipImageFilter  m_PostFilter;
    UVAddressMode       m_UAddressMode;
    UVAddressMode       m_VAddressMode;
    FilterType          m_Filter;
    bool                m_ConvertToSrgb;
    u32                 m_ApplyPostFilter[MAX_TEXTURE_MIPS];
  };


  ////////////////////////////////////////////////////////////////////////////////////////////////
  //
  //  MipSet
  //
  //  Class to hold all the output for a given texture, this can be a 2D texture, 3D volume map
  //  or a cube environment map. In addition the format can be anything supported by the hardware
  //  including DXT formats.
  //
  //  There is no support to process MipSets in any way, the soley exist to be written to the
  //  final game files.
  //
  //  The pixels in this structure are packed line after line with no padding, the only exception
  //  is when the format specifies some sort of packing, such as DXTC formats which have to be
  //  compressed in 4X4 blocks. The datasize for each mip level is always correct and thus the
  //  datasize can be used for blindly copying a mip level.
  //
  //  The width and height in the mip set class is the width and height of the top level, each
  //  mip level has its own width and height. These are the width and height of the specific mip
  //  level as sometimes mips have to be a multiple of 2 or 4. If none power of 2 mips are used
  //  it can be difficult to calculate the actual size of a given mip level.
  //
  //  The mipset also contains the runtime settings for the texture, these settings are things
  //  like filter types, anisotropic, mip bias etc. Some of the settings are specific to the
  //  ps3 (linear/normalized UVs, swizzled vs linear) and are ignored by the PC.
  //
  ////////////////////////////////////////////////////////////////////////////////////////////////
  class TEXTURE_API MipSet
  {
  public:
    struct MipInfo
    {
      u32 m_width;
      u32 m_height;
      u32 m_depth;
      u8* m_data;
    };

    // currently this entire structure is ignored on the PC, all texture state is specified
    // in the shader and is managed at runtime
    struct TEXTURE_API RuntimeSettings
    {
      UVAddressMode           m_wrap_u;
      UVAddressMode           m_wrap_v;
      UVAddressMode           m_wrap_w;
      TextureFilter           m_filter;

      ColorChannelMode        m_alpha_channel;
      ColorChannelMode        m_red_channel;
      ColorChannelMode        m_green_channel;
      ColorChannelMode        m_blue_channel;

      // set to true if this channel contains 2's complement data
      bool                    m_alpha_signed;
      bool                    m_red_signed;
      bool                    m_green_signed;
      bool                    m_blue_signed;

      // set to true for linear textures
      bool                    m_direct_uvs;       // always unused on PC, all none material textures on PS3 set this to false

      // set to true if value is to be expanded to -1 to 1 automatically
      bool                    m_expand_range;

      // set to true to enable srgb conversion on lookup, not compatible with
      // signed channels or range expansion.
      bool                    m_srgb_expand_a;
      bool                    m_srgb_expand_r;
      bool                    m_srgb_expand_g;
      bool                    m_srgb_expand_b;

      float                   m_mip_bias;

      //constructor
      RuntimeSettings();
      bool ShouldConvertToSrgb() const { return m_srgb_expand_r && m_srgb_expand_g && m_srgb_expand_b; }
      bool operator == (const RuntimeSettings& rhs);
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Data members
    ////////////////////////////////////////////////////////////////////////////////////////////////
    u32                 m_width;
    u32                 m_height;
    u32                 m_depth;
    u32                 m_texture_type;
    u32                 m_levels_used;
    OutputColorFormat   m_format;
    MipInfo             m_levels[6][MAX_TEXTURE_MIPS]; // 6 = Texture::CUBE_NUM_FACES
    u32                 m_datasize[MAX_TEXTURE_MIPS];
    RuntimeSettings     m_runtime;
    bool                m_swizzled;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructur/Destructor
    ////////////////////////////////////////////////////////////////////////////////////////////////
    MipSet();
    ~MipSet();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Process members
    ////////////////////////////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // RemoveFromTail
    //
    // Removes the specified number of mips from the tail of the mip set (removes the N smallest mips).
    // There has to be at least a single mip level remaining after levels have been removed.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool RemoveFromTail(u32 levels);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // RemoveFromHead
    //
    // Removes the specified number of mips from the head of the mip set (removes the N biggest mips).
    // There has to be at least a single mip level remaining after levels have been removed.
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool RemoveFromHead(u32 levels);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Swizzle
    //
    // Apply the PS3 swizzle to data. The swizzle is format specific and in some cases the format
    // does not require swizzling and in these cases this function does nothing and returns false.
    //
    // It is currently unknown how to swizzle none power of two texures. This function will fail
    // and return false.
    //
    // The internal swizzle flag is set based on the result of this function
    //
    // DO NOT CALL THIS FOR PC TEXTURES
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool Swizzle();
    bool VTCSwizzle();

    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // WriteRGBAFLOATDDS()
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool WriteDDS(const char* fname) const;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool ExtractNonePowerOfTwo(u32 width, u32 height, u32 depth);
  };
}