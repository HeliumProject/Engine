//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexBuffer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9VertexBuffer.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// @param[in] pD3DBuffer  Direct3D 9 vertex buffer to wrap.  Its reference count will be incremented when this
    ///                        object is constructed and decremented back when this object is destroyed.
    D3D9VertexBuffer::D3D9VertexBuffer( IDirect3DVertexBuffer9* pD3DBuffer )
        : m_pBuffer( pD3DBuffer )
    {
        HELIUM_ASSERT( pD3DBuffer );
        pD3DBuffer->AddRef();
    }

    /// Destructor.
    D3D9VertexBuffer::~D3D9VertexBuffer()
    {
        m_pBuffer->Release();
    }

    /// @copydoc RVertexBuffer::Map()
    void* D3D9VertexBuffer::Map( ERendererBufferMapHint hint )
    {
        if( !m_pBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "D3D9VertexBuffer::Map(): Attempted to map a buffer for which no Direct3D resource has been " )
                  TXT( "allocated.\n" ) ) );

            return NULL;
        }

        DWORD lockFlags = 0;
        if( hint == RENDERER_BUFFER_MAP_HINT_DISCARD )
        {
            lockFlags = D3DLOCK_DISCARD;
        }
        else if( hint == RENDERER_BUFFER_MAP_HINT_NO_OVERWRITE )
        {
            lockFlags = D3DLOCK_NOOVERWRITE;
        }

        void* pData = NULL;
        HRESULT hResult = m_pBuffer->Lock( 0, 0, &pData, lockFlags );
        if( FAILED( hResult ) )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9VertexBuffer::Map(): Failed to lock Direct3D buffer.\n" ) );

            return NULL;
        }

        HELIUM_ASSERT( pData );

        return pData;
    }

    /// @copydoc RVertexBuffer::Unmap()
    void D3D9VertexBuffer::Unmap()
    {
        if( !m_pBuffer )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "D3D9VertexBuffer::Unmap(): Attempted to unmap a buffer for which no Direct3D resource has " )
                  TXT( "been allocated.\n" ) ) );

            return;
        }

        HRESULT hResult = m_pBuffer->Unlock();
        if( FAILED( hResult ) )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "D3D9VertexBuffer::Unmap(): Failed to unlock Direct3D buffer.\n" ) );
        }
    }
}
