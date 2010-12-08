#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9SubContext.h"

#include "D3D9Rendering/D3D9Surface.h"

using namespace Lunar;

/// Constructor.
///
/// @param[in] pD3DDevice  Direct3D 9 swap chain associated with this sub-context.  Its reference count will be
///                        incremented when this object is constructed and decremented back when this object is
///                        destroyed.
D3D9SubContext::D3D9SubContext( IDirect3DSwapChain9* pSwapChain )
    : m_pSwapChain( pSwapChain )
{
    HELIUM_ASSERT( pSwapChain );
    pSwapChain->AddRef();
}

/// Destructor.
D3D9SubContext::~D3D9SubContext()
{
    m_pSwapChain->Release();
}

/// @copydoc RRenderContext::GetBackBufferSurface()
RSurface* D3D9SubContext::GetBackBufferSurface()
{
    // Create the back buffer surface reference if it does not yet exist.
    if( !m_spBackBufferSurface )
    {
        IDirect3DSurface9* pD3DSurface = NULL;
        L_D3D9_VERIFY( m_pSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pD3DSurface ) );
        HELIUM_ASSERT( pD3DSurface );

        m_spBackBufferSurface = new D3D9Surface( pD3DSurface, true );
        HELIUM_ASSERT( m_spBackBufferSurface );

        // Relinquish ownership of the IDirect3DSurface9 (D3D9Surface has it now).
        pD3DSurface->Release();
    }

    return m_spBackBufferSurface;
}

/// @copydoc RRenderContext::Swap()
void D3D9SubContext::Swap()
{
    // Release the current back buffer surface.
    m_spBackBufferSurface.Release();

    // Present the scene.
    L_D3D9_VERIFY( m_pSwapChain->Present( NULL, NULL, NULL, NULL, 0 ) );
}
