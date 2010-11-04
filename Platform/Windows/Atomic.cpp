#include "Platform/Atomic.h"
#include "Platform/Assert.h"
#include "Platform/Align.h"

#include "Windows.h"

using namespace Helium;

void Helium::AtomicIncrement( volatile int32_t* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr_t)value );
    ::InterlockedIncrement( (volatile LONG*)value );
}

void Helium::AtomicDecrement( volatile int32_t* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr_t)value );
    ::InterlockedDecrement( (volatile LONG*)value );
}

void Helium::AtomicExchange( volatile int32_t* addr, int32_t value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( addr ) == (uintptr_t)addr );
    ::InterlockedExchange( (volatile LONG*)addr, value );
}

#if HELIUM_WORDSIZE == 64

void Helium::AtomicIncrement( volatile int64_t* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr_t)value );
    ::InterlockedIncrement64( (volatile LONGLONG*)value );
}
void Helium::AtomicDecrement( volatile int64_t* value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( value ) == (uintptr_t)value );
    ::InterlockedDecrement64( (volatile LONGLONG*)value );
}

void Helium::AtomicExchange( volatile int64_t* addr, int64_t value )
{
    HELIUM_ASSERT( HELIUM_ALIGN_4( addr ) == (uintptr_t)addr );
    ::InterlockedExchange64( (volatile LONGLONG*)addr, value );
}

#endif