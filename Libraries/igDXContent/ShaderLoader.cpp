#include "ShaderLoader.h"
#include "igDXRender/shadermanager.h"

#include "Asset/AssetInit.h"
#include "Asset/ShaderAsset.h"

#include "Asset/TextureMapAttribute.h"
#include "Asset/StandardColorMapAttribute.h"
#include "Asset/StandardNormalMapAttribute.h"
#include "Asset/StandardExpensiveMapAttribute.h"

#include "d3d9.h"
#include "d3dx9.h"

//   Asset::Initialize();
//   Asset::Cleanup();

u32 TextureAddressModes( u32 mode )
{
    u32 outMode = (u32) D3DTADDRESS_CLAMP;

    switch( mode )
    {
    case Asset::WrapModes::UV_WRAP:
        outMode = (u32) D3DTADDRESS_WRAP;
        break;
    case Asset::WrapModes::UV_CLAMP:
        outMode = (u32) D3DTADDRESS_CLAMP;
        break;
    default:
        break;
    }

    return outMode;
}

D3DFORMAT GetD3DColorFormat( const Asset::ColorTexFormat format )
{
    D3DFORMAT outFormat = D3DFMT_A8R8G8B8;

    switch( format )
    {
    case Asset::ColorTexFormats::CA_8888:
        outFormat = D3DFMT_A8R8G8B8;
        break;
    case Asset::ColorTexFormats::CA_4444:
        outFormat = D3DFMT_A4R4G4B4;
        break;
    case Asset::ColorTexFormats::CA_DXT5:
        outFormat = D3DFMT_DXT5;
        break;
    default:
        break;
    }

    return outFormat;
}

D3DFORMAT GetD3DColorFormat( const Asset::NormalTexFormat format )
{
    D3DFORMAT outFormat = D3DFMT_DXT5;

    switch( format )
    {
    case Asset::NormalTexFormats::NM_AL88:
        outFormat = D3DFMT_A8L8;
        break;
    case Asset::NormalTexFormats::NM_DXT5:
        outFormat = D3DFMT_DXT5;
        break;
    default:
        break;
    }

    return outFormat;
}

D3DFORMAT GetD3DColorFormat( const Asset::GlossParaIncanTexFormat format )
{
    D3DFORMAT outFormat = D3DFMT_DXT1;

    switch( format )
    {
    case Asset::GlossParaIncanTexFormats::GPI_ARGB8888:
        outFormat = D3DFMT_A8B8G8R8;
        break;
    case Asset::GlossParaIncanTexFormats::GPI_ARGB4444:
        outFormat = D3DFMT_A4R4G4B4;
        break;
    case Asset::GlossParaIncanTexFormats::GPI_DXT:
        outFormat = D3DFMT_DXT1;
        break;
    case Asset::GlossParaIncanTexFormats::GPI_RGB565:
        outFormat = D3DFMT_R5G6B5;
        break;
    default:
        break;
    }

    return outFormat;
}

u32 TextureFilterMode( const Asset::RunTimeFilter mode )
{
    u32 outMode = igDXRender::Texture::FILTER_LINEAR;

    switch( mode )
    {
    case Asset::RunTimeFilters::RTF_POINT:
        outMode = igDXRender::Texture::FILTER_POINT;
        break;
    case Asset::RunTimeFilters::RTF_ANISO2_BI:
    case Asset::RunTimeFilters::RTF_ANISO2_TRI:
    case Asset::RunTimeFilters::RTF_ANISO4_BI:
    case Asset::RunTimeFilters::RTF_ANISO4_TRI:
        outMode = igDXRender::Texture::FILTER_ANISOTROPIC;
        break;
    case Asset::RunTimeFilters::RTF_BILINEAR:
    case Asset::RunTimeFilters::RTF_TRILINEAR:
        outMode = igDXRender::Texture::FILTER_LINEAR;
        break;
    default:
        break;
    }

    return outMode;
}

