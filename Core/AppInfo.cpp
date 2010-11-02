//----------------------------------------------------------------------------------------------------------------------
// AppInfo.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/AppInfo.h"

namespace Lunar
{
    String AppInfo::sm_name;

    /// Set the application name string.
    ///
    /// @param[in] rName  Name to set.
    ///
    /// @see GetName()
    void AppInfo::SetName( const String& rName )
    {
        sm_name = rName;
    }

    /// Clear out all application information values and free all allocated memory.
    void AppInfo::Clear()
    {
        sm_name.Clear();
    }
}
