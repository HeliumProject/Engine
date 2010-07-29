#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Mutex.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Platform;

HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle::DebugInfoStruct::ListEntryStruct) == sizeof(LIST_ENTRY) );
HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle::DebugInfoStruct) == sizeof(RTL_CRITICAL_SECTION_DEBUG) );
HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle) == sizeof(CRITICAL_SECTION) );

Mutex::Mutex()
{
    ::InitializeCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

Mutex::~Mutex()
{
    ::DeleteCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

void Mutex::Lock()
{
    ::EnterCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

void Mutex::Unlock()
{
    ::LeaveCriticalSection((CRITICAL_SECTION*)&m_Handle);
}
