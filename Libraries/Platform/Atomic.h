#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Platform
{
    PLATFORM_API void AtomicIncrement( volatile i32* value );
    PLATFORM_API void AtomicDecrement( volatile i32* value );
}