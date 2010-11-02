//----------------------------------------------------------------------------------------------------------------------
// DirectoryIterator.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the file name for the current directory entry.
    ///
    /// @param[out] rFileName  Current file name.
    ///
    /// @return  True if the file name was retrieved successfully, false if this iterator does not point to a valid file
    ///          entry.
    String DirectoryIterator::GetFileName() const
    {
        String fileName;
        GetFileName( fileName );

        return fileName;
    }

    /// Get whether this iterator is pointing to a valid directory entry.
    ///
    /// @return  True if this iterator is currently valid, false if not.
    DirectoryIterator::operator bool() const
    {
        return IsValid();
    }

    /// Advance this iterator to the next directory entry.
    ///
    /// @return  Reference to this iterator.
    DirectoryIterator& DirectoryIterator::operator++()
    {
        Advance();

        return *this;
    }
}
