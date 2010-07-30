#include "Precompile.h"
#include "XMLShaderLoader.h"
#include "ShaderManager.h"

#include "Foundation/Log.h"
#include "Foundation/File/Path.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <tinyxml.h>
#include <sys/stat.h>

using namespace Helium;

////////////////////////////////////////////////////////////////////////////////////////////////
u32 TextureAddressModes(const char* text)
{
  if (_stricmp(text, "wrap" )==0)
  {
    return (u32)D3DTADDRESS_WRAP;
  }
  else if (_stricmp(text, "mirror" )==0)
  {
    return (u32)D3DTADDRESS_MIRROR;
  }
  else if (_stricmp(text, "clamp" )==0)
  {
    return (u32)D3DTADDRESS_CLAMP;
  }
  else
  {
      Log::Warning( TXT( "'%s' is an unknown texture wrap mode, defaulting to 'wrap'\n" ) ,text);
  }

  return (u32)D3DTADDRESS_WRAP;
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 TextureFilterMode(const char* text)
{
  if (_stricmp(text, "point" )==0)
  {
    return Render::Texture::FILTER_POINT;
  }
  else if (_stricmp(text, "linear" )==0)
  {
    return Render::Texture::FILTER_LINEAR;
  }
  else if (_stricmp(text, "anisotropic" )==0)
  {
    return Render::Texture::FILTER_ANISOTROPIC;
  }
  else
  {
      Log::Warning( TXT( "'%s' is an unknown texture filter mode, defaulting to 'linear'\n" ),text);
  }

  return Render::Texture::FILTER_LINEAR;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadColor(const TiXmlElement* node,float* col)
{
  col[0]=1.0f;
  col[1]=1.0f;
  col[2]=1.0f;
  col[3]=1.0f;

  float f;
  if (node->QueryFloatAttribute( "red",&f)==TIXML_SUCCESS)
  {
    col[0]=f;
  }
  if (node->QueryFloatAttribute("green",&f)==TIXML_SUCCESS)
  {
    col[1]=f;
  }
  if (node->QueryFloatAttribute("blue",&f)==TIXML_SUCCESS)
  {
    col[2]=f;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadAlphaSettings(const TiXmlElement* node,Render::Shader* shader)
{
  shader->m_alpha_type=Render::Shader::ALPHA_OPAQUE;

  const char* res = node->Attribute("type");
  if (res)
  {
    if (_stricmp(res,"opaque")==0)
    {
      shader->m_alpha_type=Render::Shader::ALPHA_OPAQUE;
    }
    else if (_stricmp(res,"additive")==0)
    {
      shader->m_alpha_type=Render::Shader::ALPHA_ADDITIVE;
    }
    else if (_stricmp(res,"blended")==0)
    {
      shader->m_alpha_type=Render::Shader::ALPHA_BLENDED;
    }
    else if (_stricmp(res,"cutout")==0)
    {
      shader->m_alpha_type=Render::Shader::ALPHA_CUTOUT;
    }
  } 
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadGlossSettings(const TiXmlElement* node,Render::Shader* shader)
{
  shader->m_gloss_scale=1.0f;

  float f;
  if (node->QueryFloatAttribute("scale",&f)==TIXML_SUCCESS)
  {
    shader->m_gloss_scale=f;
    if (shader->m_gloss_scale<0.0f)
      shader->m_gloss_scale=0.0f;
  }


  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadSpecularSettings(const TiXmlElement* node,Render::Shader* shader)
{
  shader->m_specular_power=20.0f;

  float f;
  if (node->QueryFloatAttribute("power",&f)==TIXML_SUCCESS)
  {
    shader->m_specular_power=f;
    if (shader->m_specular_power<0.0f)
      shader->m_specular_power=0.0f;
  }
  if (node->QueryFloatAttribute("lod",&f)==TIXML_SUCCESS)
  {
    shader->m_env_lod=f;
    if (shader->m_env_lod<0.0f)
      shader->m_env_lod=0.0f;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadNormalSettings(const TiXmlElement* node,Render::Shader* shader)
{
  shader->m_normal_scale = 1.0f;

  float f;
  if (node->QueryFloatAttribute("scale",&f)==TIXML_SUCCESS)
  {
    shader->m_normal_scale=f;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////
bool LoadParallaxSettings(const TiXmlElement* node,Render::Shader* shader)
{
  shader->m_parallax_scale=1.0f;
  shader->m_parallax_bias = 0.0f;

  float f;
  if (node->QueryFloatAttribute("scale",&f)==TIXML_SUCCESS)
  {
    shader->m_parallax_scale=f;
  }
  if (node->QueryFloatAttribute("bias",&f)==TIXML_SUCCESS)
  {
    shader->m_parallax_bias=f;
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool ParseXMLTextureSettings( const TiXmlElement* node, const tchar* baseDirectory, Render::TextureSettings& textureSettings )
{
  textureSettings.m_Path = baseDirectory;

  const char* tres = node->Attribute("filename");
  if (tres)
  {
      tstring temp;
      bool converted = Platform::ConvertString( tres, temp );
      HELIUM_ASSERT( converted );
        textureSettings.m_Path += temp;
  }
  else
  {
    // must have a filaname
      Log::Error( TXT( "A texture node number have a 'filename' attribute\n" ) );
    return false;
  }

  tres = node->Attribute("format");
  if (tres)
  {
    if (_stricmp(tres,"ARGB8888")==0)
    {
      textureSettings.m_Format=D3DFMT_A8R8G8B8;
    }
    else if (_stricmp(tres,"ARGB4444")==0)
    {
      textureSettings.m_Format =D3DFMT_A4R4G4B4;
    }
    else if (_stricmp(tres,"RGB565")==0)
    {
      textureSettings.m_Format =D3DFMT_R5G6B5;
    }
    else if (_stricmp(tres,"DXT1")==0)
    {
      textureSettings.m_Format =D3DFMT_DXT1;
    }
    else if (_stricmp(tres,"DXT3")==0)
    {
      textureSettings.m_Format =D3DFMT_DXT3;
    }
    else if (_stricmp(tres,"DXT5")==0)
    {
      textureSettings.m_Format =D3DFMT_DXT5;
    }
    else if (_stricmp(tres,"L8")==0)
    {
      textureSettings.m_Format =D3DFMT_L8;
    }
    else if (_stricmp(tres,"A8")==0)
    {
      textureSettings.m_Format =D3DFMT_A8;
    }
    else
    {
        Log::Warning( TXT( "'%s' is an unkown texture format, picking based on the file\n" ), tres);
      textureSettings.m_Format =D3DFMT_UNKNOWN;
    }
  }
  else
  {
    // The loader will decide
    textureSettings.m_Format =D3DFMT_UNKNOWN;
  }

  int ires;
  if (node->QueryIntAttribute("levels",&ires)==TIXML_SUCCESS)
  {
    if (ires>0)
      textureSettings.m_Levels = ires;
    else
      textureSettings.m_Levels = D3DX_DEFAULT;
  }

  // fill in the none essential sampler properties from the XML file
  tres = node->Attribute("wrap_u");
  if (tres)
  {
    textureSettings.m_WrapU = TextureAddressModes(tres);
  }

  tres = node->Attribute("wrap_v");
  if (tres)
  {
    textureSettings.m_WrapV = TextureAddressModes(tres);
  }

  tres = node->Attribute("filter");
  if (tres)
  {
    textureSettings.m_Filter = TextureFilterMode(tres);
  }

  if (node->QueryIntAttribute("anisotropy",&ires)==TIXML_SUCCESS)
  {
    textureSettings.m_Anisotropy = ires;
  }
  
  float fres;
  if (node->QueryFloatAttribute("mip_bias",&fres)==TIXML_SUCCESS)
  {
    textureSettings.m_MipBias = fres;
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Render::Shader* Render::XMLShaderLoader::ParseFile( const tchar* fname, ShaderManager* db )
{
  Render::Shader* sh = new Render::Shader(db,fname);

  struct _stat64i32 s;
  if ( _tstat( fname, &s ) < 0 )
  {
    return sh;
  }

  std::string temp;
  bool converted = Platform::ConvertString( fname, temp );
  HELIUM_ASSERT( converted );

  TiXmlDocument shader( temp.c_str() );
  if (!shader.LoadFile())
  {
      Log::Error( TXT( "Failed to load shader file '%s': '%s' at line: %d, column: %d\n" ), fname, shader.ErrorDesc(), shader.ErrorRow(),shader.ErrorCol());
    return sh;
  }

  const TiXmlNode* root = shader.RootElement();
  if (_stricmp(root->Value(),"shader")!=0)
  {
    // this is not a texture pack
      Log::Error( TXT( "Top level tag of 'shader' not found\n" ) );
    return sh;
  }

  TextureSettings settings;
  Helium::Path shaderPath( fname );
  tstring shaderDirectory = shaderPath.Directory();

	for (const TiXmlElement* n = root->FirstChildElement(); n != 0; n = n->NextSiblingElement()) 
	{
    if (_stricmp(n->Value(),"colormap")==0)
    {
      if (!ParseXMLTextureSettings( n, shaderDirectory.c_str(), settings )
          || !db->LoadTextureWithSettings(settings,sh,Render::Texture::SAMPLER_BASE_MAP))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"normalmap")==0)
    {
      if (!ParseXMLTextureSettings( n, shaderDirectory.c_str(), settings )
          || !db->LoadTextureWithSettings(settings,sh,Render::Texture::SAMPLER_NORMAL_MAP))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"glossmap")==0)
    {
      if (!ParseXMLTextureSettings( n, shaderDirectory.c_str(), settings )
          || !db->LoadTextureWithSettings(settings,sh,Render::Texture::SAMPLER_GLOSS_MAP))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"incanmap")==0)
    {
      if (!ParseXMLTextureSettings( n, shaderDirectory.c_str(), settings )
          || !db->LoadTextureWithSettings(settings,sh,Render::Texture::SAMPLER_INCAN_MAP))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"parallaxmap")==0)
    {
      if (!ParseXMLTextureSettings( n, shaderDirectory.c_str(), settings )
          || !db->LoadTextureWithSettings(settings,sh,Render::Texture::SAMPLER_PARALLAX_MAP))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"alpha")==0)
    {
      if (!LoadAlphaSettings(n,sh))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"basetint")==0)
    {
      if (!LoadColor(n,&sh->m_basetint[0]))
      {
        return sh;
      }

      sh->m_basetint[3]=1.0f;
      float f;
      if (n->QueryFloatAttribute("alpha",&f)==TIXML_SUCCESS)
      {
        sh->m_basetint[3]=f;
      }
    }
    else if (_stricmp(n->Value(),"dirtytint")==0)
    {
      if (!LoadColor(n,&sh->m_glosstint[0]))
      {
        return sh;
      }

      sh->m_glosstint[3]=1.0f;
      float f;
      if (n->QueryFloatAttribute("dirty_factor",&f)==TIXML_SUCCESS)
      {
        sh->m_glosstint[3]=f;
      }
    }
    else if (_stricmp(n->Value(),"gloss")==0)
    {
      if (!LoadGlossSettings(n,sh))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"specular")==0)
    {
      if (!LoadSpecularSettings(n,sh))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"normal")==0)
    {
      if (!LoadNormalSettings(n,sh))
      {
        return sh;
      }
    }
    else if (_stricmp(n->Value(),"parallax")==0)
    {
      if (!LoadParallaxSettings(n,sh))
      {
        return sh;
      }
    }
	}

  return sh;
}
