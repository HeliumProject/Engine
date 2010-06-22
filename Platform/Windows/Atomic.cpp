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
