#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

namespace Helium
{
    namespace Profile
    {
        //
        // Status Data
        //

        class HELIUM_PLATFORM_API MemoryStatus
        {
        public:
            uintptr_t m_TotalReserve;
            uintptr_t m_TotalCommit;
            uintptr_t m_TotalFree;
            uintptr_t m_LargestFree;

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

        class HELIUM_PLATFORM_API MemoryProfiler
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

        HELIUM_PLATFORM_API void GetMemoryStatus(MemoryStatus* status);
        HELIUM_PLATFORM_API void GetMemoryStatus(tchar_t* buffer, size_t bufSize);
    }
}
