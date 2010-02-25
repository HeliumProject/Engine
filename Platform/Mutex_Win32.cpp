#include "Windows/Windows.h"
#include "Mutex.h"
#include "Platform.h"

#include "Common/Assert.h"
#include "Windows/Error.h"
#include "Console/Console.h"

using namespace Platform;

Mutex::Mutex()
{
  ::InitializeCriticalSection(&m_Handle);
}

Mutex::~Mutex()
{
  ::DeleteCriticalSection(&m_Handle);
}

void Mutex::Lock()
{
  ::EnterCriticalSection(&m_Handle);
}

void Mutex::Unlock()
{
  ::LeaveCriticalSection(&m_Handle);
}
