#pragma once

#include "Platform/Condition.h"
#include "Platform/Locks.h"
#include "Platform/Thread.h"

#include "Foundation/Event.h"

#include "Editor/API.h"

class DummyThread;
class DummyWindow;

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        /// ThreadProcArgs
        /////////////////////////////////////////////////////////////////////////////
        struct ThreadProcArgs
        {
            int32_t m_ThreadID;
            ThreadProcArgs( int32_t threadID )
                : m_ThreadID( threadID )
            {
            }
        };
        typedef Helium::Signature< const ThreadProcArgs& > ThreadProcSignature;


        /////////////////////////////////////////////////////////////////////////////
        /// class ThreadMechanism
        /////////////////////////////////////////////////////////////////////////////
        class ThreadMechanism
        {
        public:
            ThreadMechanism( const tstring& evenPrefix = TXT( "ThreadMechanism" ) );
            virtual ~ThreadMechanism();

            bool StartThread();
            void StopThread();

            bool IsThreadRunning() const { return !m_StopThread; }

            // DO NO CHANGE OR ACCESS thread related data outside of UpdateMutex
            Helium::Mutex& GetUpdateMutex() { return m_UpdateMutex; }

            int32_t GetCurrentThreadID() const { return m_CurrentThreadID; }
            bool IsCurrentThread( int32_t threadID ) const { return threadID == GetCurrentThreadID(); }

        protected:
            // Called by DummyThread
            virtual void InitData() = 0; // called by StartThread
            virtual void ThreadProc( int32_t threadID ) = 0;

            // Call these from within your ThreadProc
            virtual void ThreadEnter( int32_t threadID );
            virtual void ThreadPostResults( int32_t threadID );
            virtual bool CheckThreadLeave( int32_t threadID );
            virtual void ThreadLeave( int32_t threadID );

            // Uses wx events to callback to the main thread
            // Implement these callbaks to take special actions in the main thread
            virtual void OnBeginThread( const ThreadProcArgs& args );
            virtual void OnUpdateThread( const ThreadProcArgs& args );
            virtual void OnEndThread( const ThreadProcArgs& args );

        protected:
            bool              m_StopThread;
            int32_t           m_CurrentThreadID;
            Helium::Mutex     m_UpdateMutex;

            DummyWindow*      m_DummyWindow;
            Helium::Mutex     m_BeginThreadMutex;         // Take Lock until m_ThreadInitializedEvent
            Helium::Condition m_ThreadInitializedEvent;   // OK to cancel thread after this is set
            Helium::Condition m_EndThreadEvent;

            friend class DummyThread;
        };

    }
}