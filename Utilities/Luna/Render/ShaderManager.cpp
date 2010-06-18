#include "Precompile.h"
#include "ShaderManager.h"
#include "Renderer.h"
#include "Misc.h"
#include "ShaderLoader.h"
#include "XMLShaderLoader.h"

#include "Foundation/File/Path.h"

#include <d3dx9.h>

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Texture::Texture(const char* fname)
{
  m_filename = fname;  
  m_timestamp = (u64)-1;
  m_crc=StringHashDJB2(fname);
  m_load_count=0;
  m_d3d_texture = 0;

  m_wrap_u = (u32)D3DTADDRESS_WRAP;
  m_wrap_v = (u32)D3DTADDRESS_WRAP;
  m_filter = FILTER_LINEAR;
  m_aniso = 1;
  m_mip_bias = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Texture::~Texture()
{
  if (m_d3d_texture)
  {
    m_d3d_texture->Release();
    m_d3d_texture = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Shader::Shader(ShaderDatabase* sd, const char* shader)
{
  m_filename = shader;  
  m_timestamp = (u64)-1;
  m_crc=StringHashDJB2(shader);
  m_load_count = 0;
  m_flags = SHDR_FLAG_GPI_MAP;
  m_sd = sd;

  // set the default textures
  m_textures[0] = sd->LoadTexture("@@base",D3DFMT_UNKNOWN);
  m_textures[1] = sd->LoadTexture("@@normal",D3DFMT_UNKNOWN);
  m_textures[2] = sd->LoadTexture("@@gpi",D3DFMT_UNKNOWN);
  m_textures[3] = sd->LoadTexture("@@parallax",D3DFMT_UNKNOWN);
  m_textures[4] = sd->LoadTexture("@@incan",D3DFMT_UNKNOWN);

  m_alpha_type=igDXRender::Shader::ALPHA_OPAQUE;
  m_glosstint[0] = 1.0f;
  m_glosstint[1] = 1.0f;
  m_glosstint[2] = 1.0f;
  m_glosstint[3] = 0.0f;

  m_basetint[0] = 1.0f;
  m_basetint[1] = 1.0f;
  m_basetint[2] = 1.0f;
  m_basetint[3] = 1.0f;

  m_incan_scale = 1.0f;
  m_gloss_scale = 1.0f;
  m_specular_power = 20.0f;
  m_normal_scale = 1.0f;
  m_parallax_scale = 0.005f;
  m_parallax_bias = 0.0f;
  m_env_lod = 5.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Shader::~Shader()
{
  // reduce the ref counts of the textures this shader was using
  for (u32 t=0;t<igDXRender::Texture::__SAMPLER_LAST__;t++)
  {
    if (m_textures[t]!=0xffffffff)
    {
      igDXRender::Texture* tex = m_sd->ResolveTexture(m_textures[t]);
      tex->DecrementUsage();
      m_textures[t] = 0xffffffff;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::Shader::ReplaceTexture(u32 th, u32 slot)
{
  if (m_textures[slot]!=0xffffffff)
  {
    igDXRender::Texture* current = m_sd->ResolveTexture(m_textures[slot]);
    if (current->DecrementUsage()==0)
    {
#pragma TODO("delete now?")
    }
  }

  m_textures[slot] = th;

  if (th!=0xffffffff)
  {
    igDXRender::Texture* ntex = m_sd->ResolveTexture(th);
    ntex->IncrementUsage();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::ShaderDatabase::ShaderDatabase(igDXRender::Render* rc)
: m_render_class ( rc )
{
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::ShaderDatabase::~ShaderDatabase()
{
  // go thorugh all the shaders and delete them
  u32 shader_count = (u32)m_loaded_shaders.size();
  for (u32 i=0;i<shader_count;i++)
  {
    if (m_loaded_shaders[i])
    {
      delete m_loaded_shaders[i];
      m_loaded_shaders[i] = 0;
    }
  }

  u32 texture_count = (u32)m_loaded_textures.size();
  for (u32 i=0;i<texture_count;i++)
  {
    if (m_loaded_textures[i])
    {
      delete m_loaded_textures[i];
      m_loaded_textures[i]=0;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
static void FillTexture(IDirect3DTexture9* tex, u32 val)
{
  D3DLOCKED_RECT rect;

  tex->LockRect(0,&rect,0,0);

  for (u32 y=0;y<16;y++)
  {
    u32* line_data = (u32*) (((u8*)rect.pBits)+(y*rect.Pitch));
    for (u32 x=0;x<16;x++)
      line_data[x]=val;
  }
  tex->UnlockRect(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::CreateDefaults()
{
  printf("Default shaders/textures created\n");

  IDirect3DDevice9* device = m_render_class->GetD3DDevice();

  IDirect3DTexture9* default_base;
  IDirect3DTexture9* default_normal;
  IDirect3DTexture9* default_gloss;
  IDirect3DTexture9* default_parallax;
  IDirect3DTexture9* default_incan;
  IDirect3DTexture9* default_gpi;
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_base,0);
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_normal,0);
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_gloss,0);
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_parallax,0);
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_incan,0);
  device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_gpi,0);
  FillTexture(default_base,D3DCOLOR_ARGB(0xff,0x80,0x80,0x80));
  FillTexture(default_normal,D3DCOLOR_ARGB(0xff,0x80,0x80,0xff));
  FillTexture(default_gloss,D3DCOLOR_ARGB(0xff,0x00,0x00,0x00));
  FillTexture(default_parallax,D3DCOLOR_ARGB(0xff,0x00,0x00,0x00));
  FillTexture(default_incan,D3DCOLOR_ARGB(0xff,0x0,0x0,0x0));
  FillTexture(default_gpi,D3DCOLOR_ARGB(0xff,0x00,0x0,0x00));  // alpha, gloss,incan,parallax

  igDXRender::Texture* base_texture = new igDXRender::Texture("@@base");
  igDXRender::Texture* normal_texture = new igDXRender::Texture("@@normal");
  igDXRender::Texture* gloss_texture = new igDXRender::Texture("@@gloss");
  igDXRender::Texture* parallax_texture = new igDXRender::Texture("@@parallax");
  igDXRender::Texture* incan_texture = new igDXRender::Texture("@@incan");
  igDXRender::Texture* gpi_texture = new igDXRender::Texture("@@gpi");

  base_texture->m_d3d_texture = default_base;
  m_loaded_textures.push_back(base_texture);

  normal_texture->m_d3d_texture = default_normal;
  m_loaded_textures.push_back(normal_texture);

  gloss_texture->m_d3d_texture = default_gloss;
  m_loaded_textures.push_back(gloss_texture);

  parallax_texture->m_d3d_texture = default_parallax;
  m_loaded_textures.push_back(parallax_texture);

  incan_texture->m_d3d_texture = default_incan;
  m_loaded_textures.push_back(incan_texture);

  gpi_texture->m_d3d_texture = default_gpi;
  m_loaded_textures.push_back(gpi_texture);

  // now create the shader (this will internally use the default textures)
  igDXRender::Shader* sh = new igDXRender::Shader(this,"@@default");
  AddShader(sh);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// add a shader pointer to the array of loaded shaders
u32 igDXRender::ShaderDatabase::AddShader(igDXRender::Shader* sh)
{
  u32 shader_count = (u32)m_loaded_shaders.size();
  u32 handle = 0xffffffff;
  for (u32 i=0;i<shader_count;i++)
  {
    if (m_loaded_shaders[i]==0)
    {
      handle = i;
      break;
    }
  }

  if (handle!=0xffffffff)
  {
    m_loaded_shaders[handle] = sh;
  }
  else
  {
    m_loaded_shaders.push_back(sh);
    handle = (u32)m_loaded_shaders.size()-1;
  }

  return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// shader is an XML file
u32 igDXRender::ShaderDatabase::LoadNewShader( const char* fname, ShaderLoaderPtr loader )
{
  Nocturnal::Path shaderPath( fname );

  if ( loader == NULL )
  {
    std::string extension = shaderPath.Extension();
    if ( extension == "xml" )
    {
      loader = new XMLShaderLoader ();
    }
    else
    {
      printf( "WARNING: Unknown shader type '%s'.\n", fname );
    }
  }

  Shader* shader = loader->ParseFile( fname, this );

  if ( shader )
  {
    return AddShader( shader );
  }
  else
  {
    printf( "WARNING: Unable to parse shader '%s'.\n", fname );
  }

  return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 igDXRender::ShaderDatabase::LoadShader(const char* fname, bool inc, ShaderLoaderPtr loader)
{
  u32 handle = FindShader(fname);

  if (handle==0xffffffff)
  {
    // wasn't found, so load it
    handle = LoadNewShader(fname, loader);
  }

  if (inc && (handle!=0xffffffff))
  {
    m_loaded_shaders[handle]->IncrementUsage();
  }

  return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 igDXRender::ShaderDatabase::FindShader(const char* fname)
{
  u32 crc = StringHashDJB2(fname);

  u32 shader_count = (u32)m_loaded_shaders.size();
  u32 handle = 0xffffffff;
  for (u32 i=0;i<shader_count;i++)
  {
    if (m_loaded_shaders[i])
    {
      if ((m_loaded_shaders[i]->m_crc==crc))
      {
        handle = i;
        break;
      }
    }
  }

  return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 igDXRender::ShaderDatabase::DuplicateShader(u32 handle,const char* new_shader)
{
  // convert the handle to a pointer
  igDXRender::Shader* sh = ResolveShader(handle);

  // this seems like a valid shader, allocate a shader
  igDXRender::Shader* newsh = new igDXRender::Shader(this,new_shader);
 
  // copy the textures and increment the usage on any that are valid
  for (u32 t=0;t<igDXRender::Texture::__SAMPLER_LAST__;t++)
  {
    newsh->m_textures[t] = sh->m_textures[t];
    if (sh->m_textures[t]!=0xffffffff)
    {
      igDXRender::Texture* tex = ResolveTexture(sh->m_textures[t]);
      tex->IncrementUsage();
    }
  }

  newsh->m_flags = sh->m_flags;
  newsh->m_alpha_type = sh->m_alpha_type;
  newsh->m_basetint[0] = sh->m_basetint[0];
  newsh->m_basetint[1] = sh->m_basetint[1];
  newsh->m_basetint[2] = sh->m_basetint[2];
  newsh->m_basetint[3] = sh->m_basetint[3];
  newsh->m_glosstint[0] = sh->m_glosstint[0];
  newsh->m_glosstint[1] = sh->m_glosstint[1];
  newsh->m_glosstint[2] = sh->m_glosstint[2];
  newsh->m_glosstint[3] = sh->m_glosstint[3];
  newsh->m_gloss_scale = sh->m_gloss_scale;
  newsh->m_specular_power = sh->m_specular_power;
  newsh->m_normal_scale = sh->m_normal_scale;
  newsh->m_parallax_bias = sh->m_parallax_bias;
  newsh->m_parallax_scale = sh->m_parallax_scale;
  newsh->m_env_lod = sh->m_env_lod;

  return AddShader(newsh);
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 igDXRender::ShaderDatabase::LoadTexture(const char* fname,D3DFORMAT fmt, u32 levels,bool inc)
{
  u32 handle = FindTexture(fname);

  if (handle==0xffffffff)
  {
    // wasn't found so load the new texture
    igDXRender::Texture* t = new igDXRender::Texture(fname);

    if (FAILED(D3DXCreateTextureFromFileExA(m_render_class->GetD3DDevice(),fname,0,0,levels,0,fmt,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0,0,0,&t->m_d3d_texture)))
    {
      printf("Failed to load texture '%s'\n",fname);
      delete t;
      return 0xffffffff;
    }
    t->m_format = fmt;
    m_loaded_textures.push_back(t);
    handle = (u32)m_loaded_textures.size()-1;
  }

  if (inc && (handle!=0xffffffff))
  {
    m_loaded_textures[handle]->IncrementUsage();
  }

  return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool igDXRender::ShaderDatabase::LoadTextureWithSettings(const igDXRender::TextureSettings& textureSettings, igDXRender::Shader* shader, u32 sampler)
{
  u32 handle = LoadTexture( textureSettings.m_Path.c_str(), textureSettings.m_Format, textureSettings.m_Levels );

  if ( handle==0xffffffff )
    return false;
    
  UpdateTextureSettings(handle, textureSettings);

  igDXRender::Texture* texture = ResolveTexture( handle );
  texture->IncrementUsage();
  shader->m_textures[sampler]=handle;

  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::UpdateTextureSettings(u32 handle, const igDXRender::TextureSettings& textureSettings)
{
  igDXRender::Texture* texture = ResolveTexture( handle );
  texture->m_wrap_u = textureSettings.m_WrapU;
  texture->m_wrap_v = textureSettings.m_WrapV;
  texture->m_filter = textureSettings.m_Filter;
  texture->m_aniso = 1<<textureSettings.m_Anisotropy;
  texture->m_mip_bias = textureSettings.m_MipBias;

  // clamp the max aniso value
  if (texture->m_aniso>8)
  {
    texture->m_aniso=8;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool igDXRender::ShaderDatabase::ReloadTexture( const char* fname )
{
  u32 handle = FindTexture( fname );
  if ( handle == 0xffffffff )
  {
    return false;
  }

  D3DFORMAT fmt = m_loaded_textures[ handle ]->m_format;
  u32 load_count = m_loaded_textures[ handle ]->m_load_count;
  u32 level_count = m_loaded_textures[ handle ]->m_d3d_texture->GetLevelCount();

  u32 texture_count = (u32) m_loaded_textures.size();
  igDXRender::Texture* old_texture = m_loaded_textures[ handle ];
  m_loaded_textures[ handle ] = 0;
  
  u32 new_handle = LoadTexture(fname, fmt, level_count, false);
  if ( new_handle == 0xffffffff )
  {
    printf( "Failed to reload texture '%s'!\n", fname );
    m_loaded_textures[ handle ] = old_texture;
    return false;
  }

  delete old_texture;

  m_loaded_textures[ handle ] = m_loaded_textures[ new_handle ];
  m_loaded_textures[ handle ]->m_load_count = load_count;

  while( m_loaded_textures.size() > texture_count )
  {
    m_loaded_textures.pop_back();
  }
  
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 igDXRender::ShaderDatabase::FindTexture(const char* fname)
{
  // NOTE: We only include the name in the CRC, we really should include other info such as the format
  u32 crc = StringHashDJB2(fname);

  u32 texture_count = (u32)m_loaded_textures.size();
  u32 handle = 0xffffffff;
  for (u32 i=0;i<texture_count;i++)
  {
    if (m_loaded_textures[i])
    {
      if ((m_loaded_textures[i]->m_crc==crc))
      {
        handle = i;
        break;
      }
    }
  }

  return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Texture* igDXRender::ShaderDatabase::ResolveTexture(u32 handle)
{
  return m_loaded_textures[handle];
}

////////////////////////////////////////////////////////////////////////////////////////////////
igDXRender::Shader* igDXRender::ShaderDatabase::ResolveShader(u32 handle)
{
  return m_loaded_shaders[handle];
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::SetShaderDefaultTexture(const char* shaderFilename, u32 textureIndex)
{
  u32 shaderHandle = FindShader( shaderFilename );
  if ( shaderHandle == 0xffffffff )
  {
    return;
  }

  igDXRender::Shader* shader = ResolveShader( shaderHandle );
  NOC_ASSERT( shader );

  switch ( textureIndex )
  {
    case igDXRender::Texture::SAMPLER_GPI_MAP:
      shader->m_flags &= ~SHDR_FLAG_GPI_MAP;
      shader->ReplaceTexture( FindTexture( "@@gloss" ), igDXRender::Texture::SAMPLER_GLOSS_MAP );
      shader->ReplaceTexture( FindTexture( "@@parallax" ), igDXRender::Texture::SAMPLER_PARALLAX_MAP );
      shader->ReplaceTexture( FindTexture( "@@incan" ), igDXRender::Texture::SAMPLER_INCAN_MAP );
    break;
    
    case igDXRender::Texture::SAMPLER_NORMAL_MAP:
      shader->ReplaceTexture( FindTexture( "@@normal" ), igDXRender::Texture::SAMPLER_NORMAL_MAP );
    break;
    
    case igDXRender::Texture::SAMPLER_BASE_MAP:
      shader->ReplaceTexture( FindTexture( "@@base" ), igDXRender::Texture::SAMPLER_BASE_MAP );
    break;
    
    default:
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::UpdateShaderTexture( const char* shaderFilename, u32 textureIndex, const igDXRender::TextureSettings& settings )
{
  u32 shader_handle = FindShader( shaderFilename );
  if ( shader_handle == 0xffffffff )
  {
    return;
  }

  igDXRender::Shader* sh = ResolveShader( shader_handle );
  NOC_ASSERT( sh );

  u32 texture_handle = FindTexture( settings.m_Path.c_str() );
  if ( texture_handle == 0xffffffff )
  {
    LoadTextureWithSettings( settings, sh, textureIndex );
    return;
  }

  igDXRender::Texture* tex = ResolveTexture( texture_handle );
  NOC_ASSERT( tex );

  if ( tex->m_format != settings.m_Format )
  {
    tex->m_format = settings.m_Format;
    ReloadTexture( settings.m_Path.c_str() );
  }

  UpdateTextureSettings( texture_handle, settings );
  
  if ( sh->m_textures[ textureIndex ] != texture_handle )
  {
    sh->ReplaceTexture( texture_handle, textureIndex );
  }
    
  if ( textureIndex == igDXRender::Texture::SAMPLER_GPI_MAP )
  {
    sh->m_flags |= SHDR_FLAG_GPI_MAP;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::GetShaderFilenames( V_string& filenames )
{
  for( u32 i = 0; i < m_loaded_shaders.size(); ++i )
  {
    filenames.push_back( m_loaded_shaders[ i ]->m_filename );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXRender::ShaderDatabase::GetTextureFilenames( V_string& filenames )
{
  for( u32 i = 0; i < m_loaded_textures.size(); ++i )
  {
    filenames.push_back( m_loaded_textures[ i ]->m_filename );
  }
}
