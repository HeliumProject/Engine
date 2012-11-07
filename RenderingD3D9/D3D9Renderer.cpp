//----------------------------------------------------------------------------------------------------------------------
// D3D9Renderer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9Renderer.h"

#include "Platform/Thread.h"
#include "Rendering/RendererUtil.h"

#include "RenderingD3D9/D3D9BlendState.h"
#include "RenderingD3D9/D3D9ConstantBuffer.h"
#include "RenderingD3D9/D3D9DeferredCommandProxy.h"
#include "RenderingD3D9/D3D9DepthStencilState.h"
#include "RenderingD3D9/D3D9DepthStencilSurface.h"
#include "RenderingD3D9/D3D9DynamicIndexBuffer.h"
#include "RenderingD3D9/D3D9DynamicTexture2d.h"
#include "RenderingD3D9/D3D9DynamicVertexBuffer.h"
#include "RenderingD3D9/D3D9Fence.h"
#include "RenderingD3D9/D3D9ImmediateCommandProxy.h"
#include "RenderingD3D9/D3D9MainContext.h"
#include "RenderingD3D9/D3D9PixelShader.h"
#include "RenderingD3D9/D3D9RasterizerState.h"
#include "RenderingD3D9/D3D9SamplerState.h"
#include "RenderingD3D9/D3D9StaticTexture2d.h"
#include "RenderingD3D9/D3D9SubContext.h"
#include "RenderingD3D9/D3D9VertexDescription.h"
#include "RenderingD3D9/D3D9VertexInputLayout.h"
#include "RenderingD3D9/D3D9VertexShader.h"

namespace Helium
{
    HELIUM_DECLARE_RPTR( RFence );

    HELIUM_DECLARE_RPTR( D3D9RasterizerState );
    HELIUM_DECLARE_RPTR( D3D9BlendState );
    HELIUM_DECLARE_RPTR( D3D9DepthStencilState );
    HELIUM_DECLARE_RPTR( D3D9SamplerState );
}

using namespace Helium;

// Non-zero to disable Direct3D 9Ex support (for testing purposes only; shipping builds should leave support enabled).
#define L_DISABLE_DIRECT3D9EX 0

// Direct3DCreate9Ex() function signature.
typedef HRESULT ( WINAPI DIRECT3DCREATE9EX_FUNC )( UINT, IDirect3D9Ex** );

// GUID associated with engine-specific private data associated with Direct3D resources.
// {E4EC0248-A917-4163-A56C-A8962C9362F2}
const ::GUID D3D9Renderer::sm_privateDataGuid =
{
    0xe4ec0248,
    0xa917,
    0x4163,
    { 0xa5, 0x6c, 0xa8, 0x96, 0x2c, 0x93, 0x62, 0xf2 }
};

/// Constructor.
D3D9Renderer::D3D9Renderer()
    : m_pD3D( NULL )
    , m_pD3DDevice( NULL )
    , m_bExDevice( false )
    , m_bLost( false )
    , m_depthTextureFormat( D3DFMT_UNKNOWN )
{
}

/// Destructor.
D3D9Renderer::~D3D9Renderer()
{
}

/// @copydoc Renderer::Initialize()
bool D3D9Renderer::Initialize()
{
    // Trap attempts at multiple initialization.
    HELIUM_ASSERT( !m_pD3D );
    if( m_pD3D )
    {
        HELIUM_TRACE( TRACE_WARNING, TXT( "D3D9Renderer: Initialize() called on an already initialized renderer.\n" ) );

        return true;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "Initializing Direct3D 9 rendering support (D3D9Renderer).\n" ) );

#if !L_DISABLE_DIRECT3D9EX
    // Test for Direct3D9Ex support (provides better support for Windows Vista and later).
    HMODULE hD3DLibrary = LoadLibraryA( "d3d9.dll" );
    if( !hD3DLibrary )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to load d3d9.dll.\n" ) );

        return false;
    }

    DIRECT3DCREATE9EX_FUNC* pDirect3DCreate9Ex =
        reinterpret_cast< DIRECT3DCREATE9EX_FUNC* >( GetProcAddress( hD3DLibrary, "Direct3DCreate9Ex" ) );
    if( !pDirect3DCreate9Ex )
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "Direct3D9Ex support not found.\n" ) );
    }
    else
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "Direct3D9Ex support found.\n" ) );

        IDirect3D9Ex* pD3DEx = NULL;
        HRESULT createResult = pDirect3DCreate9Ex( D3D_SDK_VERSION, &pD3DEx );

        FreeLibrary( hD3DLibrary );

        if( FAILED( createResult ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "Failed to initialize Direct3D9Ex interface (error code 0x%x).  Falling back to standard " )
                TXT( "Direct3D9 support.\n" ) ),
                createResult );
        }
        else
        {
            HELIUM_ASSERT( pD3DEx );
            m_pD3D = pD3DEx;
            m_bExDevice = true;

            HELIUM_TRACE( TRACE_INFO, TXT( "Direct3D9Ex initialized successfully.\n" ) );
        }
    }
#endif

    if( !m_pD3D )
    {
        m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
        if( !m_pD3D )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Failed to initialize Direct3D9 interface.\n" ) );

            return false;
        }

        m_bExDevice = false;
    }

    // Determine the best depth texture format to use for the current driver implementation.
    const D3DFORMAT depthTextureFormats[] =
    {
        D3DFMT_D24X8,
        D3DFMT_D16,
        static_cast< D3DFORMAT >( MAKEFOURCC( 'D', 'F', '2', '4' ) ),
        static_cast< D3DFORMAT >( MAKEFOURCC( 'D', 'F', '1', '6' ) )
    };

    for( size_t depthFormatIndex = 0; depthFormatIndex < HELIUM_ARRAY_COUNT( depthTextureFormats ); ++depthFormatIndex )
    {
        D3DFORMAT format = depthTextureFormats[ depthFormatIndex ];
        HRESULT checkResult = m_pD3D->CheckDeviceFormat(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            D3DFMT_X8R8G8B8,
            D3DUSAGE_DEPTHSTENCIL,
            D3DRTYPE_TEXTURE,
            format );
        if( checkResult == D3D_OK )
        {
            HELIUM_TRACE(
                TRACE_DEBUG,
                TXT( "Using D3DFORMAT %x for depth textures.\n" ),
                static_cast< unsigned int >( format ) );

            m_depthTextureFormat = format;

            break;
        }
    }

    if( m_depthTextureFormat == D3DFMT_UNKNOWN )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            ( TXT( "Failed to find an appropriate D3DFORMAT for depth texture support.  Shadow mapping and other " )
              TXT( "depth-dependent effects will be disabled.\n" ) ) );
    }

    // Store the renderer feature flag set.
    m_featureFlags = 0;
    if( m_depthTextureFormat != D3DFMT_UNKNOWN )
    {
        m_featureFlags |= RENDERER_FEATURE_FLAG_DEPTH_TEXTURE;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "Direct3D9 initialized successfully.\n" ) );

    return true;
}

/// @copydoc Renderer::Shutdown()
void D3D9Renderer::Shutdown()
{
    HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down Direct3D 9 rendering support (D3D9Renderer).\n" ) );

    m_spMainContext.Release();
    m_spImmediateCommandProxy.Release();

    for( size_t mapPoolIndex = 0; mapPoolIndex < HELIUM_ARRAY_COUNT( m_staticTextureMapTargetPools ); ++mapPoolIndex )
    {
        DynArray< IDirect3DTexture9* >* pTextureMapPools = m_staticTextureMapTargetPools[ mapPoolIndex ];

        for( size_t pixelFormatIndex = 0;
            pixelFormatIndex < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX );
            ++pixelFormatIndex )
        {
            DynArray< IDirect3DTexture9* >& rPool = pTextureMapPools[ pixelFormatIndex ];

            size_t textureCount = rPool.GetSize();
            for( size_t textureIndex = 0; textureIndex < textureCount; ++textureIndex )
            {
                IDirect3DTexture9* pTexture = rPool[ textureIndex ];
                HELIUM_ASSERT( pTexture );
                pTexture->Release();
            }

            rPool.Clear();
        }
    }

    if( m_pD3DDevice )
    {
        m_pD3DDevice->Release();
        m_pD3DDevice = NULL;
    }

    if( m_pD3D )
    {
        m_pD3D->Release();
        m_pD3D = NULL;
    }

    m_depthTextureFormat = D3DFMT_UNKNOWN;

    m_featureFlags = 0;

    HELIUM_TRACE( TRACE_INFO, TXT( "Direct3D 9 renderer shutdown complete.\n" ) );
}

