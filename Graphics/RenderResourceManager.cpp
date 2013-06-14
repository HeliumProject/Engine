#include "GraphicsPch.h"
#include "Graphics/RenderResourceManager.h"

#include "Engine/Config.h"
#include "Engine/AssetLoader.h"
#include "Rendering/RBlendState.h"
#include "Rendering/RDepthStencilState.h"
#include "Rendering/RRasterizerState.h"
#include "Rendering/Renderer.h"
#include "Rendering/RSamplerState.h"
#include "Rendering/RSurface.h"
#include "Rendering/RVertexDescription.h"
#include "Graphics/Font.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Shader.h"

using namespace Helium;

RenderResourceManager* RenderResourceManager::sm_pInstance = NULL;

/// Constructor.
RenderResourceManager::RenderResourceManager()
    : m_shadowMode( GraphicsConfig::EShadowMode::INVALID )
    , m_viewportWidthMax( 0 )
    , m_viewportHeightMax( 0 )
    , m_shadowDepthTextureUsableSize( 0 )
{
}

/// Destructor.
RenderResourceManager::~RenderResourceManager()
{
    Shutdown();
}

/// Initialize all resources provided by this manager.
///
/// @see Shutdown(), PostConfigUpdate()
void RenderResourceManager::Initialize()
{
    // Release any existing resources.
    Shutdown();

    // Get the renderer and graphics configuration.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return;
    }

    Config& rConfig = Config::GetStaticInstance();
    StrongPtr< GraphicsConfig > spGraphicsConfig(
        rConfig.GetConfigObject< GraphicsConfig >( Name( TXT( "GraphicsConfig" ) ) ) );
    if( !spGraphicsConfig )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "RenderResourceManager::Initialize(): Initialization failed; missing GraphicsConfig.\n" ) );

        return;
    }

    // Create the standard rasterizer states.
    RRasterizerState::Description rasterizerStateDesc;

    rasterizerStateDesc.fillMode = RENDERER_FILL_MODE_SOLID;
    rasterizerStateDesc.cullMode = RENDERER_CULL_MODE_BACK;
    rasterizerStateDesc.winding = RENDERER_WINDING_CLOCKWISE;
    rasterizerStateDesc.depthBias = 0;
    rasterizerStateDesc.slopeScaledDepthBias = 0.0f;
    m_rasterizerStates[ RASTERIZER_STATE_DEFAULT ] = pRenderer->CreateRasterizerState( rasterizerStateDesc );
    HELIUM_ASSERT( m_rasterizerStates[ RASTERIZER_STATE_DEFAULT ] );

    rasterizerStateDesc.cullMode = RENDERER_CULL_MODE_NONE;
    m_rasterizerStates[ RASTERIZER_STATE_DOUBLE_SIDED ] = pRenderer->CreateRasterizerState( rasterizerStateDesc );
    HELIUM_ASSERT( m_rasterizerStates[ RASTERIZER_STATE_DOUBLE_SIDED ] );

    rasterizerStateDesc.depthBias = 1;
    rasterizerStateDesc.slopeScaledDepthBias = 2.0f;
    m_rasterizerStates[ RASTERIZER_STATE_SHADOW_DEPTH ] = pRenderer->CreateRasterizerState( rasterizerStateDesc );
    HELIUM_ASSERT( m_rasterizerStates[ RASTERIZER_STATE_SHADOW_DEPTH ] );

    rasterizerStateDesc.depthBias = 0;
    rasterizerStateDesc.slopeScaledDepthBias = 0.0f;
    rasterizerStateDesc.fillMode = RENDERER_FILL_MODE_WIREFRAME;
    m_rasterizerStates[ RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED ] = pRenderer->CreateRasterizerState(
        rasterizerStateDesc );
    HELIUM_ASSERT( m_rasterizerStates[ RASTERIZER_STATE_WIREFRAME_DOUBLE_SIDED ] );

    rasterizerStateDesc.cullMode = RENDERER_CULL_MODE_BACK;
    m_rasterizerStates[ RASTERIZER_STATE_WIREFRAME ] = pRenderer->CreateRasterizerState( rasterizerStateDesc );
    HELIUM_ASSERT( m_rasterizerStates[ RASTERIZER_STATE_WIREFRAME ] );

    // Create the standard blend states.
    RBlendState::Description blendStateDesc;

    blendStateDesc.bBlendEnable = false;
    m_blendStates[ BLEND_STATE_OPAQUE ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_OPAQUE ] );

    blendStateDesc.colorWriteMask = 0;
    m_blendStates[ BLEND_STATE_NO_COLOR ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_NO_COLOR ] );

    blendStateDesc.colorWriteMask = RENDERER_COLOR_WRITE_MASK_FLAG_ALL;
    blendStateDesc.bBlendEnable = true;

    blendStateDesc.sourceFactor = RENDERER_BLEND_FACTOR_SRC_ALPHA;
    blendStateDesc.destinationFactor = RENDERER_BLEND_FACTOR_INV_SRC_ALPHA;
    blendStateDesc.function = RENDERER_BLEND_FUNCTION_ADD;
    m_blendStates[ BLEND_STATE_TRANSPARENT ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_TRANSPARENT ] );

    blendStateDesc.sourceFactor = RENDERER_BLEND_FACTOR_ONE;
    blendStateDesc.destinationFactor = RENDERER_BLEND_FACTOR_ONE;
    m_blendStates[ BLEND_STATE_ADDITIVE ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_ADDITIVE ] );

    blendStateDesc.function = RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT;
    m_blendStates[ BLEND_STATE_SUBTRACTIVE ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_SUBTRACTIVE ] );

    blendStateDesc.sourceFactor = RENDERER_BLEND_FACTOR_DEST_COLOR;
    blendStateDesc.destinationFactor = RENDERER_BLEND_FACTOR_ZERO;
    blendStateDesc.function = RENDERER_BLEND_FUNCTION_ADD;
    m_blendStates[ BLEND_STATE_MODULATE ] = pRenderer->CreateBlendState( blendStateDesc );
    HELIUM_ASSERT( m_blendStates[ BLEND_STATE_MODULATE ] );

    // Create the standard depth/stencil states.
    RDepthStencilState::Description depthStateDesc;

    depthStateDesc.stencilWriteMask = 0;
    depthStateDesc.bStencilTestEnable = false;

    depthStateDesc.depthFunction = RENDERER_COMPARE_FUNCTION_LESS_EQUAL;
    depthStateDesc.bDepthTestEnable = true;
    depthStateDesc.bDepthWriteEnable = true;
    m_depthStencilStates[ DEPTH_STENCIL_STATE_DEFAULT ] = pRenderer->CreateDepthStencilState( depthStateDesc );
    HELIUM_ASSERT( m_depthStencilStates[ DEPTH_STENCIL_STATE_DEFAULT ] );

    depthStateDesc.bDepthWriteEnable = false;
    m_depthStencilStates[ DEPTH_STENCIL_STATE_TEST_ONLY ] = pRenderer->CreateDepthStencilState( depthStateDesc );
    HELIUM_ASSERT( m_depthStencilStates[ DEPTH_STENCIL_STATE_TEST_ONLY ] );

    depthStateDesc.bDepthTestEnable = false;
    m_depthStencilStates[ DEPTH_STENCIL_STATE_NONE ] = pRenderer->CreateDepthStencilState( depthStateDesc );
    HELIUM_ASSERT( m_depthStencilStates[ DEPTH_STENCIL_STATE_NONE ] );

    // Create the standard sampler states that are not dependent on configuration settings.
    RSamplerState::Description samplerStateDesc;
    samplerStateDesc.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT;
    samplerStateDesc.addressModeW = RENDERER_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerStateDesc.mipLodBias = 0;
    samplerStateDesc.maxAnisotropy = spGraphicsConfig->GetMaxAnisotropy();

    for( size_t addressModeIndex = 0; addressModeIndex < RENDERER_TEXTURE_ADDRESS_MODE_MAX; ++addressModeIndex )
    {
        ERendererTextureAddressMode addressMode = static_cast< ERendererTextureAddressMode >( addressModeIndex );
        samplerStateDesc.addressModeU = addressMode;
        samplerStateDesc.addressModeV = addressMode;
        samplerStateDesc.addressModeW = addressMode;

        m_samplerStates[ TEXTURE_FILTER_POINT ][ addressModeIndex ] = pRenderer->CreateSamplerState(
            samplerStateDesc );
        HELIUM_ASSERT( m_samplerStates[ TEXTURE_FILTER_POINT ][ addressModeIndex ] );
    }

    // Create the standard set of mesh vertex descriptions.
    RVertexDescription::Element vertexElements[ 6 ];

    vertexElements[ 0 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT32_3;
    vertexElements[ 0 ].semantic = RENDERER_VERTEX_SEMANTIC_POSITION;
    vertexElements[ 0 ].semanticIndex = 0;
    vertexElements[ 0 ].bufferIndex = 0;

    vertexElements[ 1 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 1 ].semantic = RENDERER_VERTEX_SEMANTIC_COLOR;
    vertexElements[ 1 ].semanticIndex = 0;
    vertexElements[ 1 ].bufferIndex = 0;

    vertexElements[ 2 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT16_2;
    vertexElements[ 2 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 2 ].semanticIndex = 0;
    vertexElements[ 2 ].bufferIndex = 0;

    vertexElements[ 3 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT32_2;
    vertexElements[ 3 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 3 ].semanticIndex = 1;
    vertexElements[ 3 ].bufferIndex = 0;

    m_spSimpleVertexDescription = pRenderer->CreateVertexDescription( vertexElements, 2 );
    HELIUM_ASSERT( m_spSimpleVertexDescription );

    m_spSimpleTexturedVertexDescription = pRenderer->CreateVertexDescription( vertexElements, 3 );
    HELIUM_ASSERT( m_spSimpleTexturedVertexDescription );

    m_spProjectedVertexDescription = pRenderer->CreateVertexDescription( vertexElements, 4 );
    HELIUM_ASSERT( m_spProjectedVertexDescription );

    vertexElements[ 1 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 1 ].semantic = RENDERER_VERTEX_SEMANTIC_NORMAL;
    vertexElements[ 1 ].semanticIndex = 0;
    vertexElements[ 1 ].bufferIndex = 0;

    vertexElements[ 2 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 2 ].semantic = RENDERER_VERTEX_SEMANTIC_TANGENT;
    vertexElements[ 2 ].semanticIndex = 0;
    vertexElements[ 2 ].bufferIndex = 0;

    vertexElements[ 3 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 3 ].semantic = RENDERER_VERTEX_SEMANTIC_COLOR;
    vertexElements[ 3 ].semanticIndex = 0;
    vertexElements[ 3 ].bufferIndex = 0;

    vertexElements[ 4 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT16_2;
    vertexElements[ 4 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 4 ].semanticIndex = 0;
    vertexElements[ 4 ].bufferIndex = 0;

    vertexElements[ 5 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT16_2;
    vertexElements[ 5 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 5 ].semanticIndex = 1;
    vertexElements[ 5 ].bufferIndex = 0;

    m_staticMeshVertexDescriptions[ 0 ] = pRenderer->CreateVertexDescription( vertexElements, 5 );
    HELIUM_ASSERT( m_staticMeshVertexDescriptions[ 0 ] );

    m_staticMeshVertexDescriptions[ 1 ] = pRenderer->CreateVertexDescription( vertexElements, 6 );
    HELIUM_ASSERT( m_staticMeshVertexDescriptions[ 1 ] );

    vertexElements[ 1 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 1 ].semantic = RENDERER_VERTEX_SEMANTIC_BLENDWEIGHT;
    vertexElements[ 1 ].semanticIndex = 0;
    vertexElements[ 1 ].bufferIndex = 0;

    vertexElements[ 2 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4;
    vertexElements[ 2 ].semantic = RENDERER_VERTEX_SEMANTIC_BLENDINDICES;
    vertexElements[ 2 ].semanticIndex = 0;
    vertexElements[ 2 ].bufferIndex = 0;

    vertexElements[ 3 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 3 ].semantic = RENDERER_VERTEX_SEMANTIC_NORMAL;
    vertexElements[ 3 ].semanticIndex = 0;
    vertexElements[ 3 ].bufferIndex = 0;

    vertexElements[ 4 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 4 ].semantic = RENDERER_VERTEX_SEMANTIC_TANGENT;
    vertexElements[ 4 ].semanticIndex = 0;
    vertexElements[ 4 ].bufferIndex = 0;

    vertexElements[ 5 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT16_2;
    vertexElements[ 5 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 5 ].semanticIndex = 0;
    vertexElements[ 5 ].bufferIndex = 0;

    m_spSkinnedMeshVertexDescription = pRenderer->CreateVertexDescription( vertexElements, 6 );
    HELIUM_ASSERT( m_spSkinnedMeshVertexDescription );

    vertexElements[ 0 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT32_2;
    vertexElements[ 0 ].semantic = RENDERER_VERTEX_SEMANTIC_POSITION;
    vertexElements[ 0 ].semanticIndex = 0;
    vertexElements[ 0 ].bufferIndex = 0;

    vertexElements[ 1 ].type = RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM;
    vertexElements[ 1 ].semantic = RENDERER_VERTEX_SEMANTIC_COLOR;
    vertexElements[ 1 ].semanticIndex = 0;
    vertexElements[ 1 ].bufferIndex = 0;

    vertexElements[ 2 ].type = RENDERER_VERTEX_DATA_TYPE_FLOAT16_2;
    vertexElements[ 2 ].semantic = RENDERER_VERTEX_SEMANTIC_TEXCOORD;
    vertexElements[ 2 ].semanticIndex = 0;
    vertexElements[ 2 ].bufferIndex = 0;

    m_spScreenVertexDescription = pRenderer->CreateVertexDescription( vertexElements, 3 );
    HELIUM_ASSERT( m_spScreenVertexDescription );

    // Create configuration-dependent render resources.
    PostConfigUpdate();

    // Attempt to load the depth-only pre-pass shader.
#pragma TODO( "XXX TMC: Migrate to a more data-driven solution." )
    AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
    HELIUM_ASSERT( pAssetLoader );

    AssetPath prePassShaderPath;
    HELIUM_VERIFY( prePassShaderPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Shaders" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "PrePass.hlsl" ) ) );

    AssetPtr spPrePassShader;
    HELIUM_VERIFY( pAssetLoader->LoadObject( prePassShaderPath, spPrePassShader ) );

    Shader* pPrePassShader = Reflect::SafeCast< Shader >( spPrePassShader.Get() );
    HELIUM_ASSERT( pPrePassShader );
    if( pPrePassShader )
    {
        size_t loadId = pPrePassShader->BeginLoadVariant( RShader::TYPE_VERTEX, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pPrePassShader->TryFinishLoadVariant( loadId, m_spPrePassVertexShader ) )
            {
                pAssetLoader->Tick();
            }
        }
    }

    // Attempt to load the simple world-space, simple screen-space, and screen-space text shaders.
#pragma TODO( "XXX TMC: Migrate to a more data-driven solution." )
    AssetPath shaderPath;
    AssetPtr spShader;
    Shader* pShader;

    HELIUM_VERIFY( shaderPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Shaders" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "Simple.hlsl" ) ) );

    HELIUM_VERIFY( pAssetLoader->LoadObject( shaderPath, spShader ) );

    pShader = Reflect::SafeCast< Shader >( spShader.Get() );
    HELIUM_ASSERT( pShader );
    if( pShader )
    {
        size_t loadId = pShader->BeginLoadVariant( RShader::TYPE_VERTEX, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spSimpleWorldSpaceVertexShader ) )
            {
                pAssetLoader->Tick();
            }
        }

        loadId = pShader->BeginLoadVariant( RShader::TYPE_PIXEL, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spSimpleWorldSpacePixelShader ) )
            {
                pAssetLoader->Tick();
            }
        }
    }

    HELIUM_VERIFY( shaderPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Shaders" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "ScreenSpaceTexture.hlsl" ) ) );

    HELIUM_VERIFY( pAssetLoader->LoadObject( shaderPath, spShader ) );

    pShader = Reflect::SafeCast< Shader >( spShader.Get() );
    HELIUM_ASSERT( pShader );
    if( pShader )
    {
        size_t loadId = pShader->BeginLoadVariant( RShader::TYPE_VERTEX, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spSimpleScreenSpaceVertexShader ) )
            {
                pAssetLoader->Tick();
            }
        }

        loadId = pShader->BeginLoadVariant( RShader::TYPE_PIXEL, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spSimpleScreenSpacePixelShader ) )
            {
                pAssetLoader->Tick();
            }
        }
    }

    HELIUM_VERIFY( shaderPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Shaders" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "ScreenText.hlsl" ) ) );

    HELIUM_VERIFY( pAssetLoader->LoadObject( shaderPath, spShader ) );

    pShader = Reflect::SafeCast< Shader >( spShader.Get() );
    HELIUM_ASSERT( pShader );
    if( pShader )
    {
        size_t loadId = pShader->BeginLoadVariant( RShader::TYPE_VERTEX, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spScreenTextVertexShader ) )
            {
                pAssetLoader->Tick();
            }
        }

        loadId = pShader->BeginLoadVariant( RShader::TYPE_PIXEL, 0 );
        HELIUM_ASSERT( IsValid( loadId ) );
        if( IsValid( loadId ) )
        {
            while( !pShader->TryFinishLoadVariant( loadId, m_spScreenTextPixelShader ) )
            {
                pAssetLoader->Tick();
            }
        }
    }

    // Attempt to load the debug fonts.
#pragma TODO( "XXX TMC: Migrate to a more data-driven solution." )
    AssetPath fontPath;
    AssetPtr spFont;

    HELIUM_VERIFY( fontPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Fonts" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "DebugSmall" ) ) );
    HELIUM_VERIFY( pAssetLoader->LoadObject( fontPath, spFont ) );
    m_debugFonts[ DEBUG_FONT_SIZE_SMALL ] = Reflect::SafeCast< Font >( spFont.Get() );
    spFont.Release();

    HELIUM_VERIFY( fontPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Fonts" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "DebugMedium" ) ) );
    HELIUM_VERIFY( pAssetLoader->LoadObject( fontPath, spFont ) );
    m_debugFonts[ DEBUG_FONT_SIZE_MEDIUM ] = Reflect::SafeCast< Font >( spFont.Get() );
    spFont.Release();

    HELIUM_VERIFY( fontPath.Set(
        HELIUM_PACKAGE_PATH_CHAR_STRING TXT( "Fonts" ) HELIUM_OBJECT_PATH_CHAR_STRING TXT( "DebugLarge" ) ) );
    HELIUM_VERIFY( pAssetLoader->LoadObject( fontPath, spFont ) );
    m_debugFonts[ DEBUG_FONT_SIZE_LARGE ] = Reflect::SafeCast< Font >( spFont.Get() );
    spFont.Release();
}

/// Release all state references.
///
/// @see Initialize(), PostConfigUpdate()
void RenderResourceManager::Shutdown()
{
    for( size_t sizeIndex = 0; sizeIndex < HELIUM_ARRAY_COUNT( m_debugFonts ); ++sizeIndex )
    {
        m_debugFonts[ sizeIndex ].Release();
    }

    m_spScreenTextPixelShader.Release();
    m_spScreenTextVertexShader.Release();
    m_spSimpleWorldSpacePixelShader.Release();
    m_spSimpleWorldSpaceVertexShader.Release();
    m_spSimpleScreenSpacePixelShader.Release();
    m_spSimpleScreenSpaceVertexShader.Release();
    m_spPrePassVertexShader.Release();

    m_spDepthStencilSurface.Release();

    m_spShadowDepthTexture.Release();
    m_spSceneTexture.Release();

    for( size_t stateIndex = 0; stateIndex < HELIUM_ARRAY_COUNT( m_rasterizerStates ); ++stateIndex )
    {
        m_rasterizerStates[ stateIndex ].Release();
    }

    for( size_t stateIndex = 0; stateIndex < HELIUM_ARRAY_COUNT( m_blendStates ); ++stateIndex )
    {
        m_blendStates[ stateIndex ].Release();
    }

    for( size_t stateIndex = 0; stateIndex < HELIUM_ARRAY_COUNT( m_depthStencilStates ); ++stateIndex )
    {
        m_depthStencilStates[ stateIndex ].Release();
    }

    for( size_t filterIndex = 0; filterIndex < HELIUM_ARRAY_COUNT( m_samplerStates ); ++filterIndex )
    {
        RSamplerStatePtr* pSamplerStates = m_samplerStates[ filterIndex ];
        for( size_t addressModeIndex = 0;
            addressModeIndex < HELIUM_ARRAY_COUNT( m_samplerStates[ 0 ] );
            ++addressModeIndex )
        {
            pSamplerStates[ addressModeIndex ].Release();
        }
    }

    m_spSimpleTexturedVertexDescription.Release();
    m_spSimpleVertexDescription.Release();
    m_spScreenVertexDescription.Release();
    m_spProjectedVertexDescription.Release();

    for( size_t descriptionIndex = 0;
        descriptionIndex < HELIUM_ARRAY_COUNT( m_staticMeshVertexDescriptions );
        ++descriptionIndex )
    {
        m_staticMeshVertexDescriptions[ descriptionIndex ].Release();
    }

    m_spSkinnedMeshVertexDescription.Release();
}

/// Reinitialize any resources dependent on graphics configuration settings.
///
/// This can be called during runtime when the graphics configuration settings change to reconstruct states and
/// other resources that are dependent on certain graphics configuration settings (i.e. default texture filtering
/// mode).  Existing resources that are reinitialized will have their reference counts decremented and will be
/// cleared out of this manager.  If no renderer is initialized, resources will remain null.
///
/// @see Initialize(), Shutdown()
void RenderResourceManager::PostConfigUpdate()
{
    // Release resources that are dependent on configuration settings.
    RSamplerStatePtr* pLinearSamplerStates = m_samplerStates[ TEXTURE_FILTER_LINEAR ];
    for( size_t addressModeIndex = 0; addressModeIndex < RENDERER_TEXTURE_ADDRESS_MODE_MAX; ++addressModeIndex )
    {
        pLinearSamplerStates[ addressModeIndex ].Release();
    }

    m_spDepthStencilSurface.Release();

    m_spShadowDepthTexture.Release();
    m_spSceneTexture.Release();

    m_viewportWidthMax = 0;
    m_viewportHeightMax = 0;

    m_shadowMode = GraphicsConfig::EShadowMode::NONE;
    m_shadowDepthTextureUsableSize = 0;

    // Get the renderer and graphics configuration.
    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        return;
    }

    Config& rConfig = Config::GetStaticInstance();
    StrongPtr< GraphicsConfig > spGraphicsConfig(
        rConfig.GetConfigObject< GraphicsConfig >( Name( TXT( "GraphicsConfig" ) ) ) );
    if( !spGraphicsConfig )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "RenderResourceManager::PostConfigUpdate(): Initialization failed; missing GraphicsConfig.\n" ) );

        return;
    }

    // Create the standard linear-filtering sampler states.
    RSamplerState::Description samplerStateDesc;
    samplerStateDesc.addressModeW = RENDERER_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerStateDesc.mipLodBias = 0;
    samplerStateDesc.maxAnisotropy = spGraphicsConfig->GetMaxAnisotropy();

    GraphicsConfig::ETextureFilter textureFiltering = spGraphicsConfig->GetTextureFiltering();
    switch( textureFiltering )
    {
    case GraphicsConfig::ETextureFilter::TRILINEAR:
        {
            samplerStateDesc.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR;
            break;
        }

    case GraphicsConfig::ETextureFilter::ANISOTROPIC:
        {
            samplerStateDesc.filter = RENDERER_TEXTURE_FILTER_ANISOTROPIC;
            break;
        }

    case GraphicsConfig::ETextureFilter::BILINEAR:
    default:
        {
            samplerStateDesc.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT;
            break;
        }
    }

    for( size_t addressModeIndex = 0; addressModeIndex < RENDERER_TEXTURE_ADDRESS_MODE_MAX; ++addressModeIndex )
    {
        ERendererTextureAddressMode addressMode = static_cast< ERendererTextureAddressMode >( addressModeIndex );
        samplerStateDesc.addressModeU = addressMode;
        samplerStateDesc.addressModeV = addressMode;
        samplerStateDesc.addressModeW = addressMode;

        pLinearSamplerStates[ addressModeIndex ] = pRenderer->CreateSamplerState( samplerStateDesc );
        HELIUM_ASSERT( pLinearSamplerStates[ addressModeIndex ] );
    }

    // Store shadow buffer settings.
    GraphicsConfig::EShadowMode shadowMode = GraphicsConfig::EShadowMode::NONE;
    uint32_t shadowBufferUsableSize = 0;

    if( pRenderer->SupportsAnyFeature( RENDERER_FEATURE_FLAG_DEPTH_TEXTURE ) )
    {
        shadowMode = spGraphicsConfig->GetShadowMode();
        if( shadowMode != GraphicsConfig::EShadowMode::INVALID && shadowMode != GraphicsConfig::EShadowMode::NONE )
        {
            shadowBufferUsableSize = spGraphicsConfig->GetShadowBufferSize();
        }
    }

    m_shadowMode = shadowMode;
    m_shadowDepthTextureUsableSize = shadowBufferUsableSize;

    // Recreate render and depth targets.
    UpdateMaxViewportSize( spGraphicsConfig->m_width, spGraphicsConfig->m_height );
	
	m_viewportWidthMax = spGraphicsConfig->m_width;
	m_viewportHeightMax = spGraphicsConfig->m_height;
}

