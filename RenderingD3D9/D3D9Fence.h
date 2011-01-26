//----------------------------------------------------------------------------------------------------------------------
// D3D9Fence.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#ifndef LUNAR_RENDERING_D3D9_D3D9_FENCE_H
#define LUNAR_RENDERING_D3D9_D3D9_FENCE_H

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RFence.h"

#include "RenderingD3D9/D3D9DeviceResetListener.h"

namespace Lunar
{
    /// Direct3D 9 GPU command fence implementation.
    class D3D9Fence : public RFence, public D3D9DeviceResetListener
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

        /// @name Device Reset Event Handlers
        //@{
        void OnPreReset();
        void OnPostReset( D3D9Renderer* pRenderer );
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

#include "RenderingD3D9/D3D9Fence.inl"

#endif  // LUNAR_RENDERING_D3D9_D3D9_FENCE_H
