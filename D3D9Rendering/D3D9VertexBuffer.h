//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_D3D9_RENDERING_D3D9_VERTEX_BUFFER_H
#define LUNAR_D3D9_RENDERING_D3D9_VERTEX_BUFFER_H

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RVertexBuffer.h"

namespace Lunar
{
    /// Direct3D 9 vertex buffer implementation.
    class D3D9VertexBuffer : public RVertexBuffer
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9VertexBuffer( IDirect3DVertexBuffer9* pD3DBuffer );
        //@}

        /// @name Data Access
        //@{
        void* Map( ERendererBufferMapHint hint );
        void Unmap();

        inline IDirect3DVertexBuffer9* GetD3DBuffer() const;
        //@}

    private:
        /// Vertex buffer instance.
        IDirect3DVertexBuffer9* m_pBuffer;

        /// @name Construction/Destruction
        //@{
        ~D3D9VertexBuffer();
        //@}
    };
}

#include "D3D9Rendering/D3D9VertexBuffer.inl"

#endif  // LUNAR_D3D9_RENDERING_D3D9_VERTEX_BUFFER_H