/// Reconstruct render resources based on the maximum render viewport size.
///
/// @param[in] width   Maximum viewport width, in pixels.
/// @param[in] height  Maximum viewport height, in pixels.
void RenderResourceManager::UpdateMaxViewportSize( uint32_t width, uint32_t height )
{
    if( width == m_viewportWidthMax && height == m_viewportHeightMax )
    {
        return;
    }

    m_spDepthStencilSurface.Release();

    m_spShadowDepthTexture.Release();
    m_spSceneTexture.Release();

    Renderer* pRenderer = Renderer::GetStaticInstance();
    if( !pRenderer )
    {
        m_viewportWidthMax = 0;
        m_viewportHeightMax = 0;
        m_shadowMode = GraphicsConfig::EShadowMode::NONE;
        m_shadowDepthTextureUsableSize = 0;

        return;
    }

    // Don't create any surfaces for zero-sized viewports.
    if( width == 0 || height == 0 )
    {
        m_viewportWidthMax = 0;
        m_viewportHeightMax = 0;

        return;
    }

    // Due to restrictions with render target settings on certain platforms (namely when using Direct3D), the scene
    // texture, depth-stencil surface, and shadow depth texture must all be the same size.
    // XXX WDI: Implement support for the NULL FOURCC format for Direct3D to avoid this restriction when possible.
    uint32_t bufferWidth = Max( width, m_shadowDepthTextureUsableSize );
    uint32_t bufferHeight = Max( height, m_shadowDepthTextureUsableSize );

    m_spSceneTexture = pRenderer->CreateTexture2d(
        bufferWidth,
        bufferHeight,
        1,
        RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT,
        RENDERER_BUFFER_USAGE_RENDER_TARGET );
    if( !m_spSceneTexture )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "Failed to create scene render texture of size %" ) PRIu32 TXT( "x%" ) PRIu32 TXT( ".\n" ),
            bufferWidth,
            bufferHeight );
    }

    m_spDepthStencilSurface = pRenderer->CreateDepthStencilSurface(
        bufferWidth,
        bufferHeight,
        RENDERER_SURFACE_FORMAT_DEPTH_STENCIL,
        0 );
    if( !m_spDepthStencilSurface )
    {
        HELIUM_TRACE(
            TraceLevels::Error,
            TXT( "Failed to create scene depth-stencil surface of size %" ) PRIu32 TXT( "x%" ) PRIu32 TXT( ".\n" ),
            bufferWidth,
            bufferHeight );
    }

    if( m_shadowMode != GraphicsConfig::EShadowMode::NONE && m_shadowMode != GraphicsConfig::EShadowMode::INVALID &&
        m_shadowDepthTextureUsableSize != 0 )
    {
        m_spShadowDepthTexture = pRenderer->CreateTexture2d(
            bufferWidth,
            bufferHeight,
            1,
            RENDERER_PIXEL_FORMAT_DEPTH,
            RENDERER_BUFFER_USAGE_DEPTH_STENCIL );
        if( !m_spShadowDepthTexture )
        {
            HELIUM_TRACE(
                TraceLevels::Error,
                TXT( "Failed to create shadow depth texture of size %" ) PRIu32 TXT( "x%" ) PRIu32 TXT( ".\n" ),
                bufferWidth,
                bufferHeight );
        }
    }
}

