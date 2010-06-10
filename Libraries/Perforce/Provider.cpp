#include "Provider.h"
#include "Perforce.h"
#include "Exceptions.h"
#include "QueryCommands.h"
#include "ClientCommands.h"
#include "ChangelistCommands.h"

#include "Common/Assert.h"
#include "Common/Version.h"
#include "AppUtils/AppUtils.h"
#include "Windows/Windows.h"
#include "Platform/Mutex.h"
#include "RCS/RCS.h"

using namespace Perforce;

static u32 g_InitThread = GetCurrentThreadId();

WaitSignature::Delegate Perforce::g_ShowWaitDialog;
MessageSignature::Delegate Perforce::g_ShowWarningDialog;

Profile::Accumulator g_CommandAccum ("Perforce Commands");

#ifdef _DEBUG
//#define PERFORCE_DEBUG_CONNECT
#endif

Provider::Provider()
: m_Enabled ( true )
, m_Connected( false )
, m_Abort ( false )
, m_Shutdown ( false )
, m_Command ( NULL )
, m_Phase ( CommandPhases::Unknown )
{
  if ( IsDebuggerPresent() )
  {
    m_ConnectionTestTimeout = 0xffffffff;
    m_ForegroundExecuteTimeout = 0xffffffff;
    m_BackgroundExecuteTimeout = 0xffffffff;
  }
  else
  {
    m_ConnectionTestTimeout = 100;
    m_ForegroundExecuteTimeout = 5000;
    m_BackgroundExecuteTimeout = 10000;
  }
}

Provider::~Provider()
{
  Cleanup();
}

void Provider::Initialize()
{
  if (!m_Thread.Create(&Platform::Thread::EntryHelper<Provider, &Provider::ThreadEntry>, this, "Perforce Transaction Thread"))
  {
    throw Perforce::Exception ( "Unable to create thread for perforce transaction" );
  }
}

void Provider::Cleanup()
{
  if ( m_Connected && !m_Client.Dropped() )
  {
    Error e;
    m_Client.Final( &e );
  }

  m_Shutdown = true;
  m_Execute.Signal();
  m_Thread.Wait();
}

void Provider::ThreadEntry()
{
  while ( !m_Shutdown )
  {
    m_Execute.Wait();
    m_Execute.Reset();

    if ( m_Command )
    {
      std::string cmd = m_Command->m_Command;
      std::vector< const char* > args;

      V_string::const_iterator itr = m_Command->m_Arguments.begin();
      V_string::const_iterator end = m_Command->m_Arguments.end();
      for ( ; itr != end; ++itr )
      {
        cmd += " " + *itr;
        args.push_back( (*itr).c_str() );
      }

      if ( m_Command->m_Arguments.size() > 0 )
      {
        m_Client.SetArgv( (int)args.size(), const_cast<char**>( &args.front() ) );
      }

      {
        char print[512];
        _snprintf(print, sizeof(print), "Command 'p4 %s'", cmd.c_str());
        PROFILE_SCOPE_ACCUM_VERBOSE( g_CommandAccum, print );
        Console::Debug("%s\n", print);
        m_Client.Run( m_Command->m_Command, m_Command );
      }

      m_Phase = CommandPhases::Complete;
      m_Completed.Signal();
    }
  }
}

