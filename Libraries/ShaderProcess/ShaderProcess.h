////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProcess.h
//
//  Core functionality for generating the engine format shaders and textures
//
////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common/Types.h"

#include "Texture/Texture.h"

#include "TextureSlots.h"

namespace IG
{
  class IGSerializer;
}

namespace Asset
{
  class ShaderAsset;
}

namespace ShaderProcess
{

////////////////////////////////////////////////////////////////////////////////////////////////
// Results of writing the unique textures
////////////////////////////////////////////////////////////////////////////////////////////////

struct TextureStats
{
  TextureStats()
  {
    m_num_textures = 0;
    m_num_mips_h = 0;
    m_num_mips_l = 0;
    m_total_tex_mem_h = 0;
    m_total_tex_mem_l = 0;
    m_color_tex_mem = 0;
    m_color_tex_width = 0;
    m_color_tex_height = 0;
    m_color_tex_format = 0;
    m_color_tex_crc = 0;
    m_normal_tex_mem = 0;
    m_normal_tex_width = 0;
    m_normal_tex_height = 0;
    m_normal_tex_format = 0;
    m_normal_tex_crc = 0;
    m_gpi_tex_mem = 0;
    m_gpi_tex_width = 0;
    m_gpi_tex_height = 0;
    m_gpi_tex_format = 0;
    m_gpi_tex_crc = 0;
    m_d_tex_mem = 0;
    m_d_tex_width = 0;
    m_d_tex_height = 0;
    m_d_tex_format = 0;
    m_d_tex_crc = 0;
    m_alpha_exists = 0;
    m_normal_map_exists = 0;
    m_gloss_exists = 0;
    m_incan_exists = 0;
    m_parallax_exists = 0;
    m_detail_exists = 0;
    m_custom_a_exists = 0;
    m_custom_b_exists = 0;
  }

  u32 m_num_textures;
  u32 m_num_mips_h;
  u32 m_num_mips_l;
  u32 m_total_tex_mem_h;
  u32 m_total_tex_mem_l;

  u32 m_color_tex_mem;
  u32 m_color_tex_width;
  u32 m_color_tex_height;
  u32 m_color_tex_format;
  u32 m_color_tex_crc;
  u32 m_normal_tex_mem;
  u32 m_normal_tex_width;
  u32 m_normal_tex_height;
  u32 m_normal_tex_format;
  u32 m_normal_tex_crc;
  u32 m_gpi_tex_mem;
  u32 m_gpi_tex_width;
  u32 m_gpi_tex_height;
  u32 m_gpi_tex_format;
  u32 m_gpi_tex_crc;
  u32 m_d_tex_mem;
  u32 m_d_tex_width;
  u32 m_d_tex_height;
  u32 m_d_tex_format;
  u32 m_d_tex_crc;
  u32 m_custom_a_tex_mem;
  u32 m_custom_a_tex_width;
  u32 m_custom_a_tex_height;
  u32 m_custom_a_tex_format;
  u32 m_custom_a_tex_crc;
  u32 m_custom_b_tex_mem;
  u32 m_custom_b_tex_width;
  u32 m_custom_b_tex_height;
  u32 m_custom_b_tex_format;
  u32 m_custom_b_tex_crc;

  u32 m_alpha_exists;
  u32 m_normal_map_exists;
  u32 m_gloss_exists;
  u32 m_incan_exists;
  u32 m_parallax_exists;
  u32 m_detail_exists;
  u32 m_custom_a_exists;
  u32 m_custom_b_exists;
};


////////////////////////////////////////////////////////////////////////////////////////////////
// Various textures used by the IG shaders
////////////////////////////////////////////////////////////////////////////////////////////////

enum ShaderTextureType
{
  TEX_UNKNOWN=0xffffffff,

  // destination textures
  TEX_RT_COLOR_MAP    =0x100,    // r,g,b,alpha
  TEX_RT_NORMAL_MAP   =0x101,    // nx,ny,nz
  TEX_RT_GPI_MAP      =0x102,    // gloss, Parallax, Incan
  TEX_RT_D_MAP        =0x103,    // Detail, Detail, Detail
  TEX_RT_CUSTOM_MAP_A =0x104,    // Custom A
  TEX_RT_CUSTOM_MAP_B =0x105,    // Custom B
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Order of the texture channels
////////////////////////////////////////////////////////////////////////////////////////////////
enum
{
  TEXTURE_CHANNEL_RED =0,
  TEXTURE_CHANNEL_GREEN=1,
  TEXTURE_CHANNEL_BLUE=2,
  TEXTURE_CHANNEL_ALPHA=3,
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
const TexelType COLOR_MAP_ALPHA_THRESHOLD = 0.99f;
const TexelType AMBIENT_OCC_THRESHOLD = 0.99f;
const TexelType INCAN_THRESHOLD = 0.003f;
const TexelType PARALLAX_THRESHOLD = 0.01f;
const TexelType DETAIL_MASK_THRESHOLD = 0.99f;
const TexelType TEX_CONST_THRESHOLD = 0.001f;
const TexelType TEX_GLOSS_THRESHOLD = 0.001f;

// this is used to identify the different custom shader types.  It matches an enum in the runtime code (igg/igCustomShaders.h)
namespace CustomShaderTypes
{
  enum CustomShaderType
  {
    Standard,
    GroundFog,
    WaterPool,				// no longer used
    SeeThroughWalls,  // run-time only
    OffScreen,
    Fur,
    Refraction,
    Anisotropic,
    Gelatonium,
    Cloaked,          // run-time only
    AudioVisual,
    MobyParticalization,
    Fur2,
    Caustics,
    Foliage,
    BRDF,
    Invisible,              // run-time only
    SeeThroughWallsZ,       // run-time only
    SeeThroughWallsStencil, // run-time only
    Graph,
  };
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Structure that holds all the information of how to build a runtime texture from an art
// texture. The information from this structure is written to the debug chunk in the texture
// file so external tools can remake the same textures without any additional help.
////////////////////////////////////////////////////////////////////////////////////////////////

struct RuntimeTexture
{
  ShaderTextureType                 m_processing_type;          // info about how to generate the runtime texture data from the source textures
  ShaderTextureType                 m_runtime_type;             // which texture slot in the runtime shader this belongs to
  bool                              m_channel_valid[4];         // Indicates each of the channel has valid data
  std::string                       m_channel_textures[4];      // filenames for the R,G,B,A channels (no filename is use default)
  u32                               m_channels[4];              // a source channel index of the RGBA channels
  IG::TextureGenerationSettings     m_settings;
  IG::MipSet::RuntimeSettings       m_runtime;