/// Get the rasterizer state instance for the specified state type.
///
/// @param[in] type  Rasterizer state type.
///
/// @return  Rasterizer state instance.
RRasterizerState* RenderResourceManager::GetRasterizerState( ERasterizerState type ) const
{
    HELIUM_ASSERT( static_cast< size_t >( type ) < static_cast< size_t >( RASTERIZER_STATE_MAX ) );

    return m_rasterizerStates[ type ];
}

/// Get the blend state instance for the specified state type.
///
/// @param[in] type  Blend state type.
///
/// @return  Blend state instance.
RBlendState* RenderResourceManager::GetBlendState( EBlendState type ) const
{
    HELIUM_ASSERT( static_cast< size_t >( type ) < static_cast< size_t >( BLEND_STATE_MAX ) );

    return m_blendStates[ type ];
}

/// Get the depth/stencil state instance for the specified state type.
///
/// @param[in] type  Depth/stencil state type.
///
/// @return  Depth/stencil state instance.
RDepthStencilState* RenderResourceManager::GetDepthStencilState( EDepthStencilState type ) const
{
    HELIUM_ASSERT( static_cast< size_t >( type ) < static_cast< size_t >( DEPTH_STENCIL_STATE_MAX ) );

    return m_depthStencilStates[ type ];
}