void Provider::RunCommand( Command* command )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  if ( !m_Enabled )
  {
    throw Perforce::Exception ("Perforce connection is not enabled");
  }

  const bool foregroundThread = g_InitThread == GetCurrentThreadId();

  Platform::TakeMutex mutex ( m_Mutex );

  m_Abort = false;
  m_Phase = CommandPhases::Unknown;

  //
  // This do/while loop is necessary because...
  //  - if we fail to execute the command we can re-connect (or wait and reconnect), and try the command again:
  //     trying the command again will happen frequently during dropped connection situations, because we cannot
  //     test the dropped status except for immediately after attempting to run a command, so in the case of a 
  //     dropped connection, we will always fail the first try, then re-connect and succeed
  //
  // Watch our for Connect(), don't call it in the main line scenarios
  //  - the most frequent case through this code is while we are already connected, so we don't always call Connect()
  //     because its very expensive... we only bother to call Connect() after we try to run a command and fail this is
  //     because testing the connection with confidence requires sending some sort of command, whether that be info
  //     or the command that the calling api wants executed...
  //

  do
  {
    if ( m_Connected )
    {
      m_Phase = CommandPhases::Executing;

      m_Completed.Reset();
      m_Command = command;
      m_Execute.Signal();

      if ( foregroundThread )
      {
        if ( !m_Completed.Wait( m_ForegroundExecuteTimeout ) )
        {
          if ( g_ShowWaitDialog.Valid() && g_ShowWaitDialog.Invoke( this ) )
          {
            m_Enabled = false;
            m_Abort = true;
          }
        }
      }
      else
      {
        if ( !m_Completed.Wait( m_BackgroundExecuteTimeout ) )
        {
          m_Abort = true;
        }
      }

      // this will wait for the transaction to complete (flagged for abort if the user cancelled it), or fall through if its done
      m_Completed.Wait();
    }

    if ( m_Client.Dropped() )
    {
      m_Connected = false;
    }

    if ( !m_Connected )
    {
      m_ConnectTimer.Reset();

      m_Phase = CommandPhases::Connecting;

      if ( !Connect() )
      {
        if ( foregroundThread )
        {
          // this will poll Connect() in a timer
          if ( g_ShowWaitDialog.Valid() && g_ShowWaitDialog.Invoke( this ) )
          {
            m_Enabled = false;

            if ( g_ShowWarningDialog.Valid() )
            {
              g_ShowWarningDialog.Invoke( MessageArgs ("Warning: Continuing to work without a perforce connection could expose unexpected problems.\n\nPlease consider saving your work and waiting until the connection can be restored.", "Warning" ));
            }

            throw Perforce::Exception ("Failed to connect to perforce server");
          }
        }

        if ( m_Connected )
        {
          Console::Print("Connection to Perforce has been established\n");
        }
      }
    }
  }
  while ( m_Phase != CommandPhases::Complete && m_Enabled && m_Connected && !m_Abort && command->m_ErrorCount == 0 );

  if ( foregroundThread )
  {
    if ( g_ShowWaitDialog.Valid() )
    {
      // this will cancel the UI dialog if the transaction completed (and was not aborted)
      m_Abort = true;
    }
  }
  else
  {
    if ( m_Abort )
    {
      // we timed out in a background thread, throw
      throw Perforce::Exception ("Perforce transaction timed out (timeout = %d)", m_BackgroundExecuteTimeout);
    }
  }

  if ( command->m_ErrorCount )
  {
    NOC_ASSERT_MSG( !command->m_ErrorString.empty(), ("No error string was captured from a failed perforce command, this indicates a command object is not properly interpreting the server's output") );
    throw Perforce::Exception( "%d error%s for command '%s':\n%s", command->m_ErrorCount, command->m_ErrorCount > 1 ? "s" : "", command->AsString().c_str(), command->m_ErrorString.c_str() );
  }
}

bool Provider::Connect()
{
  if ( m_Connected )
  {
    // This extra 'info' command is unfortunate but necessary
    //  .Dropped() can only be trusted immediately after .Run(), so do a lightweight run here to update .Dropped()'s state

    class NullClient : public ClientUser
    {
    public:
      virtual void OutputInfo( char level, const char* data ) {}
      virtual void OutputError( const char* data )            {}
    } nullClient;

    m_Client.SetBreak( this );
    m_Client.Run( "info", &nullClient );
  }

  if ( m_Client.Dropped() )
  {
    Error e;
    m_Client.Final( &e );
    m_Connected = false;

#ifdef PERFORCE_DEBUG_CONNECT
    if ( e.Test() )
    {
      StrBuf buf;
      e.Fmt( &buf, EF_PLAIN );
      Console::Warning( "%s\n", buf.Text() );
    }
#endif
  }

  if ( !m_Connected )
  {
    Error e;
    m_Client.SetProtocol( "tag", "" );
    m_Client.Init( &e );
    m_Client.SetBreak( this );

#ifdef PERFORCE_DEBUG_CONNECT
    if ( e.Test() )
    {
      StrBuf buf;
      e.Fmt( &buf, EF_PLAIN );
      Console::Warning( "%s\n", buf.Text() );
    }
#endif

    char buf[ 64 ];
    sprintf_s( buf, sizeof(buf), "Perforce.dll" );
    buf[ sizeof(buf) - 1 ] = 0; 

    m_Client.SetProg( buf );
    m_UserName = m_Client.GetUser().Text();
    m_ClientName = m_Client.GetClient().Text();
    m_Connected = e.Test() == 0;
  }

  return m_Connected;
}

