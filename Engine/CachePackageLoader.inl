namespace Helium
{
    /// Get the cache from which this loader loads its object data.
    ///
    /// @return  Cache instance.
    Cache* CachePackageLoader::GetCache() const
    {
        return m_pCache;
    }
}