/// Get the sampler state instance for the specified texture filtering and addressing mode.
///
/// @param[in] filterType   Texture filtering mode.
/// @param[in] addressMode  Texture coordinate addressing mode.
///
/// @return  Sampler state instance.
RSamplerState* RenderResourceManager::GetSamplerState(
    ETextureFilter filterType,
    ERendererTextureAddressMode addressMode ) const
{
    HELIUM_ASSERT( static_cast< size_t >( filterType ) < static_cast< size_t >( TEXTURE_FILTER_MAX ) );
    HELIUM_ASSERT( static_cast< size_t >( addressMode ) < static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );

    return m_samplerStates[ filterType ][ addressMode ];
}

/// Get the description for SimpleVertex vertices.
///
/// @return  SimpleVertex vertex description.
///
/// @see GetSimpleTexturedVertexDescription(), GetScreenVertexDescription(), GetProjectedVertexDescription(),
///      GetStaticMeshVertexDescription(), GetSkinnedMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetSimpleVertexDescription() const
{
    return m_spSimpleVertexDescription;
}

/// Get the description for SimpleTexturedVertex vertices.
///
/// @return  SimpleTexturedVertex vertex description.
///
/// @see GetSimpleVertexDescription(), GetScreenVertexDescription(), GetProjectedVertexDescription(),
///      GetStaticMeshVertexDescription(), GetSkinnedMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetSimpleTexturedVertexDescription() const
{
    return m_spSimpleTexturedVertexDescription;
}

