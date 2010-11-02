//----------------------------------------------------------------------------------------------------------------------
// AppInfo.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_APP_INFO_H
#define LUNAR_CORE_APP_INFO_H

#include "Core/String.h"

namespace Lunar
{
    /// General application information.
    class LUNAR_CORE_API AppInfo
    {
    public:
        /// @name Information Access
        //@{
        inline static const String& GetName();
        static void SetName( const String& rName );

        static void Clear();
        //@}

    private:
        /// Application name.
        static String sm_name;
    };
}

#include "Core/AppInfo.inl"

#endif  // LUNAR_CORE_APP_INFO_H
