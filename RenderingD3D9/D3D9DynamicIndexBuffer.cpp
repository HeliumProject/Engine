#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DynamicIndexBuffer.h"

using namespace Helium;

/// Constructor.
D3D9DynamicIndexBuffer::D3D9DynamicIndexBuffer( IDirect3DIndexBuffer9* pD3DBuffer )
    : D3D9IndexBuffer( pD3DBuffer )
{
    HELIUM_ASSERT( pD3DBuffer );

    D3DINDEXBUFFER_DESC bufferDesc;
    HELIUM_D3D9_VERIFY( pD3DBuffer->GetDesc( &bufferDesc ) );

    HELIUM_ASSERT( bufferDesc.Format == D3DFMT_INDEX16 || bufferDesc.Format == D3DFMT_INDEX32 );
    HELIUM_ASSERT( bufferDesc.Type == D3DRTYPE_INDEXBUFFER );
    HELIUM_ASSERT( bufferDesc.Usage == D3DUSAGE_DYNAMIC );
    HELIUM_ASSERT( bufferDesc.Pool == D3DPOOL_DEFAULT );

    m_size = bufferDesc.Size;
    m_bUse32BitIndices = ( bufferDesc.Format == D3DFMT_INDEX32 );

    // Make sure the buffer size is within the range supported for caching.
    HELIUM_ASSERT( m_size == bufferDesc.Size );
}

/// Destructor.
D3D9DynamicIndexBuffer::~D3D9DynamicIndexBuffer()
{
}

/// @copydoc D3D9DeviceResetListener::OnPreReset()
void D3D9DynamicIndexBuffer::OnPreReset()
{
    HELIUM_ASSERT( m_pBuffer );
    m_pBuffer->Release();
    m_pBuffer = NULL;
}

/// @copydoc D3D9DeviceResetListener::OnPostReset()
void D3D9DynamicIndexBuffer::OnPostReset( D3D9Renderer* pRenderer )
{
    HELIUM_ASSERT( pRenderer );
    IDirect3DDevice9* pDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pDevice );

    HELIUM_ASSERT( !m_pBuffer );
    HELIUM_D3D9_VERIFY( pDevice->CreateIndexBuffer(
        m_size,
        D3DUSAGE_DYNAMIC,
        ( m_bUse32BitIndices ? D3DFMT_INDEX32 : D3DFMT_INDEX16 ),
        D3DPOOL_DEFAULT,
        &m_pBuffer,
        NULL ) );
}
