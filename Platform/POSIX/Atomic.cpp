#include "Platform/Atomic.h"

void Platform::AtomicIncrement( volatile i32* value )
{
    NOC_ASSERT( false );
}

void Platform::AtomicDecrement( volatile i32* value )
{
    NOC_ASSERT( false );
}

void Platform::AtomicExchange( volatile i32* addr, i32 value )
{
    NOC_ASSERT( false );
}

#ifdef X64

void Platform::AtomicIncrement( volatile i64* value )
{
    NOC_ASSERT( false );
}

void Platform::AtomicDecrement( volatile i64* value )
{
    NOC_ASSERT( false );
}

void Platform::AtomicExchange( volatile i64* addr, i64 value )
{
    NOC_ASSERT( false );
}

#endif
