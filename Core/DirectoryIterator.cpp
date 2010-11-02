//----------------------------------------------------------------------------------------------------------------------
// DirectoryIterator.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/DirectoryIterator.h"

namespace Lunar
{
    /// Destructor.
    DirectoryIterator::~DirectoryIterator()
    {
    }

    /// @fn bool DirectoryIterator::GetFileName( String& rFileName ) const
    /// Get the file name for the current directory entry.
    ///
    /// @param[out] rFileName  Current file name.
    ///
    /// @return  True if the file name was retrieved successfully, false if this iterator does not point to a valid file
    ///          entry.

    /// @fn bool DirectoryIterator::IsValid() const
    /// Get whether this iterator is pointing to a valid directory entry.
    ///
    /// @return  True if this iterator is currently valid, false if not.

    /// @fn bool DirectoryIterator::Advance()
    /// Advance this iterator to the next directory entry.
    ///
    /// @return  True if this iterator was advanced to a valid entry, false if this iterator is now invalid.
}
