#include "Timer.h"
#include "Platform/Profile.h"

using namespace Profile; 

void Timer::Reset()
{
    m_StartTime = Helium::TimerGetClock();
}

float Timer::Elapsed()
{
    return Helium::CyclesToMillis(Helium::TimerGetClock() - m_StartTime);
}
