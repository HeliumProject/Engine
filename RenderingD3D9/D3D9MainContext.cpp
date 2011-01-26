//----------------------------------------------------------------------------------------------------------------------
// D3D9MainContext.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9MainContext.h"

#include "RenderingD3D9/D3D9Surface.h"

using namespace Lunar;

/// Constructor.
///
/// @param[in] pD3DDevice  Direct3D 9 device to use for rendering.  Its reference count will be incremented when
///                        this object is constructed and decremented back when this object is destroyed.
D3D9MainContext::D3D9MainContext( IDirect3DDevice9* pD3DDevice )
: m_pDevice( pD3DDevice )
{
    HELIUM_ASSERT( pD3DDevice );
    pD3DDevice->AddRef();
}

/// Destructor.
D3D9MainContext::~D3D9MainContext()
{
    m_pDevice->Release();
}

/// @copydoc RRenderContext::GetBackBufferSurface()
RSurface* D3D9MainContext::GetBackBufferSurface()
{
    // Create the back buffer surface reference if it does not yet exist.
    if( !m_spBackBufferSurface )
    {
        IDirect3DSurface9* pD3DSurface = NULL;
        L_D3D9_VERIFY( m_pDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pD3DSurface ) );
        HELIUM_ASSERT( pD3DSurface );

        m_spBackBufferSurface = new D3D9Surface( pD3DSurface, true );
        HELIUM_ASSERT( m_spBackBufferSurface );

        // Relinquish ownership of the IDirect3DSurface9 (D3D9Surface has it now).
        pD3DSurface->Release();
    }

    return m_spBackBufferSurface;
}

/// @copydoc RRenderContext::Swap()
void D3D9MainContext::Swap()
{
    // Release the current back buffer surface.
    m_spBackBufferSurface.Release();

    // Present the scene.
    HRESULT result = m_pDevice->Present( NULL, NULL, NULL, NULL );
    if( result == D3DERR_DEVICELOST )
    {
        D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetStaticInstance() );
        HELIUM_ASSERT( pRenderer );
        pRenderer->NotifyLost();
    }
    else
    {
        L_D3D9_ASSERT( result );
    }
}

/// Release this context's reference to the back buffer surface.
///
/// This is typically called during device resets, where the back buffer surface may need to be recreated.
void D3D9MainContext::ReleaseBackBufferSurface()
{
    m_spBackBufferSurface.Release();
}
