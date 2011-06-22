//----------------------------------------------------------------------------------------------------------------------
// D3D9ConstantBuffer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_RENDERING_D3D9_D3D9_CONSTANT_BUFFER_H
#define LUNAR_RENDERING_D3D9_D3D9_CONSTANT_BUFFER_H

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

#endif  // LUNAR_RENDERING_D3D9_D3D9_CONSTANT_BUFFER_H
