//----------------------------------------------------------------------------------------------------------------------
// RRasterizerState.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// Initializes the description to default state values.
    RRasterizerState::Description::Description()
        : fillMode( RENDERER_FILL_MODE_SOLID )
        , cullMode( RENDERER_CULL_MODE_BACK )
        , winding( RENDERER_WINDING_CLOCKWISE )
        , depthBias( 0 )
        , slopeScaledDepthBias( 0.0f )
    {
    }
}
