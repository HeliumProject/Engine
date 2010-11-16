//----------------------------------------------------------------------------------------------------------------------
// Package.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the loader associated with this package.
    ///
    /// @return  Package loader.
    ///
    /// @see SetLoader()
    PackageLoader* Package::GetLoader() const
    {
        return m_pLoader;
    }
}