/// @copydoc Renderer::CreateMainContext()
bool D3D9Renderer::CreateMainContext( const ContextInitParameters& rInitParameters )
{
    HELIUM_ASSERT_MSG( m_pD3D, TXT( "D3D9Renderer not initialized" ) );

    // Trap multiple calls to this function.
    HELIUM_ASSERT( !m_pD3DDevice );
    if( m_pD3DDevice )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            TXT( "D3D9Renderer: CreateMainContext() called when a main context already exists.\n" ) );

        return false;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "D3D9Renderer: Creating main display context.\n" ) );

    // Build the presentation parameters.
    if( !GetPresentParameters( rInitParameters, m_presentParameters, m_fullscreenDisplayMode ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Failed to find matching fullscreen display mode for the main context.\n" ) );

        return false;
    }

    HRESULT createResult;
    if( m_bExDevice )
    {
        IDirect3DDevice9Ex* pD3DDeviceEx = NULL;
        createResult = static_cast< IDirect3D9Ex* >( m_pD3D )->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            static_cast< HWND >( rInitParameters.pWindow ),
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &m_presentParameters,
            ( rInitParameters.bFullscreen ? &m_fullscreenDisplayMode : NULL ),
            &pD3DDeviceEx );

        m_pD3DDevice = pD3DDeviceEx;
    }
    else
    {
        createResult = m_pD3D->CreateDevice(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            static_cast< HWND >( rInitParameters.pWindow ),
            D3DCREATE_HARDWARE_VERTEXPROCESSING,
            &m_presentParameters,
            &m_pD3DDevice );
    }

    if( FAILED( createResult ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer: Device creation failed (error code: 0x%x).\n" ), createResult );

        return false;
    }

    HELIUM_ASSERT( m_pD3DDevice );

    HELIUM_TRACE(
        TRACE_INFO,
        ( TXT( "D3D9Renderer: Display context created:\n- Dimensions: %ux%u\n- Multisample count: %u\n" )
          TXT( "- Fullscreen: %d\n- VSync: %d\n" ) ),
        rInitParameters.displayWidth,
        rInitParameters.displayHeight,
        rInitParameters.multisampleCount,
        static_cast< int32_t >( rInitParameters.bFullscreen ),
        static_cast< int32_t >( rInitParameters.bVsync ) );

    // Create the immediate render command proxy interface.
    m_spImmediateCommandProxy = new D3D9ImmediateCommandProxy( m_pD3DDevice );
    HELIUM_ASSERT( m_spImmediateCommandProxy );

    // Create the main rendering context interface.
    m_spMainContext = new D3D9MainContext( m_pD3DDevice );
    HELIUM_ASSERT( m_spMainContext );

    return true;
}

/// @copydoc Renderer::ResetMainContext()
bool D3D9Renderer::ResetMainContext( const ContextInitParameters& rInitParameters )
{
    HELIUM_ASSERT_MSG( m_pD3D, TXT( "D3D9Renderer not initialized" ) );

    // Make sure the main context has been initialized.
    HELIUM_ASSERT( m_spMainContext );
    if( !m_spMainContext )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Attempted to reset the main rendering context without creating it first.\n" ) );

        return false;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "D3D9Renderer: Resetting main display context.\n" ) );

    // Build the presentation parameters and reset the device.
    D3DPRESENT_PARAMETERS presentParameters;
    D3DDISPLAYMODEEX fullscreenDisplayMode;
    if( !GetPresentParameters( rInitParameters, presentParameters, fullscreenDisplayMode ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Failed to find matching fullscreen display mode for the main context.\n" ) );

        return false;
    }

    bool bResetSuccess = SUCCEEDED( ResetDevice( presentParameters, fullscreenDisplayMode ) );

    // Store the new presentation parameters and fullscreen display mode information if resetting was successful.
    if( bResetSuccess )
    {
        m_presentParameters = presentParameters;
        m_fullscreenDisplayMode = fullscreenDisplayMode;
    }

    return bResetSuccess;
}

/// @copydoc Renderer::GetMainContext()
RRenderContext* D3D9Renderer::GetMainContext()
{
    return m_spMainContext;
}

/// @copydoc Renderer::CreateSubContext()
RRenderContext* D3D9Renderer::CreateSubContext( const ContextInitParameters& rInitParameters )
{
    // Make sure the main context has been initialized.
    HELIUM_ASSERT( m_spMainContext );
    if( !m_spMainContext )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer: Attempted creation of a renderer sub-context without creating a main context " )
              TXT( "first.\n" ) ) );

        return NULL;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "D3D9Renderer: Creating display sub-context.\n" ) );

    // Build the presentation parameters and create the swap chain.
    D3DPRESENT_PARAMETERS presentParameters;
    D3DDISPLAYMODEEX fullscreenDisplayMode;
    if( !GetPresentParameters( rInitParameters, presentParameters, fullscreenDisplayMode ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Failed to find matching fullscreen display mode for the sub-context.\n" ) );

        return false;
    }

    IDirect3DSwapChain9* pSwapChain = NULL;
    HRESULT createResult = m_pD3DDevice->CreateAdditionalSwapChain( &presentParameters, &pSwapChain );
    if( FAILED( createResult ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Swap chain creation failed (error code: 0x%x).\n" ),
            createResult );

        return NULL;
    }

    HELIUM_ASSERT( pSwapChain );

    // Create the rendering sub-context interface.
    D3D9SubContext* pSubContext = new D3D9SubContext( pSwapChain );
    HELIUM_ASSERT( pSubContext );

    // D3D9SubContext instance now has its own reference to the swap chain, so release our reference.
    pSwapChain->Release();

    // Register the sub-context object for device reset events.
    RegisterDeviceResetListener( pSubContext );

    return pSubContext;
}

/// @copydoc Renderer::GetStatus()
Renderer::EStatus D3D9Renderer::GetStatus()
{
    // Don't test anything until we have received a notification that the device was lost.
    if( !m_bLost )
    {
        return STATUS_READY;
    }

    // Test the current cooperative-level status of the device.
    HELIUM_ASSERT( m_pD3DDevice );
    HRESULT result = m_pD3DDevice->TestCooperativeLevel();
    if( result == D3D_OK )
    {
        // ???
        m_bLost = false;

        return STATUS_READY;
    }

    if( result == D3DERR_DEVICELOST )
    {
        return STATUS_LOST;
    }

    if( result == D3DERR_DEVICENOTRESET )
    {
        return STATUS_NOT_RESET;
    }

    // Unknown result.
    HELIUM_ASSERT_MSG_FALSE(
        TXT( "IDirect3DDevice9::TestCooperativeLevel() returned an unhandled result code (0x%x)." ),
        static_cast< uint32_t >( result ) );

    return STATUS_INVALID;
}

/// @copydoc Renderer::Reset()
Renderer::EStatus D3D9Renderer::Reset()
{
    HELIUM_ASSERT_MSG( m_pD3D, TXT( "D3D9Renderer not initialized" ) );

    // Make sure the main context has been initialized.
    HELIUM_ASSERT( m_spMainContext );
    if( !m_spMainContext )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer: Attempted to reset the device without creating a main context first.\n" ) );

        return STATUS_INVALID;
    }

    HELIUM_TRACE( TRACE_INFO, TXT( "D3D9Renderer: Resetting device.\n" ) );

    HRESULT resetResult = ResetDevice( m_presentParameters, m_fullscreenDisplayMode );

    return ( resetResult == D3D_OK ? STATUS_READY : GetStatus() );
}

