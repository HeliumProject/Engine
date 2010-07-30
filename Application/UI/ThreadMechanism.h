#pragma once

#include "Application/API.h"

#include "Foundation/Automation/Event.h"
#include "Platform/Types.h"
#include "Platform/Mutex.h"
#include "Platform/Thread.h"


namespace Helium
{
  //
  // Forwards
  //
  class DummyThread;
  class DummyWindow;

  /////////////////////////////////////////////////////////////////////////////
  /// ThreadProcArgs
  /////////////////////////////////////////////////////////////////////////////
  struct APPLICATION_API ThreadProcArgs
  {
    i32 m_ThreadID;
    ThreadProcArgs( i32 threadID )
      : m_ThreadID( threadID )
    {
    }
  };
  typedef APPLICATION_API Helium::Signature< void, const ThreadProcArgs& > ThreadProcSignature;


  /////////////////////////////////////////////////////////////////////////////
  /// class ThreadMechanism
  /////////////////////////////////////////////////////////////////////////////
  class APPLICATION_API ThreadMechanism
  {
  public:
    ThreadMechanism( const tstring& evenPrefix = TXT( "ThreadMechanism" ) );
    virtual ~ThreadMechanism();
  
    bool StartThread();
    void StopThread();

    bool IsThreadRunning() const { return !m_StopThread; }

    // DO NO CHANGE OR ACCESS thread related data outside of UpdateMutex
    Helium::Mutex& GetUpdateMutex() { return m_UpdateMutex; }

    i32 GetCurrentThreadID() const { return m_CurrentThreadID; }
    bool IsCurrentThread( i32 threadID ) const { return threadID == GetCurrentThreadID(); }

  protected:
    // Called by DummyThread
    virtual void InitData() = 0; // called by StartThread
    virtual void ThreadProc( i32 threadID ) = 0;

    // Call these from within your ThreadProc
    virtual void ThreadEnter( i32 threadID );
    virtual void ThreadPostResults( i32 threadID );
    virtual bool CheckThreadLeave( i32 threadID );
    virtual void ThreadLeave( i32 threadID );

    // Uses wx events to callback to the main thread
    // Implement these callbaks to take special actions in the main thread
    virtual void OnBeginThread( const Helium::ThreadProcArgs& args );
    virtual void OnUpdateThread( const Helium::ThreadProcArgs& args );
    virtual void OnEndThread( const Helium::ThreadProcArgs& args );

  protected:
    bool             m_StopThread;
    i32              m_CurrentThreadID;
    Helium::Mutex  m_UpdateMutex;
    
    DummyWindow*     m_DummyWindow;
    Helium::Mutex  m_BeginThreadMutex;         // Take Lock until m_ThreadInitializedEvent
    Helium::Thread::Handle m_ThreadInitializedEvent;   // OK to cancel thread after this is set
    Helium::Thread::Handle m_EndThreadEvent;

    friend class DummyThread;
  };

} // namespace Helium