/// Get the description for ScreenVertex vertices.
///
/// @return  ScreenVertex vertex description.
///
/// @see GetSimpleVertexDescription(), GetSimpleTexturedVertexDescription(), GetProjectedVertexDescription(),
///      GetStaticMeshVertexDescription(), GetSkinnedMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetScreenVertexDescription() const
{
    return m_spScreenVertexDescription;
}

/// Get the description for ProjectedVertex vertices.
///
/// @return  ScreenVertex vertex description.
///
/// @see GetSimpleVertexDescription(), GetSimpleTexturedVertexDescription(), GetScreenVertexDescription(),
///      GetStaticMeshVertexDescription(), GetSkinnedMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetProjectedVertexDescription() const
{
    return m_spProjectedVertexDescription;
}

/// Get the description for static mesh vertices with the specified number of texture coordinate sets.
///
/// @param[in] textureCoordinateSetCount  Number of texture coordinate sets (must be between 1 and
///                                       MESH_TEXTURE_COORDINATE_SET_COUNT_MAX, inclusive).
///
/// @return  Vertex description.
///
/// @see GetSimpleVertexDescription(), GetSimpleTexturedVertexDescription(), GetScreenVertexDescription(),
///      GetProjectedVertexDescription(), GetSkinnedMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetStaticMeshVertexDescription( size_t textureCoordinateSetCount ) const
{
    HELIUM_ASSERT( textureCoordinateSetCount >= 1 );
    HELIUM_ASSERT( textureCoordinateSetCount <= MESH_TEXTURE_COORDINATE_SET_COUNT_MAX );

    return m_staticMeshVertexDescriptions[ textureCoordinateSetCount - 1 ];
}

