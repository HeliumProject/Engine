#include "Command.h"
#include "Perforce.h"

#include "Console/Console.h"
#include "Platform/Mutex.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <sstream>

using namespace Perforce;

Profile::Accumulator g_CommandAccum ("Perforce Commands");

// timeouts are in ms
const static u32 g_ConnectionTestTimeout = 100;       // the time we are willing to wait to test if the server is running
const static u32 g_ForegroundExecuteTimeout = 5000;   // the timeout in the foreground thread before we open the wait dialog
const static u32 g_BackgroundExecuteTimeout = 10000;  // this is lame and we should consider opening a new connection per-calling thread?

#ifdef _DEBUG
//#define PERFORCE_DEBUG_CONNECT
#endif

void Command::Run()
{
  PERFORCE_SCOPE_TIMER( ( "" ) );

  if ( !m_Provider->m_Enabled )
  {
    throw Perforce::Exception ("Perforce connection is not enabled");
  }

  Platform::TakeMutex mutex ( *m_Provider->m_Mutex );

  const bool foregroundThread = g_InitThread == GetCurrentThreadId();

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
    if ( m_Provider->m_Connected )
    {
      m_Phase = CommandPhases::Executing;

      if (!m_Thread.Create(&Platform::Thread::EntryHelper<Command, &Command::Execute>, this, "Perforce Transaction Thread"))
      {
        throw Perforce::Exception ( "Unable to create thread for perforce transaction" );
      }

      if ( foregroundThread )
      {
        m_Thread.Wait( g_ForegroundExecuteTimeout );

        if ( m_Thread.Running() )
        {
          if ( g_ShowWaitDialog.Valid() && g_ShowWaitDialog.Invoke( this ) )
          {
            m_Provider->m_Enabled = false;
            m_Abort = true;
          }
        }
      }
      else
      {
        m_Thread.Wait( g_BackgroundExecuteTimeout );

        if ( m_Thread.Running() )
        {
          m_Abort = true;
        }
      }

      // this will wait for the transaction thread to complete (flagged for abort if the user cancelled it), or fall through if its done
      m_Thread.Wait();
    }

    if ( m_Provider->m_Client->Dropped() )
    {
      m_Provider->m_Connected = false;
    }

    if ( !m_Provider->m_Connected )
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
            m_Provider->m_Enabled = false;

            if ( g_ShowWarningDialog.Valid() )
            {
              g_ShowWarningDialog.Invoke( MessageArgs ("Warning: Continuing to work without a perforce connection could expose unexpected problems.\n\nPlease consider saving your work and waiting until the connection can be restored.", "Warning" ));
            }

            throw Perforce::Exception ("Failed to connect to perforce server");
          }
        }

        if ( m_Provider->m_Connected )
        {
          Console::Print("Connection to Perforce has been established\n");
        }
      }
    }
  }
  while ( m_Phase != CommandPhases::Complete && m_Provider->m_Enabled && m_Provider->m_Connected && !m_Abort && m_ErrorCount == 0 );

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
      throw Perforce::Exception ("Perforce transaction timed out (timeout = %d)", g_BackgroundExecuteTimeout);
    }
  }

  if ( m_ErrorCount )
  {
    NOC_ASSERT_MSG( !m_ErrorString.empty(), ("No error string was captured from a failed perforce command, this indicates a command object is not properly interpreting the server's output") );
    throw Perforce::Exception( "%d error%s for command '%s':\n%s", m_ErrorCount, m_ErrorCount > 1 ? "s" : "", AsString().c_str(), m_ErrorString.c_str() );
  }
}

