#include "Platform/Atomic.h"

void Helium::AtomicIncrement( volatile i32* value )
{
    HELIUM_ASSERT( false );
}

void Helium::AtomicDecrement( volatile i32* value )
{
    HELIUM_ASSERT( false );
}

void Helium::AtomicExchange( volatile i32* addr, i32 value )
{
    HELIUM_ASSERT( false );
}

#ifdef X64

void Helium::AtomicIncrement( volatile i64* value )
{
    HELIUM_ASSERT( false );
}

void Helium::AtomicDecrement( volatile i64* value )
{
    HELIUM_ASSERT( false );
}

void Helium::AtomicExchange( volatile i64* addr, i64 value )
{
    HELIUM_ASSERT( false );
}

#endif
