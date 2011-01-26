//----------------------------------------------------------------------------------------------------------------------
// D3D9Fence.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9Fence.h"

using namespace Lunar;

/// Constructor.
///
/// @param[in] pD3DQuery  Direct3D query interface to wrap.  Its reference count will be incremented when this
///                       object is constructed and decremented back when this object is destroyed.
D3D9Fence::D3D9Fence( IDirect3DQuery9* pD3DQuery )
: m_pQuery( pD3DQuery )
{
    HELIUM_ASSERT( pD3DQuery );
    pD3DQuery->AddRef();
}

/// Destructor.
D3D9Fence::~D3D9Fence()
{
    if( m_pQuery )
    {
        m_pQuery->Release();
    }
}

/// @copydoc D3D9DeviceResetListener::OnPreReset()
void D3D9Fence::OnPreReset()
{
    if( m_pQuery )
    {
        m_pQuery->Release();
    }
}

/// @copydoc D3D9DeviceResetListener::OnPostReset()
void D3D9Fence::OnPostReset( D3D9Renderer* /*pRenderer*/ )
{
    // Don't recreate fences on device reset.
}
