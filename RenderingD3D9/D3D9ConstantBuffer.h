#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RConstantBuffer.h"

namespace Helium
{
    /// Direct3D 9 constant buffer implementation.
    class D3D9ConstantBuffer : public RConstantBuffer
    {
    public:
        /// @name Construction/Destruction
        //@{
        D3D9ConstantBuffer( void* pData, uint16_t registerCount );
        //@}

        /// @name Data Access
        //@{
        void* Map( ERendererBufferMapHint hint );
        void Unmap();

        inline const void* GetData() const;
        inline uint32_t GetTag() const;
        inline uint16_t GetRegisterCount() const;
        //@}

    private:
        /// Constant buffer data.
        void* m_pData;
        /// Map tag (incremented after each Unmap() call).
        uint32_t m_tag;
        /// Number of floating-point vector registers covered by this buffer.
        uint16_t m_registerCount;

        /// @name Construction/Destruction
        //@{
        ~D3D9ConstantBuffer();
        //@}
    };
}

#include "RenderingD3D9/D3D9ConstantBuffer.inl"
