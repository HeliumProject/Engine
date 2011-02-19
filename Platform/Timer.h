#pragma once

#include "Platform/Platform.h"

#if HELIUM_OS_WIN
# include "Platform/Windows/Timer.h"
#endif

namespace Helium
{
    /// Application timer support.
    class PLATFORM_API Timer : public TimerPlatformData
    {
    public:
        /// @name Static Timing Support
        //@{
        static void StaticInitialize();
        static bool IsInitialized();

        static uint64_t GetTickCount();
        inline static uint64_t GetStartTickCount();
        inline static uint64_t GetTicksPerSecond();
        inline static float64_t GetSecondsPerTick();
        static float64_t GetSeconds();
        //@}
    };
}

#if HELIUM_OS_WIN
# include "Platform/Windows/Timer.inl"
#endif
