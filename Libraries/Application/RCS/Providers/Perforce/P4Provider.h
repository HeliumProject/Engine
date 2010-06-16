#pragma once

#include "Application/API.h"

#include "Application/RCS/RCS.h"
#include "Application/RCS/Provider.h"
#include "Platform/Compiler.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Profile.h"
#include "Platform/Thread.h"
#include "Platform/Mutex.h"
#include "Platform/Event.h"

#ifndef P4CLIENTAPI_H
# define P4CLIENTAPI_H
# pragma warning (disable : 4267 4244)
# include "p4/clientapi.h"
# pragma warning (default : 4267 4244)
#endif

namespace Perforce
{
  //
  // Command transaction thread object
  //

  namespace CommandPhases
  {
    enum CommandPhase
    {
      Unknown,
      Executing,
      Connecting,
      Complete,
    };
  };
  typedef CommandPhases::CommandPhase CommandPhase;

  class APPLICATION_API WaitInterface
  {
  public:
    virtual ~WaitInterface();
    virtual bool StopWaiting() = 0;
  };
  typedef Nocturnal::Signature<bool, WaitInterface*> WaitSignature;
  APPLICATION_API extern WaitSignature::Delegate g_ShowWaitDialog;

  struct MessageArgs
  {
    std::string m_Message;
    std::string m_Title;

    MessageArgs( const std::string& message, const std::string& title )
      : m_Message( message )
      , m_Title( title )
    {

    }
  };
  typedef Nocturnal::Signature<void, const MessageArgs&> MessageSignature;
  APPLICATION_API extern MessageSignature::Delegate g_ShowWarningDialog;

#pragma warning (disable : 4275)
  class APPLICATION_API Provider : public RCS::Provider, public KeepAlive, public WaitInterface
#pragma warning (default : 4275)
  {
  public:
    Provider();
    ~Provider();

    void Initialize();
    void Cleanup();

  private:
    void ThreadEntry();

  public:
    void RunCommand( class Command* command );
    bool Connect();
    virtual int	IsAlive() NOC_OVERRIDE;
    virtual bool StopWaiting() NOC_OVERRIDE;

  public:
    //
    // RCS::Provider
    //

    virtual bool IsEnabled() NOC_OVERRIDE;
    virtual void SetEnabled( bool online ) NOC_OVERRIDE;

    virtual const char* GetName() NOC_OVERRIDE;

    virtual void Sync( RCS::File& file, const u64 timestamp = 0 ) NOC_OVERRIDE;

    virtual void GetInfo( RCS::File& file, const RCS::GetInfoFlag flags = RCS::GetInfoFlags::Default ) NOC_OVERRIDE;
    virtual void GetInfo( const std::string& folder, RCS::V_File& files, bool recursive = false, u32 fileData = RCS::FileData::All, u32 actionData = RCS::ActionData::All ) NOC_OVERRIDE;

    virtual void Add( RCS::File& file ) NOC_OVERRIDE;
    virtual void Edit( RCS::File& file ) NOC_OVERRIDE;
    virtual void Delete( RCS::File& file ) NOC_OVERRIDE;

    virtual void GetOpenedFiles( RCS::V_File& file ) NOC_OVERRIDE;

    virtual void Reopen( RCS::File& file ) NOC_OVERRIDE;

    virtual void Rename( RCS::File& source, RCS::File& dest ) NOC_OVERRIDE;
    virtual void Integrate( RCS::File& source, RCS::File& dest ) NOC_OVERRIDE;

    virtual void Revert( RCS::Changeset& changeset, bool revertUnchangedOnly = false ) NOC_OVERRIDE;
    virtual void Revert( RCS::File& file, bool revertUnchangedOnly = false ) NOC_OVERRIDE;

    virtual void Commit( RCS::Changeset& changeset ) NOC_OVERRIDE;

    virtual void CreateChangeset( RCS::Changeset& changeset ) NOC_OVERRIDE;
    virtual void GetChangesets( RCS::V_Changeset& changesets ) NOC_OVERRIDE;

  public:
    bool                  m_Enabled;
    bool                  m_Connected;
    Profile::Timer        m_ConnectTimer;
    u32                   m_ConnectionTestTimeout;        // the time we are willing to wait to test if the server is running
    u32                   m_ForegroundExecuteTimeout;     // the timeout in the foreground thread before we open the wait dialog
    u32                   m_BackgroundExecuteTimeout;     // this is lame and we should consider opening a new connection per-calling thread?

    bool                  m_Abort;
    ClientApi             m_Client;
    std::string           m_UserName;
    std::string           m_ClientName;

  private:
    // transaction thread
    Platform::Thread      m_Thread;     // the thread to run commands in
    bool                  m_Shutdown;   // the shutdown signal
    Platform::Mutex       m_Mutex;      // to ensure thread safety
    Platform::Event       m_Execute;    // to wakeup the command thread
    Platform::Event       m_Completed;  // to wakeup the calling thread

    // command to execute
    class Command*        m_Command;    // the command to run
    CommandPhase          m_Phase;
  };
}