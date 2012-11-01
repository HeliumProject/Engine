#pragma once

#include "Platform/API.h"

#include <string>
#include <iostream>
#include <strstream>

#include "Platform/Types.h"
#include "Platform/Exception.h"

namespace Helium
{
    HELIUM_PLATFORM_API bool GetUserName( tstring& username );
    HELIUM_PLATFORM_API bool GetMachineName( tstring& computername );
    HELIUM_PLATFORM_API bool GetPreferencesDirectory( tstring& profileDirectory );
    HELIUM_PLATFORM_API bool GetAppDataDirectory( tstring& appDataDirectory );
}