/// @copydoc Renderer::CreateRasterizerState()
RRasterizerState* D3D9Renderer::CreateRasterizerState( const RRasterizerState::Description& rDescription )
{
    D3D9RasterizerState* pState = new D3D9RasterizerState;
    HELIUM_ASSERT( pState );

    if( !pState->Initialize( rDescription ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer: Failed to create rasterizer state.\n" ) );

        D3D9RasterizerStatePtr spState( pState );

        return NULL;
    }

    return pState;
}

/// @copydoc Renderer::CreateBlendState()
RBlendState* D3D9Renderer::CreateBlendState( const RBlendState::Description& rDescription )
{
    D3D9BlendState* pState = new D3D9BlendState;
    HELIUM_ASSERT( pState );

    if( !pState->Initialize( rDescription ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer: Failed to create blend state.\n" ) );

        D3D9BlendStatePtr spState( pState );

        return NULL;
    }

    return pState;
}

/// @copydoc Renderer::CreateDepthStencilState()
RDepthStencilState* D3D9Renderer::CreateDepthStencilState( const RDepthStencilState::Description& rDescription )
{
    D3D9DepthStencilState* pState = new D3D9DepthStencilState;
    HELIUM_ASSERT( pState );

    if( !pState->Initialize( rDescription ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer: Failed to create depth-stencil state.\n" ) );

        D3D9DepthStencilStatePtr spState( pState );

        return NULL;
    }

    return pState;
}

/// @copydoc Renderer::CreateSamplerState()
RSamplerState* D3D9Renderer::CreateSamplerState( const RSamplerState::Description& rDescription )
{
    D3D9SamplerState* pState = new D3D9SamplerState;
    HELIUM_ASSERT( pState );

    if( !pState->Initialize( rDescription ) )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer: Failed to create sampler state.\n" ) );

        D3D9SamplerStatePtr spState( pState );

        return NULL;
    }

    return pState;
}

/// @copydoc Renderer::CreateDepthStencilSurface()
RSurface* D3D9Renderer::CreateDepthStencilSurface(
    uint32_t width,
    uint32_t height,
    ERendererSurfaceFormat format,
    uint32_t multisampleCount )
{
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_SURFACE_FORMAT_MAX ) );

    static const D3DFORMAT d3dFormats[ RENDERER_SURFACE_FORMAT_MAX ] =
    {
        D3DFMT_D24X8,  // RENDERER_SURFACE_FORMAT_DEPTH_ONLY
        D3DFMT_D24S8   // RENDERER_SURFACE_FORMAT_DEPTH_STENCIL
    };

    D3DMULTISAMPLE_TYPE multisampleType = D3DMULTISAMPLE_NONE;
    if( multisampleCount > 1 )
    {
        if( multisampleCount > 16 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "D3D9Renderer::CreateDepthStencilSurface(): Multisample count cannot be more than 16.  Value " )
                  TXT( "will be clamped.\n" ) ) );
            multisampleCount = 16;
        }

        multisampleType = static_cast< D3DMULTISAMPLE_TYPE >(
            static_cast< uint32_t >( D3DMULTISAMPLE_2_SAMPLES ) + multisampleCount - 2 );
    }

    IDirect3DSurface9* pD3DSurface = NULL;
    HRESULT result = m_pD3DDevice->CreateDepthStencilSurface(
        width,
        height,
        d3dFormats[ format ],
        multisampleType,
        0,
        FALSE,
        &pD3DSurface,
        NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateDepthStencilSurface(): Failed to create depth-stencil surface (error code: " )
              TXT( "0x%x).\n" ) ),
            result );

        return NULL;
    }

    D3D9Surface* pSurface;
    if( !m_bExDevice )
    {
        D3D9DepthStencilSurface* pDepthStencilSurface = new D3D9DepthStencilSurface( pD3DSurface, false );
        HELIUM_ASSERT( pDepthStencilSurface );
        RegisterDeviceResetListener( pDepthStencilSurface );

        pSurface = pDepthStencilSurface;
    }
    else
    {
        pSurface = new D3D9Surface( pD3DSurface, false );
    }

    HELIUM_ASSERT( pSurface );

    pD3DSurface->Release();

    return pSurface;
}

/// @copydoc Renderer::CreateVertexShader()
RVertexShader* D3D9Renderer::CreateVertexShader( size_t size, const void* pData )
{
    // Create the shader immediately if shader data was provided.
    if( pData )
    {
        // Verify that the shader data is DWORD-aligned since we need to cast it when passing it to the shader
        // creation function.
        HELIUM_ASSERT( ( reinterpret_cast< uintptr_t >( pData ) & ( sizeof( DWORD ) - 1 ) ) == 0 );

        HELIUM_ASSERT( m_pD3DDevice );

        IDirect3DVertexShader9* pD3DShader = NULL;
        HRESULT createResult = m_pD3DDevice->CreateVertexShader(
            static_cast< const DWORD* >( pData ),
            &pD3DShader );
        if( FAILED( createResult ) )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer::CreateVertexShader(): Vertex shader creation failed.\n" ) );

            return NULL;
        }

        D3D9VertexShader* pShader = new D3D9VertexShader( pD3DShader, false );
        HELIUM_ASSERT( pShader );

        pD3DShader->Release();

        return pShader;
    }

    // Allocate a staging buffer for deferred loading of the shader data.
    void* pStaging = DefaultAllocator().Allocate( size );
    if( !pStaging )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateVertexShader(): Failed to allocate staging buffer of %" ) TPRIuSZ
            TXT( " bytes for loading.\n" ) ),
            size );

        return NULL;
    }

    D3D9VertexShader* pShader = new D3D9VertexShader( pStaging, true );
    HELIUM_ASSERT( pShader );

    return pShader;
}

/// @copydoc Renderer::CreatePixelShader()
RPixelShader* D3D9Renderer::CreatePixelShader( size_t size, const void* pData )
{
    // Create the shader immediately if shader data was provided.
    if( pData )
    {
        // Verify that the shader data is DWORD-aligned since we need to cast it when passing it to the shader
        // creation function.
        HELIUM_ASSERT( ( reinterpret_cast< uintptr_t >( pData ) & ( sizeof( DWORD ) - 1 ) ) == 0 );

        HELIUM_ASSERT( m_pD3DDevice );

        IDirect3DPixelShader9* pD3DShader = NULL;
        HRESULT createResult = m_pD3DDevice->CreatePixelShader( static_cast< const DWORD* >( pData ), &pD3DShader );
        if( FAILED( createResult ) )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer::CreatePixelShader(): Pixel shader creation failed.\n" ) );

            return NULL;
        }

        D3D9PixelShader* pShader = new D3D9PixelShader( pD3DShader, false );
        HELIUM_ASSERT( pShader );

        pD3DShader->Release();

        return pShader;
    }

    // Allocate a staging buffer for deferred loading of the shader data.
    void* pStaging = DefaultAllocator().Allocate( size );
    if( !pStaging )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreatePixelShader(): Failed to allocate staging buffer of %" ) TPRIuSZ
            TXT( " bytes for loading.\n" ) ),
            size );

        return NULL;
    }

    D3D9PixelShader* pShader = new D3D9PixelShader( pStaging, true );
    HELIUM_ASSERT( pShader );

    return pShader;
}

/// @copydoc Renderer::CreateVertexBuffer()
RVertexBuffer* D3D9Renderer::CreateVertexBuffer( size_t size, ERendererBufferUsage usage, const void* pData )
{
    // Verify the size is valid for valid truncation due to casting to a UINT for the parameters passed to
    // CreateVertexBuffer().
    HELIUM_ASSERT( size <= UINT32_MAX );

    // Vertex and index buffers can only be created with static or dynamic usage semantics.
    HELIUM_ASSERT( usage == RENDERER_BUFFER_USAGE_STATIC || usage == RENDERER_BUFFER_USAGE_DYNAMIC );
    if( usage != RENDERER_BUFFER_USAGE_STATIC && usage != RENDERER_BUFFER_USAGE_DYNAMIC )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateVertexBuffer(): Vertex buffers can only be created with static or " )
            TXT( "dynamic usage semantics.\n" ) ) );

        return NULL;
    }

    bool bDynamic = ( usage == RENDERER_BUFFER_USAGE_DYNAMIC );

    DWORD d3dUsage = ( bDynamic ? D3DUSAGE_DYNAMIC : 0 );
    D3DPOOL pool = ( m_bExDevice || bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED );

    IDirect3DVertexBuffer9* pD3DBuffer = NULL;
    HRESULT result = m_pD3DDevice->CreateVertexBuffer(
        static_cast< UINT >( size ),
        d3dUsage,
        0,
        pool,
        &pD3DBuffer,
        NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer::CreateVertexBuffer(): Vertex buffer creation failed (result code: 0x%x).\n" ),
            result );

        return NULL;
    }

    if( pData )
    {
        void* pDestination = NULL;
        L_D3D9_VERIFY( pD3DBuffer->Lock( 0, static_cast< UINT >( size ), &pDestination, D3DLOCK_DISCARD ) );
        HELIUM_ASSERT( pDestination );
        MemoryCopy( pDestination, pData, size );
        L_D3D9_VERIFY( pD3DBuffer->Unlock() );
    }

    D3D9VertexBuffer* pBuffer;
    if( bDynamic && !m_bExDevice )
    {
        D3D9DynamicVertexBuffer* pDynamicBuffer = new D3D9DynamicVertexBuffer( pD3DBuffer );
        HELIUM_ASSERT( pDynamicBuffer );
        RegisterDeviceResetListener( pDynamicBuffer );

        pBuffer = pDynamicBuffer;
    }
    else
    {
        pBuffer = new D3D9VertexBuffer( pD3DBuffer );
    }

    HELIUM_ASSERT( pBuffer );

    pD3DBuffer->Release();

    return pBuffer;
}

