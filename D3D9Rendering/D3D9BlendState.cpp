//----------------------------------------------------------------------------------------------------------------------
// D3D9BlendState.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9BlendState.h"

using namespace Lunar;

// Translate a Direct3D 9 blend factor to an engine blend factor without using a lookup table (safe and
// future-proof).
static ERendererBlendFactor TranslateD3D9BlendFactor( D3DBLEND blendFunction )
{
    switch( blendFunction )
    {
    case D3DBLEND_ZERO:
        {
            return RENDERER_BLEND_FACTOR_ZERO;
        }

    case D3DBLEND_ONE:
        {
            return RENDERER_BLEND_FACTOR_ONE;
        }

    case D3DBLEND_SRCCOLOR:
        {
            return RENDERER_BLEND_FACTOR_SRC_COLOR;
        }

    case D3DBLEND_INVSRCCOLOR:
        {
            return RENDERER_BLEND_FACTOR_INV_SRC_COLOR;
        }

    case D3DBLEND_SRCALPHA:
        {
            return RENDERER_BLEND_FACTOR_SRC_ALPHA;
        }

    case D3DBLEND_INVSRCALPHA:
        {
            return RENDERER_BLEND_FACTOR_INV_SRC_ALPHA;
        }

    case D3DBLEND_DESTCOLOR:
        {
            return RENDERER_BLEND_FACTOR_DEST_COLOR;
        }

    case D3DBLEND_INVDESTCOLOR:
        {
            return RENDERER_BLEND_FACTOR_INV_DEST_COLOR;
        }

    case D3DBLEND_DESTALPHA:
        {
            return RENDERER_BLEND_FACTOR_DEST_ALPHA;
        }

    case D3DBLEND_INVDESTALPHA:
        {
            return RENDERER_BLEND_FACTOR_INV_DEST_ALPHA;
        }

    case D3DBLEND_SRCALPHASAT:
        {
            return RENDERER_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        }
    }

    return RENDERER_BLEND_FACTOR_INVALID;
}

/// Destructor.
D3D9BlendState::~D3D9BlendState()
{
}

/// Initialize this state object.
///
/// @param[in] rDescription  State description.
///
/// @return  True if initialization was successful, false if not.
bool D3D9BlendState::Initialize( const Description& rDescription )
{
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.sourceFactor ) < static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) );
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.destinationFactor ) <
        static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) );
    HELIUM_ASSERT(
        static_cast< size_t >( rDescription.function ) < static_cast< size_t >( RENDERER_BLEND_FUNCTION_MAX ) );
    if( ( static_cast< size_t >( rDescription.sourceFactor ) >=
        static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) ) ||
        ( static_cast< size_t >( rDescription.destinationFactor ) >=
        static_cast< size_t >( RENDERER_BLEND_FACTOR_MAX ) ) ||
        static_cast< size_t >( rDescription.function ) >= static_cast< size_t >( RENDERER_BLEND_FUNCTION_MAX ) )
    {
        return false;
    }

    static const D3DBLEND blendFactors[ RENDERER_BLEND_FACTOR_MAX ] =
    {
        D3DBLEND_ZERO,          // RENDERER_BLEND_FACTOR_ZERO
        D3DBLEND_ONE,           // RENDERER_BLEND_FACTOR_ONE
        D3DBLEND_SRCCOLOR,      // RENDERER_BLEND_FACTOR_SRC_COLOR
        D3DBLEND_INVSRCCOLOR,   // RENDERER_BLEND_FACTOR_INV_SRC_COLOR
        D3DBLEND_SRCALPHA,      // RENDERER_BLEND_FACTOR_SRC_ALPHA
        D3DBLEND_INVSRCALPHA,   // RENDERER_BLEND_FACTOR_INV_SRC_ALPHA
        D3DBLEND_DESTCOLOR,     // RENDERER_BLEND_FACTOR_DEST_COLOR
        D3DBLEND_INVDESTCOLOR,  // RENDERER_BLEND_FACTOR_INV_DEST_COLOR
        D3DBLEND_DESTALPHA,     // RENDERER_BLEND_FACTOR_DEST_ALPHA
        D3DBLEND_INVDESTALPHA,  // RENDERER_BLEND_FACTOR_INV_DEST_ALPHA
        D3DBLEND_SRCALPHASAT    // RENDERER_BLEND_FACTOR_SRC_ALPHA_SATURATE
    };

    static const D3DBLENDOP blendFunctions[ RENDERER_BLEND_FUNCTION_MAX ] =
    {
        D3DBLENDOP_ADD,          // RENDERER_BLEND_FUNCTION_ADD
        D3DBLENDOP_SUBTRACT,     // RENDERER_BLEND_FUNCTION_SUBTRACT
        D3DBLENDOP_REVSUBTRACT,  // RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT
        D3DBLENDOP_MIN,          // RENDERER_BLEND_FUNCTION_MINIMUM
        D3DBLENDOP_MAX           // RENDERER_BLEND_FUNCTION_MAXIMUM
    };

    static const uint8_t colorWriteMaskTable[ 0x10 ] =
    {
        0,
        D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_GREEN,
        D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_BLUE,
        D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN,
        D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_ALPHA,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_GREEN,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_RED,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN,
        D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED,
    };

    m_sourceFactor = blendFactors[ rDescription.sourceFactor ];
    m_destinationFactor = blendFactors[ rDescription.destinationFactor ];
    m_function = blendFunctions[ rDescription.function ];

    m_colorWriteMask = colorWriteMaskTable[ rDescription.colorWriteMask & RENDERER_COLOR_WRITE_MASK_FLAG_ALL ];

    m_bBlendEnable = rDescription.bBlendEnable;

    return true;
}

/// @copydoc RBlendState::GetDescription()
void D3D9BlendState::GetDescription( Description& rDescription ) const
{
    rDescription.sourceFactor = TranslateD3D9BlendFactor( m_sourceFactor );
    rDescription.destinationFactor = TranslateD3D9BlendFactor( m_destinationFactor );

    switch( m_function )
    {
    case D3DBLENDOP_ADD:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_ADD;

            break;
        }

    case D3DBLENDOP_SUBTRACT:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_SUBTRACT;

            break;
        }

    case D3DBLENDOP_REVSUBTRACT:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_REVERSE_SUBTRACT;

            break;
        }

    case D3DBLENDOP_MIN:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_MINIMUM;

            break;
        }

    case D3DBLENDOP_MAX:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_MAXIMUM;

            break;
        }

    default:
        {
            rDescription.function = RENDERER_BLEND_FUNCTION_INVALID;
        }
    }

    rDescription.colorWriteMask = 0;
    if( m_colorWriteMask & D3DCOLORWRITEENABLE_RED )
    {
        rDescription.colorWriteMask |= RENDERER_COLOR_WRITE_MASK_FLAG_RED;
    }

    if( m_colorWriteMask & D3DCOLORWRITEENABLE_GREEN )
    {
        rDescription.colorWriteMask |= RENDERER_COLOR_WRITE_MASK_FLAG_GREEN;
    }

    if( m_colorWriteMask & D3DCOLORWRITEENABLE_BLUE )
    {
        rDescription.colorWriteMask |= RENDERER_COLOR_WRITE_MASK_FLAG_BLUE;
    }

    if( m_colorWriteMask & D3DCOLORWRITEENABLE_ALPHA )
    {
        rDescription.colorWriteMask |= RENDERER_COLOR_WRITE_MASK_FLAG_ALPHA;
    }

    rDescription.bBlendEnable = ( m_bBlendEnable != FALSE );
}
