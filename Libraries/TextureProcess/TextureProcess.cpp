////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TextureProcess.cpp
//
//  Written by: Rob Wyatt
//
//  Generic texture packer
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <time.h>

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "Platform/Types.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Exception.h"
#include "Foundation/Checksum/CRC32.h"

#include "Foundation/Log.h"
#include "Finder/Finder.h"

#include "Texture/Texture.h"
#include "TextureProcess.h"

using namespace TextureProcess;

bool                    TextureProcess::g_PowerOfTwo            = false;
float                   TextureProcess::g_DefaultScaleX         = 1.0f;
float                   TextureProcess::g_DefaultScaleY         = 1.0f;
IG::OutputColorFormat   TextureProcess::g_DefaultOutputFormat   = IG::OUTPUT_CF_DXT5;
IG::PostMipImageFilter  TextureProcess::g_DefaultPostMipFilter  = IG::IMAGE_FILTER_NONE;


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LoadImages
//
//  Loads the source textures and fills in the remainder of the process class
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool TextureProcess::Bank::LoadImages()
{
  Log::Bullet bullet ("Loading...\n");

  u32 c=0;
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();++i)
  {
    std::string filename = (*i)->m_texture_file;
    size_t pos = filename.find_last_of("\\/");
    if (pos != std::string::npos)
      filename = filename.substr(pos+1);

    Log::Print( Log::Levels::Verbose, "[%d] : %s", c, filename.c_str());

    bool convert_to_linear = (*i)->m_is_normal_map ? false : true;
    IG::Texture* tex = IG::Texture::LoadFile((*i)->m_texture_file.c_str(), convert_to_linear, NULL);
    if (tex)
    {
      // convert all input images to either RGBA 8 bit or RGBA floating point
      if (ColorFormatHDR( (*i)->m_output_format))
      {
        tex->m_NativeFormat = IG::CF_RGBAFLOATMAP;
      }
      else
      {
        tex->m_NativeFormat = IG::CF_ARGB8888;
      }

      char* type[] = {"2D TEXTURE","CUBE MAP","VOLUME TEXTURE"};
      if (tex->Type()==IG::Texture::VOLUME)
      {
        Log::Print( Log::Levels::Verbose, "%s %d x %d x %d\n",type[tex->Type()],tex->m_Width,tex->m_Height,tex->m_Depth);
      }
      else
      {
        Log::Print( Log::Levels::Verbose, "%s %d x %d\n",type[tex->Type()],tex->m_Width,tex->m_Height);
      }

      (*i)->m_texture = tex;
    }
    else
    {
      Log::Warning("Failed to load '%s'\n",(*i)->m_texture_file.c_str());
      return false;
    }
    c++;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AdjustImages
//
//  Adjust the source image for being non power of 2 or if it requires prescaling
//
////////////////////////////////////////////////////////////////////////////////////////////////

inline bool IsOne(float v)
{
    const float diff = fabsf(1.f - v);
    return diff <= 0.001f;
}

bool TextureProcess::Bank::AdjustImages()
{
  Log::Bullet bullet ("Adjusting...\n");

  // relative scale should be done before the power of 2 restriction...
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();i++)
  {
    if ((*i)->m_texture)
    {
      // now if there is a relative scale set in the input then scale


      if ( !IsOne( (*i)->m_relscale_x ) || !IsOne( (*i)->m_relscale_y ) )
      {
        IG::Texture* new_tex = (*i)->m_texture->RelativeScaleImage((*i)->m_relscale_x, (*i)->m_relscale_y, (*i)->m_texture->m_NativeFormat, IG::MIP_FILTER_CUBIC);
        if (new_tex)
        {
          throw Nocturnal::Exception("Failed to rescale, aborting");
        }
        delete (*i)->m_texture;
        (*i)->m_texture = new_tex;
      }
    }
  }


  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();i++)
  {
    if ((*i)->m_texture && (*i)->m_force_power_of_2)
    {
      // first process for being a power of 2
      if ( !Math::IsPowerOfTwo((*i)->m_texture->m_Width) || !Math::IsPowerOfTwo((*i)->m_texture->m_Height) )
      {
        // this texture is not a power of 2 so rescale it to fix it
        Log::Warning("Rescaling texture '%s', it is not a power of 2 (%d x %d)\n",(*i)->m_texture_file.c_str(),(*i)->m_texture->m_Width,(*i)->m_texture->m_Height);

        IG::Texture* new_tex = (*i)->m_texture->ScaleUpNextPowerOfTwo((*i)->m_texture->m_NativeFormat,IG::MIP_FILTER_CUBIC);
        if (!new_tex)
        {
          throw Nocturnal::Exception("Failed to rescale, aborting");
        }
        delete (*i)->m_texture;
        (*i)->m_texture = new_tex;
      }
    }

    if ((*i)->m_texture == NULL)
    {
      char buf[120];
      sprintf(buf, "Texture %s has no pixel data, file may be missing or corrupted", (*i)->m_texture_file.c_str());
      throw Nocturnal::Exception(buf);
    }
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CompressImages
//
//  Compress and generate the output data along with all the associated mip maps
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool TextureProcess::Bank::CompressImages()
{
  Log::Bullet bullet ("Compressing...\n");

  u32 count=0;
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();i++)
  {
    if ((*i)->m_texture)
    {
      IG::MipGenOptions m;
      if ( (*i)->m_force_single_mip_level || !Math::IsPowerOfTwo((*i)->m_texture->m_Width) || !Math::IsPowerOfTwo((*i)->m_texture->m_Height) )
      {
        // NP2 textures get only 1 mip level
        m.m_Levels  = 1;
      }

      // set the output format
      m.m_OutputFormat  = (*i)->m_output_format;
      m.m_PostFilter    = (*i)->m_post_filter;

      // generate mipset
      IG::MipSet* mips = NULL;
      if ( (*i)->m_is_normal_map )
      {
        // does a clone if sizes are the same
        IG::Texture* nt = (*i)->m_texture->ScaleImage((*i)->m_texture->m_Width, (*i)->m_texture->m_Height, (*i)->m_texture->m_NativeFormat, m.m_Filter);

        nt->PrepareFor2ChannelNormalMap((*i)->m_is_detail_normal_map, (*i)->m_is_detail_map_only);

        //Don't convert to sRGB
        m.m_ConvertToSrgb = false;
        mips              = nt->GenerateMipSet(m, (*i)->m_runtime);

        delete nt;
      }
      else
      {
        //Convert to sRGB
        m.m_ConvertToSrgb = true;
        mips              = (*i)->m_texture->GenerateMipSet(m, (*i)->m_runtime);
      }

      if (!mips)
      {
        throw Nocturnal::Exception("Failed to generate mips, aborting");
      }

      u32 size = 0;
      for (u32 level=0; level<mips->m_levels_used; level++)
        size += mips->m_datasize[level];

      Log::Print( Log::Levels::Verbose, "%8.02f kb used by '%s'\n", (float)(size) / 1024.f, (*i)->m_texture_file.c_str() );

      (*i)->m_mips = mips;
    }

    count++;
  }

  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pack
//
//  Performs default processing of textures in global texture array
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool TextureProcess::Bank::Pack()
{
  // load the images, missing textures are written as default textures but
  // textures ommited for a given level are written as NULL entries in the
  // contents of the pack file.
  if (!TextureProcess::Bank::LoadImages())
  {
    return false;
  }

  // Give our definition processors a chance to modify the defintions if they need to
  V_Definition::iterator it = m_textures.begin();
  V_Definition::iterator end = m_textures.end();
  for ( ; it != end; ++it )
  {
    m_ProcessDefinition.Raise( *it );
  }

  m_PostLoad.Raise( PostLoadArgs() );

  // resize if not a power of two or if we are rescaling the input
  if (!TextureProcess::Bank::AdjustImages())
  {
    return false;
  }

  // compress and generate mip levels
  if (!TextureProcess::Bank::CompressImages())
  {
    return false;
  }

  return true;
}

bool Bank::WriteDebugFile( const std::string& debug_file )
{
  if ( debug_file.empty() )
  {
    return true;
  }

  FILE* f = fopen( debug_file.c_str(), "w" );
  if ( !f )
  {
    Log::Warning( "Failed to open '%s' for write.\n", debug_file.c_str() );
    return false;
  }

  char* type[] = {"2D TEXTURE","CUBE MAP","VOLUME TEXTURE"};

  V_Definition::iterator it = m_textures.begin();
  V_Definition::iterator end = m_textures.end();
  for ( ; it != end; ++it )
  {
    const DefinitionPtr& def = *it;

    fprintf( f,"%s [%s,%s,%dx%dx%d][%s]\n",def->m_texture_file.c_str(),type[def->m_texture->Type()],ColorFormatName(def->m_output_format),def->m_mips->m_width,def->m_mips->m_height,def->m_mips->m_depth,def->m_enum.c_str());
  }

  fclose( f );

  return true;
}