/// @copydoc Renderer::CreateIndexBuffer()
RIndexBuffer* D3D9Renderer::CreateIndexBuffer(
    size_t size,
    ERendererBufferUsage usage,
    ERendererIndexFormat format,
    const void* pData )
{
    // Verify the size is valid for valid truncation due to casting to a UINT for the parameters passed to
    // CreateVertexBuffer().
    HELIUM_ASSERT( size <= UINT32_MAX );

    // Vertex and index buffers can only be created with static or dynamic usage semantics.
    HELIUM_ASSERT( usage == RENDERER_BUFFER_USAGE_STATIC || usage == RENDERER_BUFFER_USAGE_DYNAMIC );
    if( usage != RENDERER_BUFFER_USAGE_STATIC && usage != RENDERER_BUFFER_USAGE_DYNAMIC )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateIndexBuffer(): Index buffers can only be created with static or dynamic " )
              TXT( "usage semantics.\n" ) ) );

        return NULL;
    }

    bool bDynamic = ( usage == RENDERER_BUFFER_USAGE_DYNAMIC );

    DWORD d3dUsage = ( bDynamic ? D3DUSAGE_DYNAMIC : 0 );
    D3DFORMAT d3dFormat = ( format == RENDERER_INDEX_FORMAT_UINT32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16 );
    D3DPOOL pool = ( m_bExDevice || bDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED );

    IDirect3DIndexBuffer9* pD3DBuffer = NULL;
    HRESULT result = m_pD3DDevice->CreateIndexBuffer(
        static_cast< UINT >( size ),
        d3dUsage,
        d3dFormat,
        pool,
        &pD3DBuffer,
        NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer::CreateIndexBuffer(): Index buffer creation failed (result code: 0x%x).\n" ),
            result );

        return NULL;
    }

    if( pData )
    {
        void* pDestination = NULL;
        L_D3D9_VERIFY( pD3DBuffer->Lock(
            0,
            static_cast< UINT >( size ),
            &pDestination,
            ( bDynamic ? D3DLOCK_DISCARD : 0 ) ) );
        HELIUM_ASSERT( pDestination );
        MemoryCopy( pDestination, pData, size );
        L_D3D9_VERIFY( pD3DBuffer->Unlock() );
    }

    D3D9IndexBuffer* pBuffer;
    if( bDynamic && !m_bExDevice )
    {
        D3D9DynamicIndexBuffer* pDynamicBuffer = new D3D9DynamicIndexBuffer( pD3DBuffer );
        HELIUM_ASSERT( pDynamicBuffer );
        RegisterDeviceResetListener( pDynamicBuffer );

        pBuffer = pDynamicBuffer;
    }
    else
    {
        pBuffer = new D3D9IndexBuffer( pD3DBuffer );
    }

    HELIUM_ASSERT( pBuffer );

    pD3DBuffer->Release();

    return pBuffer;
}

/// @copydoc Renderer::CreateConstantBuffer()
RConstantBuffer* D3D9Renderer::CreateConstantBuffer(
    size_t size,
    ERendererBufferUsage /*usage*/,
    const void* pData )
{
    HELIUM_ASSERT( size != 0 );

    // Pad the buffer size to be a multiple of the size of a single float vector register.
    size_t actualSize = Align( size, sizeof( float32_t ) * 4 );

    // Compute the number of registers covered by the buffer and test for any possible overflow when casting to a
    // 16-bit integer.
    size_t registerCount = actualSize / ( sizeof( float32_t ) * 4 );
    if( registerCount > UINT16_MAX )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateConstantBuffer(): Buffer size (%" ) TPRIuSZ TXT( ") is larger than the " )
            TXT( "maximum size supported (%" ) TPRIuSZ TXT( ").\n" ) ),
            size,
            sizeof( float32_t ) * 4 * UINT16_MAX );

        return NULL;
    }

    // Allocate the buffer memory.
    void* pBufferMemory = DefaultAllocator().Allocate( actualSize );
    HELIUM_ASSERT( pBufferMemory );
    if( !pBufferMemory )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateConstantBuffer(): Failed to allocate %" ) TPRIuSZ TXT( " bytes for " )
            TXT( "constant buffer data.\n" ) ),
            actualSize );

        return NULL;
    }

    // Initialize the buffer contents if a data pointer was provided.
    if( pData )
    {
        MemoryCopy( pBufferMemory, pData, size );
    }

    // Create the buffer interface.
    D3D9ConstantBuffer* pBuffer = new D3D9ConstantBuffer( pBufferMemory, static_cast< uint16_t >( registerCount ) );
    HELIUM_ASSERT( pBuffer );

    return pBuffer;
}

/// @copydoc Renderer::CreateVertexDescription()
RVertexDescription* D3D9Renderer::CreateVertexDescription(
    const RVertexDescription::Element* pElements,
    size_t elementCount )
{
    HELIUM_ASSERT( pElements );
    HELIUM_ASSERT( elementCount != 0 );

    // Make sure we have vertex elements from which to create a description object.
    if( elementCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateVertexDescription(): Cannot create a vertex description with no " )
            TXT( "elements.\n" ) ) );

        return NULL;
    }

    // Build the list of Direct3D elements.
    static const BYTE d3dDataTypes[ RENDERER_VERTEX_DATA_TYPE_MAX ] =
    {
        D3DDECLTYPE_FLOAT2,     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_2
        D3DDECLTYPE_FLOAT3,     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_3
        D3DDECLTYPE_FLOAT4,     // RENDERER_VERTEX_DATA_TYPE_FLOAT32_4
        D3DDECLTYPE_UBYTE4N,    // RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM
        D3DDECLTYPE_UBYTE4,     // RENDERER_VERTEX_DATA_TYPE_UINT8_4
        D3DDECLTYPE_FLOAT16_2,  // RENDERER_VERTEX_DATA_TYPE_FLOAT16_2
        D3DDECLTYPE_FLOAT16_4,  // RENDERER_VERTEX_DATA_TYPE_FLOAT16_4
    };

    static const WORD d3dDataTypeSizes[ RENDERER_VERTEX_DATA_TYPE_MAX ] =
    {
        8,   // RENDERER_VERTEX_DATA_TYPE_FLOAT32_2
        12,  // RENDERER_VERTEX_DATA_TYPE_FLOAT32_3
        16,  // RENDERER_VERTEX_DATA_TYPE_FLOAT32_4
        4,   // RENDERER_VERTEX_DATA_TYPE_UINT8_4_NORM
        4,   // RENDERER_VERTEX_DATA_TYPE_UINT8_4
        4,   // RENDERER_VERTEX_DATA_TYPE_FLOAT16_2
        8,   // RENDERER_VERTEX_DATA_TYPE_FLOAT16_4
    };

    static const BYTE d3dUsages[ RENDERER_VERTEX_SEMANTIC_MAX ] =
    {
        D3DDECLUSAGE_POSITION,      // RENDERER_VERTEX_SEMANTIC_POSITION
        D3DDECLUSAGE_BLENDWEIGHT,   // RENDERER_VERTEX_SEMANTIC_BLENDWEIGHT
        D3DDECLUSAGE_BLENDINDICES,  // RENDERER_VERTEX_SEMANTIC_BLENDINDICES
        D3DDECLUSAGE_NORMAL,        // RENDERER_VERTEX_SEMANTIC_NORMAL
        D3DDECLUSAGE_PSIZE,         // RENDERER_VERTEX_SEMANTIC_PSIZE
        D3DDECLUSAGE_TEXCOORD,      // RENDERER_VERTEX_SEMANTIC_TEXCOORD
        D3DDECLUSAGE_TANGENT,       // RENDERER_VERTEX_SEMANTIC_TANGENT
        D3DDECLUSAGE_BINORMAL,      // RENDERER_VERTEX_SEMANTIC_BINORMAL
        D3DDECLUSAGE_COLOR          // RENDERER_VERTEX_SEMANTIC_COLOR
    };

    DynArray< WORD > d3dStreamOffsets;

    D3DVERTEXELEMENT9 d3dCurrentElement;

    DynArray< D3DVERTEXELEMENT9 > d3dElements;
    for( size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex )
    {
        const RVertexDescription::Element& rElement = pElements[ elementIndex ];
        HELIUM_ASSERT( static_cast< size_t >( rElement.type ) < static_cast< size_t >( RENDERER_VERTEX_DATA_TYPE_MAX ) );
        HELIUM_ASSERT(
            static_cast< size_t >( rElement.semantic ) < static_cast< size_t >( RENDERER_VERTEX_SEMANTIC_MAX ) );

        if( rElement.bufferIndex >= d3dStreamOffsets.GetSize() )
        {
            d3dStreamOffsets.Add( 0, rElement.bufferIndex - d3dStreamOffsets.GetSize() + 1 );
        }

        d3dCurrentElement.Stream = rElement.bufferIndex;

        d3dCurrentElement.Offset = d3dStreamOffsets[ rElement.bufferIndex ];
        d3dStreamOffsets[ rElement.bufferIndex ] += d3dDataTypeSizes[ rElement.type ];

        d3dCurrentElement.Type = d3dDataTypes[ rElement.type ];

        d3dCurrentElement.Method = D3DDECLMETHOD_DEFAULT;

        d3dCurrentElement.Usage = d3dUsages[ rElement.semantic ];
        d3dCurrentElement.UsageIndex = rElement.semanticIndex;

        d3dElements.Push( d3dCurrentElement );
    }

    D3DVERTEXELEMENT9 d3dEndElement = D3DDECL_END();
    d3dElements.Push( d3dEndElement );

    IDirect3DVertexDeclaration9* pD3DDeclaration = NULL;
    HRESULT result = m_pD3DDevice->CreateVertexDeclaration( d3dElements.GetData(), &pD3DDeclaration );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateVertexInputDescription(): Failed to create vertex declaration (error " )
            TXT( "code: 0x%x).\n" ) ),
            result );

        return NULL;
    }

    D3D9VertexDescription* pDescription = new D3D9VertexDescription( pD3DDeclaration );
    HELIUM_ASSERT( pDescription );

    pD3DDeclaration->Release();

    return pDescription;
}

