//----------------------------------------------------------------------------------------------------------------------
// AppInfo.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the application name string.
    ///
    /// @return  Application name.
    ///
    /// @see SetName()
    const String& AppInfo::GetName()
    {
        return sm_name;
    }
}
