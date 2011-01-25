#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DynamicTexture2d.h"

#include "RenderingD3D9/D3D9Surface.h"

using namespace Lunar;

/// Constructor.
D3D9DynamicTexture2d::D3D9DynamicTexture2d( IDirect3DTexture9* pD3DTexture, bool bSrgb )
    : D3D9Texture2d( pD3DTexture, bSrgb )
{
    HELIUM_ASSERT( pD3DTexture );

    uint32_t mipLevelCount = pD3DTexture->GetLevelCount();
    HELIUM_ASSERT( mipLevelCount > 0 );
    HELIUM_ASSERT( mipLevelCount <= LEVEL_COUNT_MAX );

    D3DSURFACE_DESC surfaceDesc;
    L_D3D9_VERIFY( pD3DTexture->GetLevelDesc( 0, &surfaceDesc ) );

    HELIUM_ASSERT(
        surfaceDesc.Usage == D3DUSAGE_DYNAMIC ||
        surfaceDesc.Usage == D3DUSAGE_DEPTHSTENCIL ||
        surfaceDesc.Usage == D3DUSAGE_RENDERTARGET );
    HELIUM_ASSERT( surfaceDesc.Pool == D3DPOOL_DEFAULT );
    HELIUM_ASSERT( surfaceDesc.MultiSampleType == 0 );
    HELIUM_ASSERT( surfaceDesc.MultiSampleQuality == 0 );

    ERendererBufferUsage usage = RENDERER_BUFFER_USAGE_DYNAMIC;
    if( surfaceDesc.Usage == D3DUSAGE_DEPTHSTENCIL )
    {
        usage = RENDERER_BUFFER_USAGE_DEPTH_STENCIL;
    }
    else if( surfaceDesc.Usage == D3DUSAGE_RENDERTARGET )
    {
        usage = RENDERER_BUFFER_USAGE_RENDER_TARGET;
    }

    m_width = surfaceDesc.Width;
    m_height = surfaceDesc.Height;
    m_format = surfaceDesc.Format;
    m_mipLevelCountMinusOne = mipLevelCount - 1;
    m_usage = static_cast< uint32_t >( usage );

    // Make sure the mip level count and usage mode values fit within the range supported for caching.
    HELIUM_ASSERT( m_mipLevelCountMinusOne == mipLevelCount - 1 );
    HELIUM_ASSERT( m_usage == static_cast< uint32_t >( usage ) );
}

/// Destructor.
D3D9DynamicTexture2d::~D3D9DynamicTexture2d()
{
}

/// @copydoc D3D9DeviceResetListener::OnPreReset()
void D3D9DynamicTexture2d::OnPreReset()
{
    HELIUM_ASSERT( m_pTexture );

    // Cache all existing D3D9Surface objects first and release their references to the actual Direct3D surface
    // interfaces.
    const GUID& rPrivateDataGuid = D3D9Renderer::GetPrivateDataGuid();

    uint_fast32_t mipLevelCount = m_mipLevelCountMinusOne + 1;
    for( uint_fast32_t levelIndex = 0; levelIndex < mipLevelCount; ++levelIndex )
    {
        IDirect3DSurface9* pD3DSurface = NULL;
        L_D3D9_VERIFY( m_pTexture->GetSurfaceLevel( static_cast< UINT >( levelIndex ), &pD3DSurface ) );
        HELIUM_ASSERT( pD3DSurface );

        D3D9Surface* pSurface = NULL;
        DWORD privateDataSize = static_cast< DWORD >( sizeof( pSurface ) );
        HRESULT result = pD3DSurface->GetPrivateData( rPrivateDataGuid, &pSurface, &privateDataSize );
        if( SUCCEEDED( result ) && pSurface )
        {
            m_surfaces[ levelIndex ] = pSurface;
            pSurface->SetD3DSurface( NULL );
        }
    }

    // Release the texture.
    m_pTexture->Release();
}

/// @copydoc D3D9DeviceResetListener::OnPostReset()
void D3D9DynamicTexture2d::OnPostReset( D3D9Renderer* pRenderer )
{
    HELIUM_ASSERT( pRenderer );
    IDirect3DDevice9* pDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pDevice );

    // Recreate the texture.
    static const DWORD d3dUsages[] =
    {
        0,                      // RENDERER_BUFFER_USAGE_STATIC
        D3DUSAGE_DYNAMIC,       // RENDERER_BUFFER_USAGE_DYNAMIC
        D3DUSAGE_RENDERTARGET,  // RENDERER_BUFFER_USAGE_RENDER_TARGET
        D3DUSAGE_DEPTHSTENCIL   // RENDERER_BUFFER_USAGE_DEPTH_STENCIL
    };

    HELIUM_ASSERT( !m_pTexture );
    L_D3D9_VERIFY( pDevice->CreateTexture(
        m_width,
        m_height,
        m_mipLevelCountMinusOne + 1,
        d3dUsages[ m_usage ],
        m_format,
        D3DPOOL_DEFAULT,
        &m_pTexture,
        NULL ) );

    // Reassign surface references to any D3D9Surface objects cached during OnPreReset().
    uint_fast32_t mipLevelCount = m_mipLevelCountMinusOne + 1;
    for( uint_fast32_t levelIndex = 0; levelIndex < mipLevelCount; ++levelIndex )
    {
        D3D9Surface* pSurface = m_surfaces[ levelIndex ];
        if( pSurface )
        {
            IDirect3DSurface9* pD3DSurface = NULL;
            L_D3D9_VERIFY( m_pTexture->GetSurfaceLevel( static_cast< UINT >( levelIndex ), &pD3DSurface ) );
            HELIUM_ASSERT( pD3DSurface );

            pSurface->SetD3DSurface( pD3DSurface );

            m_surfaces[ levelIndex ].Release();
        }
    }
}
