/// Get the tick count on timer static initialization.
///
/// The timer tick frequency can vary depending on the platform and hardware on which the application is running.
/// To get the application tick frequency, use GetTicksPerSecond().
///
/// @return  Tick count when StaticInitialize() was called.
///
/// @see GetTickCount(), GetTicksPerSecond(), GetSecondsPerTick(), GetSeconds()
uint64_t Helium::Timer::GetStartTickCount()
{
    return sm_startTickCount;
}

/// Get the number of timer ticks per second.
///
/// @return  Timer tick frequency, in ticks per second.
///
/// @see GetTickCount(), GetStartTickCount(), GetSecondsPerTick(), GetSeconds()
uint64_t Helium::Timer::GetTicksPerSecond()
{
    return sm_ticksPerSecond;
}

/// Get the number of seconds per tick.
///
/// @return  Seconds per tick.
///
/// @see GetSeconds(), GetTickCount(), GetStartTickCount(), GetTicksPerSecond()
float64_t Helium::Timer::GetSecondsPerTick()
{
    return sm_secondsPerTick;
}
