//----------------------------------------------------------------------------------------------------------------------
// D3D9DepthStencilState.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "D3D9RenderingPch.h"
#include "D3D9Rendering/D3D9DepthStencilState.h"

namespace Lunar
{
    // Translate a Direct3D 9 compare function to an engine compare function without using a lookup table (safe and
    // future-proof).
    static ERendererCompareFunction TranslateD3D9CompareFunction( D3DCMPFUNC compareFunction )
    {
        switch( compareFunction )
        {
            case D3DCMP_NEVER:
            {
                return RENDERER_COMPARE_FUNCTION_NEVER;
            }

            case D3DCMP_LESS:
            {
                return RENDERER_COMPARE_FUNCTION_LESS;
            }

            case D3DCMP_EQUAL:
            {
                return RENDERER_COMPARE_FUNCTION_EQUAL;
            }

            case D3DCMP_LESSEQUAL:
            {
                return RENDERER_COMPARE_FUNCTION_LESS_EQUAL;
            }

            case D3DCMP_GREATER:
            {
                return RENDERER_COMPARE_FUNCTION_GREATER;
            }

            case D3DCMP_NOTEQUAL:
            {
                return RENDERER_COMPARE_FUNCTION_NOT_EQUAL;
            }

            case D3DCMP_GREATEREQUAL:
            {
                return RENDERER_COMPARE_FUNCTION_GREATER_EQUAL;
            }

            case D3DCMP_ALWAYS:
            {
                return RENDERER_COMPARE_FUNCTION_ALWAYS;
            }
        }

        return RENDERER_COMPARE_FUNCTION_MAX;
    }

    // Translate a Direct3D 9 stencil operation to an engine stencil operation without using a lookup table (safe and
    // future-proof).
    static ERendererStencilOperation TranslateD3D9StencilOperation( D3DSTENCILOP stencilOperation )
    {
        switch( stencilOperation )
        {
            case D3DSTENCILOP_KEEP:
            {
                return RENDERER_STENCIL_OPERATION_KEEP;
            }

            case D3DSTENCILOP_ZERO:
            {
                return RENDERER_STENCIL_OPERATION_ZERO;
            }

            case D3DSTENCILOP_REPLACE:
            {
                return RENDERER_STENCIL_OPERATION_REPLACE;
            }

            case D3DSTENCILOP_INCRSAT:
            {
                return RENDERER_STENCIL_OPERATION_INCREMENT;
            }

            case D3DSTENCILOP_INCR:
            {
                return RENDERER_STENCIL_OPERATION_INCREMENT_WRAP;
            }

            case D3DSTENCILOP_DECRSAT:
            {
                return RENDERER_STENCIL_OPERATION_DECREMENT;
            }

            case D3DSTENCILOP_DECR:
            {
                return RENDERER_STENCIL_OPERATION_DECREMENT_WRAP;
            }

            case D3DSTENCILOP_INVERT:
            {
                return RENDERER_STENCIL_OPERATION_INVERT;
            }
        }

        return RENDERER_STENCIL_OPERATION_MAX;
    }

    /// Destructor.
    D3D9DepthStencilState::~D3D9DepthStencilState()
    {
    }

