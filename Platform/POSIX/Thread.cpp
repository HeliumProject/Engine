#include "Platform/Thread.h"
#include "Platform/Platform.h"

#include "Platform/Assert.h"

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

bool Thread::Create(Entry entry, void* obj, const tchar* name, int priority)
{
    HELIUM_ASSERT( false );
    return true;
}

Thread::Return Thread::Exit()
{
    HELIUM_ASSERT( false );
}

void Thread::Close()
{
    m_Handle = 0x0;
}

Thread::Return Thread::Wait(u32 timeout)
{
    HELIUM_ASSERT( false );
}

bool Thread::Running()
{
    HELIUM_ASSERT( false );
}

bool Thread::Valid()
{
    return m_Handle != 0;
}

ThreadLocalPointer::ThreadLocalPointer()
{
#if 0
    int status = pthread_key_create(&m_Key, NULL);
    HELIUM_ASSERT( status == 0 && "Profile library could not create pthread_key");
#endif

    SetPointer(NULL);

    HELIUM_BREAK();
}

ThreadLocalPointer::~ThreadLocalPointer()
{
#if 0
    pthread_key_delete(m_Key);
#endif

    HELIUM_BREAK();
}

void* ThreadLocalPointer::GetPointer()
{
#if 0
    return pthread_getspecific(m_Key);
#endif

    HELIUM_BREAK();
    return NULL;
}

void ThreadLocalPointer::SetPointer(void* pointer)
{
#if 0
    pthread_setspecific(m_Key, pointer);
#endif

    HELIUM_BREAK();
}
