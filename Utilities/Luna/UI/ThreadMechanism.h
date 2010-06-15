#pragma once

#include "Luna/API.h"

#include "Foundation/Automation/Event.h"
#include "Platform/Types.h"
#include "Platform/Mutex.h"


namespace Luna
{
  //
  // Forwards
  //
  class DummyThread;
  class DummyWindow;

  /////////////////////////////////////////////////////////////////////////////
  /// ThreadProcArgs
  /////////////////////////////////////////////////////////////////////////////
  struct LUNA_CORE_API ThreadProcArgs
  {
    i32 m_ThreadID;
    ThreadProcArgs( i32 threadID )
      : m_ThreadID( threadID )
    {
    }
  };
  typedef LUNA_CORE_API Nocturnal::Signature< void, const ThreadProcArgs& > ThreadProcSignature;


  /////////////////////////////////////////////////////////////////////////////
  /// class ThreadMechanism
  /////////////////////////////////////////////////////////////////////////////
  class LUNA_CORE_API ThreadMechanism
  {
  public:
    ThreadMechanism( const std::string& evenPrefix = std::string( "ThreadMechanism" ) );
    virtual ~ThreadMechanism();
  
    bool StartThread();
    void StopThread();

    bool IsThreadRunning() const { return !m_StopThread; }

    // DO NO CHANGE OR ACCESS thread related data outside of UpdateMutex
    Platform::Mutex& GetUpdateMutex() { return m_UpdateMutex; }

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
    virtual void OnBeginThread( const Luna::ThreadProcArgs& args );
    virtual void OnUpdateThread( const Luna::ThreadProcArgs& args );
    virtual void OnEndThread( const Luna::ThreadProcArgs& args );

  protected:
    bool             m_StopThread;
    i32              m_CurrentThreadID;
    Platform::Mutex  m_UpdateMutex;
    
    DummyWindow*     m_DummyWindow;
    Platform::Mutex  m_BeginThreadMutex;         // Take Lock until m_ThreadInitializedEvent
    HANDLE           m_ThreadInitializedEvent;   // OK to cancel thread after this is set
    HANDLE           m_EndThreadEvent;

    friend class DummyThread;
  };

} // namespace Luna