/// Get the description for skinned mesh vertices.
///
/// @return  Skinned mesh vertex description.
///
/// @see GetSimpleVertexDescription(), GetSimpleTexturedVertexDescription(), GetScreenVertexDescription(),
///      GetProjectedVertexDescription(), GetStaticMeshVertexDescription()
RVertexDescription* RenderResourceManager::GetSkinnedMeshVertexDescription() const
{
    return m_spSkinnedMeshVertexDescription;
}

/// Get the texture to which scene color data is written each frame.
///
/// @return  Scene color target texture.
///
/// @see GetShadowDepthTexture()
RTexture2d* RenderResourceManager::GetSceneTexture() const
{
    return m_spSceneTexture;
}

/// Get the depth texture resource for shadow depth rendering.
///
/// @return  Shadow depth texture resource.
///
/// @see GetSceneTexture(), GetShadowDepthTextureUsableSize()
RTexture2d* RenderResourceManager::GetShadowDepthTexture() const
{
    return m_spShadowDepthTexture;
}

/// Get the main depth-stencil surface for scene rendering.
///
/// @return  Scene depth-stencil surface.
///
/// @see GetSceneTexture()
RSurface* RenderResourceManager::GetDepthStencilSurface() const
{
    return m_spDepthStencilSurface;
}

