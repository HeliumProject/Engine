#include "Windows/Windows.h"
#include "Thread.h"
#include "Platform.h"

#include "Common/Assert.h"
#include "Windows/Error.h"
#include "Console/Console.h"

using namespace Platform;

Thread::Thread()
: m_Handle (0x0)
{

}

Thread::~Thread()
{
  if (Valid())
  {
    Close();
  }
}

bool Thread::Create(Entry entry, void* obj, const char* name, int priority)
{
  if (Valid())
  {
    Close();
  }

  m_Handle = ::CreateThread(0,0,(LPTHREAD_START_ROUTINE)entry,obj,0,0);
  if (!m_Handle)
  {
    Platform::Print("Failed to create thread: %s\n [0x%x: %s]", name, ::GetLastError(), Windows::GetErrorString().c_str());
    return false;
  }

  if ( priority != THREAD_PRIORITY_NORMAL )
  {
    SetThreadPriority( m_Handle, priority );
  }

  return true;
}

Thread::Return Thread::Exit()
{
  return 0;
}

void Thread::Close()
{
  BOOL result = ::CloseHandle(m_Handle);
  if ( result != TRUE )
  {
    Platform::Print("Failed to close thread (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }
  m_Handle = NULL;
}

Thread::Return Thread::Wait(u32 timeout)
{
  DWORD result = ::WaitForSingleObject(m_Handle, timeout);

  if ( timeout != 0xffffffff && result == WAIT_TIMEOUT )
  {
    return -1;
  }

  if ( timeout == 0xffffffff && result != WAIT_OBJECT_0 )
  {
    Platform::Print("Failed to wait for thread (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }

  DWORD code;
  if ( !::GetExitCodeThread(m_Handle, &code) )
  {
    Platform::Print("Failed to get thread exit code (%s)\n", Windows::GetErrorString().c_str());
    NOC_BREAK();
  }

  return code;
}

bool Thread::Running()
{
  DWORD status;
  ::GetExitCodeThread( m_Handle, &status );
  return status == STILL_ACTIVE;
}

bool Thread::Valid()
{
  return m_Handle != 0;
}