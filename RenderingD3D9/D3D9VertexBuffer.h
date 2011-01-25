//----------------------------------------------------------------------------------------------------------------------
// D3D9VertexBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_VERTEX_BUFFER_H
#define LUNAR_RENDERING_D3D9_D3D9_VERTEX_BUFFER_H

#include "RenderingD3D9/RenderingD3D9.h"
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

    protected:
        /// Vertex buffer instance.
        IDirect3DVertexBuffer9* m_pBuffer;

        /// @name Construction/Destruction
        //@{
        virtual ~D3D9VertexBuffer();
        //@}
    };
}

#include "RenderingD3D9/D3D9VertexBuffer.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_VERTEX_BUFFER_H