/// Get the vertex shader variant resource for depth-only pre-pass rendering.
///
/// @return  Pre-pass vertex shader variant.
ShaderVariant* RenderResourceManager::GetPrePassVertexShader() const
{
    return m_spPrePassVertexShader;
}

/// Get the vertex shader variant resource for basic world-space primitive rendering.
///
/// @return  Simple world-space primitive vertex shader variant.
///
/// @see GetSimpleWorldSpacePixelShader(), GetSimpleScreenSpaceVertexShader(), GetSimpleScreenSpacePixelShader(),
///      GetScreenTextVertexShader(), GetScreenTextPixelShader()
ShaderVariant* RenderResourceManager::GetSimpleWorldSpaceVertexShader() const
{
    return m_spSimpleWorldSpaceVertexShader;
}

/// Get the pixel shader variant resource for basic world-space primitive rendering.
///
/// @return  Simple world-space primitive pixel shader variant.
///
/// @see GetSimpleWorldSpaceVertexShader(), GetSimpleScreenSpaceVertexShader(), GetSimpleScreenSpacePixelShader(),
///      GetScreenTextVertexShader(), GetScreenTextPixelShader()
ShaderVariant* RenderResourceManager::GetSimpleWorldSpacePixelShader() const
{
    return m_spSimpleWorldSpacePixelShader;
}

