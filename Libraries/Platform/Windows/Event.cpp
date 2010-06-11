#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Event.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Platform;

Event::Event()
{
  m_Handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if ( !m_Handle )
  {
    Platform::Print("Failed to create event (%s)\n", Platform::GetErrorString().c_str());
    NOC_BREAK();
  }
}

Event::~Event()
{
  BOOL result = ::CloseHandle(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to close event (%s)\n", Platform::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Event::Signal()
{
  BOOL result = ::SetEvent(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to signal event (%s)\n", Platform::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Event::Reset()
{
  BOOL result = ::ResetEvent(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to reset event (%s)\n", Platform::GetErrorString().c_str());
    NOC_BREAK();
  }
}

bool Event::Wait(u32 timeout)
{
  DWORD result = ::WaitForSingleObject(m_Handle, timeout);

  if ( timeout != 0xffffffff && result == WAIT_TIMEOUT )
  {
    return false;
  }

  if ( result != WAIT_OBJECT_0 )
  {
    Platform::Print("Failed to wait for event (%s)\n", Platform::GetErrorString().c_str());
    NOC_BREAK();
  }

  return true;
}
