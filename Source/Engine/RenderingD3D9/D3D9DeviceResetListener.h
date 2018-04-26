#pragma once

#include "RenderingD3D9/RenderingD3D9.h"

namespace Helium
{
    /// Interface for resources listening for Direct3D 9 device reset events.
    class D3D9DeviceResetListener
    {
        friend class D3D9Renderer;

    public:
        /// @name Device Reset Event Handlers
        //@{
        virtual void OnPreReset() = 0;
        virtual void OnPostReset( D3D9Renderer* pRenderer ) = 0;
        //@}

    protected:
        /// @name Construction/Destruction, Protected
        //@{
        D3D9DeviceResetListener();
        D3D9DeviceResetListener( const D3D9DeviceResetListener& rSource );
        virtual ~D3D9DeviceResetListener();
        //@}

        /// @name Overloaded Operators, Protected
        //@{
        D3D9DeviceResetListener& operator=( const D3D9DeviceResetListener& rSource );
        //@}

    private:
        /// Previous listener.
        D3D9DeviceResetListener* m_pPreviousListener;
        /// Next listener.
        D3D9DeviceResetListener* m_pNextListener;
    };
}
