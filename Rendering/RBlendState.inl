//----------------------------------------------------------------------------------------------------------------------
// RBlendState.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// Initializes the description to default state values.
    RBlendState::Description::Description()
        : sourceFactor( RENDERER_BLEND_FACTOR_ONE )
        , destinationFactor( RENDERER_BLEND_FACTOR_ZERO )
        , function( RENDERER_BLEND_FUNCTION_ADD )
        , colorWriteMask( RENDERER_COLOR_WRITE_MASK_FLAG_ALL )
        , bBlendEnable( false )
    {
    }
}
