#pragma once

#include "Foundation/String.h"

namespace Helium
{
    /// General application information.
    class FOUNDATION_API AppInfo
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

#include "Foundation/AppInfo.inl"
