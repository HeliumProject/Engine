#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RVertexBuffer.h"

namespace Helium
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
