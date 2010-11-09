#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Mutex.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Helium;

HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle::DebugInfoStruct::ListEntryStruct) == sizeof(LIST_ENTRY) );
HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle::DebugInfoStruct) == sizeof(RTL_CRITICAL_SECTION_DEBUG) );
HELIUM_COMPILE_ASSERT( sizeof(Mutex::Handle) == sizeof(CRITICAL_SECTION) );

/// Constructor.
Mutex::Mutex()
{
    ::InitializeCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

/// Destructor.
Mutex::~Mutex()
{
    ::DeleteCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

/// Lock this mutex.
///
/// @see Unlock(), TryLock()
void Mutex::Lock()
{
    ::EnterCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

/// Unlock this mutex.
///
/// @see Lock(), TryLock()
void Mutex::Unlock()
{
    ::LeaveCriticalSection((CRITICAL_SECTION*)&m_Handle);
}

/// Try to lock this mutex without blocking.
///
/// @return  True if the mutex was unlocked and this thread managed to acquire the lock, false the mutex is already
///          locked by another thread.
///
/// @see Lock(), Unlock()
bool Mutex::TryLock()
{
    return ( TryEnterCriticalSection( reinterpret_cast< CRITICAL_SECTION* >( &m_Handle ) ) != FALSE );
}
