#pragma once

#include "RenderingD3D9/D3D9IndexBuffer.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Helium
{
    /// Dynamic Direct3D 9 index buffer.
    ///
    /// Dynamic index buffers are stored in the D3DPOOL_DEFAULT pool, which requires them to be recreated on device
    /// reset.
    ///
    /// Note that when using the Direct3D 9Ex device, this class is not used, as Direct3D 9Ex does not lose resources
    /// when the device is lost.
    class D3D9DynamicIndexBuffer : public D3D9IndexBuffer, public D3D9DeviceResetListener
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9DynamicIndexBuffer( IDirect3DIndexBuffer9* pD3DBuffer );
        //@}

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
        //@}

    private:
        /// Cached index buffer size, in bytes.
        uint32_t m_size : 31;
        /// Non-zero if the buffer is using 32-bit indices, zero if 16-bit indices are being used.
        uint32_t m_bUse32BitIndices : 1;

        /// @name Construction/Destruction
        //@{
        ~D3D9DynamicIndexBuffer();
        //@}
    };
}
