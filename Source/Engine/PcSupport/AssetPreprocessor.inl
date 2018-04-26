namespace Helium
{
    /// Get the platform preprocessor used for caching objects and processing resources for a specific platform.
    ///
    /// @param[in] platform  Target platform.
    ///
    /// @return  Registered platform preprocessor.
    ///
    /// @see SetPlatformPreprocessor()
    PlatformPreprocessor* AssetPreprocessor::GetPlatformPreprocessor( Cache::EPlatform platform ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

        return m_pPlatformPreprocessors[ platform ];
    }
}
