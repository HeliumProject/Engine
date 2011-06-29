//----------------------------------------------------------------------------------------------------------------------
// D3D9IndexBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_RENDERING_D3D9_D3D9_INDEX_BUFFER_H
#define HELIUM_RENDERING_D3D9_D3D9_INDEX_BUFFER_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RIndexBuffer.h"

namespace Helium
{
    /// Direct3D 9 index buffer implementation.
    class D3D9IndexBuffer : public RIndexBuffer
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9IndexBuffer( IDirect3DIndexBuffer9* pD3DBuffer );
        //@}

        /// @name Data Access
        //@{
        void* Map( ERendererBufferMapHint hint );
        void Unmap();

        inline IDirect3DIndexBuffer9* GetD3DBuffer() const;
        //@}

    protected:
        /// Vertex buffer instance.
        IDirect3DIndexBuffer9* m_pBuffer;

        /// @name Construction/Destruction
        //@{
        virtual ~D3D9IndexBuffer();
        //@}
    };
}

#include "RenderingD3D9/D3D9IndexBuffer.inl"

#endif  // HELIUM_RENDERING_D3D9_D3D9_INDEX_BUFFER_H
