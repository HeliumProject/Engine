#include "Platform/Profile.h"

using namespace Helium;

#pragma TODO("Move this class into Timer.h/cpp when Timer subsystems merge")

void SimpleTimer::Reset()
{
    m_StartTime = Helium::TimerGetClock();
}

float SimpleTimer::Elapsed()
{
    return Helium::CyclesToMillis(Helium::TimerGetClock() - m_StartTime);
}
