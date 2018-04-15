#include "Precompile.h"
#include "RenderingD3D9/D3D9Surface.h"

using namespace Helium;

/// Constructor.
///
/// @param[in] pD3DSurface  Direct3D 9 surface to wrap.  Its reference count will be incremented when this object is
///                         constructed and decremented back when this object is destroyed.
/// @param[in] bSrgb        True if gamma correction to sRGB should be applied when writing to this surface, false
///                         if not.
D3D9Surface::D3D9Surface( IDirect3DSurface9* pD3DSurface, bool bSrgb )
    : m_pSurface( NULL )
    , m_bSrgb( bSrgb )
{
    HELIUM_ASSERT( pD3DSurface );

    SetD3DSurface( pD3DSurface );
}

/// Destructor.
D3D9Surface::~D3D9Surface()
{
    SetD3DSurface( NULL );
}

/// Explicitly set the Direct3D surface reference.
///
/// This is provided primarily for use by D3D9DynamicTexture2d during device resets.
///
/// @param[in] pSurface  Direct3D surface interface to set.
void D3D9Surface::SetD3DSurface( IDirect3DSurface9* pSurface )
{
    if( m_pSurface != pSurface )
    {
        const GUID& rPrivateDataGuid = D3D9Renderer::GetPrivateDataGuid();

        if( m_pSurface )
        {
            // Clear the pointer back to this object from the surface.  Note that we don't free the private data in case
            // the same Direct3D surface is reacquired and wrapped by another D3D9Surface later on.
            D3D9Surface* pNullSurface = NULL;
            HELIUM_D3D9_VERIFY( m_pSurface->SetPrivateData(
                rPrivateDataGuid,
                &pNullSurface,
                static_cast< DWORD >( sizeof( pNullSurface ) ),
                0 ) );

            // Release this object's reference to the surface.
            m_pSurface->Release();
        }

        m_pSurface = pSurface;

        if( pSurface )
        {
            // Acquire a reference to the surface for this object.
            pSurface->AddRef();

#ifndef NDEBUG
            // Make sure the Direct3D surface is not wrapped by an existing D3D9Surface instance.
            D3D9Surface* pExistingSurface = NULL;
            DWORD privateDataSize = static_cast< DWORD >( sizeof( pExistingSurface ) );
            HRESULT getResult = pSurface->GetPrivateData( rPrivateDataGuid, &pExistingSurface, &privateDataSize );
            HELIUM_ASSERT( FAILED( getResult ) || pExistingSurface == NULL );
#endif

            // Store a pointer back to this object with the surface.
            D3D9Surface* pThis = this;
            HELIUM_D3D9_VERIFY( pSurface->SetPrivateData(
                rPrivateDataGuid,
                &pThis,
                static_cast< DWORD >( sizeof( this ) ),
                0 ) );
        }
    }
}
