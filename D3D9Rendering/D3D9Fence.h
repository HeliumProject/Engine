//----------------------------------------------------------------------------------------------------------------------
// D3D9Fence.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#ifndef LUNAR_D3D9_RENDERING_D3D9_FENCE_H
#define LUNAR_D3D9_RENDERING_D3D9_FENCE_H

#include "D3D9Rendering/D3D9Rendering.h"
#include "Rendering/RFence.h"

namespace Lunar
{
    /// Direct3D 9 GPU command fence implementation.
    class D3D9Fence : public RFence
    {
    public:
        /// @name Construction/Destruction
        //@{
        explicit D3D9Fence( IDirect3DQuery9* pD3DQuery );
        //@}

        /// @name Data Access
        //@{
        inline IDirect3DQuery9* GetQuery() const;
        //@}

    protected:
        /// Direct3D event query interface.
        IDirect3DQuery9* m_pQuery;

        /// @name Construction/Destruction
        //@{
        ~D3D9Fence();
        //@}
    };
}

#include "D3D9Rendering/D3D9Fence.inl"

#endif  // LUNAR_D3D9_RENDERING_D3D9_FENCE_H
