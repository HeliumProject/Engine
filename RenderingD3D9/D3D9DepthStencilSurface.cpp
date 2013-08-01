#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DepthStencilSurface.h"

using namespace Helium;

/// Constructor.
D3D9DepthStencilSurface::D3D9DepthStencilSurface( IDirect3DSurface9* pD3DSurface, bool bSrgb )
    : D3D9Surface( pD3DSurface, bSrgb )
{
    HELIUM_ASSERT( pD3DSurface );

    D3DSURFACE_DESC surfaceDesc;
    HELIUM_D3D9_VERIFY( pD3DSurface->GetDesc( &surfaceDesc ) );

    HELIUM_ASSERT( surfaceDesc.Format == D3DFMT_D24X8 || surfaceDesc.Format == D3DFMT_D24S8 );
    HELIUM_ASSERT( surfaceDesc.Type == D3DRTYPE_SURFACE );
    HELIUM_ASSERT( surfaceDesc.Usage == D3DUSAGE_DEPTHSTENCIL );

    m_width = surfaceDesc.Width;
    m_height = surfaceDesc.Height;

    m_multisampleCountMinusOne = 0;
    if( surfaceDesc.MultiSampleQuality >= D3DMULTISAMPLE_2_SAMPLES &&
        surfaceDesc.MultiSampleQuality <= D3DMULTISAMPLE_16_SAMPLES )
    {
        m_multisampleCountMinusOne = surfaceDesc.MultiSampleQuality - D3DMULTISAMPLE_2_SAMPLES + 1;
    }

    m_bStencil = ( surfaceDesc.Format == D3DFMT_D24S8 );
}

/// Destructor.
D3D9DepthStencilSurface::~D3D9DepthStencilSurface()
{
}

/// @copydoc D3D9DeviceResetListener::OnPreReset()
void D3D9DepthStencilSurface::OnPreReset()
{
    HELIUM_ASSERT( m_pSurface );
    m_pSurface->Release();
    m_pSurface = NULL;
}

/// @copydoc D3D9DeviceResetListener::OnPostReset()
void D3D9DepthStencilSurface::OnPostReset( D3D9Renderer* pRenderer )
{
    HELIUM_ASSERT( pRenderer );
    IDirect3DDevice9* pDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pDevice );

    D3DMULTISAMPLE_TYPE multisampleType = D3DMULTISAMPLE_NONE;
    if( m_multisampleCountMinusOne != 0 )
    {
        multisampleType =
            static_cast< D3DMULTISAMPLE_TYPE >( D3DMULTISAMPLE_2_SAMPLES + m_multisampleCountMinusOne - 1 );
    }

    HELIUM_ASSERT( !m_pSurface );
    HELIUM_D3D9_VERIFY( pDevice->CreateDepthStencilSurface(
        m_width,
        m_height,
        ( m_bStencil ? D3DFMT_D24S8 : D3DFMT_D24X8 ),
        multisampleType,
        0,
        FALSE,
        &m_pSurface,
        NULL ) );
}
