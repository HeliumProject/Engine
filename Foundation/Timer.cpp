#include "Timer.h"
#include "Platform/Profile.h"

using namespace Helium::Profile; 

void Timer::Reset()
{
    m_StartTime = Platform::TimerGetClock();
}

float Timer::Elapsed()
{
    return Platform::CyclesToMillis(Platform::TimerGetClock() - m_StartTime);
}