    /// Initialize this state object.
    ///
    /// @param[in] rDescription  State description.
    ///
    /// @return  True if initialization was successful, false if not.
    bool D3D9DepthStencilState::Initialize( const Description& rDescription )
    {
        HELIUM_ASSERT(
            static_cast< size_t >( rDescription.depthFunction ) <
            static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) );
        HELIUM_ASSERT(
            static_cast< size_t >( rDescription.stencilFailOperation ) <
            static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
        HELIUM_ASSERT(
            static_cast< size_t >( rDescription.stencilDepthFailOperation ) <
            static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
        HELIUM_ASSERT(
            static_cast< size_t >( rDescription.stencilDepthPassOperation ) <
            static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) );
        HELIUM_ASSERT(
            static_cast< size_t >( rDescription.stencilFunction ) <
            static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) );
        if( ( static_cast< size_t >( rDescription.depthFunction ) >=
              static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) ) ||
            ( static_cast< size_t >( rDescription.stencilFailOperation ) >=
              static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
            ( static_cast< size_t >( rDescription.stencilDepthFailOperation ) >=
              static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
            ( static_cast< size_t >( rDescription.stencilDepthPassOperation ) >=
              static_cast< size_t >( RENDERER_STENCIL_OPERATION_MAX ) ) ||
            ( static_cast< size_t >( rDescription.stencilFunction ) >=
              static_cast< size_t >( RENDERER_COMPARE_FUNCTION_MAX ) ) )
        {
            return false;
        }

        static const D3DCMPFUNC compareFunctions[ RENDERER_COMPARE_FUNCTION_MAX ] =
        {
            D3DCMP_NEVER,         // RENDERER_COMPARE_FUNCTION_NEVER
            D3DCMP_LESS,          // RENDERER_COMPARE_FUNCTION_LESS,
            D3DCMP_EQUAL,         // RENDERER_COMPARE_FUNCTION_EQUAL
            D3DCMP_LESSEQUAL,     // RENDERER_COMPARE_FUNCTION_LESS_EQUAL
            D3DCMP_GREATER,       // RENDERER_COMPARE_FUNCTION_GREATER
            D3DCMP_NOTEQUAL,      // RENDERER_COMPARE_FUNCTION_NOT_EQUAL
            D3DCMP_GREATEREQUAL,  // RENDERER_COMPARE_FUNCTION_GREATER_EQUAL
            D3DCMP_ALWAYS         // RENDERER_COMPARE_FUNCTION_ALWAYS
        };

        static const D3DSTENCILOP stencilOperations[ RENDERER_STENCIL_OPERATION_MAX ] =
        {
            D3DSTENCILOP_KEEP,     // RENDERER_STENCIL_OPERATION_KEEP
            D3DSTENCILOP_ZERO,     // RENDERER_STENCIL_OPERATION_ZERO
            D3DSTENCILOP_REPLACE,  // RENDERER_STENCIL_OPERATION_REPLACE
            D3DSTENCILOP_INCRSAT,  // RENDERER_STENCIL_OPERATION_INCREMENT
            D3DSTENCILOP_INCR,     // RENDERER_STENCIL_OPERATION_INCREMENT_WRAP
            D3DSTENCILOP_DECRSAT,  // RENDERER_STENCIL_OPERATION_DECREMENT
            D3DSTENCILOP_DECR,     // RENDERER_STENCIL_OPERATION_DECREMENT_WRAP,
            D3DSTENCILOP_INVERT,   // RENDERER_STENCIL_OPERATION_INVERT
        };

        m_depthFunction = compareFunctions[ rDescription.depthFunction ];

        m_stencilFailOperation = stencilOperations[ rDescription.stencilFailOperation ];
        m_stencilDepthFailOperation = stencilOperations[ rDescription.stencilDepthFailOperation ];
        m_stencilDepthPassOperation = stencilOperations[ rDescription.stencilDepthPassOperation ];
        m_stencilFunction = compareFunctions[ rDescription.stencilFunction ];

        m_stencilReadMask = rDescription.stencilReadMask;
        m_stencilWriteMask = rDescription.stencilWriteMask;

        m_bDepthTestEnable = rDescription.bDepthTestEnable;
        m_bDepthWriteEnable = rDescription.bDepthWriteEnable;

        m_bStencilTestEnable = rDescription.bStencilTestEnable;

        return true;
    }

    /// @copydoc RDepthStencilState::GetDescription()
    void D3D9DepthStencilState::GetDescription( Description& rDescription ) const
    {
        rDescription.depthFunction = TranslateD3D9CompareFunction( m_depthFunction );

        rDescription.stencilFailOperation = TranslateD3D9StencilOperation( m_stencilFailOperation );
        rDescription.stencilDepthFailOperation = TranslateD3D9StencilOperation( m_stencilDepthFailOperation );
        rDescription.stencilDepthPassOperation = TranslateD3D9StencilOperation( m_stencilDepthPassOperation );
        rDescription.stencilFunction = TranslateD3D9CompareFunction( m_stencilFunction );

        rDescription.stencilReadMask = static_cast< uint8_t >( m_stencilReadMask );
        rDescription.stencilWriteMask = static_cast< uint8_t >( m_stencilWriteMask );

        rDescription.bDepthTestEnable = ( m_bDepthTestEnable != FALSE );
        rDescription.bDepthWriteEnable = ( m_bDepthWriteEnable != FALSE );

        rDescription.bStencilTestEnable = ( m_bStencilTestEnable != FALSE );
    }
}
