//----------------------------------------------------------------------------------------------------------------------
// CachePackageLoader.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the cache from which this loader loads its object data.
    ///
    /// @return  Cache instance.
    Cache* CachePackageLoader::GetCache() const
    {
        return m_pCache;
    }
}
