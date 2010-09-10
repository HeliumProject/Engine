#pragma once

#include "API.h"
#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Platform/Thread.h"

#include "Timer.h"
#include "Automation/Event.h"

namespace Helium
{

    struct FOUNDATION_API TimerTickArgs
    {
        f32 m_Elapsed;

        TimerTickArgs( f32 elapsed )
            : m_Elapsed( elapsed )
        {
        }
    };
    typedef Helium::Signature< const TimerTickArgs& > TimerTickSignature;

    class FOUNDATION_API TimerThread
    {
    public:
        TimerThread( const std::string& timerName, i32 intervalInMilliseconds, bool singleShot = false )
            : m_Name( timerName )
            , m_Interval( intervalInMilliseconds )
            , m_SingleShot( singleShot )
            , m_Alive( false )
        {
        }
        virtual ~TimerThread();

        void SetInterval( i32 interval )
        {
            m_Interval = interval;
        }
        i32 GetInterval()
        {
            return m_Interval;
        }

        void Start();
        void Stop();

        void AddTickListener( const TimerTickSignature::Delegate& listener )
        {
            m_TimerTickEvent.Add( listener );
        }

        void RemoveTickListener( const TimerTickSignature::Delegate& listener )
        {
            m_TimerTickEvent.Remove( listener );
        }

        bool IsAlive()
        {
            return m_Alive;
        }

    private:
        struct TimerThreadArgs
        {
            TimerThread* m_TimerInstance;
            i32 m_Interval;
            
            TimerThreadArgs( TimerThread* instance = NULL, i32 interval = -1 )
                : m_TimerInstance( instance )
                , m_Interval( interval )
            {
            }
        };

        void ThreadEntryPoint( TimerThreadArgs& args );
        void Fire();

    private:
        std::string m_Name;
        i32 m_Interval;
        bool m_SingleShot;

        TimerTickSignature::Event m_TimerTickEvent;

        bool   m_Alive;
        Thread m_Thread;
        Timer  m_Timer;
    };
}