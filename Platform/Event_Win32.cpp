#include "Windows/Windows.h"
#include "Event.h"
#include "Platform.h"

#include "Common/Assert.h"
#include "Windows/Error.h"
#include "Console/Console.h"

using namespace Platform;

Event::Event()
{
  m_Handle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  if ( !m_Handle )
  {
    Platform::Print("Failed to create event (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

Event::~Event()
{
  BOOL result = ::CloseHandle(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to close event (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Event::Signal()
{
  BOOL result = ::SetEvent(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to signal event (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Event::Reset()
{
  BOOL result = ::ResetEvent(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to reset event (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}

void Event::Wait()
{
  DWORD result = ::WaitForSingleObject(m_Handle, INFINITE);
  if ( result != WAIT_OBJECT_0 )
  {
    Platform::Print("Failed to wait for event (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
}
