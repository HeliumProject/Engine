//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexInputLayout.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9VertexInputLayout.h"

using namespace Lunar;

/// Constructor.
///
/// @param[in] pD3DDeclaration  Direct3D vertex declaration to wrap.  Its reference count will be incremented when
///                             this object is constructed and decremented back when this object is destroyed.
D3D9VertexInputLayout::D3D9VertexInputLayout( IDirect3DVertexDeclaration9* pD3DDeclaration )
: m_pDeclaration( pD3DDeclaration )
{
    HELIUM_ASSERT( pD3DDeclaration );
    pD3DDeclaration->AddRef();
}

/// Destructor.
D3D9VertexInputLayout::~D3D9VertexInputLayout()
{
    m_pDeclaration->Release();
}
