#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DynamicVertexBuffer.h"

using namespace Lunar;

/// Constructor.
D3D9DynamicVertexBuffer::D3D9DynamicVertexBuffer( IDirect3DVertexBuffer9* pD3DBuffer )
    : D3D9VertexBuffer( pD3DBuffer )
{
    HELIUM_ASSERT( pD3DBuffer );

    D3DVERTEXBUFFER_DESC bufferDesc;
    L_D3D9_VERIFY( pD3DBuffer->GetDesc( &bufferDesc ) );

    HELIUM_ASSERT( bufferDesc.Format == D3DFMT_VERTEXDATA );
    HELIUM_ASSERT( bufferDesc.Type == D3DRTYPE_VERTEXBUFFER );
    HELIUM_ASSERT( bufferDesc.Usage == D3DUSAGE_DYNAMIC );
    HELIUM_ASSERT( bufferDesc.Pool == D3DPOOL_DEFAULT );
    HELIUM_ASSERT( bufferDesc.FVF == 0 );

    m_size = bufferDesc.Size;
}

/// Destructor.
D3D9DynamicVertexBuffer::~D3D9DynamicVertexBuffer()
{
}

/// @copydoc D3D9DeviceResetListener::OnPreReset()
void D3D9DynamicVertexBuffer::OnPreReset()
{
    HELIUM_ASSERT( m_pBuffer );
    m_pBuffer->Release();
    m_pBuffer = NULL;
}

/// @copydoc D3D9DeviceResetListener::OnPostReset()
void D3D9DynamicVertexBuffer::OnPostReset( D3D9Renderer* pRenderer )
{
    HELIUM_ASSERT( pRenderer );
    IDirect3DDevice9* pDevice = pRenderer->GetD3DDevice();
    HELIUM_ASSERT( pDevice );

    HELIUM_ASSERT( !m_pBuffer );
    L_D3D9_VERIFY( pDevice->CreateVertexBuffer( m_size, D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pBuffer, NULL ) );
}
