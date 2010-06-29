#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"
#include "Platform/Compiler.h"

namespace Platform
{
    PLATFORM_API void AtomicIncrement( volatile i32* value );
    PLATFORM_API void AtomicDecrement( volatile i32* value );
    PLATFORM_API void AtomicExchange( volatile i32* addr, i32 value );

#ifdef X64
    PLATFORM_API void AtomicIncrement( volatile i64* value );
    PLATFORM_API void AtomicDecrement( volatile i64* value );
    PLATFORM_API void AtomicExchange( volatile i64* addr, i64 value );
#endif
}