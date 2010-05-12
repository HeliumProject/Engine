#include "Precompile.h"
#include "ToolTimer.h"

using namespace Luna;

wxMutex ToolTimer::m_NotifyMutex;

ToolTimer::ToolTimer()
: m_LastMilliseconds (0)
{
}

ToolTimer::~ToolTimer()
{
}

void ToolTimer::CheckUpdate()
{
  unsigned int currentMilliseconds = wxGetLocalTimeMillis().GetLo();
  unsigned int milliseconds = ( currentMilliseconds < m_LastMilliseconds ) ? 0xFFFFFFFF - ( m_LastMilliseconds - currentMilliseconds ) : ( currentMilliseconds - m_LastMilliseconds );
  if ( milliseconds > (unsigned int) m_milli )
  {
    Notify();
    Start();
  }
}

bool ToolTimer::Start( int milliseconds, bool oneShot )
{
  m_LastMilliseconds = wxGetLocalTimeMillis().GetLo();
  return __super::Start( milliseconds, oneShot );
}

void ToolTimer::Notify()
{
  if ( m_NotifyMutex.TryLock() == wxMUTEX_NO_ERROR )
  {
    TickArgs args;
    m_TickEvent.Raise( args );

    m_NotifyMutex.Unlock();
  }
}
