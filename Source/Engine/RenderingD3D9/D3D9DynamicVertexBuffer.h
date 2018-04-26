#pragma once

#include "RenderingD3D9/D3D9VertexBuffer.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Helium
{
    /// Dynamic Direct3D 9 vertex buffer.
    ///
    /// Dynamic vertex buffers are stored in the D3DPOOL_DEFAULT pool, which requires them to be recreated on device
    /// reset.
    ///
    /// Note that when using the Direct3D 9Ex device, this class is not used, as Direct3D 9Ex does not lose resources
    /// when the device is lost.
    class D3D9DynamicVertexBuffer : public D3D9VertexBuffer, public D3D9DeviceResetListener
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9DynamicVertexBuffer( IDirect3DVertexBuffer9* pD3DBuffer );
        //@}

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
        //@}

    private:
        /// Cached vertex buffer size, in bytes.
        uint32_t m_size;

        /// @name Construction/Destruction
        //@{
        ~D3D9DynamicVertexBuffer();
        //@}
    };
}
