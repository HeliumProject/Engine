//----------------------------------------------------------------------------------------------------------------------
// D3D9Fence.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9Fence.h"

namespace Lunar
{
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
        m_pQuery->Release();
    }
}
