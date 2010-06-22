#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Thread.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"

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

bool Thread::Create(Entry entry, void* obj, const tchar* name, int priority)
{
    if (Valid())
    {
        Close();
    }

    m_Handle = ::CreateThread(0,0,(LPTHREAD_START_ROUTINE)entry,obj,0,0);
    if (!m_Handle)
    {
        Platform::Print(TXT("Failed to create thread: %s\n [0x%x: %s]"), name, ::GetLastError(), Platform::GetErrorString().c_str());
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
        Platform::Print(TXT("Failed to close thread (%s)\n"), Platform::GetErrorString().c_str());
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
        Platform::Print(TXT("Failed to wait for thread (%s)\n"), Platform::GetErrorString().c_str());
        NOC_BREAK();
    }

    DWORD code;
    if ( !::GetExitCodeThread(m_Handle, &code) )
    {
        Platform::Print(TXT("Failed to get thread exit code (%s)\n"), Platform::GetErrorString().c_str());
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

ThreadLocalPointer::ThreadLocalPointer()
{
    m_Key = TlsAlloc(); 
    NOC_ASSERT(m_Key != TLS_OUT_OF_INDEXES); 
    SetPointer(NULL); 
}

ThreadLocalPointer::~ThreadLocalPointer()
{
    TlsFree(m_Key); 
}

void* ThreadLocalPointer::GetPointer()
{
    return TlsGetValue(m_Key); 
}

void ThreadLocalPointer::SetPointer(void* pointer)
{
    TlsSetValue(m_Key, pointer); 
}
