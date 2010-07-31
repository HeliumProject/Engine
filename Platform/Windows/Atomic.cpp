#include "Platform/Atomic.h"
#include "Platform/Assert.h"
#include "Platform/Align.h"

#include "Windows.h"

using namespace Helium;

void Helium::AtomicIncrement( volatile i32* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedIncrement( (volatile LONG*)value );
}

void Helium::AtomicDecrement( volatile i32* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedDecrement( (volatile LONG*)value );
}

void Helium::AtomicExchange( volatile i32* addr, i32 value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( addr ) == (uintptr)addr );
    ::InterlockedExchange( (volatile LONG*)addr, value );
}

#ifdef X64

void Helium::AtomicIncrement( volatile i64* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedIncrement64( (volatile LONGLONG*)value );
}
void Helium::AtomicDecrement( volatile i64* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr)value );
    ::InterlockedDecrement64( (volatile LONGLONG*)value );
}

void Helium::AtomicExchange( volatile i64* addr, i64 value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( addr ) == (uintptr)addr );
    ::InterlockedExchange64( (volatile LONGLONG*)addr, value );
}

#endif