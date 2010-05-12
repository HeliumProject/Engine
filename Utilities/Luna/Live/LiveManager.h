#pragma once

#include "API.h"
#include "Common/Compiler.h"

namespace Luna
{
  namespace Live
  {
    void Initialize();
    void Cleanup();
  }

  LUNA_LIVE_API void OpenLiveFrame();
  void OnTargetEvent( u32 eventId, SNPS3_DBG_EVENT_HDR* dbgHeader, SNPS3_DBG_EVENT_DATA* dbgData, void* pUserData );

  class TargetManagerProcessTimer : public wxTimer
  {
    void Notify();
  };

  class StartViewerThread : public wxThread
  {
  public:
    StartViewerThread( wxEvtHandler* handler, const TargetManager::TargetPtr& target, const std::string& viewerName );

    virtual wxThread::ExitCode Entry() NOC_OVERRIDE;

    bool Success() { return m_Success; }
    const std::string& GetErrorString() { return m_ErrorString; }

  private:
    wxEvtHandler*                 m_EventHandler;
    TargetManager::TargetPtr      m_Target;
    std::string                   m_ViewerName;

    bool                          m_Success;
    std::string                   m_ErrorString;
  };

  class SendCrashInfoThread : public wxThread
  {
  public:
    SendCrashInfoThread( wxEvtHandler* handler, const D_string& tty, const std::string& elf, const std::string& targetName, u32 crashLines )
      : wxThread( wxTHREAD_JOINABLE )
      , m_Elf( elf )
      , m_CrashLines( crashLines )
      , m_EventHandler( handler )
      , m_TargetName( targetName )
    {
      m_TTY.insert( m_TTY.end(), tty.begin(), tty.end() );
    }

    virtual wxThread::ExitCode Entry() NOC_OVERRIDE;

    const std::string& GetBacktrace() { return m_Backtrace; }

  private:
    wxEvtHandler*       m_EventHandler;
    D_string	  				m_TTY;
    std::string					m_Elf;
    std::string         m_TargetName;
    u32                 m_CrashLines;

    std::string         m_Backtrace;
  };

  class LiveFrame;

  namespace ViewerModes
  {
    enum ViewerMode
    {
      Uberview,
      Game,
    };
  }
  typedef ViewerModes::ViewerMode ViewerMode;

  class LUNA_LIVE_API Manager : wxEvtHandler
  {
  private:
    Manager();
    ~Manager();
    static Manager* s_Manager;
  public:
    static void Initialize();
    static void Cleanup();

    static Manager* GetInstance();

    void OpenLiveFrame();
    void LiveFrameClosed();

    bool WriteRegistryValue( const std::string& key, const std::string& value );
    bool ReadRegistryValue( const std::string& key, std::string& value );

    // starts the specified SELF with the standard set of arguments, and the ones passed in prepended to those
    void StartViewer( TargetManager::TargetPtr& target, const std::string& selfName, const V_string& extraArguments );
    // starts uberview
    void StartViewer();

    void SetupDefaults();
    void RefreshTargets();
    void FindSelectedTarget();

    const TargetManager::V_Target& GetTargets();
    const TargetManager::TargetPtr& GetCurrentTarget();

    void PerformAutoStart();
    void SetViewerMode( ViewerMode viewerMode );
    ViewerMode   GetViewerMode();

    void PostEventsProcessed();
    void SendPS3DebugInfo();

    void SetAutoOpenFrame( bool enable );
    void SetAutoStartViewer( bool enable );
    void SetForceAutoStartViewer( bool enable );
    void SetDefaultTarget( const std::string& target );

    bool GetAutoOpenFrame();
    bool GetAutoStartViewer();
    bool GetForceAutoStartViewer();
    std::string GetDefaultTarget();

    void SetLastBacktrace( const std::string& backtrace );

    void ProcessTTY( const TargetManager::TTYArgs& args );
    bool ParseTTYForCrash( const std::string& tty );

    void OnTargetEvent( TargetManager::Target* target, u32 eventId, SNPS3_DBG_EVENT_HDR* dbgHeader, SNPS3_DBG_EVENT_DATA* dbgData );

    // wx event handlers
    void OnViewerStartThreadFinished( wxCommandEvent& evt );
    void OnCrashInfoThreadFinished( wxCommandEvent& evt );

  protected:
    LiveFrame* m_LiveFrame;
    StartViewerThread* m_StartViewerThread;
    SendCrashInfoThread* m_CrashInfoThread;

    TargetManager::V_Target     m_Targets;
    TargetManager::TargetPtr    m_CurrentTarget;
    TargetManagerProcessTimer   m_ProcessTimer;

    bool                        m_Crashed;
    u32                         m_CrashLines;
    u32                         m_LastCrashTime;
    ViewerMode                  m_ViewerMode;

    std::string                 m_LastBackTrace;
  };
}