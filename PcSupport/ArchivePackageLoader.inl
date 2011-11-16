//----------------------------------------------------------------------------------------------------------------------
// XmlPackageLoader.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the package file path.
    ///
    /// @return  Package file path.
    const Path& ArchivePackageLoader::GetPackageFileSystemPath() const
    {
        return m_packageDirPath;
    }
}
