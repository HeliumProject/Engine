/*#include "Precompile.h"*/
#include "ShaderManager.h"
#include "Renderer.h"
#include "ShaderLoader.h"

#include "Foundation/Log.h"
#include "Foundation/File/Path.h"
#include "Foundation/Checksum/CRC32.h"

#include <d3dx9.h>

using namespace Helium;
using namespace Helium::Render;

////////////////////////////////////////////////////////////////////////////////////////////////
Texture::Texture(const tchar_t* fname)
{
    m_filename = fname;  
    m_timestamp = (uint64_t)-1;
    m_crc= Helium::Crc32( fname, _tcslen(fname) );
    m_load_count=0;
    m_d3d_texture = 0;

    m_wrap_u = (uint32_t)D3DTADDRESS_WRAP;
    m_wrap_v = (uint32_t)D3DTADDRESS_WRAP;
    m_filter = FILTER_LINEAR;
    m_aniso = 1;
    m_mip_bias = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////
Texture::~Texture()
{
    if (m_d3d_texture)
    {
        m_d3d_texture->Release();
        m_d3d_texture = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
RenderShader::RenderShader(ShaderManager* sd, const tchar_t* shader)
{
    m_filename = shader;  
    m_timestamp = (uint64_t)-1;
    m_crc=Helium::Crc32(shader, _tcslen(shader));
    m_load_count = 0;
    m_flags = SHDR_FLAG_GPI_MAP;
    m_sd = sd;

    // set the default textures
    m_textures[0] = sd->LoadTexture( TXT( "@@base" ),D3DFMT_UNKNOWN);
    m_textures[1] = sd->LoadTexture( TXT( "@@normal" ),D3DFMT_UNKNOWN);
    m_textures[2] = sd->LoadTexture( TXT( "@@gpi" ),D3DFMT_UNKNOWN);
    m_textures[3] = sd->LoadTexture( TXT( "@@parallax" ),D3DFMT_UNKNOWN);
    m_textures[4] = sd->LoadTexture( TXT( "@@incan" ),D3DFMT_UNKNOWN);

    m_alpha_type=RenderShader::ALPHA_OPAQUE;
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
RenderShader::~RenderShader()
{
    // reduce the ref counts of the textures this shader was using
    for (uint32_t t=0;t<Texture::__SAMPLER_LAST__;t++)
    {
        if (m_textures[t]!=0xffffffff)
        {
            Texture* tex = m_sd->ResolveTexture(m_textures[t]);
            tex->DecrementUsage();
            m_textures[t] = 0xffffffff;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RenderShader::ReplaceTexture(uint32_t th, uint32_t slot)
{
    if (m_textures[slot]!=0xffffffff)
    {
        Texture* current = m_sd->ResolveTexture(m_textures[slot]);
        if (current->DecrementUsage()==0)
        {
#pragma TODO("delete now?")
        }
    }

    m_textures[slot] = th;

    if (th!=0xffffffff)
    {
        Texture* ntex = m_sd->ResolveTexture(th);
        ntex->IncrementUsage();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
ShaderManager::ShaderManager()
: m_device ( NULL )
{

}

////////////////////////////////////////////////////////////////////////////////////////////////
ShaderManager::~ShaderManager()
{
    // go thorugh all the shaders and delete them
    uint32_t shader_count = (uint32_t)m_loaded_shaders.size();
    for (uint32_t i=0;i<shader_count;i++)
    {
        if (m_loaded_shaders[i])
        {
            delete m_loaded_shaders[i];
            m_loaded_shaders[i] = 0;
        }
    }

    uint32_t texture_count = (uint32_t)m_loaded_textures.size();
    for (uint32_t i=0;i<texture_count;i++)
    {
        if (m_loaded_textures[i])
        {
            delete m_loaded_textures[i];
            m_loaded_textures[i]=0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
static void FillTexture(IDirect3DTexture9* tex, uint32_t val)
{
    D3DLOCKED_RECT rect;

    tex->LockRect(0,&rect,0,0);

    for (uint32_t y=0;y<16;y++)
    {
        uint32_t* line_data = (uint32_t*) (((uint8_t*)rect.pBits)+(y*rect.Pitch));
        for (uint32_t x=0;x<16;x++)
            line_data[x]=val;
    }
    tex->UnlockRect(0);
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::Init(IDirect3DDevice9* device)
{
    Log::Print( TXT( "Default shaders/textures created\n" ) );
    m_device = device;

    IDirect3DTexture9* default_base;
    IDirect3DTexture9* default_normal;
    IDirect3DTexture9* default_gloss;
    IDirect3DTexture9* default_parallax;
    IDirect3DTexture9* default_incan;
    IDirect3DTexture9* default_gpi;
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_base,0);
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_normal,0);
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_gloss,0);
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_parallax,0);
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_incan,0);
    m_device->CreateTexture(16,16,1,0,D3DFMT_A8R8G8B8,D3DPOOL_MANAGED,&default_gpi,0);
    FillTexture(default_base,D3DCOLOR_ARGB(0xff,0x80,0x80,0x80));
    FillTexture(default_normal,D3DCOLOR_ARGB(0xff,0x80,0x80,0xff));
    FillTexture(default_gloss,D3DCOLOR_ARGB(0xff,0x00,0x00,0x00));
    FillTexture(default_parallax,D3DCOLOR_ARGB(0xff,0x00,0x00,0x00));
    FillTexture(default_incan,D3DCOLOR_ARGB(0xff,0x0,0x0,0x0));
    FillTexture(default_gpi,D3DCOLOR_ARGB(0xff,0x00,0x0,0x00));  // alpha, gloss,incan,parallax

    Texture* base_texture = new Texture( TXT( "@@base" ) );
    Texture* normal_texture = new Texture( TXT( "@@normal" ) );
    Texture* gloss_texture = new Texture( TXT( "@@gloss" ) );
    Texture* parallax_texture = new Texture( TXT( "@@parallax" ) );
    Texture* incan_texture = new Texture( TXT( "@@incan" ) );
    Texture* gpi_texture = new Texture( TXT( "@@gpi" ) );

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
    RenderShader* sh = new RenderShader(this, TXT( "@@default" ) );
    AddShader(sh);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// add a shader pointer to the array of loaded shaders
uint32_t ShaderManager::AddShader(RenderShader* sh)
{
    uint32_t shader_count = (uint32_t)m_loaded_shaders.size();
    uint32_t handle = 0xffffffff;
    for (uint32_t i=0;i<shader_count;i++)
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
        handle = (uint32_t)m_loaded_shaders.size()-1;
    }

    return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// shader is an XML file
uint32_t ShaderManager::LoadNewShader( const tchar_t* fname, ShaderLoaderPtr loader )
{
    Helium::Path shaderPath( fname );

    RenderShader* shader = loader->ParseFile( fname, this );

    if ( shader )
    {
        return AddShader( shader );
    }
    else
    {
        Log::Warning( TXT( "Unable to parse shader '%s'.\n" ), fname );
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t ShaderManager::LoadShader(const tchar_t* fname, bool inc, ShaderLoaderPtr loader)
{
    uint32_t handle = FindShader(fname);

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
uint32_t ShaderManager::FindShader(const tchar_t* fname)
{
    uint32_t crc = Helium::Crc32(fname, _tcslen(fname));

    uint32_t shader_count = (uint32_t)m_loaded_shaders.size();
    uint32_t handle = 0xffffffff;
    for (uint32_t i=0;i<shader_count;i++)
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
uint32_t ShaderManager::DuplicateShader(uint32_t handle,const tchar_t* new_shader)
{
    // convert the handle to a pointer
    RenderShader* sh = ResolveShader(handle);

    // this seems like a valid shader, allocate a shader
    RenderShader* newsh = new RenderShader(this,new_shader);

    // copy the textures and increment the usage on any that are valid
    for (uint32_t t=0;t<Texture::__SAMPLER_LAST__;t++)
    {
        newsh->m_textures[t] = sh->m_textures[t];
        if (sh->m_textures[t]!=0xffffffff)
        {
            Texture* tex = ResolveTexture(sh->m_textures[t]);
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
uint32_t ShaderManager::LoadTexture(const tchar_t* fname,D3DFORMAT fmt, uint32_t levels,bool inc)
{
    uint32_t handle = FindTexture(fname);

    if (handle==0xffffffff)
    {
        // wasn't found so load the new texture
        Texture* t = new Texture(fname);

        if (FAILED(D3DXCreateTextureFromFileEx(m_device,fname,0,0,levels,0,fmt,D3DPOOL_MANAGED,D3DX_DEFAULT,D3DX_DEFAULT,0,0,0,&t->m_d3d_texture)))
        {
            Log::Error( TXT( "Failed to load texture '%s'\n" ),fname);
            delete t;
            return 0xffffffff;
        }
        t->m_format = fmt;
        m_loaded_textures.push_back(t);
        handle = (uint32_t)m_loaded_textures.size()-1;
    }

    if (inc && (handle!=0xffffffff))
    {
        m_loaded_textures[handle]->IncrementUsage();
    }

    return handle;
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool ShaderManager::LoadTextureWithSettings(const TextureSettings& textureSettings, RenderShader* shader, uint32_t sampler)
{
    uint32_t handle = LoadTexture( textureSettings.m_Path.c_str(), textureSettings.m_Format, textureSettings.m_Levels );

    if ( handle==0xffffffff )
        return false;

    UpdateTextureSettings(handle, textureSettings);

    Texture* texture = ResolveTexture( handle );
    texture->IncrementUsage();
    shader->m_textures[sampler]=handle;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::UpdateTextureSettings(uint32_t handle, const TextureSettings& textureSettings)
{
    Texture* texture = ResolveTexture( handle );
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
bool ShaderManager::ReloadTexture( const tchar_t* fname )
{
    uint32_t handle = FindTexture( fname );
    if ( handle == 0xffffffff )
    {
        return false;
    }

    D3DFORMAT fmt = m_loaded_textures[ handle ]->m_format;
    uint32_t load_count = m_loaded_textures[ handle ]->m_load_count;
    uint32_t level_count = m_loaded_textures[ handle ]->m_d3d_texture->GetLevelCount();

    uint32_t texture_count = (uint32_t) m_loaded_textures.size();
    Texture* old_texture = m_loaded_textures[ handle ];
    m_loaded_textures[ handle ] = 0;

    uint32_t new_handle = LoadTexture(fname, fmt, level_count, false);
    if ( new_handle == 0xffffffff )
    {
        Log::Error( TXT( "Failed to reload texture '%s'!\n" ), fname );
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
uint32_t ShaderManager::FindTexture(const tchar_t* fname)
{
    // NOTE: We only include the name in the CRC, we really should include other info such as the format
    uint32_t crc = Helium::Crc32(fname, _tcslen(fname));

    uint32_t texture_count = (uint32_t)m_loaded_textures.size();
    uint32_t handle = 0xffffffff;
    for (uint32_t i=0;i<texture_count;i++)
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
Texture* ShaderManager::ResolveTexture(uint32_t handle)
{
    return m_loaded_textures[handle];
}

////////////////////////////////////////////////////////////////////////////////////////////////
RenderShader* ShaderManager::ResolveShader(uint32_t handle)
{
    return m_loaded_shaders[handle];
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::SetShaderDefaultTexture(const tchar_t* shaderFilename, uint32_t textureIndex)
{
    uint32_t shaderHandle = FindShader( shaderFilename );
    if ( shaderHandle == 0xffffffff )
    {
        return;
    }

    RenderShader* shader = ResolveShader( shaderHandle );
    HELIUM_ASSERT( shader );

    switch ( textureIndex )
    {
    case Texture::SAMPLER_GPI_MAP:
        shader->m_flags &= ~SHDR_FLAG_GPI_MAP;
        shader->ReplaceTexture( FindTexture( TXT( "@@gloss" ) ), Texture::SAMPLER_GLOSS_MAP );
        shader->ReplaceTexture( FindTexture( TXT( "@@parallax" ) ), Texture::SAMPLER_PARALLAX_MAP );
        shader->ReplaceTexture( FindTexture( TXT( "@@incan" ) ), Texture::SAMPLER_INCAN_MAP );
        break;

    case Texture::SAMPLER_NORMAL_MAP:
        shader->ReplaceTexture( FindTexture( TXT( "@@normal" ) ), Texture::SAMPLER_NORMAL_MAP );
        break;

    case Texture::SAMPLER_BASE_MAP:
        shader->ReplaceTexture( FindTexture( TXT( "@@base" ) ), Texture::SAMPLER_BASE_MAP );
        break;

    default:
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::UpdateShaderTexture( const tchar_t* shaderFilename, uint32_t textureIndex, const TextureSettings& settings )
{
    uint32_t shader_handle = FindShader( shaderFilename );
    if ( shader_handle == 0xffffffff )
    {
        return;
    }

    RenderShader* sh = ResolveShader( shader_handle );
    HELIUM_ASSERT( sh );

    uint32_t texture_handle = FindTexture( settings.m_Path.c_str() );
    if ( texture_handle == 0xffffffff )
    {
        LoadTextureWithSettings( settings, sh, textureIndex );
        return;
    }

    Texture* tex = ResolveTexture( texture_handle );
    HELIUM_ASSERT( tex );

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

    if ( textureIndex == Texture::SAMPLER_GPI_MAP )
    {
        sh->m_flags |= SHDR_FLAG_GPI_MAP;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::GetShaderFilenames( std::vector< tstring >& filenames )
{
    for( uint32_t i = 0; i < m_loaded_shaders.size(); ++i )
    {
        filenames.push_back( m_loaded_shaders[ i ]->m_filename );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void ShaderManager::GetTextureFilenames( std::vector< tstring >& filenames )
{
    for( uint32_t i = 0; i < m_loaded_textures.size(); ++i )
    {
        filenames.push_back( m_loaded_textures[ i ]->m_filename );
    }
}