void SetShaderClassAlpha( igDXRender::Shader* sh, Asset::AlphaType alphaMode )
{
    switch ( alphaMode )
    {
    case Asset::AlphaTypes::ALPHA_OPAQUE:
        sh->m_alpha_type = igDXRender::Shader::ALPHA_OPAQUE;
        break;

        break;

    case Asset::AlphaTypes::ALPHA_ADDITIVE:
        sh->m_alpha_type = igDXRender::Shader::ALPHA_ADDITIVE;
        break;

    case Asset::AlphaTypes::ALPHA_CUTOUT:
    case Asset::AlphaTypes::ALPHA_SOFT_EDGE:
        sh->m_alpha_type = igDXRender::Shader::ALPHA_CUTOUT;
        break;

    case Asset::AlphaTypes::ALPHA_SCUNGE:
    case Asset::AlphaTypes::ALPHA_OVERLAY:
    case Asset::AlphaTypes::ALPHA_BLENDED:
        sh->m_alpha_type = igDXRender::Shader::ALPHA_BLENDED;
        break;
    }
}

igDXContent::RBShaderLoader::RBShaderLoader()
{

}

igDXContent::RBShaderLoader::~RBShaderLoader()
{

}

igDXRender::Shader* igDXContent::RBShaderLoader::ParseFile( const char* fname, igDXRender::ShaderDatabase* db )
{
    Asset::ShaderAssetPtr shaderClass = Asset::AssetClass::LoadAssetClass< Asset::ShaderAsset >( fname );
    if ( !shaderClass.ReferencesObject() )
    {
        printf( "WARNING: Could not load shader '%s'.\n", fname );
        return NULL;
    }

    // this seems like a valid shader, allocate a shader
    igDXRender::Shader* sh = new igDXRender::Shader(db,fname);

    std::string texturePath;
    bool hadError = false;

    SetShaderClassAlpha( sh, shaderClass->m_AlphaMode );

    if (shaderClass->m_DoubleSided)
        sh->m_flags|=SHDR_FLAG_TWO_SIDED;

    igDXRender::TextureSettings settings;
    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    Asset::StandardColorMapAttribute* colorMap = shaderClass->GetAttribute< Asset::StandardColorMapAttribute >();
    if( colorMap )
    {
        Asset::TextureMapAttribute* textureMap = Reflect::ObjectCast< Asset::TextureMapAttribute >( colorMap );

        settings.m_Path = colorMap->GetPath().Get();
        settings.m_Anisotropy = 0;
        settings.m_Filter = TextureFilterMode( colorMap->m_TexFilter );
        settings.m_Format = GetD3DColorFormat( colorMap->m_TexFormat );
        settings.m_MipBias = colorMap->m_MipBias;
    }
    else
    {
        settings.m_Format = D3DFMT_UNKNOWN;
        settings.m_Path = "@@base";
    }

    UpdateShaderColorMap(sh, colorMap);

    if( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_BASE_MAP ) )
    {
        if ( settings.m_Path != "@@base" )
        {
            printf( "WARNING: Could not load base map '%s', loading default.\n", settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;
            settings.m_Path = "@@base";

            if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_BASE_MAP ) )
            {
                printf( "ERROR: Could not load default base map.\n" );
                hadError = true;
            }
        }
    }

    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    Asset::StandardNormalMapAttribute* normalMap = shaderClass->GetAttribute< Asset::StandardNormalMapAttribute >();
    if( normalMap && normalMap->m_NormalMapScale>0.0f)
    {
        Asset::TextureMapAttribute* textureMap = Reflect::ObjectCast< Asset::TextureMapAttribute >( normalMap );

        settings.m_Path = normalMap->GetPath().Get();
        settings.m_Anisotropy = 0;
        settings.m_Filter = TextureFilterMode( normalMap->m_TexFilter );
        settings.m_Format = GetD3DColorFormat( normalMap->m_TexFormat );
        settings.m_MipBias = normalMap->m_MipBias;
    }
    else
    {
        settings.m_Format = D3DFMT_UNKNOWN;
        settings.m_Path = "@@normal";
    }

    UpdateShaderNormalMap( sh, normalMap );

    if( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_NORMAL_MAP ) )
    {
        if ( settings.m_Path != "@@normal" )
        {
            printf( "WARNING: Could not load normal map '%s', loading default.\n", settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;
            settings.m_Path = "@@normal";

            if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_NORMAL_MAP ) )
            {
                printf( "ERROR: Could not load default normal map.\n" );
                hadError = true;
            }
        }
    }

    settings.Clear();
    settings.m_WrapU = TextureAddressModes( shaderClass->m_WrapModeU );
    settings.m_WrapV = TextureAddressModes( shaderClass->m_WrapModeV );

    Asset::StandardExpensiveMapAttribute* expensiveMap = shaderClass->GetAttribute< Asset::StandardExpensiveMapAttribute >();
    if( expensiveMap )
    {
        Asset::TextureMapAttribute* textureMap = Reflect::ObjectCast< Asset::TextureMapAttribute >( expensiveMap );

        settings.m_Path = expensiveMap->GetPath().Get();
        settings.m_Anisotropy = 0;
        settings.m_Filter = TextureFilterMode( expensiveMap->m_TexFilter );

        settings.m_Format = GetD3DColorFormat( expensiveMap->m_TexFormat );
        settings.m_MipBias = expensiveMap->m_MipBias;

        // if detail mask is enabled, we need to switch from dxt1 to dxt5
        if (expensiveMap->m_DetailMapMaskEnabled && settings.m_Format == D3DFMT_DXT1)
        {
            settings.m_Format = D3DFMT_DXT5;
        }

        if( db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_GPI_MAP ) )
        {
            sh->m_textures[ igDXRender::Texture::SAMPLER_INCAN_MAP ] = 0xffffffff;
            sh->m_textures[ igDXRender::Texture::SAMPLER_PARALLAX_MAP ] = 0xffffffff;

            sh->m_flags |= SHDR_FLAG_GPI_MAP;

            UpdateShaderExpensiveMap( sh, expensiveMap );
        }
        else
        {
            UpdateShaderExpensiveMap( sh, 0 );
            printf( "WARNING: Could not load expensive map '%s', loading defaults.\n", settings.m_Path.c_str() );
            settings.Clear();
            settings.m_Format = D3DFMT_UNKNOWN;

            settings.m_Path = "@@gloss";
            if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_GLOSS_MAP ) )
            {
                printf( "ERROR: Could not load default gloss map.\n" );
                hadError = true;
            }
            settings.m_Path = "@@parallax";
            if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_PARALLAX_MAP ) )
            {
                printf( "ERROR: Could not load default parallax map.\n" );
                hadError = true;
            }
            settings.m_Path = "@@incan";
            if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_INCAN_MAP ) )
            {
                printf( "ERROR: Could not load default incan map.\n" );
                hadError = true;
            }
        }
    }
    else
    {
        UpdateShaderExpensiveMap( sh, 0 );

        settings.Clear();
        settings.m_Format = D3DFMT_UNKNOWN;

        settings.m_Path = "@@gloss";
        if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_GLOSS_MAP ) )
        {
            printf( "ERROR: Could not load default gloss map.\n" );
            hadError = true;
        }
        settings.m_Path = "@@parallax";
        if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_PARALLAX_MAP ) )
        {
            printf( "ERROR: Could not load default parallax map.\n" );
            hadError = true;
        }
        settings.m_Path = "@@incan";
        if ( !db->LoadTextureWithSettings( settings, sh, igDXRender::Texture::SAMPLER_INCAN_MAP ) )
        {
            printf( "ERROR: Could not load default incan map.\n" );
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
void igDXContent::RBShaderLoader::SetWrapUV( igDXRender::TextureSettings* settings, u32 wrapU, u32 wrapV )
{
    settings->m_WrapU = TextureAddressModes( wrapU );
    settings->m_WrapV = TextureAddressModes( wrapV );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::SetFilter( igDXRender::TextureSettings* settings, u32 filter )
{
    settings->m_Filter = TextureFilterMode( (Asset::RunTimeFilter) filter );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::SetColorFormat( igDXRender::TextureSettings* settings, u32 colorFormat, u32 mode )
{
    switch ( mode )
    {
    case igDXRender::Texture::SAMPLER_GPI_MAP:
        settings->m_Format = GetD3DColorFormat( (Asset::GlossParaIncanTexFormat) colorFormat );
        break;

    case igDXRender::Texture::SAMPLER_NORMAL_MAP:
        settings->m_Format = GetD3DColorFormat( (Asset::NormalTexFormat) colorFormat );
        break;

    case igDXRender::Texture::SAMPLER_BASE_MAP:
    default:
        settings->m_Format = GetD3DColorFormat( (Asset::ColorTexFormat) colorFormat );
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::UpdateShaderClass(igDXRender::ShaderDatabase* db, const char* shaderFilename, u32 alphaMode)
{
    u32 shaderHandle = db->FindShader( shaderFilename );
    if ( shaderHandle == 0xffffffff )
    {
        return;
    }

    igDXRender::Shader* sh = db->ResolveShader( shaderHandle );
    NOC_ASSERT( sh );

    SetShaderClassAlpha( sh, (Asset::AlphaType) alphaMode );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::UpdateShaderColorMap(igDXRender::Shader* sh, const Asset::StandardColorMapAttribute* colorMap)
{
    if (colorMap && !colorMap->m_DisableBaseTint)
    {
        sh->m_basetint[0] = (float)colorMap->m_BaseMapTint.r/255.0f;
        sh->m_basetint[1] = (float)colorMap->m_BaseMapTint.g/255.0f;
        sh->m_basetint[2] = (float)colorMap->m_BaseMapTint.b/255.0f;
        sh->m_basetint[3] = 1.0f;
    }
    else
    {
        sh->m_basetint[0] = 1.0f;
        sh->m_basetint[1] = 1.0f;
        sh->m_basetint[2] = 1.0f;
        sh->m_basetint[3] = 1.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::UpdateShaderNormalMap(igDXRender::Shader* sh, const Asset::StandardNormalMapAttribute* normalMap)
{
    if (!normalMap)
    {
        sh->m_normal_scale = 0.0f;
        return;
    }

    sh->m_normal_scale = normalMap->m_NormalMapScale;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void igDXContent::RBShaderLoader::UpdateShaderExpensiveMap(igDXRender::Shader* sh, const Asset::StandardExpensiveMapAttribute* expensiveMap)
{
    if (!expensiveMap)
    {
        sh->m_parallax_scale = 0.0f;
        sh->m_parallax_bias = 0.0f;

        sh->m_gloss_scale = 0.0f;
        sh->m_glosstint[0] = 1.0f;
        sh->m_glosstint[1] = 1.0f;
        sh->m_glosstint[2] = 1.0f;
        sh->m_glosstint[3] = 1.0f;

        sh->m_incan_scale = 0.0f;

        sh->m_env_lod = 5.0f;

        return;
    }

    if (expensiveMap->m_ParallaxMapEnabled)
    {
        sh->m_parallax_scale = expensiveMap->m_ParallaxScale;
        //sh->m_parallax_bias = expensiveMap->m_ParallaxBias;        
    }
    else
    {
        sh->m_parallax_scale = 0.0f;
        sh->m_parallax_bias = 0.0f;
    }

    if (expensiveMap->m_GlossMapEnabled)
    {
        sh->m_gloss_scale = expensiveMap->m_GlossScale;

        sh->m_glosstint[0] = (float)expensiveMap->m_GlossTint.r/255.0f;
        sh->m_glosstint[1] = (float)expensiveMap->m_GlossTint.g/255.0f;
        sh->m_glosstint[2] = (float)expensiveMap->m_GlossTint.b/255.0f;
        sh->m_glosstint[3] = expensiveMap->m_GlossDirty;

        sh->m_specular_power = expensiveMap->m_RealTimeSpecPower;
        //sh->m_env_lod = expensiveMap->m_CubeBias;
    }
    else
    {
        sh->m_gloss_scale = 0.0f;
    }

    if (expensiveMap->m_IncanMapEnabled)
    {
        sh->m_incan_scale = 1.0f;
    }
    else
    {
        sh->m_incan_scale = 0.0f;
    }
}