bool Command::Connect()
{
  if ( m_Provider->m_Connected )
  {
    // This extra 'info' command is unfortunate but necessary
    //  .Dropped() can only be trusted immediately after .Run(), so do a lightweight run here to update .Dropped()'s state

    class NullClient : public ClientUser
    {
    public:
      virtual void OutputInfo( char level, const char* data ) {}
      virtual void OutputError( const char* data )            {}
    } nullClient;

    m_Provider->m_Client->SetBreak( this );
    m_Provider->m_Client->Run( "info", &nullClient );
  }

  if ( m_Provider->m_Client->Dropped() )
  {
    Error e;
    m_Provider->m_Client->Final( &e );
    m_Provider->m_Connected = false;

#ifdef PERFORCE_DEBUG_CONNECT
    if ( e.Test() )
    {
      StrBuf buf;
      e.Fmt( &buf, EF_PLAIN );
      Console::Warning( "%s\n", buf.Text() );
    }
#endif
  }

  if ( !m_Provider->m_Connected )
  {
    Error e;
    m_Provider->m_Client->SetProtocol( "tag", "" );
    m_Provider->m_Client->Init( &e );

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

    m_Provider->m_Client->SetProg( buf );
    m_Provider->m_UserName = m_Provider->m_Client->GetUser().Text();
    m_Provider->m_ClientName = m_Provider->m_Client->GetClient().Text();
    m_Provider->m_Connected = e.Test() == 0;
  }

  return m_Provider->m_Connected;
}

void Command::Execute()
{
  std::string cmd = m_Command;
  std::vector< const char* > args;

  V_string::const_iterator itr = m_Arguments.begin();
  V_string::const_iterator end = m_Arguments.end();
  for ( ; itr != end; ++itr )
  {
    cmd += " " + *itr;
    args.push_back( (*itr).c_str() );
  }

  if ( m_Arguments.size() > 0 )
  {
    m_Provider->m_Client->SetArgv( (int)args.size(), const_cast<char**>( &args.front() ) );
  }

  m_Provider->m_Client->SetBreak( this );

  {
    char print[512];
    _snprintf(print, sizeof(print), "Command 'p4 %s'\n", cmd.c_str());
    PROFILE_SCOPE_ACCUM_VERBOSE( g_CommandAccum, print );
    Console::Debug("%s\n", print);
    m_Provider->m_Client->Run( m_Command, this );
  }

  m_Phase = CommandPhases::Complete;
}

// this instructs when a p4api transaction should keep waiting (return true to keep waiting)
int	Command::IsAlive()
{
  switch ( m_Phase )
  {
  case CommandPhases::Connecting:
    {
      return m_ConnectTimer.Elapsed() <= g_ConnectionTestTimeout;
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
bool Command::StopWaiting()
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

std::string Command::AsString()
{
  std::stringstream str;
  str << m_Command;

  V_string::const_iterator itr = m_Arguments.begin();
  V_string::const_iterator end = m_Arguments.end();
  for ( ; itr != end; ++itr )
  {
    str << " " << (*itr);
  }

  return str.str();
}

void Command::HandleError( Error *error )
{
  StrBuf buf;

  if ( error->IsWarning() )
  {
    error->Fmt( &buf );
    Console::Warning( buf.Text() );
  }
  else if ( error->IsError() )
  {
    ++m_ErrorCount;
    error->Fmt( &buf );

    if ( m_ErrorCount == 1 )
    {
      m_ErrorString = buf.Text();
    }
    else
    {
      //
      // Beyond 10 errors just print ellipsis
      //

      if ( m_ErrorCount < 10 )
      {
        m_ErrorString += "\n";
        m_ErrorString += buf.Text();
      }
      else if ( m_ErrorCount == 10 )
      {
        m_ErrorString += "\n...";
      }
    }
  }
  else if ( error->IsFatal() )
  {
    error->Fmt( &buf );
    throw Nocturnal::Exception( buf.Text() );
  }
}

void Command::OutputStat( StrDict* dict )
{
  Console::Warning( "Unhandled perforce response for command '%s':\n", m_Command );
  Console::Indentation indent;

  StrRef var;
  StrRef value;

  for ( int i = 0; i < PERFORCE_MAX_DICT_ENTRIES; ++i )
  {
    if ( !dict->GetVar( i, var, value ) )
    {
      break;
    }
    Console::Warning( "%s: %s\n", var.Text(), value.Text() );
  }

  Console::Warning( "\n" );
}