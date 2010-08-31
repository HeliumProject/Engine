#include "Platform/Windows/Windows.h"
#include "Platform/Error.h"
#include "Platform/Thread.h"
#include "Platform/Platform.h"
#include "Platform/Assert.h"
#include "Platform/String.h"

using namespace Helium;

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

void SetThreadName( DWORD threadId, const char* name )
{
    //
    // name the thread (this is brutal)
    //
    struct tagTHREADNAME_INFO
    {
        DWORD dwType; // must be 0x1000
        LPCSTR szName; // pointer to name (in user addr space)
        DWORD dwThreadID; // thread ID (-1=caller thread)
        DWORD dwFlags; // reserved for future use, must be zero
    } threadInfo;

    threadInfo.dwType = 0x1000;
    threadInfo.szName = name;
    threadInfo.dwThreadID = threadId;
    threadInfo.dwFlags = 0;

    __try
    {
        RaiseException( 0x406D1388, 0, 4, (ULONG_PTR*)&threadInfo );
    }
    __except( EXCEPTION_CONTINUE_EXECUTION )
    {
    }
}

bool Thread::Create(Entry entry, void* obj, const char* name, int priority)
{
    if (Valid())
    {
        Close();
    }

    DWORD threadId;
    m_Handle = ::CreateThread(0,0,(LPTHREAD_START_ROUTINE)entry,obj,0, &threadId );
    if (!m_Handle)
    {
        Helium::Print(TXT("Failed to create thread: %s\n [0x%x: %s]"), name, ::GetLastError(), Helium::GetErrorString().c_str());
        return false;
    }

    if ( priority != THREAD_PRIORITY_NORMAL )
    {
        SetThreadPriority( m_Handle, priority );
    }
    
    SetThreadName( threadId, name );

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
        Helium::Print(TXT("Failed to close thread (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }
    m_Handle = NULL;
}

Thread::Return Thread::Wait(u32 timeout)
{
    if ( !Valid() )
    {
        return 0;
    }

    DWORD result = ::WaitForSingleObject(m_Handle, timeout);

    if ( timeout != 0xffffffff && result == WAIT_TIMEOUT )
    {
        return -1;
    }

    if ( timeout == 0xffffffff && result != WAIT_OBJECT_0 )
    {
        Helium::Print(TXT("Failed to wait for thread (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
    }

    DWORD code;
    if ( !::GetExitCodeThread(m_Handle, &code) )
    {
        Helium::Print(TXT("Failed to get thread exit code (%s)\n"), Helium::GetErrorString().c_str());
        HELIUM_BREAK();
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
    HELIUM_ASSERT(m_Key != TLS_OUT_OF_INDEXES);
    SetPointer(NULL); 
}

ThreadLocalPointer::~ThreadLocalPointer()
{
    TlsFree(m_Key); 
}

void* ThreadLocalPointer::GetPointer()
{
    void* value = TlsGetValue(m_Key);
    HELIUM_ASSERT( value );
    return value;
}

void ThreadLocalPointer::SetPointer(void* pointer)
{
    TlsSetValue(m_Key, pointer); 
}