/// @copydoc Renderer::CreateVertexInputLayout()
RVertexInputLayout* D3D9Renderer::CreateVertexInputLayout(
    RVertexDescription* pDescription,
    RVertexShader* /*pShader*/ )
{
    HELIUM_ASSERT( pDescription );

    IDirect3DVertexDeclaration9* pD3DDeclaration =
        static_cast< D3D9VertexDescription* >( pDescription )->GetD3DDeclaration();
    HELIUM_ASSERT( pD3DDeclaration );

    D3D9VertexInputLayout* pInputLayout = new D3D9VertexInputLayout( pD3DDeclaration );
    HELIUM_ASSERT( pInputLayout );

    return pInputLayout;
}

/// @copydoc Renderer::CreateTexture2d()
RTexture2d* D3D9Renderer::CreateTexture2d(
    uint32_t width,
    uint32_t height,
    uint32_t mipCount,
    ERendererPixelFormat format,
    ERendererBufferUsage usage,
    const RTexture2d::CreateData* pData )
{
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );
    HELIUM_ASSERT( static_cast< size_t >( usage ) < static_cast< size_t >( RENDERER_BUFFER_USAGE_MAX ) );

    // Attempt to create the Direct3D texture resource.
    static const DWORD d3dUsages[] =
    {
        0,                      // RENDERER_BUFFER_USAGE_STATIC
        D3DUSAGE_DYNAMIC,       // RENDERER_BUFFER_USAGE_DYNAMIC
        D3DUSAGE_RENDERTARGET,  // RENDERER_BUFFER_USAGE_RENDER_TARGET
        D3DUSAGE_DEPTHSTENCIL   // RENDERER_BUFFER_USAGE_DEPTH_STENCIL
    };

    HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( d3dUsages ) == RENDERER_BUFFER_USAGE_MAX );

    D3DPOOL d3dPool = ( m_bExDevice || usage != RENDERER_BUFFER_USAGE_STATIC ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED );

    IDirect3DTexture9* pD3DTexture = NULL;
    HRESULT result = m_pD3DDevice->CreateTexture(
        width,
        height,
        mipCount,
        d3dUsages[ usage ],
        PixelFormatToD3DFormat( format ),
        d3dPool,
        &pD3DTexture,
        NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "D3D9Renderer::CreateTexture2d(): Failed to create 2D texture (error code: 0x%x).\n" ),
            result );

        return NULL;
    }

    // Initialize the texture if an initial set of data was specified.
    if( pData )
    {
        uint32_t mipHeight = height;
        D3DLOCKED_RECT lockedRect;

        for( uint32_t mipIndex = 0; mipIndex < mipCount; ++mipIndex )
        {
            const RTexture2d::CreateData& rCreateData = pData[ mipIndex ];
            const uint8_t* pSourceRow = static_cast< const uint8_t* >( rCreateData.pData );
            HELIUM_ASSERT( pSourceRow );
            size_t sourcePitch = rCreateData.pitch;

            L_D3D9_VERIFY( pD3DTexture->LockRect( mipIndex, &lockedRect, NULL, 0 ) );
            uint8_t* pDestRow = static_cast< uint8_t* >( lockedRect.pBits );
            HELIUM_ASSERT( pDestRow );
            size_t destPitch = static_cast< size_t >( lockedRect.Pitch );

            size_t copyPitch = Min( sourcePitch, destPitch );

            uint_fast32_t blockRowCount = RendererUtil::PixelToBlockRowCount( mipHeight, format );
            for( uint_fast32_t rowIndex = 0; rowIndex < blockRowCount; ++rowIndex )
            {
                MemoryCopy( pDestRow, pSourceRow, copyPitch );
                pSourceRow += sourcePitch;
                pDestRow += destPitch;
            }

            L_D3D9_VERIFY( pD3DTexture->UnlockRect( mipIndex ) );

            mipHeight = ( mipHeight + 1 ) / 2;
        }
    }

    // Create the appropriate renderer resource wrapper.
    bool bSrgb = RendererUtil::IsSrgbPixelFormat( format );

    D3D9Texture2d* pTexture;
    if( d3dPool == D3DPOOL_DEFAULT && usage == RENDERER_BUFFER_USAGE_STATIC )
    {
        HELIUM_ASSERT( m_bExDevice );  // Combination should only occur with Direct3D 9Ex.

        // Static textures in the default pool cannot be locked directly, so use a texture type with support for
        // copying over the texture data from a staging area.
        pTexture = new D3D9StaticTexture2d( pD3DTexture, bSrgb );
    }
    else if( d3dPool == D3DPOOL_DEFAULT && !m_bExDevice )
    {
        // Textures in the default pool need to be recreated on device reset when not using Direct3D 9Ex.
        D3D9DynamicTexture2d* pDynamicTexture = new D3D9DynamicTexture2d( pD3DTexture, bSrgb );
        HELIUM_ASSERT( pDynamicTexture );
        RegisterDeviceResetListener( pDynamicTexture );

        pTexture = pDynamicTexture;
    }
    else
    {
        pTexture = new D3D9Texture2d( pD3DTexture, bSrgb );
    }

    HELIUM_ASSERT( pTexture );

    pD3DTexture->Release();

    return pTexture;
}

/// @copydoc Renderer::CreateFence()
RFence* D3D9Renderer::CreateFence()
{
    IDirect3DQuery9* pD3DQuery = NULL;
    HRESULT createResult = m_pD3DDevice->CreateQuery( D3DQUERYTYPE_EVENT, &pD3DQuery );
    HELIUM_ASSERT( SUCCEEDED( createResult ) );
    if( FAILED( createResult ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::CreateFence(): Failed to create Direct3D event query instance (error code: " )
              TXT( "0x%x).\n" ) ),
            createResult );

        return NULL;
    }

    D3D9Fence* pFence = new D3D9Fence( pD3DQuery );
    HELIUM_ASSERT( pFence );

    RegisterDeviceResetListener( pFence );

    pD3DQuery->Release();

    return pFence;
}

