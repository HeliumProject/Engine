#include "Thread.h"
#include "Platform.h"

#include "Assert.h"

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
    NOC_ASSERT( false );
    return true;
}

Thread::Return Thread::Exit()
{
    NOC_ASSERT( false );
}

void Thread::Close()
{
    m_Handle = 0x0;
}

Thread::Return Thread::Wait(u32 timeout)
{
    NOC_ASSERT( false );
}

bool Thread::Running()
{
    NOC_ASSERT( false );
}

bool Thread::Valid()
{
    return m_Handle != 0;
}

ThreadLocalPointer::ThreadLocalPointer()
{
    int status = pthread_key_create(&m_Key, NULL); 
    NOC_ASSERT( status == 0 && "Profile library could not create pthread_key"); 
    SetPointer(NULL); 
}

ThreadLocalPointer::~ThreadLocalPointer()
{
    pthread_key_delete(m_Key); 
}

void* ThreadLocalPointer::GetPointer()
{
    return pthread_getspecific(m_Key); 
}

void ThreadLocalPointer::SetPointer(void* pointer)
{
    pthread_setspecific(m_Key, pointer); 
}
