namespace Helium
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
