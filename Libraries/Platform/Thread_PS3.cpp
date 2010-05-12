#include "Thread.h"
#include "Platform.h"

#include "Common/Assert.h"

using namespace Platform;

#define PPU_THR_PRIO          2000
#define PPU_THR_STACK_ADDR       0
#define PPU_THR_STACK_SIZE  0x4000

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
  if (sys_ppu_thread_create(&m_Handle, 
      entry,
      (PointerSizedUInt)obj,
      PPU_THR_PRIO,
      PPU_THR_STACK_SIZE,
      SYS_PPU_THREAD_CREATE_JOINABLE,
      name) != CELL_OK)
  {
    Platform::Print("Failed to create thread: %s\n", name);
    return false;
  }

  return true;
}

Thread::Return Thread::Exit()
{
  return sys_ppu_thread_exit(0);
}

void Thread::Close()
{
  m_Handle = 0x0;
}

Thread::Return Thread::Wait(u32 timeout)
{
  NOC_ASSERT( timeout == 0 ); // not supported
  sys_ppu_thread_join(m_Handle, NULL);
}

bool Thread::Running()
{
  NOC_BREAK(); // not supported
  return true;
}

bool Thread::Valid()
{
  return m_Handle != 0;
}
