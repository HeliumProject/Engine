#pragma once

#include "API.h"
#include "Common/Types.h"

namespace Profile
{
  //
  // Status Data
  //

  class PROFILE_API MemoryStatus
  {
  public:
    PointerSizedUInt m_TotalReserve;
    PointerSizedUInt m_TotalCommit;
    PointerSizedUInt m_TotalFree;
    PointerSizedUInt m_LargestFree;

    MemoryStatus()
      : m_TotalReserve (0)
      , m_TotalCommit (0)
      , m_TotalFree (0)
      , m_LargestFree (0)
    {

    }
  };

  //
  // Profiler, drop this on the stack to profile memory consumption
  //

  class PROFILE_API MemoryProfiler
  {
  private:
    static bool s_Enabled;
    static MemoryStatus s_Last;
    MemoryStatus m_Start;

  public:
    MemoryProfiler();
    ~MemoryProfiler();
    static void EnableProfiling(bool enable);
  };

  //
  // Query status
  //

  PROFILE_API void GetMemoryStatus(MemoryStatus* status);
  PROFILE_API void GetMemoryStatus(char* buffer, size_t bufSize);
}