/// Get the vertex shader variant resource for basic screen-space primitive rendering.
///
/// @return  Simple screen-space primitive vertex shader variant.
///
/// @see GetSimpleScreenSpacePixelShader(), GetSimpleWorldSpaceVertexShader(), GetSimpleWorldSpacePixelShader(),
///      GetScreenTextVertexShader(), GetScreenTextPixelShader()
ShaderVariant* RenderResourceManager::GetSimpleScreenSpaceVertexShader() const
{
    return m_spSimpleScreenSpaceVertexShader;
}

/// Get the pixel shader variant resource for basic screen-space primitive rendering.
///
/// @return  Simple screen-space primitive pixel shader variant.
///
/// @see GetSimpleScreenSpaceVertexShader(), GetSimpleWorldSpaceVertexShader(), GetSimpleWorldSpacePixelShader(),
///      GetScreenTextVertexShader(), GetScreenTextPixelShader()
ShaderVariant* RenderResourceManager::GetSimpleScreenSpacePixelShader() const
{
    return m_spSimpleScreenSpacePixelShader;
}

/// Get the vertex shader variant for screen-space text rendering.
///
/// @return  Screen-space text vertex shader variant.
///
/// @see GetSimpleScreenSpaceVertexShader(), GetSimpleWorldSpaceVertexShader(), GetSimpleWorldSpacePixelShader(),
///      GetSimpleScreenSpacePixelShader(), GetScreenTextPixelShader()
ShaderVariant* RenderResourceManager::GetScreenTextVertexShader() const
{
    return m_spScreenTextVertexShader;
}

/// Get the pixel shader variant for screen-space text rendering.
///
/// @return  Screen-space text pixel shader variant.
///
/// @see GetSimpleScreenSpaceVertexShader(), GetSimpleWorldSpaceVertexShader(), GetSimpleWorldSpacePixelShader(),
///      GetSimpleScreenSpacePixelShader(), GetScreenTextVertexShader()
ShaderVariant* RenderResourceManager::GetScreenTextPixelShader() const
{
    return m_spScreenTextPixelShader;
}

/// Get the debug text font resource of the specified size.
///
/// @param[in] size  Debug text font size identifier.
///
/// @return  Pointer to the debug text font of the specified size.
Font* RenderResourceManager::GetDebugFont( EDebugFontSize size ) const
{
    HELIUM_ASSERT( static_cast< size_t >( size ) < HELIUM_ARRAY_COUNT( m_debugFonts ) );

    return m_debugFonts[ size ];
}

/// Get the singleton RenderResourceManager instance, creating it if necessary.
///
/// @return  Reference to the RenderResourceManager instance.
///
/// @see DestroyStaticInstance()
RenderResourceManager& RenderResourceManager::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new RenderResourceManager;
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the singleton RenderResourceManager instance.
///
/// @see GetStaticInstance()
void RenderResourceManager::DestroyStaticInstance()
{
    if( sm_pInstance )
    {
        sm_pInstance->Shutdown();
        delete sm_pInstance;
        sm_pInstance = NULL;
    }
}
