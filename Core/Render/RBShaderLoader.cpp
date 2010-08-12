/*#include "Precompile.h"*/
#include "RBShaderLoader.h"
#include "ShaderManager.h"

#include "Foundation/Log.h"

#include "Core/Asset/AssetInit.h"
#include "Core/Asset/Classes/ShaderAsset.h"
#include "Core/Asset/Classes/Texture.h"

#include <d3d9.h>
#include <d3dx9.h>

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Core::Render;

u32 TextureAddressModes( u32 mode )
{
    u32 outMode = (u32) D3DTADDRESS_CLAMP;

    switch( mode )
    {
    case Asset::WrapModes::Wrap:
        outMode = (u32) D3DTADDRESS_WRAP;
        break;
    case Asset::WrapModes::Clamp:
        outMode = (u32) D3DTADDRESS_CLAMP;
        break;
    default:
        break;
    }

    return outMode;
}

D3DFORMAT GetD3DColorFormat( const Asset::TextureFormat format )
{
    D3DFORMAT outFormat = D3DFMT_A8R8G8B8;

    switch( format )
    {
    case Asset::TextureFormats::ARGB8888:
        outFormat = D3DFMT_A8R8G8B8;
        break;
    case Asset::TextureFormats::ARGB4444:
        outFormat = D3DFMT_A4R4G4B4;
        break;
    case Asset::TextureFormats::DXT5:
        outFormat = D3DFMT_DXT5;
        break;
    case Asset::TextureFormats::AL88:
        outFormat = D3DFMT_A8L8;
        break;
    case Asset::TextureFormats::DXT1:
        outFormat = D3DFMT_DXT1;
        break;
    case Asset::TextureFormats::RGB565:
        outFormat = D3DFMT_R5G6B5;
        break;
    default:
        break;
    }

    return outFormat;
}

u32 TextureFilterMode( const Asset::TextureFilter mode )
{
    u32 outMode = Render::Texture::FILTER_LINEAR;

    switch( mode )
    {
    case Asset::TextureFilters::Point:
        outMode = Render::Texture::FILTER_POINT;
        break;
    case Asset::TextureFilters::Bilinear:
    case Asset::TextureFilters::Trilinear:
        outMode = Render::Texture::FILTER_LINEAR;
        break;
    case Asset::RunTimeFilters::RTF_ANISO2_BI:
    case Asset::RunTimeFilters::RTF_ANISO2_TRI:
    case Asset::RunTimeFilters::RTF_ANISO4_BI:
    case Asset::RunTimeFilters::RTF_ANISO4_TRI:
        outMode = Render::Texture::FILTER_ANISOTROPIC;
        break;
    default:
        break;
    }

    return outMode;
}

void SetShaderClassAlpha( Render::Shader* sh, Asset::AlphaType alphaMode )
{
    switch ( alphaMode )
    {
    case Asset::AlphaTypes::ALPHA_OPAQUE:
        sh->m_alpha_type = Render::Shader::ALPHA_OPAQUE;
        break;

        break;

    case Asset::AlphaTypes::ALPHA_ADDITIVE:
        sh->m_alpha_type = Render::Shader::ALPHA_ADDITIVE;
        break;

    case Asset::AlphaTypes::ALPHA_CUTOUT:
    case Asset::AlphaTypes::ALPHA_SOFT_EDGE:
        sh->m_alpha_type = Render::Shader::ALPHA_CUTOUT;
        break;

    case Asset::AlphaTypes::ALPHA_SCUNGE:
    case Asset::AlphaTypes::ALPHA_OVERLAY:
    case Asset::AlphaTypes::ALPHA_BLENDED:
        sh->m_alpha_type = Render::Shader::ALPHA_BLENDED;
        break;
    }
}

RBShaderLoader::RBShaderLoader()
{

}

RBShaderLoader::~RBShaderLoader()
{

}

