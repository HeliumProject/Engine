namespace Helium
{
    /// Get the set of bit flags specifying which optional renderer features are supported by the current renderer
    /// implementation and system drivers.
    ///
    /// @return  Combination of ERendererFeatureFlag flags specifying which features are supported.
    ///
    /// @see SupportsAllFeatures(), SupportsAnyFeature()
    uint32_t Renderer::GetFeatureFlags() const
    {
        return m_featureFlags;
    }

    /// Check whether the current renderer implementation and system drivers support all of the features specified by a
    /// given combination of feature flags.
    ///
    /// @param[in] featureFlags  Combination of ERendererFeatureFlag flags specifying which features to test.
    ///
    /// @return  True if all of the features specified by the given set of flags are supported, false if not.
    ///
    /// @see SupportsAnyFeature(), GetFeatureFlags()
    bool Renderer::SupportsAllFeatures( uint32_t featureFlags ) const
    {
        return ( ( m_featureFlags & featureFlags ) == featureFlags );
    }

    /// Check whether the current renderer implementation and system drivers support any of the features specified by a
    /// given combination of feature flags.
    ///
    /// @param[in] featureFlags  Combination of ERendererFeatureFlag flags specifying which features to test.
    ///
    /// @return  True if any of the features specified by the given set of flags are supported, false if not.
    ///
    /// @see SupportsAllFeatures(), GetFeatureFlags()
    bool Renderer::SupportsAnyFeature( uint32_t featureFlags ) const
    {
        return ( ( m_featureFlags & featureFlags ) != 0 );
    }

    /// Constructor.
    ///
    /// Initializes to a default set of parameters.
    Renderer::ContextInitParameters::ContextInitParameters()
        : pWindow( NULL )
        , displayWidth( 0 )
        , displayHeight( 0 )
        , multisampleCount( 0 )
        , bFullscreen( false )
        , bVsync( false )
    {
    }
}
