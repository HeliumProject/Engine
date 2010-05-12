////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ShaderProcess.cpp
//
//  written by: Rob Wyatt
//
//  Core functionality for generating the engine format shaders and textures
//
////////////////////////////////////////////////////////////////////////////////////////////////

#include "Asset/ShaderAsset.h"
#include "igmath/igmath.h"
#include "Common/Exception.h"
#include "rcs/rcs.h"
#include "Console/Console.h"
#include "Texture/Texture.h"
#include "Texture/Decode.h"
#include "IGSerializer/IGSerializer.h"
#include "FileSystem/FileSystem.h"

#include "igscene/igscene.h"
#include "TextureProcess/TextureWriter.h"
#include "igCore/igHeaders/rsxtypes.h"
#include "igCore/igHeaders/ps3structs.h"
#include "igCore/igHeaders/FileChunkIDs.h"

#include "ShaderProcess.h"
#include "ShaderBuffer.h"
#include "ShaderRegistry.h"

#include <algorithm>

using namespace Nocturnal;

namespace ShaderProcess
{

////////////////////////////////////////////////////////////////////////////////////////////////
// Create an uninitialized runtime texture
////////////////////////////////////////////////////////////////////////////////////////////////

RuntimeTexture::RuntimeTexture()
{
  m_processing_type = TEX_UNKNOWN;
  m_runtime_type = TEX_UNKNOWN;
  m_channel_textures[0].clear();
  m_channel_textures[1].clear();
  m_channel_textures[2].clear();
  m_channel_textures[3].clear();
  m_channels[0] = 0xffffffff;
  m_channels[1] = 0xffffffff;
  m_channels[2] = 0xffffffff;
  m_channels[3] = 0xffffffff;
  m_channel_valid[0] = false;
  m_channel_valid[1] = false;
  m_channel_valid[2] = false;
  m_channel_valid[3] = false;

  //aux
  m_aux_channel_texture.clear();
  m_aux_data[0] = 0.0f;
  m_aux_data[1] = 0.0f;
  m_aux_data[2] = 0.0f;
  m_aux_data[3] = 0.0f;
  m_aux_flag = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Compare two runtime textures for equality, this is used when making the master unique list
// of runtime textures.
////////////////////////////////////////////////////////////////////////////////////////////////

bool RuntimeTexture::operator == (const RuntimeTexture& rhs)
{
  // Not the same type of texture
  if (m_processing_type != rhs.m_processing_type)
    return false;
  if (m_runtime_type != rhs.m_runtime_type)
    return false;

  if (m_channel_textures[0] != rhs.m_channel_textures[0])
    return false;
  if (m_channel_textures[1] != rhs.m_channel_textures[1])
    return false;
  if (m_channel_textures[2] != rhs.m_channel_textures[2])
    return false;
  if (m_channel_textures[3] != rhs.m_channel_textures[3])
    return false;

  if (m_channels[0] != rhs.m_channels[0])
    return false;
  if (m_channels[1] != rhs.m_channels[1])
    return false;
  if (m_channels[2] != rhs.m_channels[2])
    return false;
  if (m_channels[3] != rhs.m_channels[3])
    return false;

  if (m_channel_valid[0] != rhs.m_channel_valid[0])
    return false;
  if (m_channel_valid[1] != rhs.m_channel_valid[1])
    return false;
  if (m_channel_valid[2] != rhs.m_channel_valid[2])
    return false;
  if (m_channel_valid[3] != rhs.m_channel_valid[3])
    return false;

  if (m_aux_channel_texture != rhs.m_aux_channel_texture)
    return false;

  if (m_aux_flag != rhs.m_aux_flag)
    return false;

  if (m_aux_data[0] != rhs.m_aux_data[0])
    return false;
  if (m_aux_data[1] != rhs.m_aux_data[1])
    return false;
  if (m_aux_data[2] != rhs.m_aux_data[2])
    return false;
  if (m_aux_data[3] != rhs.m_aux_data[3])
    return false;

  if (!(m_runtime == rhs.m_runtime))
    return false;

  // We could look in the tex gen settings here if we wanted to be more specific on what is
  // considered an identical texture

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor
////////////////////////////////////////////////////////////////////////////////////////////////

ShaderGenerator::ShaderGenerator
(
 Asset::ShaderAsset*  shader,       // the shader to be built
 IGSerializer&        shader_file,  // IGSerializer to which to write the shader chunk
 IGSerializer&        texh_file,    // IGSerializer to which to write texture headers and data (hi res)
 const char*          dump_path
)
: m_shader(shader)
, m_shader_file(shader_file)
, m_texh_file(texh_file)
, m_dump_path(dump_path)
{
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Destructor
////////////////////////////////////////////////////////////////////////////////////////////////

ShaderGenerator::~ShaderGenerator()
{
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Builds and writes the shaders and their textures
//
////////////////////////////////////////////////////////////////////////////////////////////////

TextureStats ShaderGenerator::MakeShader()
{
  // handle textures
  MakeRuntimeTextureInfo();

  // this will adjust the texture remap table
  WriteRuntimeTextures();

  // handle shaders
  WriteShader();

  // return the results
  return m_tex_stats;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// This function will make the unique runtime textures for the entire list of shaders
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::MakeRuntimeTextureInfo()
{
  // make the runtime textures putting the texture index into the remap structure
  m_texture_remap.m_texture_index[TextureSlots::ColorMap    ] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::ColorMap    );
  m_texture_remap.m_texture_index[TextureSlots::NormalMap   ] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::NormalMap   );
  m_texture_remap.m_texture_index[TextureSlots::ExpensiveMap] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::ExpensiveMap);
  m_texture_remap.m_texture_index[TextureSlots::DetailMap   ] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::DetailMap   );
  m_texture_remap.m_texture_index[TextureSlots::CustomMapA  ] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::CustomMapA  );
  m_texture_remap.m_texture_index[TextureSlots::CustomMapB  ] = AddUniqueRuntimeTextureFromShader( m_shader, TextureSlots::CustomMapB  );
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given a shader and a runtime texture type this function will create the RuntimeTexture
// and return true or false depending on validity
//
////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderGenerator::GetUniqueRuntimeTextureFromShader(Asset::ShaderAsset* shader, TextureSlot slot, RuntimeTexture&  rt)
{
  return ShaderRegistry::MakeRuntimeTexture( rt, shader, slot );
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Given a shader and a runtime texture type this function will create the RuntimeTexture
// structure if the requested texture is not already in the array and return the index. If
// the texture is already in the array the existing entry is returned.
//
// If the specified texture cannot be created from the shader then -1 is returned.
//
////////////////////////////////////////////////////////////////////////////////////////////////

u32 ShaderGenerator::AddUniqueRuntimeTextureFromShader(Asset::ShaderAsset* shader, TextureSlot slot)
{
  RuntimeTexture  rt;
  if (!GetUniqueRuntimeTextureFromShader(shader, slot, rt))
  {
    return 0xffffffff;
  }

  // rt is the runtime that we want to create, check if it already exists before
  // adding to the unique array.

  u32 count = 0;
  for (std::vector<RuntimeTexture>::iterator i = m_unique_textures.begin(); i!=m_unique_textures.end(); i++, count++)
  {
    if (rt==(*i))
    {
      return count;
    }
  }

  // did not find the texture so add it to the end of the unqiue array
  m_unique_textures.push_back(rt);
  return (u32)m_unique_textures.size()-1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Remove the specified index from the remap table. Any remap value that matches the
//  specified index is set to -1 because the specified texture is not invalid. In addition
//  any entry that is greater than the specified index has 1 subtracted from it.
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::RemoveTextureFromRemap(u32 idx)
{
  for (int t = 0; t < TextureSlots::NumTextureSlots; ++t)
  {
    // any entry that matches this index is now invalid because the texture is not required
    if (m_texture_remap.m_texture_index[t]==idx)
      m_texture_remap.m_texture_index[t]=0xffffffff;

    // anything greater than idx needs to be reduced by 1 to account for the missing texture
    if (m_texture_remap.m_texture_index[t]!=0xffffffff)
    {
      if (m_texture_remap.m_texture_index[t]>idx)
        m_texture_remap.m_texture_index[t]-=1;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// If given texture channel in image has a pixel satisfying (> or <) threshold with its condition for validity then return true
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool ShaderGenerator::IsTexChannelDataValid(IG::Texture* ip_image, u8 ip_channel_id, float threshold, bool valid_if_greater)
{
  f32* color_channel = ip_image->GetFacePtr(0, ip_channel_id);
  u32 image_size = ip_image->m_Width*ip_image->m_Height;

  if (valid_if_greater)
  {
    for(u32 i = 0; i < image_size; i++)
    {
      if (color_channel[i] > threshold)
        return true;
    }
  }
  else
  {
    for(u32 i = 0; i < image_size; i++)
    {
      if (color_channel[i] < threshold)
        return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// tests if runtime texture data is constant for specified channels
//  (if max-min tex values of any of the specified channels is > threshold then return false else true)
//
////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ShaderGenerator::IsTexDataConstant(IG::Texture* images[4], RuntimeTexture& texture, u8 channels, float threshold)
{
  TexelType* dataR;
  u32 width = 0;
  u32 height = 0;
  if(channels & 0x01)
  {
    dataR = (TexelType*)(images[0]->GetFacePtr(0, texture.m_channels[0]));
    width = images[0]->m_Width;
    height = images[0]->m_Height;
  }
  TexelType* dataG;
  if(channels & 0x02)
  {
    dataG = (TexelType*)(images[1]->GetFacePtr(0, texture.m_channels[1]));
    width = images[1]->m_Width;
    height = images[1]->m_Height;
  }
  TexelType* dataB;
  if(channels & 0x04)
  {
    dataB = (TexelType*)(images[2]->GetFacePtr(0, texture.m_channels[2]));
    width = images[2]->m_Width;
    height = images[2]->m_Height;
  }
  TexelType* dataA;
  if(channels & 0x08)
  {
    dataA = (TexelType*)(images[3]->GetFacePtr(0, texture.m_channels[3]));
    width = images[3]->m_Width;
    height = images[3]->m_Height;
  }

  TexelType maxR = BLACK_TEXEL;
  TexelType maxG = BLACK_TEXEL;
  TexelType maxB = BLACK_TEXEL;
  TexelType maxA = BLACK_TEXEL;
  TexelType minR = WHITE_TEXEL;
  TexelType minG = WHITE_TEXEL;
  TexelType minB = WHITE_TEXEL;
  TexelType minA = WHITE_TEXEL;

  bool data_is_const = true;
  for(u32 i = 0; i < width*height; i ++)
  {
    //R channel
    if(channels & 0x01)
    {
       if(*dataR  > maxR)
       {
         maxR = *dataR;
       }

       if(*dataR  < minR)
       {
         minR = *dataR;
       }

       if ( (maxR - minR) > threshold)
       {
         data_is_const = false;
         break;
       }
       ++dataR;
    }

    //B channel
    if(channels & 0x02)
    {
       if(*dataG  > maxG)
       {
         maxG = *dataG;
       }

       if(*dataG  < minG)
       {
         minG = *dataG;
       }

       if ( (maxG - minG) > threshold)
       {
         data_is_const = false;
         break;
       }
       ++dataG;
    }

    //G channel
    if(channels & 0x04)
    {
       if(*dataB  > maxB)
       {
         maxB = *dataB;
       }

       if(*dataB  < minB)
       {
         minB = *dataB;
       }

       if ( (maxB - minB) > threshold)
       {
         data_is_const = false;
         break;
       }
       ++dataB;
    }

    //Alpha
    if(channels & 0x08)
    {
       if(*dataA  > maxA)
       {
         maxA = *dataA;
       }

       if(*dataA  < minA)
       {
         minA = *dataA;
       }

       if ( (maxA - minA) > threshold)
       {
         data_is_const = false;
         break;
       }
       ++dataA;
    }
  }
  return data_is_const;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
//   -Loads input texture, converts to TEXTURE_PROCESS_FORMAT and adds it to the remap table
//
////////////////////////////////////////////////////////////////////////////////////////////////

IG::Texture* ShaderGenerator::LoadInputTexture(const std::string& file_name, bool convert_to_linear )
{
  IG::Texture* file_image = 0;
  if (file_name == "")
  {
    return file_image;
  }

  // this name is not found, load the image and add to the name map, if we cannot find
  // an image load/create a default
  char ext[128];
  _splitpath(file_name.c_str(),0,0,0,ext);

  if (_stricmp(ext,".psd")==0)
  {

    throw Nocturnal::Exception("PSD is not supported");
  }
  else
  {
    // regular single image texture
    //printf("%s\n",texture.m_channel_textures[m].c_str());
    file_image = IG::Texture::LoadFile(file_name.c_str(), convert_to_linear, NULL);
    if (file_image)
    {
      file_image->m_NativeFormat = TEXTURE_PROCESS_FORMAT;
    }
  }

  return file_image;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Creates the image for the specified runtime texture. This function should only return NULL if
// the texture is not required. If the specified texture has invalid inputs then a suitable default
// is created.
//
////////////////////////////////////////////////////////////////////////////////////////////////

IG::Texture* ShaderGenerator::CreateRuntimeTexture(RuntimeTexture& texture )
{
  // The four images for each channel in the destination
  IG::Texture* images[4]={0};      // the image
  IG::Texture* aux_image = 0;      // the image
  TexelType default_texels[4];
  bool local_tex[4] = {0};
  bool local_aux_tex = 0;
  bool has_amb_occl = false; //used only to generate color map
  bool bake_in_normal_map = texture.m_aux_flag; // used only to generate color map

  switch (texture.m_processing_type)
  {
  case TEX_RT_CUSTOM_MAP_A:    // CUSTOM MAP A
  case TEX_RT_CUSTOM_MAP_B:    // CUSTOM MAP B
  case TEX_RT_COLOR_MAP:       // COLOR MAP
    default_texels[TEXTURE_CHANNEL_RED]   = COLOR_MAP_DEFAULT_RED;            // Gray color
    default_texels[TEXTURE_CHANNEL_GREEN] = COLOR_MAP_DEFAULT_GREEN;          // Gray color
    default_texels[TEXTURE_CHANNEL_BLUE]  = COLOR_MAP_DEFAULT_BLUE;           // Gray color
    default_texels[TEXTURE_CHANNEL_ALPHA] = COLOR_MAP_DEFAULT_ALPHA;          // solid alpha
    break;

  case TEX_RT_NORMAL_MAP:       // NORMAL MAP
    default_texels[TEXTURE_CHANNEL_RED] = NORMAL_MAP_DEFAULT_RED;       //R
    default_texels[TEXTURE_CHANNEL_GREEN] = NORMAL_MAP_DEFAULT_GREEN;   //G
    default_texels[TEXTURE_CHANNEL_BLUE] = NORMAL_MAP_DEFAULT_BLUE;     //B
    default_texels[TEXTURE_CHANNEL_ALPHA] = NORMAL_MAP_DEFAULT_ALPHA;
    break;

  case TEX_RT_GPI_MAP:       // Gloss, Parallax and Incan
    default_texels[TEXTURE_CHANNEL_RED] = GPI_MAP_DEFAULT_RED;
    default_texels[TEXTURE_CHANNEL_GREEN] = GPI_MAP_DEFAULT_GREEN;
    default_texels[TEXTURE_CHANNEL_BLUE] = GPI_MAP_DEFAULT_BLUE;
    default_texels[TEXTURE_CHANNEL_ALPHA] = GPI_MAP_DEFAULT_ALPHA;
    break;

  case TEX_RT_D_MAP:       // Detail
    default_texels[TEXTURE_CHANNEL_RED] = D_MAP_DEFAULT_RED;
    default_texels[TEXTURE_CHANNEL_GREEN] = D_MAP_DEFAULT_GREEN;
    default_texels[TEXTURE_CHANNEL_BLUE] = D_MAP_DEFAULT_BLUE;
    default_texels[TEXTURE_CHANNEL_ALPHA] = D_MAP_DEFAULT_ALPHA;
    break;
  }

  IG::Texture* file_image = 0;

  // load the 4 textures in the source channel array, we use the name map because most of the time
  // the same texture will be used multiple times within a single combined texture as well as
  // multiple times across all combined textures within a shader.
  for (u32 m=0;m<4;m++)
  {
    // if the runtime settings say it'll eventually be written out in srgb space, then assume
    // the input is srgb
    bool convert_to_linear = texture.m_runtime.ShouldConvertToSrgb();
    if (texture.m_channels[m]!=0xffffffff)
    {
      images[m] = LoadInputTexture(texture.m_channel_textures[m], convert_to_linear);
    }
    else
    {
      // there is no image
      images[m] = 0;
    }
  }

  //load aux_image (can only be a normal map, always linear);
  aux_image = LoadInputTexture(texture.m_aux_channel_texture, false);

  u32 width = 32;     // default width and height
  u32 height = 32;

  // Now we have the 4 RGBA8888 image pointers, which may all be the same or may all be different. We also have 4
  // shift values so we can correctly order the elements. First we need to check the sizes and if needed
  // rescale some of the images to fit, how we scale depends on the type of the image.
  if (texture.m_processing_type == TEX_RT_COLOR_MAP)
  {
    // DESTINATION COLOR MAP R,G,B,A - SAMPLER 0

    if (images[0]==0)
    {
      width = 32;
      height = 32;

      delete aux_image;

      aux_image = 0;
    }
    else
    {
      texture.m_channel_valid[0] =  texture.m_channel_valid[1] = texture.m_channel_valid[2]= true;
      width = images[0]->m_Width;
      height = images[0]->m_Height;
    }

    //If amb occl factor is insignificant then ignore (Add extrac check for bake in normal map )
    if (texture.m_aux_data[0] > (1-AMBIENT_OCC_THRESHOLD) )
    {
      has_amb_occl = true;
    }

    if (aux_image && has_amb_occl)
    {
      /// ??? do we really mean to declare a new version of has_amb_occl here ???
      bool has_amb_occl = IsTexChannelDataValid(aux_image, texture.m_channels[3], AMBIENT_OCC_THRESHOLD, false);

      if (!has_amb_occl)
      {
        texture.m_aux_data[0] = 0.0f;
      }
    }

    if (!has_amb_occl && !bake_in_normal_map)
    {
      delete aux_image;

      aux_image = 0;
    }

    if (aux_image)
    {
      if ((aux_image->m_Width != images[0]->m_Width) || (aux_image->m_Height != images[0]->m_Height))
      {
        // we need to rescale the alpha map to fit the color map before we combine them, however we
        // cannot modify the source image as it is in the name cache and may be used by other texture
        // combinations
        IG::Texture* nt = aux_image->ScaleImage(images[0]->m_Width,
                                                images[0]->m_Height,
                                                aux_image->m_NativeFormat,
                                                IG::MIP_FILTER_CUBIC);
        delete aux_image;

        local_aux_tex = true;
        aux_image = nt;
      }
    }

    // if there is no alpha image then we are good to go
    if (images[3]!=0)
    {
      // we have an alpha image so check its size matches the size of one of the color channels, if it
      // does not scale up so it does match. Before we do anything check to see if the alpha channel is
      // used, if it is not used change the output format to DXT1

      bool has_per_pixel_alpha = IsTexChannelDataValid(images[3], texture.m_channels[3], COLOR_MAP_ALPHA_THRESHOLD, false);


      if (has_per_pixel_alpha)
      {
        texture.m_channel_valid[3] = true;
        if ((images[3]->m_Width != images[0]->m_Width) || (images[3]->m_Height != images[0]->m_Height))
        {
          // we need to rescale the alpha map to fit the color map before we combine them, however we
          // cannot modify the source image as it is in the name cache and may be used by other texture
          // combinations
          IG::Texture* nt = images[3]->ScaleImage(images[0]->m_Width, images[0]->m_Height, images[3]->m_NativeFormat, IG::MIP_FILTER_CUBIC);
          delete images[3];

          local_tex[3]    = true;
          images[3]       = nt;
        }
      }
      else
      {
        texture.m_runtime.m_alpha_channel = IG::COLOR_CHANNEL_FORCE_ONE;
        // there is no per pixel alpha in the texture so set the output format to
        // be DXT1 and zero out image[3]

        if ((texture.m_settings.m_output_format == IG::OUTPUT_CF_DXT5) || (texture.m_settings.m_output_format == IG::OUTPUT_CF_DXT3))
        {
          texture.m_settings.m_output_format = IG::OUTPUT_CF_DXT1;
        }
        else if (texture.m_settings.m_output_format == IG::OUTPUT_CF_ARGB4444)
        {
          texture.m_settings.m_output_format = IG::OUTPUT_CF_RGB565;
        }

        delete images[3];
        images[3] = 0;

        // keep the EngineTexture struct up to date so we can calculate its key properly
        texture.m_channels[3] = 0xffffffff;
        texture.m_channel_textures[3] = "";
      }
    }
    else
    {
      texture.m_channels[3] = 0xffffffff;
      texture.m_channel_textures[3] = "";
    }
  }
  else if((texture.m_processing_type == TEX_RT_CUSTOM_MAP_A) ||
          (texture.m_processing_type == TEX_RT_CUSTOM_MAP_B))
  {
    // DESTINATION COLOR MAP R,G,B,A - SAMPLER 0
    if (images[0]==0)
    {
      width = 32;
      height = 32;

      delete aux_image;

      aux_image = 0;
    }
    else
    {
      texture.m_channel_valid[0] =  texture.m_channel_valid[1] = texture.m_channel_valid[2]= true;
      width = images[0]->m_Width;
      height = images[0]->m_Height;
    }

    // if there is no alpha image then we are good to go
    if (images[3]!=0)
    {
      // we have an alpha image so check its size matches the size of one of the color channels, if it
      // does not scale up so it does match. Before we do anything check to see if the alpha channel is
      // used, if it is not used change the output format to DXT1

      bool has_per_pixel_alpha = IsTexChannelDataValid(images[3], texture.m_channels[3], COLOR_MAP_ALPHA_THRESHOLD, false);


      if (has_per_pixel_alpha)
      {
        texture.m_channel_valid[3] = true;
        if ((images[3]->m_Width != images[0]->m_Width) || (images[3]->m_Height != images[0]->m_Height))
        {
          // we need to rescale the alpha map to fit the color map before we combine them, however we
          // cannot modify the source image as it is in the name cache and may be used by other texture
          // combinations
          IG::Texture* nt = images[3]->ScaleImage(images[0]->m_Width, images[0]->m_Height, images[3]->m_NativeFormat, IG::MIP_FILTER_CUBIC);
          delete images[3];

          local_tex[3]    = true;
          images[3]       = nt;
        }
      }
      else
      {
        texture.m_runtime.m_alpha_channel = IG::COLOR_CHANNEL_FORCE_ONE;
        // there is no per pixel alpha in the texture so set the output format to
        // be DXT1 and zero out image[3]

        if ((texture.m_settings.m_output_format == IG::OUTPUT_CF_DXT5) || (texture.m_settings.m_output_format == IG::OUTPUT_CF_DXT3))
        {
          texture.m_settings.m_output_format = IG::OUTPUT_CF_DXT1;
        }
        else if (texture.m_settings.m_output_format == IG::OUTPUT_CF_ARGB4444)
        {
          texture.m_settings.m_output_format = IG::OUTPUT_CF_RGB565;
        }

        delete images[3];
        images[3] = 0;

        // keep the EngineTexture struct up to date so we can calculate its key properly
        texture.m_channels[3] = 0xffffffff;
        texture.m_channel_textures[3] = "";
      }
    }
    else
    {
      texture.m_channels[3] = 0xffffffff;
      texture.m_channel_textures[3] = "";
    }
  }
  else if (texture.m_processing_type == TEX_RT_NORMAL_MAP)
  {
    // DESTINATION NORMAL MAP Nx,Ny,Nz,* - SAMPLER 1

    // There is nothing we need to do for a normal map, All three channels we need
    // are in a single texture in the source art. We can remove the third image
    // so it does not confuse later stages.
    if (images[0]==0)
    {
      width = 32;
      height = 32;
    }
    else if (IsTexDataConstant(images, texture, (u8)(0x01|0x02|0x04), TEX_CONST_THRESHOLD) )
    {

      width = 32;
      height = 32;

      delete images[0];
      delete images[1];
      delete images[2];

      images[0] = 0;
      images[1] = 0;
      images[2] = 0;

      // keep the EngineTexture struct up to date so we can calculate its key properly
      texture.m_channels[0] = 0xffffffff;
      texture.m_channel_textures[0] = "";
      texture.m_channels[1] = 0xffffffff;
      texture.m_channel_textures[1] = "";
      texture.m_channels[2] = 0xffffffff;
      texture.m_channel_textures[2] = "";
    }
    else
    {
      width = images[0]->m_Width;
      height = images[0]->m_Height;
      texture.m_channel_valid[0] = texture.m_channel_valid[1] = texture.m_channel_valid[2] = true;
    }

    delete images[3];
    images[3] = 0;

    texture.m_channels[3] = 0xffffffff;
    texture.m_channel_textures[3] = "";
  }
  else if (texture.m_processing_type == TEX_RT_GPI_MAP)
  {
    //if there is no texture then return 0
    if (images[0]==0)
    {
      delete images[0];
      delete images[1];
      delete images[2];
      delete images[3];
      delete aux_image;

      images[0] = 0;
      images[1] = 0;
      images[2] = 0;
      images[3] = 0;
      aux_image = 0;

      return 0;
    }
    else
    {
      width = images[0]->m_Width;
      height = images[0]->m_Height;
    }

    bool gloss = texture.m_channels[0]!=0xffffffff;
    if (gloss)
    {
      gloss = IsTexChannelDataValid(images[0], texture.m_channels[0], TEX_GLOSS_THRESHOLD, true);
      if (gloss)
      {
        texture.m_channel_valid[0] = true;
      }
    }

    // If any pixel which is not 0 then we have valid data.
    bool para = texture.m_channels[1]!=0xffffffff;
    if (para)
    {
      para = IsTexChannelDataValid(images[1], texture.m_channels[1], PARALLAX_THRESHOLD, true);
      if( para )
      {
        para = true;
        texture.m_channel_valid[1] = true;
      }
    }

    bool incan = texture.m_channels[2]!=0xffffffff;
    if (incan)
    {
      incan = IsTexChannelDataValid(images[2], texture.m_channels[2], INCAN_THRESHOLD, true);
      if (incan)
      {
        texture.m_channel_valid[2] = true;
      }
      /*
      incan = false;
      TexelType* data = (TexelType*)(images[2]->GetFacePtr(0));
      for(u32 i = 0; i < images[2]->m_Width*images[2]->m_Height; i ++)
      {
        if (data[ texture.m_channels[2] ]>INCAN_THRESHOLD)
        {
          incan = true;
          texture.m_channel_valid[2] = true;
          break;
        }
        data+=4;
      }
      */
    }

    //if we have anypixel < 255 then we have non-default mask so pack it
    bool detail_mask =   texture.m_channels[3]!=0xffffffff;
    if (detail_mask)
    {
      detail_mask = IsTexChannelDataValid(images[2], texture.m_channels[3], DETAIL_MASK_THRESHOLD, false);
      if (detail_mask)
      {
        texture.m_channel_valid[3] = true;
      }
      /*
      detail_mask = false;
      TexelType* data = (TexelType*)(images[2]->GetFacePtr(0));
      for(u32 i = 0; i < images[2]->m_Width*images[2]->m_Height; i ++)
      {
        if (data[ texture.m_channels[3] ]<DETAIL_MASK_THRESHOLD)
        {
          detail_mask = true;
          texture.m_channel_valid[3] = true;
          break;
        }
        data+=4;
      }
      */
    }

    if (!gloss && !incan && !para && !detail_mask)
    {
      delete images[0];
      delete images[1];
      delete images[2];
      delete images[3];
      delete aux_image;

      images[0] = 0;
      images[1] = 0;
      images[2] = 0;
      images[3] = 0;
      aux_image = 0;

      texture.m_channels[0] = 0xffffffff;
      texture.m_channel_textures[0] = "";
      texture.m_channels[1] = 0xffffffff;
      texture.m_channel_textures[1] = "";
      texture.m_channels[2] = 0xffffffff;
      texture.m_channel_textures[2] = "";
      texture.m_channels[3] = 0xffffffff;
      texture.m_channel_textures[3] = "";

      return 0;
    }
  }
  else if (texture.m_processing_type==TEX_RT_D_MAP)
  {
    // Check if detail map is constant
    if (images[0]==0)
    {
      delete images[0];
      delete images[1];
      delete images[2];
      delete images[3];
      delete aux_image;

      images[0] = 0;
      images[1] = 0;
      images[2] = 0;
      images[3] = 0;
      aux_image = 0;

      return 0;
    }
    else
    {
      width = images[0]->m_Width;
      height = images[0]->m_Height;
    }

    if (IsTexDataConstant(images, texture, (u8)(0x01|0x02|0x04|0x8), TEX_CONST_THRESHOLD) )
    {
      delete images[0];
      delete images[1];
      delete images[2];
      delete images[3];
      delete aux_image;

      images[0] = 0;
      images[1] = 0;
      images[2] = 0;
      images[3] = 0;
      aux_image = 0;

      texture.m_channels[0] = 0xffffffff;
      texture.m_channel_textures[0] = "";
      texture.m_channels[1] = 0xffffffff;
      texture.m_channel_textures[1] = "";
      texture.m_channels[2] = 0xffffffff;
      texture.m_channel_textures[2] = "";
      texture.m_channels[3] = 0xffffffff;
      texture.m_channel_textures[3] = "";
      return 0;
    }
    texture.m_channel_valid[0] = texture.m_channel_valid[1] = texture.m_channel_valid[2] = texture.m_channel_valid[3] = true;
  }

  // make a 32bit texture from the above channels
  IG::Texture* combined = new IG::Texture(width,height,TEXTURE_PROCESS_FORMAT);

  TexelType* dst_r = (TexelType*)(combined->GetFacePtr(0, IG::Texture::R));
  TexelType* dst_g = (TexelType*)(combined->GetFacePtr(0, IG::Texture::G));
  TexelType* dst_b = (TexelType*)(combined->GetFacePtr(0, IG::Texture::B));
  TexelType* dst_a = (TexelType*)(combined->GetFacePtr(0, IG::Texture::A));

  TexelType* src_data_ptrs[4];

  src_data_ptrs[0] = images[0]  ? (TexelType*)(images[0]->GetFacePtr(0, texture.m_channels[0])) : 0;
  src_data_ptrs[1] = images[1]  ? (TexelType*)(images[1]->GetFacePtr(0, texture.m_channels[1])) : 0;
  src_data_ptrs[2] = images[2]  ? (TexelType*)(images[2]->GetFacePtr(0, texture.m_channels[2])) : 0;
  src_data_ptrs[3] = images[3]  ? (TexelType*)(images[3]->GetFacePtr(0, texture.m_channels[3])) : 0;

  for (u32 y=0;y<height;y++)
  {
    for (u32 x=0;x<width;x++)
    {
      *dst_r++ = images[0]  ? *src_data_ptrs[0]  : default_texels[0];
      *dst_g++ = images[1]  ? *src_data_ptrs[1]  : default_texels[1];
      *dst_b++ = images[2]  ? *src_data_ptrs[2]  : default_texels[2];
      *dst_a++ = images[3]  ? *src_data_ptrs[3]  : default_texels[3];

      ++src_data_ptrs[ 0 ];
      ++src_data_ptrs[ 1 ];
      ++src_data_ptrs[ 2 ];
      ++src_data_ptrs[ 3 ];
    }
  }

  //Post processing on output texture (just color map so far)
  if (texture.m_processing_type == TEX_RT_COLOR_MAP)
  {
    if (aux_image && (has_amb_occl || bake_in_normal_map))
    {
      TexelType* dst_texel_r = (TexelType*)(combined->GetFacePtr(0, IG::Texture::R));
      TexelType* dst_texel_g = (TexelType*)(combined->GetFacePtr(0, IG::Texture::G));
      TexelType* dst_texel_b = (TexelType*)(combined->GetFacePtr(0, IG::Texture::B));

      TexelType* aux_texel_r = (TexelType*)(aux_image->GetFacePtr(0, IG::Texture::R));
      TexelType* aux_texel_g = (TexelType*)(aux_image->GetFacePtr(0, IG::Texture::G));
      TexelType* aux_texel_b = (TexelType*)(aux_image->GetFacePtr(0, IG::Texture::B));
      TexelType* aux_texel_a = (TexelType*)(aux_image->GetFacePtr(0, IG::Texture::A));

      f32 n_rot_z   = texture.m_aux_data[2] * PI/180.0f;
      f32 n_rot_xy  = texture.m_aux_data[3] * PI/180.0f;

      vec3 light_dir(sin(n_rot_z)*cos(n_rot_xy), sin(n_rot_z)*sin(n_rot_xy), cos(n_rot_z));

      vec3  color;
      vec3  normal;
      f32   amb_occ_factor        = 1.0f;
      f32   normal_bake_in_factor = 1.0f;

      for (u32 y=0;y<height;y++)
      {
        for (u32 x=0;x<width;x++)
        {
          color.SetXYZ(*dst_texel_r, *dst_texel_g, *dst_texel_b);
          if (has_amb_occl)
          {
            amb_occ_factor = (1.0f-texture.m_aux_data[0]) + (texture.m_aux_data[0] * (*aux_texel_a));
            color *= amb_occ_factor;
          }

          if (bake_in_normal_map)
          {
            normal.SetXYZ(*aux_texel_r*2.0f - 1.0f, *aux_texel_g*2.0f - 1.0f, *aux_texel_b*2.0f - 1.0f);
            normal_bake_in_factor = VecDot3(normal, light_dir);
            std::max(normal_bake_in_factor, 0.0f);
            normal_bake_in_factor = (1.0f-texture.m_aux_data[1]) + (texture.m_aux_data[1] * normal_bake_in_factor);
            color *= normal_bake_in_factor;
          }

          *dst_texel_r++ = std::min(color.x, 1.0f);
          *dst_texel_g++ = std::min(color.y, 1.0f);
          *dst_texel_b++ = std::min(color.z, 1.0f);

          ++aux_texel_r;
          ++aux_texel_g;
          ++aux_texel_b;
          ++aux_texel_a;
        }
      }
    }
  }

  combined->FlipVertical();

  for (u32 i=0;i<4;i++)
  {
    delete images[i];
  }

  delete aux_image;

  return combined;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// This goes through the unique array and actually make the runtime textures as specified.
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::WriteRuntimeTextures()
{
  // we need to go through each of the unique textures, create them, and write them out to
  // the tex file.
  u32 count=0;
  for (std::vector<RuntimeTexture>::iterator i=m_unique_textures.begin();i!=m_unique_textures.end();)
  {
    // this will always return an image, it may be a default image but the function will not fail. In anything
    // critical goes wrong it will throw an exception.
    IG::Texture* image = CreateRuntimeTexture(m_unique_textures[count] );

    if (image)
    {
      IG::MipSet *mips = image->GenerateFinalizedMipSet( (*i).m_settings, (*i).m_runtime, (*i).m_processing_type == TEX_RT_NORMAL_MAP, (*i).m_processing_type == TEX_RT_D_MAP );

      if(mips)
      {
        // we have a dump path so dump the generated mips and the source images
        if (m_dump_path)
        {
          IG::DecodeMips decoded(mips);

          char* types[] = {"Color","Normal","GPI","D", "CustomA", "CustomB"};
          char fname[512];

          IG::Texture* face     = image->CloneFace(0);
          face->m_NativeFormat  = IG::CF_ARGB8888;
          sprintf(fname,"%s%s.tga",m_dump_path,types[(*i).m_processing_type&0xff]);

          if ( RCS::PathIsManaged( fname ) )
          {
            RCS::File rcsFile( fname );
            rcsFile.Open();
          }

          bool convert_to_srgb = ( ((*i).m_processing_type&0xff) == TEX_RT_COLOR_MAP );
          face->WriteTGA(fname, 0, convert_to_srgb);
          delete face;

          for (u32 l=0;l<decoded.m_levels;l++)
          {
            sprintf(fname,"%s%s_mip%d.tga",m_dump_path,types[(*i).m_processing_type&0xff],l);
            RCS::File rcsFile( fname );
            rcsFile.Open();
            decoded.m_images[l]->WriteTGA(fname, 0, convert_to_srgb);
          }
        }

        m_tex_stats.m_num_textures++;

        // count the number of bytes used by this texture
        u32 tex_mem_h = 0;
        for (u32 m = 0; m < mips->m_levels_used; m++)
          tex_mem_h += mips->m_datasize[m];

        m_tex_stats.m_num_mips_h += mips->m_levels_used;
        m_tex_stats.m_total_tex_mem_h += tex_mem_h;

        u32 texture_crc = 0xffffffff;
        {
          BasicBufferPtr data_h = m_texh_file.AddChunk(IGG::FILECHUNK_TEXTURE_DATA,IG::CHUNK_TYPE_SINGLE,0,0,0);
          BasicBufferPtr header_h = m_texh_file.AddChunk(IGG::FILECHUNK_TEXTURE_ARRAY,IG::CHUNK_TYPE_ARRAY,sizeof(IG::IGTexture),0,0);
          BasicBufferPtr misc_h = m_texh_file.AddChunk(IGG::FILECHUNK_TEXTURE_DEBUG,IG::CHUNK_TYPE_ARRAY,sizeof(TextureProcess::TextureDebugInfo),0,0);
          texture_crc = TextureProcess::WriteMipSet(mips, header_h, data_h, misc_h);

          // This is done so the packer doesn't have to load the high res file each time a shader is
          // referenced
          BasicBufferPtr misc_copy = m_shader_file.AddChunk(IGG::FILECHUNK_TEXTURE_DEBUG,IG::CHUNK_TYPE_ARRAY,sizeof(TextureProcess::TextureDebugInfo),0,0);
          misc_copy->AddBuffer( misc_h, false );
        }

        // track data usage per texture type
        switch ((*i).m_runtime_type)
        {
          case TEX_RT_COLOR_MAP:
            m_tex_stats.m_color_tex_mem += tex_mem_h;
            m_tex_stats.m_color_tex_width = mips->m_width;
            m_tex_stats.m_color_tex_height = mips->m_height;
            m_tex_stats.m_color_tex_format = mips->m_format;
            m_tex_stats.m_color_tex_crc = texture_crc;
            m_tex_stats.m_alpha_exists = ((*i).m_channels[3] != 0xFFFFFFFF);
            break;
          case TEX_RT_NORMAL_MAP:
            m_tex_stats.m_normal_tex_mem += tex_mem_h;
            m_tex_stats.m_normal_tex_width = mips->m_width;
            m_tex_stats.m_normal_tex_height = mips->m_height;
            m_tex_stats.m_normal_tex_format = mips->m_format;
            m_tex_stats.m_normal_tex_crc = texture_crc;
            m_tex_stats.m_normal_map_exists = ((*i).m_channels[0] != 0xFFFFFFFF);
            break;
          case TEX_RT_GPI_MAP:
            m_tex_stats.m_gpi_tex_mem += tex_mem_h;
            m_tex_stats.m_gpi_tex_width = mips->m_width;
            m_tex_stats.m_gpi_tex_height = mips->m_height;
            m_tex_stats.m_gpi_tex_format = mips->m_format;
            m_tex_stats.m_gpi_tex_crc = texture_crc;
            m_tex_stats.m_gloss_exists = ((*i).m_channels[0] != 0xFFFFFFFF);
            m_tex_stats.m_parallax_exists = ((*i).m_channels[1] != 0xFFFFFFFF);
            m_tex_stats.m_incan_exists = ((*i).m_channels[2] != 0xFFFFFFFF);
            break;
          case TEX_RT_D_MAP:
            m_tex_stats.m_d_tex_mem += tex_mem_h;
            m_tex_stats.m_d_tex_width = mips->m_width;
            m_tex_stats.m_d_tex_height = mips->m_height;
            m_tex_stats.m_d_tex_format = mips->m_format;
            m_tex_stats.m_d_tex_crc = texture_crc;
            m_tex_stats.m_detail_exists = ((*i).m_channels[0] != 0xFFFFFFFF);
            break;
          case TEX_RT_CUSTOM_MAP_A:
            m_tex_stats.m_custom_a_tex_mem    += tex_mem_h;
            m_tex_stats.m_custom_a_tex_width   = mips->m_width;
            m_tex_stats.m_custom_a_tex_height  = mips->m_height;
            m_tex_stats.m_custom_a_tex_format  = mips->m_format;
            m_tex_stats.m_custom_a_tex_crc     = texture_crc;
            m_tex_stats.m_custom_a_exists      = ((*i).m_channels[0] != 0xFFFFFFFF);
            break;
          case TEX_RT_CUSTOM_MAP_B:
            m_tex_stats.m_custom_b_tex_mem    += tex_mem_h;
            m_tex_stats.m_custom_b_tex_width   = mips->m_width;
            m_tex_stats.m_custom_b_tex_height  = mips->m_height;
            m_tex_stats.m_custom_b_tex_format  = mips->m_format;
            m_tex_stats.m_custom_b_tex_crc     = texture_crc;
            m_tex_stats.m_custom_b_exists      = ((*i).m_channels[0] != 0xFFFFFFFF);
            break;
        }

        Console::Print("  texture %d\t(%d x %d)\t%s\t%d mips\n",count, mips->m_width, mips->m_height, IG::ColorFormatName(mips->m_format), mips->m_levels_used);

        ++i;
        count++;

        delete mips;
      }

      delete image;
    }
    else
    {
      RemoveTextureFromRemap(count);

      // erase the current texture, do not increment the iterator
      i = m_unique_textures.erase(i);
    }
  }
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Write the run-time shaders to the shader chunk in Ps3 format
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::WriteShader()
{
  ShaderBuffer shaderBuffer( m_shader );
  shaderBuffer.SetTextureIndex( TextureSlots::ColorMap,     m_texture_remap.m_texture_index[TextureSlots::ColorMap    ] );
  shaderBuffer.SetTextureIndex( TextureSlots::NormalMap,    m_texture_remap.m_texture_index[TextureSlots::NormalMap   ] );
  shaderBuffer.SetTextureIndex( TextureSlots::ExpensiveMap, m_texture_remap.m_texture_index[TextureSlots::ExpensiveMap] );
  shaderBuffer.SetTextureIndex( TextureSlots::DetailMap,    m_texture_remap.m_texture_index[TextureSlots::DetailMap   ] );
  shaderBuffer.SetTextureIndex( TextureSlots::CustomMapA,   m_texture_remap.m_texture_index[TextureSlots::CustomMapA  ] );
  shaderBuffer.SetTextureIndex( TextureSlots::CustomMapB,   m_texture_remap.m_texture_index[TextureSlots::CustomMapB  ] );

  V_TextureParam channels( TextureSlots::NumTextureSlots );
  channels[ TextureSlots::ColorMap      ] = TextureParam( GetRuntimeTexture( TextureSlots::ColorMap     ), true );
  channels[ TextureSlots::NormalMap     ] = TextureParam( GetRuntimeTexture( TextureSlots::NormalMap    ), true );
  channels[ TextureSlots::ExpensiveMap  ] = TextureParam( GetRuntimeTexture( TextureSlots::ExpensiveMap ), true );
  channels[ TextureSlots::DetailMap     ] = TextureParam( GetRuntimeTexture( TextureSlots::DetailMap    ), true );
  channels[ TextureSlots::CustomMapA    ] = TextureParam( GetRuntimeTexture( TextureSlots::CustomMapA   ), true );
  channels[ TextureSlots::CustomMapB    ] = TextureParam( GetRuntimeTexture( TextureSlots::CustomMapB   ), true );

  shaderBuffer.Build( channels );

  BasicBufferPtr shader_buf = m_shader_file.AddChunk( IGG::FILECHUNK_SHADER_ARRAY, IG::CHUNK_TYPE_ARRAY, sizeof(IGPS3::Shader), 0 );
  shader_buf->AdoptBuffer( shaderBuffer.RelinquishBuffer() );
  // At this point shaderBuffer is invalidated because it's internal buffer was passed into shader_buf
}

const RuntimeTexture* ShaderGenerator::GetRuntimeTexture( u32 channel ) const
{
  NOC_ASSERT( channel < TextureSlots::NumTextureSlots );

  if ( channel < TextureSlots::NumTextureSlots )
  {
    const u32 remapIndex = m_texture_remap.m_texture_index[ channel ];
    if ( remapIndex != 0xFFFFFFFF )
    {
      return &m_unique_textures[ remapIndex ];
    }
  }

  return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
// Dumps built info about the shaders to the console
//
////////////////////////////////////////////////////////////////////////////////////////////////

const char* GetRTFilter(u32 filter)
{
  switch (filter)
  {
  case IG::FILTER_POINT_SELECT_MIP:
      return "Point";
      break;
    case IG::FILTER_LINEAR_SELECT_MIP:
      return "Bilinear";
      break;
    case IG::FILTER_LINEAR_LINEAR_MIP:
      return "Tri linear";
      break;
    case IG::FILTER_ANISO_2_SELECT_MIP:
      return "2x Anisotropic";
      break;
    case IG::FILTER_ANISO_2_LINEAR_MIP:
      return "2x Anisotropic linear mip";
      break;
    case IG::FILTER_ANISO_4_SELECT_MIP:
      return "4x Anisotropic";
      break;
    case IG::FILTER_ANISO_4_LINEAR_MIP:
      return "4x Anisotropic linear mip";
      break;
  }

  return "Unknown";
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// returns the source texture name of the specified runtime texture
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::GetSourceTextureName(ShaderTextureType tex_type, std::string& tex_name)
{
  size_t rt_tex_cnt = m_unique_textures.size();
  for (size_t itex = 0; itex < rt_tex_cnt; ++itex)
  {
    RuntimeTexture* rt_tex = &m_unique_textures[itex];
    if (rt_tex->m_runtime_type != tex_type)
      continue;

    // return first valid source texture name from a texture channel
    for (u32 ichannel = 0; ichannel < 4; ichannel++)
    {
      if (rt_tex->m_channel_textures[ichannel].size())
      {
        tex_name = rt_tex->m_channel_textures[ichannel];
        return;
      }
    }
  }
  tex_name.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prints out shader debug info
//
////////////////////////////////////////////////////////////////////////////////////////////////

void ShaderGenerator::DumpShaderInfo()
{
  Console::Print("Shader stats:\n");
  Console::Print("High res: %d textures, %d mip levels, [%d Kbytes]\n", m_tex_stats.m_num_textures, m_tex_stats.m_num_mips_h, m_tex_stats.m_total_tex_mem_h/1024);
  Console::Print("Low res: %d textures, %d mip levels, [%d Kbytes]\n", m_tex_stats.m_num_textures, m_tex_stats.m_num_mips_l, m_tex_stats.m_total_tex_mem_l/1024);

  const std::string c_texture_types[] = { "Color Map", "Normal Map", "GPI Map", "Detail Map", "Custom Map A", "Custom Map B" };

  Console::Print("Output Textures\n");
  for ( u32 filterIndex = 0; filterIndex < TextureSlots::NumTextureSlots; ++filterIndex )
  {
    if ( m_texture_remap.m_texture_index[filterIndex] != 0xFFFFFFFF )
    {
      const std::string rtFilter = GetRTFilter( m_unique_textures[ m_texture_remap.m_texture_index[filterIndex] ].m_runtime.m_filter );
      Console::Print("  %s:  %d,%s\n",    c_texture_types[filterIndex].c_str(),
                                          m_texture_remap.m_texture_index[filterIndex], 
                                          rtFilter.c_str());
    }
  }

  Console::Print("Shader info\n");
  Console::Print("Shader type: ");
  if ( m_tex_stats.m_normal_map_exists )
  {
    Console::Print("USES NORMAL MAP, ");
  }
  if ( m_tex_stats.m_gloss_exists )
  {
    Console::Print("USES GLOSS MAP, ");
  }
  if ( m_tex_stats.m_parallax_exists )
  {
    Console::Print("USES PARALLAX MAP, ");
  }
  if ( m_tex_stats.m_incan_exists )
  {
    Console::Print("USES INCAN MAP, ");
  }
  if ( m_tex_stats.m_detail_exists )
  {
    Console::Print("USES DETAIL MAP MASK, ");
  }
  if ( m_tex_stats.m_custom_a_exists )
  {
    Console::Print("USES CUSTOM MAP A, ");
  }
  if ( m_tex_stats.m_custom_b_exists )
  {
    Console::Print("USES CUSTOM MAP B, ");
  }
  Console::Print("\n");

  // Additional information would need to be provided through a virtual function call on ShaderAsset.
  // It has been removed for now, but can be implemented per shader type if necessary.
}

// namespace IG
}
