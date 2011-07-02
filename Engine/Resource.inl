//----------------------------------------------------------------------------------------------------------------------
// Resource.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
#if HELIUM_EDITOR
    /// Get the preprocessed resource data for the specified platform.
    ///
    /// This data is only valid if the bLoaded flag in the returned structure is set.
    ///
    /// @param[in] platform  Cache platform.
    ///
    /// @return  Reference to the preprocessed resource data currently in memory.
    Resource::PreprocessedData& Resource::GetPreprocessedData( Cache::EPlatform platform )
    {
        HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

        return m_preprocessedData[ platform ];
    }

    /// Get the preprocessed resource data for the specified platform.
    ///
    /// This data is only valid if the bLoaded flag in the returned structure is set.
    ///
    /// @param[in] platform  Cache platform.
    ///
    /// @return  Constant reference to the preprocessed resource data currently in memory.
    const Resource::PreprocessedData& Resource::GetPreprocessedData( Cache::EPlatform platform ) const
    {
        HELIUM_ASSERT( static_cast< size_t >( platform ) < static_cast< size_t >( Cache::PLATFORM_MAX ) );

        return m_preprocessedData[ platform ];
    }
#endif  // HELIUM_EDITOR
}