/// @copydoc Renderer::SyncFence()
void D3D9Renderer::SyncFence( RFence* pFence )
{
    HELIUM_ASSERT( pFence );

    IDirect3DQuery9* pD3DQuery = static_cast< D3D9Fence* >( pFence )->GetQuery();
    if( !pD3DQuery )
    {
        // Direct3D query interface was released due to a device reset, so there is nothing to sync.
        return;
    }

    for( ; ; )
    {
        HRESULT syncResult = pD3DQuery->GetData( NULL, 0, D3DGETDATA_FLUSH );
        if( syncResult != S_FALSE )
        {
            if( syncResult == D3DERR_DEVICELOST )
            {
                HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer::SyncFence(): Device lost, aborting sync.\n" ) );
            }

            return;
        }

        m_pD3DDevice->SetTexture( 0, NULL );
        m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, 0, 0, 0, 0, 0 );

        Thread::Yield();
    }
}

/// @copydoc Renderer::TrySyncFence()
bool D3D9Renderer::TrySyncFence( RFence* pFence )
{
    HELIUM_ASSERT( pFence );

    IDirect3DQuery9* pD3DQuery = static_cast< D3D9Fence* >( pFence )->GetQuery();
    if( !pD3DQuery )
    {
        // Direct3D query interface was released due to a device reset, so there is nothing to sync.
        return true;
    }

    HRESULT syncResult = pD3DQuery->GetData( NULL, 0, D3DGETDATA_FLUSH );
    if( syncResult == D3DERR_DEVICELOST )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9Renderer::TrySyncFence(): Device lost.\n" ) );
    }

    return ( syncResult != S_FALSE );
}

/// @copydoc Renderer::GetImmediateCommandProxy()
RRenderCommandProxy* D3D9Renderer::GetImmediateCommandProxy()
{
    return m_spImmediateCommandProxy;
}

/// @copydoc Renderer::CreateDeferredCommandProxy()
RRenderCommandProxy* D3D9Renderer::CreateDeferredCommandProxy()
{
    D3D9DeferredCommandProxy* pCommandProxy = new D3D9DeferredCommandProxy;
    HELIUM_ASSERT( pCommandProxy );

    return pCommandProxy;
}

/// @copydoc Renderer::Flush()
void D3D9Renderer::Flush()
{
    RFencePtr spFence = CreateFence();
    HELIUM_ASSERT( spFence );
    SyncFence( spFence );
}

/// Notify this renderer that a call on another resource has signaled that we have lost the device.
void D3D9Renderer::NotifyLost()
{
    m_bLost = true;
}

/// Register an object as a listener for Direct3D device reset events.
///
/// This should only be called by D3D9Renderer when creating resources that need to respond to device resets.
///
/// @param[in] pListener  Listener to register.
///
/// @see UnregisterDeviceResetListener()
void D3D9Renderer::RegisterDeviceResetListener( D3D9DeviceResetListener* pListener )
{
    HELIUM_ASSERT( pListener );
    HELIUM_ASSERT( !pListener->m_pPreviousListener );
    HELIUM_ASSERT( !pListener->m_pNextListener );

    D3D9DeviceResetListener* pHeadListener = m_pDeviceResetListenerHead;
    if( pHeadListener )
    {
        HELIUM_ASSERT( !pHeadListener->m_pPreviousListener );
        pHeadListener->m_pPreviousListener = pListener;
    }

    pListener->m_pNextListener = pHeadListener;

    m_pDeviceResetListenerHead = pListener;
}

/// Unregister an object as a listener for Direct3D device reset events.
///
/// This is automatically called by the D3D9DeviceResetListener destructor and should not be called elsewhere.
///
/// @param[in] pListener  Listener to unregister.
///
/// @see RegisterDeviceResetListener()
void D3D9Renderer::UnregisterDeviceResetListener( D3D9DeviceResetListener* pListener )
{
    HELIUM_ASSERT( pListener );
    D3D9DeviceResetListener* pPreviousListener = pListener->m_pPreviousListener;
    D3D9DeviceResetListener* pNextListener = pListener->m_pNextListener;
    if( pPreviousListener )
    {
        pPreviousListener->m_pNextListener = pNextListener;
    }
    else if( m_pDeviceResetListenerHead == pListener )
    {
        m_pDeviceResetListenerHead = pNextListener;
    }

    if( pNextListener )
    {
        pNextListener->m_pPreviousListener = pPreviousListener;
    }
}

/// Acquire a dynamic texture for use as a temporary staging area for mapped static texture loads from the pool if
/// the texture properties are supported by the texture pool.
///
/// @param[in]  width           Static texture width.
/// @param[in]  height          Static texture height.
/// @param[in]  format          Texture format.
/// @param[out] rStartMipLevel  Mip level in the provided texture to use as the staging area for the base mip level.
///
/// @return  Pointer to a dynamic texture to use for mapping if one could be retrieved, or null if the texture
///          properties are not compatible with the pooled textures.
///
/// @see ReleasePooledStaticTextureMapTarget()
IDirect3DTexture9* D3D9Renderer::GetPooledStaticTextureMapTarget(
    uint32_t width,
    uint32_t height,
    ERendererPixelFormat format,
    uint32_t& rStartMipLevel )
{
    HELIUM_ASSERT( width != 0 );
    HELIUM_ASSERT( height != 0 );

    // Only allow supported formats.
    if( static_cast< size_t >( format ) >= static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) )
    {
        return NULL;
    }

    // Only power-of-two textures whose dimensions are less than or equal to 2 ^ STATIC_TEXTURE_MAP_TARGET_LOG2_MAX
    // are pooled.
    if( !IsPowerOfTwo( width ) || !IsPowerOfTwo( height ) )
    {
        return NULL;
    }

    size_t widthLog2 = Log2( width );
    if( widthLog2 > STATIC_TEXTURE_MAP_TARGET_LOG2_MAX )
    {
        return NULL;
    }

    size_t heightLog2 = Log2( height );
    if( heightLog2 > STATIC_TEXTURE_MAP_TARGET_LOG2_MAX )
    {
        return NULL;
    }

    // Get information about the texture pool to use.
    size_t poolIndex;
    uint32_t poolTextureWidth;
    uint32_t poolTextureHeight;
    GetStaticTextureMapTargetPool(
        widthLog2,
        heightLog2,
        poolIndex,
        poolTextureWidth,
        poolTextureHeight,
        rStartMipLevel );

    DynArray< IDirect3DTexture9* >& rTexturePool = m_staticTextureMapTargetPools[ poolIndex ][ format ];

    // Use a texture from the pool if the pool is not empty.
    if( !rTexturePool.IsEmpty() )
    {
        IDirect3DTexture9* pTexture = rTexturePool.GetLast();
        HELIUM_ASSERT( pTexture );
        rTexturePool.Pop();

        return pTexture;
    }

    // Allocate a new texture of the necessary size and format.
    D3DFORMAT d3dFormat = PixelFormatToD3DFormat( format );

    IDirect3DTexture9* pTexture;
    HRESULT result = m_pD3DDevice->CreateTexture(
        poolTextureWidth,
        poolTextureHeight,
        0,
        D3DUSAGE_DYNAMIC,
        d3dFormat,
        D3DPOOL_SYSTEMMEM,
        &pTexture,
        NULL );
    if( FAILED( result ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "D3D9Renderer::GetPooledStaticTextureMapTarget(): Failed to create pool texture (width: %" )
            TPRIu32 TXT( "; height: %" ) TPRIu32 TXT( "; format: %x).\n" ) ),
            poolTextureWidth,
            poolTextureHeight,
            static_cast< unsigned int >( d3dFormat ) );

        return NULL;
    }

    HELIUM_ASSERT( pTexture );

    return pTexture;
}

