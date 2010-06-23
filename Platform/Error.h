#pragma once

#include <string>

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Platform
{
    //
    // Decode the last error string
    //

    PLATFORM_API u32 GetLastError();

    PLATFORM_API tstring GetErrorString( u32 errorOverride = 0 );
}