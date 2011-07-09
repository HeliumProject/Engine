#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#if HELIUM_OS_WIN

namespace Helium
{
    /// Application timer support.
    class HELIUM_PLATFORM_API TimerPlatformData
    {
    protected:
        /// Performance counter frequency.
        static uint64_t sm_ticksPerSecond;
        /// Tick count on static initialization.
        static uint64_t sm_startTickCount;
        /// Seconds per performance counter tick.
        static float64_t sm_secondsPerTick;
    };
}

#endif  // HELIUM_OS_WIN
