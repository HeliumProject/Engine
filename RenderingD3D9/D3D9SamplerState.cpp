//----------------------------------------------------------------------------------------------------------------------
// D3D9SamplerState.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9SamplerState.h"

using namespace Helium;

// Translate a Direct3D 9 texture address mode to an engine texture address mode without using a lookup table (safe
// and future-proof).
static ERendererTextureAddressMode TranslateD3D9TextureAddressMode( D3DTEXTUREADDRESS addressMode )
{
    switch( addressMode )
    {
    case D3DTADDRESS_WRAP:
        {
            return RENDERER_TEXTURE_ADDRESS_MODE_WRAP;
        }

    case D3DTADDRESS_CLAMP:
        {
            return RENDERER_TEXTURE_ADDRESS_MODE_CLAMP;
        }
    }

    return RENDERER_TEXTURE_ADDRESS_MODE_MAX;
}

/// Destructor.
D3D9SamplerState::~D3D9SamplerState()
{
}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool D3D9SamplerState::Initialize( const Description& rDescription )
{
    HELIUM_ASSERT( static_cast< size_t >( rDescription.filter ) < static_cast< size_t >( RENDERER_TEXTURE_FILTER_MAX ) );
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.addressModeU ) <
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.addressModeV ) <
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.addressModeW ) <
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) );
    if( static_cast< size_t >( rDescription.filter ) >= static_cast< size_t >( RENDERER_TEXTURE_FILTER_MAX ) ||
        ( static_cast< size_t >( rDescription.addressModeU ) >=
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) ||
        ( static_cast< size_t >( rDescription.addressModeV ) >=
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) ||
        ( static_cast< size_t >( rDescription.addressModeW ) >=
        static_cast< size_t >( RENDERER_TEXTURE_ADDRESS_MODE_MAX ) ) )
    {
        return false;
    }

    static const D3DTEXTUREFILTERTYPE filterTypes[ RENDERER_TEXTURE_FILTER_MAX ][ 3 ] =
    {
        // RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT
        { D3DTEXF_POINT,       D3DTEXF_POINT,       D3DTEXF_POINT       },
        // RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_LINEAR
        { D3DTEXF_POINT,       D3DTEXF_POINT,       D3DTEXF_LINEAR      },
        // RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT
        { D3DTEXF_POINT,       D3DTEXF_LINEAR,      D3DTEXF_POINT       },
        // RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_LINEAR
        { D3DTEXF_POINT,       D3DTEXF_LINEAR,      D3DTEXF_LINEAR      },
        // RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT
        { D3DTEXF_LINEAR,      D3DTEXF_POINT,       D3DTEXF_POINT       },
        // RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
        { D3DTEXF_LINEAR,      D3DTEXF_POINT,       D3DTEXF_LINEAR      },
        // RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT
        { D3DTEXF_LINEAR,      D3DTEXF_LINEAR,      D3DTEXF_POINT       },
        // RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR
        { D3DTEXF_LINEAR,      D3DTEXF_LINEAR,      D3DTEXF_LINEAR      },
        // RENDERER_TEXTURE_FILTER_ANISOTROPIC
        { D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC }
    };

    static const D3DTEXTUREADDRESS addressModes[ RENDERER_TEXTURE_ADDRESS_MODE_MAX ] =
    {
        D3DTADDRESS_WRAP,  // RENDERER_TEXTURE_ADDRESS_MODE_WRAP
        D3DTADDRESS_CLAMP  // RENDERER_TEXTURE_ADDRESS_MODE_CLAMP
    };

    const D3DTEXTUREFILTERTYPE ( &translatedFilterTypes )[ 3 ] = filterTypes[ rDescription.filter ];
    m_minFilter = translatedFilterTypes[ 0 ];
    m_magFilter = translatedFilterTypes[ 1 ];
    m_mipFilter = translatedFilterTypes[ 2 ];

    m_addressModeU = addressModes[ rDescription.addressModeU ];
    m_addressModeV = addressModes[ rDescription.addressModeV ];
    m_addressModeW = addressModes[ rDescription.addressModeW ];

    m_mipLodBias = rDescription.mipLodBias;

    m_maxAnisotropy = Max< uint32_t >( rDescription.maxAnisotropy, 1 );

    return true;
}

/// @copydoc RSamplerState::GetDescription()
void D3D9SamplerState::GetDescription( Description& rDescription ) const
{
    switch( m_minFilter )
    {
    case D3DTEXF_POINT:
        {
            switch( m_magFilter )
            {
            case D3DTEXF_POINT:
                {
                    switch( m_mipFilter )
                    {
                    case D3DTEXF_POINT:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_POINT;
                            break;
                        }

                    case D3DTEXF_LINEAR:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_POINT_MIP_LINEAR;
                            break;
                        }

                    default:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                        }
                    }

                    break;
                }

            case D3DTEXF_LINEAR:
                {
                    switch( m_mipFilter )
                    {
                    case D3DTEXF_POINT:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
                            break;
                        }

                    case D3DTEXF_LINEAR:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_POINT_MAG_LINEAR_MIP_LINEAR;
                            break;
                        }

                    default:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                        }
                    }

                    break;
                }

            default:
                {
                    rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                }
            }

            break;
        }

    case D3DTEXF_LINEAR:
        {
            switch( m_magFilter )
            {
            case D3DTEXF_POINT:
                {
                    switch( m_mipFilter )
                    {
                    case D3DTEXF_POINT:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_POINT;
                            break;
                        }

                    case D3DTEXF_LINEAR:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
                            break;
                        }

                    default:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                        }
                    }

                    break;
                }

            case D3DTEXF_LINEAR:
                {
                    switch( m_mipFilter )
                    {
                    case D3DTEXF_POINT:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_POINT;
                            break;
                        }

                    case D3DTEXF_LINEAR:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR;
                            break;
                        }

                    default:
                        {
                            rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                        }
                    }

                    break;
                }

            default:
                {
                    rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
                }
            }

            break;
        }

    case D3DTEXF_ANISOTROPIC:
        {
            rDescription.filter =
                ( m_magFilter == D3DTEXF_ANISOTROPIC && m_mipFilter == D3DTEXF_ANISOTROPIC
                ? RENDERER_TEXTURE_FILTER_ANISOTROPIC
                : RENDERER_TEXTURE_FILTER_INVALID );
            break;
        }

    default:
        {
            rDescription.filter = RENDERER_TEXTURE_FILTER_INVALID;
        }
    }

    rDescription.addressModeU = TranslateD3D9TextureAddressMode( m_addressModeU );
    rDescription.addressModeV = TranslateD3D9TextureAddressMode( m_addressModeV );
    rDescription.addressModeW = TranslateD3D9TextureAddressMode( m_addressModeW );

    rDescription.mipLodBias = m_mipLodBias;

    rDescription.maxAnisotropy = m_maxAnisotropy;
}
