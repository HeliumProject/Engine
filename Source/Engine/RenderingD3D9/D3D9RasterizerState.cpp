#include "Precompile.h"
#include "RenderingD3D9/D3D9RasterizerState.h"

using namespace Helium;

/// Destructor.
D3D9RasterizerState::~D3D9RasterizerState()
{
}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool D3D9RasterizerState::Initialize( const Description& rDescription )
{
    HELIUM_ASSERT( static_cast< size_t >( rDescription.fillMode ) < static_cast< size_t >( RENDERER_FILL_MODE_MAX ) );
    HELIUM_ASSERT( static_cast< size_t >( rDescription.cullMode ) < static_cast< size_t >( RENDERER_CULL_MODE_MAX ) );
    HELIUM_ASSERT( static_cast< size_t >( rDescription.winding ) < static_cast< size_t >( RENDERER_WINDING_MAX ) );
    if( static_cast< size_t >( rDescription.fillMode ) >= static_cast< size_t >( RENDERER_FILL_MODE_MAX ) ||
        static_cast< size_t >( rDescription.cullMode ) >= static_cast< size_t >( RENDERER_CULL_MODE_MAX ) ||
        static_cast< size_t >( rDescription.winding ) >= static_cast< size_t >( RENDERER_WINDING_MAX ) )
    {
        return false;
    }

    static const D3DFILLMODE fillModes[ RENDERER_FILL_MODE_MAX ] =
    {
        D3DFILL_WIREFRAME,  // RENDERER_FILL_MODE_WIREFRAME
        D3DFILL_SOLID       // RENDERER_FILL_MODE_SOLID
    };

    m_fillMode = fillModes[ rDescription.fillMode ];

    m_winding = rDescription.winding;
    if( rDescription.cullMode == RENDERER_CULL_MODE_NONE )
    {
        m_cullMode = D3DCULL_NONE;
    }
    else if( m_winding == RENDERER_WINDING_COUNTER_CLOCKWISE )
    {
        m_cullMode = ( rDescription.cullMode == RENDERER_CULL_MODE_FRONT ? D3DCULL_CCW : D3DCULL_CW );
    }
    else
    {
        HELIUM_ASSERT( m_winding == RENDERER_WINDING_CLOCKWISE );
        m_cullMode = ( rDescription.cullMode == RENDERER_CULL_MODE_FRONT ? D3DCULL_CW : D3DCULL_CCW );
    }

    m_depthBias = static_cast< float32_t >( rDescription.depthBias ) / 65536.0f;
    m_slopeScaledDepthBias = rDescription.slopeScaledDepthBias;

    return true;
}

/// @copydoc RRasterizerState::GetDescription()
void D3D9RasterizerState::GetDescription( Description& rDescription ) const
{
    switch( m_fillMode )
    {
    case D3DFILL_WIREFRAME:
        {
            rDescription.fillMode = RENDERER_FILL_MODE_WIREFRAME;

            break;
        }

    case D3DFILL_SOLID:
        {
            rDescription.fillMode = RENDERER_FILL_MODE_SOLID;

            break;
        }

    default:
        {
            rDescription.fillMode = RENDERER_FILL_MODE_INVALID;
        }
    }

    switch( m_cullMode )
    {
    case D3DCULL_NONE:
        {
            rDescription.cullMode = RENDERER_CULL_MODE_NONE;

            break;
        }

    case D3DCULL_CW:
        {
            rDescription.cullMode =
                ( m_winding == RENDERER_WINDING_CLOCKWISE ? RENDERER_CULL_MODE_FRONT : RENDERER_CULL_MODE_BACK );

            break;
        }

    case D3DCULL_CCW:
        {
            rDescription.cullMode =
                ( m_winding == RENDERER_WINDING_CLOCKWISE ? RENDERER_CULL_MODE_BACK : RENDERER_CULL_MODE_FRONT );

            break;
        }

    default:
        {
            rDescription.cullMode = RENDERER_CULL_MODE_INVALID;
        }
    }

    rDescription.winding = m_winding;

    rDescription.depthBias = static_cast< int32_t >( m_depthBias * 65536.0f + 0.5f );
    rDescription.slopeScaledDepthBias = m_slopeScaledDepthBias;
}