/// Release a static texture map target back to the appropriate pool.
///
/// The provided texture must have been previously retrieved using GetPooledStaticTextureMapTarget().
///
/// @param[in] pTexture  Texture to release.
/// @param[in] bSrgb     True if the texture is in sRGB color space.
///
/// @see GetPooledStaticTextureMapTarget()
void D3D9Renderer::ReleasePooledStaticTextureMapTarget( IDirect3DTexture9* pTexture, bool bSrgb )
{
    HELIUM_ASSERT( pTexture );

    // Determine the pool used based on the texture dimensions and format.
    D3DSURFACE_DESC surfaceDesc;
    L_D3D9_VERIFY( pTexture->GetLevelDesc( 0, &surfaceDesc ) );
    HELIUM_ASSERT( IsPowerOfTwo( surfaceDesc.Width ) );
    HELIUM_ASSERT( IsPowerOfTwo( surfaceDesc.Height ) );

    size_t widthLog2 = Log2( surfaceDesc.Width );
    size_t heightLog2 = Log2( surfaceDesc.Height );
    HELIUM_ASSERT( widthLog2 <= STATIC_TEXTURE_MAP_TARGET_LOG2_MAX );
    HELIUM_ASSERT( heightLog2 <= STATIC_TEXTURE_MAP_TARGET_LOG2_MAX );

    size_t poolIndex;
    uint32_t poolTextureWidth;
    uint32_t poolTextureHeight;
    uint32_t startMipLevel;
    GetStaticTextureMapTargetPool(
        widthLog2,
        heightLog2,
        poolIndex,
        poolTextureWidth,
        poolTextureHeight,
        startMipLevel );

    ERendererPixelFormat format = D3DFormatToPixelFormat( surfaceDesc.Format, bSrgb );
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );

    DynArray< IDirect3DTexture9* >& rTexturePool = m_staticTextureMapTargetPools[ poolIndex ][ format ];

    // Return the texture to the pool.
    rTexturePool.Push( pTexture );
}

/// Get the Direct3D format identifier for the specified pixel format.
///
/// @param[in] format  Pixel format.
///
/// @return  Direct3D surface format.
///
/// @see D3DFormatToPixelFormat()
D3DFORMAT D3D9Renderer::PixelFormatToD3DFormat( ERendererPixelFormat format ) const
{
    HELIUM_ASSERT( static_cast< size_t >( format ) < static_cast< size_t >( RENDERER_PIXEL_FORMAT_MAX ) );

    // Handle depth formats manually.
    if( format == RENDERER_PIXEL_FORMAT_DEPTH )
    {
        return m_depthTextureFormat;
    }

    // Convert the format to the corresponding Direct3D format.
    static const D3DFORMAT d3dFormats[] =
    {
        D3DFMT_A8B8G8R8,       // RENDERER_PIXEL_FORMAT_R8G8B8A8
        D3DFMT_A8R8G8B8,       // RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB
        D3DFMT_L8,             // RENDERER_PIXEL_FORMAT_R8
        D3DFMT_DXT1,           // RENDERER_PIXEL_FORMAT_BC1
        D3DFMT_DXT1,           // RENDERER_PIXEL_FORMAT_BC1_SRGB
        D3DFMT_DXT3,           // RENDERER_PIXEL_FORMAT_BC2
        D3DFMT_DXT3,           // RENDERER_PIXEL_FORMAT_BC2_SRGB
        D3DFMT_DXT5,           // RENDERER_PIXEL_FORMAT_BC3
        D3DFMT_DXT5,           // RENDERER_PIXEL_FORMAT_BC3_SRGB
        D3DFMT_A16B16G16R16F,  // RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT
        D3DFMT_UNKNOWN         // RENDERER_PIXEL_FORMAT_DEPTH (dummy entry; depth formats handled manually)
    };

    HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( d3dFormats ) == RENDERER_PIXEL_FORMAT_MAX );

    return d3dFormats[ format ];
}

/// Create the static renderer instance as a D3D9Renderer.
///
/// @return  True if the renderer was created successfully, false if not or another renderer instance already
///          exists.
bool D3D9Renderer::CreateStaticInstance()
{
    if( sm_pInstance )
    {
        return false;
    }

    sm_pInstance = new D3D9Renderer;
    HELIUM_ASSERT( sm_pInstance );

    return ( sm_pInstance != NULL );
}

/// Fill out a D3DPRESENT_PARAMETERS structure from values provided by a Renderer::ContextInitParameters structure for
/// initializing a render context.
///
/// @param[in]  rContextInitParameters  Render context initialization parameters.
/// @param[out] rParameters             Direct3D presentation parameters.
/// @param[out] rFullscreenDisplayMode  Fullscreen display mode parameters (fullscreen modes for Direct3D 9Ex devices
///                                     only.
///
/// @return  True if a valid display was found, false if not.  Note that all windowed display modes will be mapped
///          succesfully.
bool D3D9Renderer::GetPresentParameters(
    const ContextInitParameters& rContextInitParameters,
    D3DPRESENT_PARAMETERS& rParameters,
    D3DDISPLAYMODEEX& rFullscreenDisplayMode ) const
{
    // Enumerate full-screen modes, comparing with the requested mode to find a valid match.
    UINT fullscreenRefreshRate = 0;

    if( rContextInitParameters.bFullscreen )
    {
        UINT bestModeIndex;
        SetInvalid( bestModeIndex );

        if( m_bExDevice )
        {
            // Check progressive-scan modes first, then interlaced.
            D3DDISPLAYMODEEX mode;
            MemoryZero( &mode, sizeof( mode ) );
            mode.Size = sizeof( mode );

            D3DDISPLAYMODEFILTER modeFilter;
            MemoryZero( &modeFilter, sizeof( modeFilter ) );
            modeFilter.Size = sizeof( modeFilter );
            modeFilter.Format = D3DFMT_X8R8G8B8;
            modeFilter.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;

            UINT modeCount = static_cast< IDirect3D9Ex* >( m_pD3D )->GetAdapterModeCountEx(
                D3DADAPTER_DEFAULT,
                &modeFilter );
            for( UINT modeIndex = 0; modeIndex < modeCount; ++modeIndex )
            {
                L_D3D9_VERIFY( static_cast< IDirect3D9Ex* >( m_pD3D )->EnumAdapterModesEx(
                    D3DADAPTER_DEFAULT,
                    &modeFilter,
                    modeIndex,
                    &mode ) );
                if( mode.Width == rContextInitParameters.displayWidth &&
                    mode.Height == rContextInitParameters.displayHeight )
                {
                    UINT refreshRate = mode.RefreshRate;
                    if( IsInvalid( bestModeIndex ) || refreshRate > fullscreenRefreshRate )
                    {
                        bestModeIndex = modeIndex;
                        fullscreenRefreshRate = refreshRate;
                        rFullscreenDisplayMode = mode;
                    }
                }
            }

            if( IsInvalid( bestModeIndex ) )
            {
                modeFilter.ScanLineOrdering = D3DSCANLINEORDERING_INTERLACED;

                modeCount = static_cast< IDirect3D9Ex* >( m_pD3D )->GetAdapterModeCountEx(
                    D3DADAPTER_DEFAULT,
                    &modeFilter );
                for( UINT modeIndex = 0; modeIndex < modeCount; ++modeIndex )
                {
                    L_D3D9_VERIFY( static_cast< IDirect3D9Ex* >( m_pD3D )->EnumAdapterModesEx(
                        D3DADAPTER_DEFAULT,
                        &modeFilter,
                        modeIndex,
                        &mode ) );
                    if( mode.Width == rContextInitParameters.displayWidth &&
                        mode.Height == rContextInitParameters.displayHeight )
                    {
                        UINT refreshRate = mode.RefreshRate;
                        if( IsInvalid( bestModeIndex ) || refreshRate > fullscreenRefreshRate )
                        {
                            bestModeIndex = modeIndex;
                            fullscreenRefreshRate = refreshRate;
                            rFullscreenDisplayMode = mode;
                        }
                    }
                }
            }
        }
        else
        {
            D3DDISPLAYMODE mode;
            MemoryZero( &mode, sizeof( mode ) );

            UINT modeCount = m_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8 );
            for( UINT modeIndex = 0; modeIndex < modeCount; ++modeIndex )
            {
                L_D3D9_VERIFY( m_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, modeIndex, &mode ) );
                if( mode.Width == rContextInitParameters.displayWidth &&
                    mode.Height == rContextInitParameters.displayHeight )
                {
                    UINT refreshRate = mode.RefreshRate;
                    if( IsInvalid( bestModeIndex ) || refreshRate > fullscreenRefreshRate )
                    {
                        bestModeIndex = modeIndex;
                        fullscreenRefreshRate = refreshRate;
                    }
                }
            }
        }

        if( IsInvalid( bestModeIndex ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "D3D9Renderer: Failed to locate valid fullscreen display mode for resolution %" ) TPRIu32
                  TXT( "x%" ) TPRIu32 TXT( ".\n" ) ),
                rContextInitParameters.displayWidth,
                rContextInitParameters.displayHeight );

            return false;
        }
    }

    // Build the presentation parameters structure information.
    MemoryZero( &rParameters, sizeof( rParameters ) );

    rParameters.BackBufferWidth = rContextInitParameters.displayWidth;
    rParameters.BackBufferHeight = rContextInitParameters.displayHeight;
    rParameters.BackBufferFormat = D3DFMT_X8R8G8B8;
    rParameters.BackBufferCount = 1;

    rParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
    if( rContextInitParameters.multisampleCount > 1 )
    {
        uint32_t multisampleCount = rContextInitParameters.multisampleCount;
        if( multisampleCount > 16 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                TXT( "D3D9Renderer: Multisample count cannot be more than 16.  Value will be clamped.\n" ) );
            multisampleCount = 16;
        }

        D3DMULTISAMPLE_TYPE multisampleType = static_cast< D3DMULTISAMPLE_TYPE >(
            static_cast< uint32_t >( D3DMULTISAMPLE_2_SAMPLES ) + multisampleCount - 2 );

        HRESULT checkResult = m_pD3D->CheckDeviceMultiSampleType(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            D3DFMT_X8R8G8B8,
            !rContextInitParameters.bFullscreen,
            multisampleType,
            NULL );
        if( FAILED( checkResult ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "D3D9Renderer: Multisample count of %u is not supported.  Disabling multisampling.\n" ),
                multisampleCount );
        }
        else
        {
            rParameters.MultiSampleType = multisampleType;
        }
    }

    rParameters.MultiSampleQuality = 0;

    rParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    rParameters.hDeviceWindow = static_cast< HWND >( rContextInitParameters.pWindow );
    rParameters.Windowed = !rContextInitParameters.bFullscreen;
    rParameters.EnableAutoDepthStencil = FALSE;
    rParameters.Flags = 0;
    rParameters.FullScreen_RefreshRateInHz = fullscreenRefreshRate;
    rParameters.PresentationInterval =
        rContextInitParameters.bVsync ? D3DPRESENT_INTERVAL_ONE : D3DPRESENT_INTERVAL_IMMEDIATE;

    return true;
}

