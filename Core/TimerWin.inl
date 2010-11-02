//----------------------------------------------------------------------------------------------------------------------
// TimerWin.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the current application timer tick count.
    ///
    /// The timer tick frequency can vary depending on the platform and hardware on which the application is running.
    /// To get the application tick frequency, use GetTicksPerSecond().
    ///
    /// @return  Current application timer tick count.
    ///
    /// @see GetTicksPerSecond(), GetStartTickCount(), GetSecondsPerTick(), GetSeconds()
    uint64_t Timer::GetTickCount()
    {
        LARGE_INTEGER perfCounter;
        QueryPerformanceCounter( &perfCounter );

        return perfCounter.QuadPart;
    }

    /// Get the tick count on timer static initialization.
    ///
    /// The timer tick frequency can vary depending on the platform and hardware on which the application is running.
    /// To get the application tick frequency, use GetTicksPerSecond().
    ///
    /// @return  Tick count when StaticInitialize() was called.
    ///
    /// @see GetTickCount(), GetTicksPerSecond(), GetSecondsPerTick(), GetSeconds()
    uint64_t Timer::GetStartTickCount()
    {
        return sm_startTickCount;
    }

    /// Get the number of timer ticks per second.
    ///
    /// @return  Timer tick frequency, in ticks per second.
    ///
    /// @see GetTickCount(), GetStartTickCount(), GetSecondsPerTick(), GetSeconds()
    uint64_t Timer::GetTicksPerSecond()
    {
        return sm_ticksPerSecond;
    }

    /// Get the number of seconds per tick.
    ///
    /// @return  Seconds per tick.
    ///
    /// @see GetSeconds(), GetTickCount(), GetStartTickCount(), GetTicksPerSecond()
    float64_t Timer::GetSecondsPerTick()
    {
        return sm_secondsPerTick;
    }

    /// Get the number of seconds elapsed since StaticInitialize() was called.
    ///
    /// @return  Elapsed seconds since static initialization.
    ///
    /// @see GetSecondsPerTick(), GetTickCount(), GetStartTickCount(), GetTicksPerSecond()
    float64_t Timer::GetSeconds()
    {
        LARGE_INTEGER perfCounter;
        QueryPerformanceCounter( &perfCounter );

        return ( static_cast< float64_t >( perfCounter.QuadPart - sm_startTickCount ) * sm_secondsPerTick );
    }
}
