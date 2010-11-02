//----------------------------------------------------------------------------------------------------------------------
// Path.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get a string containing the characters that are invalid for use in a full path name string.
    ///
    /// @return  String containing all invalid path name characters.
    ///
    /// @see GetInvalidFileNameCharacters()
    const String& Path::GetInvalidPathCharacters()
    {
        return sm_invalidPathCharacters;
    }

    /// Get a string containing the characters that are invalid for use in a file or directory name.
    ///
    /// @return  String containing all invalid file or directory name characters.
    ///
    /// @see GetInvalidPathCharacters()
    const String& Path::GetInvalidFileNameCharacters()
    {
        return sm_invalidFileNameCharacters;
    }
}
