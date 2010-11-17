//----------------------------------------------------------------------------------------------------------------------
// D3D9Surface.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9Surface.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] pD3DSurface  Direct3D 9 surface to wrap.  Its reference count will be incremented when this object is
    ///                         constructed and decremented back when this object is destroyed.
    /// @param[in] bSrgb        True if gamma correction to sRGB should be applied when writing to this surface, false
    ///                         if not.
    D3D9Surface::D3D9Surface( IDirect3DSurface9* pD3DSurface, bool bSrgb )
        : m_pSurface( pD3DSurface )
        , m_bSrgb( bSrgb )
    {
        HELIUM_ASSERT( pD3DSurface );
        pD3DSurface->AddRef();

#ifndef NDEBUG
        // Make sure the Direct3D surface is not wrapped by an existing D3D9Surface instance.
        D3D9Surface* pExistingSurface = NULL;
        DWORD privateDataSize = static_cast< DWORD >( sizeof( pExistingSurface ) );
        HRESULT getResult = pD3DSurface->GetPrivateData(
            D3D9Renderer::GetPrivateDataGuid(),
            &pExistingSurface,
            &privateDataSize );
        HELIUM_ASSERT( FAILED( getResult ) || pExistingSurface == NULL );
#endif

        // Store a pointer back to this object with the surface.
        D3D9Surface* pThis = this;
        L_D3D9_VERIFY( pD3DSurface->SetPrivateData(
            D3D9Renderer::GetPrivateDataGuid(),
            &pThis,
            static_cast< DWORD >( sizeof( this ) ),
            0 ) );
    }

    /// Destructor.
    D3D9Surface::~D3D9Surface()
    {
        // Clear the pointer back to this object from the surface.  Note that we don't free the private data in case the
        // same Direct3D surface is reacquired and wrapped by another D3D9Surface later on.
        D3D9Surface* pNullSurface = NULL;
        L_D3D9_VERIFY( m_pSurface->SetPrivateData(
            D3D9Renderer::GetPrivateDataGuid(),
            &pNullSurface,
            static_cast< DWORD >( sizeof( pNullSurface ) ),
            0 ) );

        // Release this object's reference to the surface.
        m_pSurface->Release();
    }
}
