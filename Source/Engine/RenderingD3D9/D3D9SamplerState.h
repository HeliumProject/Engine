#pragma once

#include "RenderingD3D9/RenderingD3D9.h"
#include "Rendering/RSamplerState.h"

namespace Helium
{
    /// Direct3D 9 texture sampler state object.
    class D3D9SamplerState : public RSamplerState
    {
    public:
        /// Minification filter.
        D3DTEXTUREFILTERTYPE m_minFilter;
        /// Magnification filter.
        D3DTEXTUREFILTERTYPE m_magFilter;
        /// Mip-level sampling filter.
        D3DTEXTUREFILTERTYPE m_mipFilter;

        /// Texture u-coordinate address mode.
        D3DTEXTUREADDRESS m_addressModeU;
        /// Texture v-coordinate address mode.
        D3DTEXTUREADDRESS m_addressModeV;
        /// Texture w-coordinate address mode.
        D3DTEXTUREADDRESS m_addressModeW;

        /// Offset from the calculated mip level.
        DWORD m_mipLodBias;

        /// Maximum anisotropy value for anistropic texture filtering.
        DWORD m_maxAnisotropy;

        /// @name Initialization
        //@{
        bool Initialize( const Description& rDescription );
        //@}

        /// @name State Information
        //@{
        void GetDescription( Description& rDescription ) const;
        //@}

    private:
        /// @name Construction/Destruction
        //@{
        ~D3D9SamplerState();
        //@}
    };
}
