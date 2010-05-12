#pragma once

#include "Common/Compiler.h"
#include "Common/Automation/Event.h"

namespace Luna
{
  struct TickArgs {};
  typedef Nocturnal::Signature<void, const TickArgs&> TickSignature;
  
  class ToolTimer : public wxTimer
  {
    public:
      ToolTimer();
      virtual ~ToolTimer();

      void CheckUpdate();
      
      virtual bool Start( int milliseconds = -1, bool oneShot = false );
      virtual void Notify() NOC_OVERRIDE;
      
      void AddTickListener( const TickSignature::Delegate& listener )
      {
        m_TickEvent.Add( listener );
      }

      void RemoveTickListener( const TickSignature::Delegate& listener )
      {
        m_TickEvent.Remove( listener );
      }
    
    private:
      TickSignature::Event m_TickEvent;

      unsigned int m_LastMilliseconds;

      static wxMutex m_NotifyMutex;
  };


}