#pragma once

#include "Application/API.h"

namespace Helium
{
    namespace Debug
    {
        APPLICATION_API void InitializeExceptionListener();
        APPLICATION_API void CleanupExceptionListener();
    }
}