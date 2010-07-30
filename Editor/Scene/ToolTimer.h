#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    namespace Editor
    {
        struct TickArgs {};
        typedef Helium::Signature<void, const TickArgs&> TickSignature;

        class ToolTimer : public wxTimer
        {
        public:
            ToolTimer();
            virtual ~ToolTimer();

            void CheckUpdate();

            virtual bool Start( int milliseconds = -1, bool oneShot = false );
            virtual void Notify() HELIUM_OVERRIDE;

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
}