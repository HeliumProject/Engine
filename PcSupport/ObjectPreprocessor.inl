//----------------------------------------------------------------------------------------------------------------------
// ObjectPreprocessor.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the platform preprocessor used for caching objects and processing resources for a specific platform.
    ///
    /// @param[in] platform  Target platform.
    ///
    /// @return  Registered platform preprocessor.
    ///
    /// @see SetPlatformPreprocessor()
    PlatformPreprocessor* ObjectPreprocessor::GetPlatformPreprocessor( Cache::EPlatform platform ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

        return m_pPlatformPreprocessors[ platform ];
    }
}
