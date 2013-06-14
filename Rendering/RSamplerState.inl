namespace Helium
{
    /// Constructor.
    ///
    /// Initializes the description to default state values.
    RSamplerState::Description::Description()
        : filter( RENDERER_TEXTURE_FILTER_MIN_LINEAR_MAG_LINEAR_MIP_LINEAR )
        , addressModeU( RENDERER_TEXTURE_ADDRESS_MODE_WRAP )
        , addressModeV( RENDERER_TEXTURE_ADDRESS_MODE_WRAP )
        , addressModeW( RENDERER_TEXTURE_ADDRESS_MODE_WRAP )
        , mipLodBias( 0 )
        , maxAnisotropy( 1 )
    {
    }
}