  // aux channels for modifying the above map (keeping it generalized) (doing this to build multiple shaders at once if need arises)
  // currently it is used for color map where normal (optional) and amb occlusion info are baked in
  std::string                       m_aux_channel_texture;      // File names for the RGBA channels for aux channels which are blend into main channels
  f32                               m_aux_data[4];              // for color map it stores gloss, bake_in_norm_factor, bake_in_norm_light dir rot
  bool                              m_aux_flag;                 // for color map we use it to inidicate if we should bake in normal map

  RuntimeTexture();
  bool operator == (const RuntimeTexture& rhs);       // compare two RuntimeTextures
};

////////////////////////////////////////////////////////////////////////////////////////////////
// The texture remap is is a per shader array of indices (one for each texture) into the
// unique texture array.
////////////////////////////////////////////////////////////////////////////////////////////////

struct TextureRemap
{
  u32 m_texture_index[TextureSlots::NumTextureSlots];
};

////////////////////////////////////////////////////////////////////////////////////////////////
// Shader Generator is the main class for converting and generating engine shaders from
// source data. In order to prevent this class from keeping everything in memory the data
// is written to IG file as it is generated.
//
// The entire array of shaders passed to the constructor is converted and the order remains
// the same. Any processing that needs to be done on the source shader array should be done
// before this class is invoked.
//
// In the current scheme shaders are written to the geometry IG file while texture data and
// the texture headers are written to their own file. All the data can be written to the
// same file if both IGSerializer classes are the same.
// Shaders reference textures by index thus allowing the texture file to be
// regenerated with different compression without the geometry file being rebuilt, this is the
// main reason textures are in a seperate file.
//
////////////////////////////////////////////////////////////////////////////////////////////////

class ShaderGenerator
{
public:

  // constructor with all the parameters
  ShaderGenerator
  (
    Asset::ShaderAsset*   shader,         // the shader to be built
    IG::IGSerializer&     shader_file,    // IGSerializer to which to write the shader chunk
    IG::IGSerializer&     texh_file,      // IGSerializer to which to write texture headers and data (hi res)
    const char*           dump_path = 0   // output path for debug TGA of each mip (null for none)
   );

  ~ShaderGenerator();

  TextureStats MakeShader();

  void DumpShaderInfo();

  static IG::Texture* CreateRuntimeTexture(RuntimeTexture& texture );

  void GetSourceTextureName(ShaderTextureType tex_type, std::string& tex_name);

protected:
  static bool GetUniqueRuntimeTextureFromShader(Asset::ShaderAsset* shader, TextureSlot slot, RuntimeTexture&  rt);

  static IG::Texture* LoadInputTexture(const std::string& file_name, bool convert_to_linear );

  static bool IsTexDataConstant(IG::Texture* images[4], RuntimeTexture& texture, u8 channels, float threshold = 0.001f);

  static bool IsTexChannelDataValid(IG::Texture* ip_image, u8 ip_channel_id, float threshold, bool valid_if_greater);

  void MakeRuntimeTextureInfo();

  void WriteRuntimeTextures();

  void WriteShader();

  const RuntimeTexture* GetRuntimeTexture( u32 channel ) const;

  u32 AddUniqueRuntimeTextureFromShader(Asset::ShaderAsset* shader, TextureSlot slot);

  void RemoveTextureFromRemap(u32 idx);

  Asset::ShaderAsset*             m_shader;                   // the shader to be built
  IG::IGSerializer&               m_shader_file;              // file to which to write shaders
  IG::IGSerializer&               m_texh_file;                // file to which to write textures (hi res)
  const char*                     m_dump_path;                // debug output path
  TextureRemap                    m_texture_remap;            // indices into the unique textures
  std::vector<RuntimeTexture>     m_unique_textures;          // A global array of unique textures

  // these are filled in by the writes, indicating many assests have been created and how much memory is used
  TextureStats                    m_tex_stats;

};

}