Render::Shader* RBShaderLoader::ParseFile( const tchar* fname, Render::ShaderManager* db )
{
    Asset::ShaderAssetPtr shaderClass = Asset::AssetClass::LoadAssetClass< Asset::ShaderAsset >( fname );
    if ( !shaderClass.ReferencesObject() )
    {
        Log::Warning( TXT( "Could not load shader '%s'.\n" ), fname );
        return NULL;
    }

    // this seems like a valid shader, allocate a shader
    Render::Shader* sh = new Render::Shader(db,fname);

    tstring texturePath;
    bool hadError = false;

    SetShaderClassAlpha( sh, shaderClass->m_AlphaMode );

    if (shaderClass->m_DoubleSided)
        sh->m_flags|=SHDR_FLAG_TWO_SIDED;

    Render::TextureSettings settings;
    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    Asset::TexturePtr textureClass = Asset::AssetClass::LoadAssetClass< Asset::Texture >( shaderClass->m_ColorMapPath );
    if( textureClass.ReferencesObject() )
    {
        settings.m_Path = textureClass->GetPath().Get();
        settings.m_Anisotropy = 0;
        settings.m_Filter = TextureFilterMode( textureClass->GetFilter() );
        settings.m_Format = GetD3DColorFormat( textureClass->GetFormat() );
        settings.m_MipBias = 0.0f;
    }
    else
    {
        settings.m_Format = D3DFMT_UNKNOWN;
        settings.m_Path = TXT( "@@base" );
    }

    UpdateShader(sh, shaderClass);

    if( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_BASE_MAP ) )
    {
        if ( settings.m_Path != TXT( "@@base" ) )
        {
            Log::Warning( TXT( "Could not load base map '%s', loading default.\n" ), settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;
            settings.m_Path = TXT( "@@base" );

            if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_BASE_MAP ) )
            {
                Log::Error( TXT( "Could not load default base map.\n" ) );
                hadError = true;
            }
        }
    }

    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    if ( shaderClass->m_NormalMapScaling > 0.0f )
    {
        Asset::TexturePtr normalMap = Asset::AssetClass::LoadAssetClass< Asset::Texture >( shaderClass->m_NormalMapPath );

        if ( normalMap.ReferencesObject() )
        {
            settings.m_Path = normalMap->GetPath().Get();
            settings.m_Anisotropy = 0;
            settings.m_Filter = TextureFilterMode( normalMap->GetFilter() );
            settings.m_Format = GetD3DColorFormat( normalMap->GetFormat() );
            settings.m_MipBias = 0.0f;
        }
        else
        {
            settings.m_Format = D3DFMT_UNKNOWN;
            settings.m_Path = TXT( "@@normal" );
        }
    }

    UpdateShader(sh, shaderClass);

    if( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_NORMAL_MAP ) )
    {
        if ( settings.m_Path != TXT( "@@normal" ) )
        {
            Log::Warning( TXT( "Could not load normal map '%s', loading default.\n" ), settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;
            settings.m_Path = TXT( "@@normal" );

            if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_NORMAL_MAP ) )
            {
                Log::Error( TXT( "Could not load default normal map.\n" ) );
                hadError = true;
            }
        }
    }

    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    Asset::TexturePtr gpiMap = Asset::AssetClass::LoadAssetClass< Asset::Texture >( shaderClass->m_GPIMapPath );
    if( gpiMap.ReferencesObject() )
    {
        settings.m_Path = gpiMap->GetPath().Get();
        settings.m_Anisotropy = 0;
        settings.m_Filter = TextureFilterMode( gpiMap->GetFilter() );
        settings.m_Format = GetD3DColorFormat( gpiMap->GetFormat() );
        settings.m_MipBias = 0.0f;

        if( db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_GPI_MAP ) )
        {
            sh->m_textures[ Render::Texture::SAMPLER_INCAN_MAP ] = 0xffffffff;
            sh->m_textures[ Render::Texture::SAMPLER_PARALLAX_MAP ] = 0xffffffff;

            sh->m_flags |= SHDR_FLAG_GPI_MAP;

            UpdateShader( sh, shaderClass );
        }
        else
        {
            UpdateShader( sh, 0 );
            Log::Warning( TXT( "Could not load expensive map '%s', loading defaults.\n" ), settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;

            settings.m_Path = TXT( "@@gloss" );
            if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_GLOSS_MAP ) )
            {
                Log::Error( TXT( "Could not load default gloss map.\n" ) );
                hadError = true;
            }
            settings.m_Path = TXT( "@@parallax" );
            if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_PARALLAX_MAP ) )
            {
                Log::Error( TXT( "Could not load default parallax map.\n" ) );
                hadError = true;
            }
            settings.m_Path = TXT( "@@incan" );
            if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_INCAN_MAP ) )
            {
                Log::Error( TXT( "Could not load default incan map.\n" ) );
                hadError = true;
            }
        }
    }
    else
    {
        UpdateShader( sh, 0 );

        settings.Clear();
        settings.m_Format = D3DFMT_UNKNOWN;

        settings.m_Path = TXT( "@@gloss" );
        if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_GLOSS_MAP ) )
        {
            Log::Error( TXT( "Could not load default gloss map.\n" ) );
            hadError = true;
        }
        settings.m_Path = TXT( "@@parallax" );
        if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_PARALLAX_MAP ) )
        {
            Log::Error( TXT( "Could not load default parallax map.\n" ) );
            hadError = true;
        }
        settings.m_Path = TXT( "@@incan" );
        if ( !db->LoadTextureWithSettings( settings, sh, Render::Texture::SAMPLER_INCAN_MAP ) )
        {
            Log::Error( TXT( "Could not load default incan map.\n" ) );
            hadError = true;
        }
    }

    if ( hadError )
    {
        delete sh;
        return NULL;
    }

    return sh;
}