/// Get the pixel format identifier for the specified Direct3D format.
///
/// @param[in] d3dFormat  Direct3D surface format.
/// @param[in] bSrgb      True if the pixel format is used in sRGB color space, false if it is in linear color
///                       space.
///
/// @return  Pixel format.
///
/// @see PixelFormatToD3DFormat()
ERendererPixelFormat D3D9Renderer::D3DFormatToPixelFormat( D3DFORMAT d3dFormat, bool bSrgb )
{
    switch( static_cast< DWORD >( d3dFormat ) )
    {
    case D3DFMT_A8B8G8R8:
        {
            return ( bSrgb ? RENDERER_PIXEL_FORMAT_R8G8B8A8_SRGB : RENDERER_PIXEL_FORMAT_R8G8B8A8 );
        }

    case D3DFMT_L8:
        {
            return RENDERER_PIXEL_FORMAT_R8;
        }

    case D3DFMT_DXT1:
        {
            return ( bSrgb ? RENDERER_PIXEL_FORMAT_BC1_SRGB : RENDERER_PIXEL_FORMAT_BC1 );
        }

    case D3DFMT_DXT3:
        {
            return ( bSrgb ? RENDERER_PIXEL_FORMAT_BC2_SRGB : RENDERER_PIXEL_FORMAT_BC2 );
        }

    case D3DFMT_DXT5:
        {
            return ( bSrgb ? RENDERER_PIXEL_FORMAT_BC3_SRGB : RENDERER_PIXEL_FORMAT_BC3 );
        }

    case D3DFMT_A16B16G16R16F:
        {
            return RENDERER_PIXEL_FORMAT_R16G16B16A16_FLOAT;
        }

    case D3DFMT_D24X8:
    case D3DFMT_D16:
    case MAKEFOURCC( 'D', 'F', '2', '4' ):
    case MAKEFOURCC( 'D', 'F', '1', '6' ):
        {
            return RENDERER_PIXEL_FORMAT_DEPTH;
        }
    }

    return RENDERER_PIXEL_FORMAT_INVALID;
}

/// Reset the device with the specified parameters.
///
/// @param[in] rPresentParameters      Direct3D presentation parameters.
/// @param[in] rFullscreenDisplayMode  Fullscreen display mode parameters.  This is ignored if not using a fullscreen
///                                    display mode with the Direct3D 9Ex interface.
///
/// @return  Direct3D result from resetting the device.
HRESULT D3D9Renderer::ResetDevice( D3DPRESENT_PARAMETERS& rPresentParameters, D3DDISPLAYMODEEX& rFullscreenDisplayMode )
{
    HELIUM_ASSERT( m_spMainContext );
    m_spMainContext->ReleaseBackBufferSurface();

    HELIUM_ASSERT( m_pD3DDevice );
    HRESULT resetResult;
    if( m_bExDevice )
    {
        resetResult = static_cast< IDirect3DDevice9Ex* >( m_pD3DDevice )->ResetEx(
            &rPresentParameters,
            ( rPresentParameters.Windowed ? NULL : &rFullscreenDisplayMode ) );
    }
    else
    {
        // Device resets without Direct3D 9Ex require certain resources to be reallocated, so prepare those resources
        // before resetting and restore them afterward.
        for( D3D9DeviceResetListener* pListener = m_pDeviceResetListenerHead;
             pListener != NULL;
             pListener = pListener->m_pNextListener )
        {
            pListener->OnPreReset();
        }

        resetResult = m_pD3DDevice->Reset( &rPresentParameters );

        for( D3D9DeviceResetListener* pListener = m_pDeviceResetListenerHead;
             pListener != NULL;
             pListener = pListener->m_pNextListener )
        {
            pListener->OnPostReset( this );
        }
    }

    m_bLost = false;

    if( resetResult == D3DERR_DEVICELOST )
    {
        NotifyLost();
    }

    return resetResult;
}

/// Get information about the static texture map target pool for a given texture size.
///
/// @param[in]  widthLog2           Base-2 logarithm of the texture width.
/// @param[in]  heightLog2          Base-2 logarithm of the texture height.
/// @param[out] rPoolIndex          Index of the pool to use.
/// @param[out] rPoolTextureWidth   Width of textures in the pool.
/// @param[out] rPoolTextureHeight  Height of textures in the pool.
/// @param[out] rStartMipLevel      Mip level of textures in the pool to use as the staging area for the base mip
///                                 level.
void D3D9Renderer::GetStaticTextureMapTargetPool(
    size_t widthLog2,
    size_t heightLog2,
    size_t& rPoolIndex,
    uint32_t& rPoolTextureWidth,
    uint32_t& rPoolTextureHeight,
    uint32_t& rStartMipLevel )
{
    HELIUM_ASSERT( widthLog2 <= STATIC_TEXTURE_MAP_TARGET_LOG2_MAX );
    HELIUM_ASSERT( heightLog2 <= STATIC_TEXTURE_MAP_TARGET_LOG2_MAX );

    // Compute the actual size of the pool texture to use for the specified texture dimensions (smaller textures
    // will use lower-level mips of a larger texture).
    size_t maxDimensionLog2 = Max( widthLog2, heightLog2 );

    rStartMipLevel = static_cast< uint32_t >( STATIC_TEXTURE_MAP_TARGET_LOG2_MAX - maxDimensionLog2 );

    size_t biasedWidthLog2 = widthLog2 + rStartMipLevel;
    size_t biasedHeightLog2 = heightLog2 + rStartMipLevel;

    // If the pooled texture width is less than or equal to the height, use the lower set of texture pools.
    rPoolIndex =
        ( biasedWidthLog2 <= biasedHeightLog2
        ? biasedWidthLog2
        : STATIC_TEXTURE_MAP_TARGET_LOG2_MAX + biasedHeightLog2 );

    // Compute the actual pool texture width and height.
    rPoolTextureWidth = ( 1U << biasedWidthLog2 );
    rPoolTextureHeight = ( 1U << biasedHeightLog2 );
}