// this instructs when a p4api transaction should keep waiting (return true to keep waiting)
int	Provider::IsAlive()
{
  switch ( m_Phase )
  {
  case CommandPhases::Connecting:
    {
      return m_ConnectTimer.Elapsed() <= m_ConnectionTestTimeout;
    }

  case CommandPhases::Executing:
    {
      return !m_Abort;
    }

  default:
    {
      // this would not make any sense... why would we be complete in the Alive callback?
      NOC_ASSERT( m_Phase != CommandPhases::Complete );
      return true;
    }
  }
}

// this instruct our waiting UI to stop waiting (return true to stop waiting)
bool Provider::StopWaiting()
{
  switch ( m_Phase )
  {
  case CommandPhases::Executing:
    {
      return m_Abort;
    }

  case CommandPhases::Connecting:
    {
      return Connect();
    }

  default:
    {
      // this is the natural closing of the dialog if the server comes back online and completes successfully
      NOC_ASSERT( m_Phase == CommandPhases::Complete );
      return true;
    }
  }
}

///////////////////////////////////////////////////////////////////
// Implementation

bool Provider::IsEnabled()
{
  return m_Enabled;
}

void Provider::SetEnabled( bool enabled )
{
  m_Enabled = enabled;
}

const char* Provider::GetName()
{
  return "Perforce";
}

void Provider::Sync( RCS::File& file, const u64 timestamp )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  SyncCommand command( this, &file, timestamp );

  command.Run();
}

void Provider::GetInfo( RCS::File& file, const RCS::GetInfoFlag flags )
{
  SingleFStatCommand command( this, &file );
  command.Run();

  if ( ( flags & RCS::GetInfoFlags::GetHistory ) == RCS::GetInfoFlags::GetHistory )
  {
    FileLogCommand filelogCommand( this, &file, ( ( flags & RCS::GetInfoFlags::GetIntegrationHistory ) == RCS::GetInfoFlags::GetIntegrationHistory ) );
    filelogCommand.Run();
  }
}

void Provider::GetInfo( const std::string& folder, RCS::V_File& files, bool recursive, u32 fileData, u32 actionData )
{
  MultiFStatCommand command( this, folder, &files, recursive, fileData, actionData );
  command.Run();
}

void Provider::Add( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command( this, "add", &file );
  command.Run();
}

void Provider::Edit( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command ( this, "edit", &file );
  command.Run();
}

void Provider::Delete( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command ( this, "delete", &file );
  command.Run();
}

void Provider::Integrate( RCS::File& source, RCS::File& dest )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  IntegrateCommand command( this, &source, &dest );
  command.Run();
}

void Provider::Reopen( RCS::File& file )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenCommand command( this, "reopen", &file );
  command.Run();
}

void Provider::GetChangesets( RCS::V_Changeset& changesets )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  GetChangelistsCommand command( this, &changesets );
  command.Run();
}

void Provider::CreateChangeset( RCS::Changeset& changeset )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  CreateChangelistCommand command( this, &changeset );
  command.Run();
}

void Provider::GetOpenedFiles( RCS::V_File& files )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  OpenedCommand command( this, &files );
  command.Run();
}

void Provider::Commit( RCS::Changeset& changeset )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  SubmitCommand command( this, &changeset );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( changeset.m_Id ) );

  command.Run();
}

void Provider::Revert( RCS::Changeset& changeset, bool revertUnchangedOnly )
{
  RevertCommand command( this );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( changeset.m_Id ) );

  if ( revertUnchangedOnly )
  {
    command.AddArg( "-a" );
  }

  command.AddArg( "//..." ); // careful

  command.Run();
}


void Provider::Revert( RCS::File& file, bool revertUnchangedOnly )
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  RevertCommand command( this, &file );

  command.AddArg( "-c" );
  command.AddArg( RCS::GetChangesetIdAsString( file.m_ChangesetId ) );

  if ( revertUnchangedOnly )
  {
    command.AddArg( "-a" );
  }

  command.AddArg( file.m_LocalPath );

  command.Run();
}

void Provider::Rename( RCS::File& source, RCS::File& dest )
{
  IntegrateCommand integrateCommand( this, &source, &dest );
  integrateCommand.Run();

  OpenCommand deleteCommand( this, "delete", &source );
  deleteCommand.Run();
}
