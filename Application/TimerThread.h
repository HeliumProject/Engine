#pragma once

#include "API.h"
#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Platform/Thread.h"
#include "Platform/Profile.h"
#include "Foundation/Event.h"

namespace Helium
{

    struct HELIUM_APPLICATION_API TimerTickArgs
    {
        float32_t m_Elapsed;

        TimerTickArgs( float32_t elapsed )
            : m_Elapsed( elapsed )
        {
        }
    };
    typedef Helium::Signature< const TimerTickArgs& > TimerTickSignature;

    class HELIUM_APPLICATION_API TimerThread
    {
    public:
        TimerThread( const std::string& timerName, int32_t intervalInMilliseconds, bool singleShot = false )
            : m_Name( timerName )
            , m_Interval( intervalInMilliseconds )
            , m_SingleShot( singleShot )
            , m_Alive( false )
        {
        }
        virtual ~TimerThread();

        void SetInterval( int32_t interval )
        {
            m_Interval = interval;
        }
        int32_t GetInterval()
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
            int32_t m_Interval;
            
            TimerThreadArgs( TimerThread* instance = NULL, int32_t interval = -1 )
                : m_TimerInstance( instance )
                , m_Interval( interval )
            {
            }
        };

        void ThreadEntryPoint( TimerThreadArgs& args );
        void Fire();

    private:
        std::string m_Name;
        int32_t m_Interval;
        bool m_SingleShot;

        TimerTickSignature::Event m_TimerTickEvent;

        bool            m_Alive;
        CallbackThread  m_Thread;
        SimpleTimer     m_Timer;
    };
}