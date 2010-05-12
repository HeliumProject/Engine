#include "Timer.h"
#include "Profile.h"

using namespace Profile; 

void Timer::Reset()
{
  m_StartTime = Platform::TimerGetClock();
}

float Timer::Elapsed()
{
  return Platform::CyclesToMillis(Platform::TimerGetClock() - m_StartTime);
}
