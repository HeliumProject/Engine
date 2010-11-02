//----------------------------------------------------------------------------------------------------------------------
// Log.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the current logging level.
    ///
    /// @return  Current logging level.
    ///
    /// @see SetLevel()
    ELogLevel Log::GetLevel() const
    {
        return m_level;
    }
}
