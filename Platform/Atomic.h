#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"
#include "Platform/Compiler.h"

namespace Helium
{
    PLATFORM_API void AtomicIncrement( volatile int32_t* value );
    PLATFORM_API void AtomicDecrement( volatile int32_t* value );
    PLATFORM_API void AtomicExchange( volatile int32_t* addr, int32_t value );

#ifdef X64
    PLATFORM_API void AtomicIncrement( volatile int64_t* value );
    PLATFORM_API void AtomicDecrement( volatile int64_t* value );
    PLATFORM_API void AtomicExchange( volatile int64_t* addr, int64_t value );
#endif
}