////////////////////////////////////////////////////////////////////////////////////////////////
void RBShaderLoader::SetWrapUV( Render::TextureSettings* settings, u32 wrapU, u32 wrapV )
{
    settings->m_WrapU = TextureAddressModes( wrapU );
    settings->m_WrapV = TextureAddressModes( wrapV );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RBShaderLoader::SetFilter( Render::TextureSettings* settings, u32 filter )
{
    settings->m_Filter = TextureFilterMode( (Asset::TextureFilter) filter );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RBShaderLoader::SetColorFormat( Render::TextureSettings* settings, u32 colorFormat, u32 mode )
{
    switch ( mode )
    {
    case Render::Texture::SAMPLER_GPI_MAP:
        settings->m_Format = GetD3DColorFormat( (Asset::TextureFormat) colorFormat );
        break;

    case Render::Texture::SAMPLER_NORMAL_MAP:
        settings->m_Format = GetD3DColorFormat( (Asset::TextureFormat) colorFormat );
        break;

    case Render::Texture::SAMPLER_BASE_MAP:
    default:
        settings->m_Format = GetD3DColorFormat( (Asset::TextureFormat) colorFormat );
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RBShaderLoader::UpdateShaderClass(Render::ShaderManager* db, const tchar* shaderFilename, u32 alphaMode)
{
    u32 shaderHandle = db->FindShader( shaderFilename );
    if ( shaderHandle == 0xffffffff )
    {
        return;
    }

    Render::Shader* sh = db->ResolveShader( shaderHandle );
    HELIUM_ASSERT( sh );

    SetShaderClassAlpha( sh, (Asset::AlphaType) alphaMode );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void RBShaderLoader::UpdateShader(Render::Shader* sh, const Asset::ShaderAsset* shader)
{
    if ( shader->m_EnableColorMapTint )
    {
        sh->m_basetint[0] = (float)shader->m_ColorMapTint.r/255.0f;
        sh->m_basetint[1] = (float)shader->m_ColorMapTint.g/255.0f;
        sh->m_basetint[2] = (float)shader->m_ColorMapTint.b/255.0f;
        sh->m_basetint[3] = 1.0f;
    }
    else
    {
        sh->m_basetint[0] = 1.0f;
        sh->m_basetint[1] = 1.0f;
        sh->m_basetint[2] = 1.0f;
        sh->m_basetint[3] = 1.0f;
    }

    sh->m_normal_scale = shader->m_NormalMapScaling;
 
    sh->m_parallax_scale = shader->m_ParallaxMapEnabled ? shader->m_ParallaxMapScaling : 0.0f;
    sh->m_parallax_bias = 0.0f;

    sh->m_gloss_scale = shader->m_GlossMapEnabled ? shader->m_GlossMapScaling : 0.0f;

    if ( shader->m_GlossMapEnabled )
    {
        sh->m_glosstint[0] = (float)shader->m_GlossMapTint.r/255.0f;
        sh->m_glosstint[1] = (float)shader->m_GlossMapTint.g/255.0f;
        sh->m_glosstint[2] = (float)shader->m_GlossMapTint.b/255.0f;
        sh->m_glosstint[3] = shader->m_GlossMapDirtiness;
    }
    else
    {
        sh->m_glosstint[0] = 1.0f;
        sh->m_glosstint[1] = 1.0f;
        sh->m_glosstint[2] = 1.0f;
        sh->m_glosstint[3] = 1.0f;
    }

    sh->m_env_lod = 5.0f;

    sh->m_incan_scale = shader->m_IncandescentMapEnabled ? shader->m_IncandescentMapScaling : 0.0f;
}
