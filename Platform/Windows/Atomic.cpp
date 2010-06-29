#include "Platform/Atomic.h"
#include "Platform/Assert.h"
#include "Platform/Align.h"

#include "Windows.h"

using namespace Platform;

void Platform::AtomicIncrement( volatile i32* value )
{
    NOC_ASSERT( NOC_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedIncrement( (volatile LONG*)value );
}

void Platform::AtomicDecrement( volatile i32* value )
{
    NOC_ASSERT( NOC_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedDecrement( (volatile LONG*)value );
}

void Platform::AtomicExchange( volatile i32* addr, i32 value )
{
    NOC_ASSERT( NOC_ALIGN_4( addr ) == (uintptr)addr );
    ::InterlockedExchange( (volatile LONG*)addr, value );
}

#ifdef X64

void Platform::AtomicIncrement( volatile i64* value )
{
    NOC_ASSERT( NOC_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedIncrement64( (volatile LONGLONG*)value );
}
void Platform::AtomicDecrement( volatile i64* value )
{
    NOC_ASSERT( NOC_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedDecrement64( (volatile LONGLONG*)value );
}

void Platform::AtomicExchange( volatile i64* addr, i64 value )
{
    NOC_ASSERT( NOC_ALIGN_4( addr ) == (uintptr)addr );
    ::InterlockedExchange64( (volatile LONGLONG*)addr, value );
}

#endif