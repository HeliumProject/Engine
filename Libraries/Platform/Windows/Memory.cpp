#include "Memory.h"
#include "Platform/Profile.h"
#include "Platform/Platform.h"
#include "Platform/Windows/Windows.h"

using namespace Profile; 

bool MemoryProfiler::s_Enabled = false;
MemoryStatus MemoryProfiler::s_Last;

MemoryProfiler::MemoryProfiler()
{
  if (s_Enabled)
  {
    GetMemoryStatus(&m_Start);
  }
}

MemoryProfiler::~MemoryProfiler()
{
  if (s_Enabled)
  {
    MemoryStatus finish;
    GetMemoryStatus(&finish);
    PointerSizedInt delta = finish.m_TotalCommit - s_Last.m_TotalCommit;
    PointerSizedInt scope = finish.m_TotalCommit - m_Start.m_TotalCommit;
    Platform::Print("Memory Commit: %u M bytes (Delta: %i K, %i K within scope)\n", (finish.m_TotalCommit) >> 20, (delta) >> 10, (scope) >> 10);
    s_Last = finish;
  }
}

void MemoryProfiler::EnableProfiling(bool enable)
{
  s_Enabled = enable;
}

void Profile::GetMemoryStatus(MemoryStatus* status)
{
  PointerSizedUInt base = 0;
  PointerSizedUInt res = 1;
  while (res)
  {
    MEMORY_BASIC_INFORMATION mem;
    res = VirtualQueryEx(GetCurrentProcess(), (void*)base, &mem, sizeof(mem));
    if (res)
    {
      if (mem.State & MEM_FREE)
      {
        status->m_TotalFree += mem.RegionSize;

        if (mem.RegionSize > status->m_LargestFree)
        {
          status->m_LargestFree = mem.RegionSize;
        }
      }
      else
      {
        if (mem.State & MEM_RESERVE)
        {
          status->m_TotalReserve += mem.RegionSize;
        }

        if (mem.State & MEM_COMMIT)
        {
          status->m_TotalCommit += mem.RegionSize;
        }
      }

      base += mem.RegionSize;
    }
  }
}

void Profile::GetMemoryStatus(char* buffer, size_t bufSize)
{
  MemoryStatus status;
  GetMemoryStatus(&status);

  try
  {
    memset(buffer, 0, bufSize); 

    _snprintf(buffer, bufSize,
      "Memory:\n" \
      "Total Reserved %iK bytes\n" \
      "Total Commit   %iK bytes\n" \
      "Total Free     %iK bytes\n" \
      "Largest Free   %iK bytes\n" ,
      (status.m_TotalReserve >> 10),
      (status.m_TotalCommit >> 10),
      (status.m_TotalFree >> 10),
      (status.m_LargestFree >> 10) );

    buffer[ bufSize - 1] = 0; 

  }
  catch ( const std::exception& )
  {

  }
}