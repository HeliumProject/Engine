/// Get whether the table of contents has been fully loaded if it exists.
///
/// @return  True if a TOC load process has been performed, false if not.
///
/// @see BeginLoadToc(), TryFinishLoadToc()
bool Helium::Cache::IsTocLoaded() const
{
    return m_bTocLoaded;
}

/// Get the name used to identify this cache.
///
/// @return  Cache name.
Helium::Name Helium::Cache::GetName() const
{
    return m_name;
}

/// Get the platform for this cache.
///
/// @return  Cache platform.
Helium::Cache::EPlatform Helium::Cache::GetPlatform() const
{
    return m_platform;
}

/// Get the path name of the cache table of contents file.
///
/// @return  TOC file path name.
///
/// @see GetCacheFileName()
const Helium::String& Helium::Cache::GetTocFileName() const
{
    return m_tocFileName;
}

/// Get the path name of the cache file.
///
/// @return  Cache file path name.
///
/// @see GetTocFileName()
const Helium::String& Helium::Cache::GetCacheFileName() const
{
    return m_cacheFileName;
}

/// Get the number of object entries in this cache.
///
/// @return  Asset entry count.
///
/// @see GetEntry()
uint32_t Helium::Cache::GetEntryCount() const
{
    size_t entryCount = m_entries.GetSize();
    HELIUM_ASSERT( entryCount <= UINT32_MAX );

    return static_cast< uint32_t >( entryCount );
}

/// Get the information for the cache entry with the specified index.
///
/// @param[in] index  Asset entry index.
///
/// @return  Asset entry information.
///
/// @see GetEntryCount()
const Helium::Cache::Entry& Helium::Cache::GetEntry( uint32_t index ) const
{
    HELIUM_ASSERT( index < m_entries.GetSize() );

    Entry* pEntry = m_entries[ index ];
    HELIUM_ASSERT( pEntry );

    return *pEntry;
}
