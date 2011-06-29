#include "PipelinePch.h"
#include "ImageProcess.h"

#include "Platform/Types.h"
#include "Platform/Exception.h"

#include "Foundation/Checksum/CRC32.h"
#include "Foundation/String/Utilities.h"

#include "Foundation/Log.h"

#include "Pipeline/Image/Image.h"

#include <stdio.h>
#include <assert.h>
#include <string>
#include <vector>
#include <time.h>

using namespace Helium;
using namespace Helium::ImageProcess;

bool ImageProcess::g_PowerOfTwo            = false;
float ImageProcess::g_DefaultScaleX         = 1.0f;
float ImageProcess::g_DefaultScaleY         = 1.0f;
Helium::OutputColorFormat ImageProcess::g_DefaultOutputFormat   = Helium::OUTPUT_CF_DXT5;
Helium::PostMipImageFilter ImageProcess::g_DefaultPostMipFilter  = Helium::IMAGE_FILTER_NONE;


////////////////////////////////////////////////////////////////////////////////////////////////
//
//  LoadImages
//
//  Loads the source textures and fills in the remainder of the process class
//
////////////////////////////////////////////////////////////////////////////////////////////////
bool ImageProcess::Bank::LoadImages()
{
  Log::Bullet bullet( TXT( "Loading...\n" ) );

  uint32_t c=0;
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();++i)
  {
    tstring filename = (*i)->m_texture_file;
    size_t pos = filename.find_last_of( TXT( "\\/" ) );
    if (pos != tstring::npos)
      filename = filename.substr(pos+1);

    Log::Print( Log::Levels::Verbose, TXT( "[%d] : %s" ), c, filename.c_str());

    bool convert_to_linear = (*i)->m_is_normal_map ? false : true;
    Helium::Image* tex = Helium::Image::LoadFile((*i)->m_texture_file.c_str(), convert_to_linear, NULL);
    if (tex)
    {
      // convert all input images to either RGBA 8 bit or RGBA floating point
      if (ColorFormatHDR( (*i)->m_output_format))
      {
        tex->m_NativeFormat = Helium::CF_RGBAFLOATMAP;
      }
      else
      {
        tex->m_NativeFormat = Helium::CF_ARGB8888;
      }

      tchar_t* type[] = { TXT( "2D TEXTURE" ), TXT( "CUBE MAP") , TXT( "VOLUME TEXTURE" ) };
      if (tex->Type()==Helium::Image::VOLUME)
      {
        Log::Print( Log::Levels::Verbose, TXT( "%s %d x %d x %d\n" ),type[tex->Type()],tex->m_Width,tex->m_Height,tex->m_Depth);
      }
      else
      {
        Log::Print( Log::Levels::Verbose, TXT( "%s %d x %d\n" ),type[tex->Type()],tex->m_Width,tex->m_Height);
      }

      (*i)->m_texture = tex;
    }
    else
    {
      Log::Warning( TXT( "Failed to load '%s'\n" ),(*i)->m_texture_file.c_str());
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

bool ImageProcess::Bank::AdjustImages()
{
  Log::Bullet bullet( TXT( "Adjusting...\n" ) );

  // relative scale should be done before the power of 2 restriction...
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();i++)
  {
    if ((*i)->m_texture)
    {
      // now if there is a relative scale set in the input then scale


      if ( !IsOne( (*i)->m_relscale_x ) || !IsOne( (*i)->m_relscale_y ) )
      {
        Helium::Image* new_tex = (*i)->m_texture->RelativeScaleImage((*i)->m_relscale_x, (*i)->m_relscale_y, (*i)->m_texture->m_NativeFormat, Helium::MIP_FILTER_CUBIC);
        if (new_tex)
        {
          throw Helium::Exception( TXT( "Failed to rescale, aborting" ) );
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
      if ( !IsPowerOfTwo((*i)->m_texture->m_Width) || !IsPowerOfTwo((*i)->m_texture->m_Height) )
      {
        // this texture is not a power of 2 so rescale it to fix it
        Log::Warning( TXT( "Rescaling texture '%s', it is not a power of 2 (%d x %d)\n" ),(*i)->m_texture_file.c_str(),(*i)->m_texture->m_Width,(*i)->m_texture->m_Height);

        Helium::Image* new_tex = (*i)->m_texture->ScaleUpNextPowerOfTwo((*i)->m_texture->m_NativeFormat,Helium::MIP_FILTER_CUBIC);
        if (!new_tex)
        {
          throw Helium::Exception( TXT( "Failed to rescale, aborting" ) );
        }
        delete (*i)->m_texture;
        (*i)->m_texture = new_tex;
      }
    }

    if ((*i)->m_texture == NULL)
    {
      tchar_t buf[120];
      _stprintf(buf, TXT( "Image %s has no pixel data, file may be missing or corrupted" ), (*i)->m_texture_file.c_str());
      throw Helium::Exception(buf);
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
bool ImageProcess::Bank::CompressImages()
{
  Log::Bullet bullet( TXT( "Compressing...\n" ) );

  uint32_t count=0;
  for (V_Definition::iterator i=m_textures.begin();i!=m_textures.end();i++)
  {
    if ((*i)->m_texture)
    {
      Helium::MipGenOptions m;
      if ( (*i)->m_force_single_mip_level || !IsPowerOfTwo((*i)->m_texture->m_Width) || !IsPowerOfTwo((*i)->m_texture->m_Height) )
      {
        // NP2 textures get only 1 mip level
        m.m_Levels  = 1;
      }

      // set the output format
      m.m_OutputFormat  = (*i)->m_output_format;
      m.m_PostFilter    = (*i)->m_post_filter;

      // generate mipset
      Helium::MipSet* mips = NULL;
      if ( (*i)->m_is_normal_map )
      {
        // does a clone if sizes are the same
        Helium::Image* nt = (*i)->m_texture->ScaleImage((*i)->m_texture->m_Width, (*i)->m_texture->m_Height, (*i)->m_texture->m_NativeFormat, m.m_Filter);

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
        throw Helium::Exception( TXT( "Failed to generate mips, aborting" ) );
      }

      uint32_t size = 0;
      for (uint32_t level=0; level<mips->m_levels_used; level++)
        size += mips->m_datasize[level];

      Log::Print( Log::Levels::Verbose, TXT( "%8.02f kb used by '%s'\n" ), (float)(size) / 1024.f, (*i)->m_texture_file.c_str() );

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
bool ImageProcess::Bank::Pack()
{
  // load the images, missing textures are written as default textures but
  // textures ommited for a given level are written as NULL entries in the
  // contents of the pack file.
  if (!ImageProcess::Bank::LoadImages())
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
  if (!ImageProcess::Bank::AdjustImages())
  {
    return false;
  }

  // compress and generate mip levels
  if (!ImageProcess::Bank::CompressImages())
  {
    return false;
  }

  return true;
}

bool Bank::WriteDebugFile( const tstring& debug_file )
{
  if ( debug_file.empty() )
  {
    return true;
  }

  FILE* f = _tfopen( debug_file.c_str(), TXT( "w" ) );
  if ( !f )
  {
    Log::Warning( TXT( "Failed to open '%s' for write.\n" ), debug_file.c_str() );
    return false;
  }

  tchar_t* type[] = { TXT( "2D TEXTURE" ), TXT( "CUBE MAP" ), TXT( "VOLUME TEXTURE" )};

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