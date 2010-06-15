#include "Platform/Windows/Windows.h"
#include "Process.h"

#include <sstream>

// libraries
#include "Debug/Exception.h"
#include "Application/Application.h"
#include "Foundation/CommandLine.h"
#include "Foundation/Log.h"
#include "Foundation/Exception.h"
#include "Foundation/IPC/Pipe.h"

using namespace Worker;

const char* Worker::Args::Worker  = "worker";
const char* Worker::Args::Debug   = "worker_debug";
const char* Worker::Args::Wait    = "worker_wait";

// the worker processes for a master application
std::set< Nocturnal::SmartPtr< Worker::Process > > g_Workers;

// Called from Debug if an exception occurs
static void TerminateListener(const Debug::TerminateArgs& args)
{
  // murder!
  Process::ReleaseAll();
}

// Called from Application on Shutdown()
static void ShutdownListener(const Application::ShutdownArgs& args)
{
  // murder!
  Process::ReleaseAll();
}

Process* Process::Create( const std::string& executable )
{
  static bool firstCreate = true;

  if (firstCreate)
  {
    firstCreate = false;

    // hook up our handler to Application
    Application::g_ShuttingDown.Add( &ShutdownListener );

    // hook up our handler to debug
    Debug::g_Terminating.Add( &TerminateListener );
  }

  return g_Workers.insert( new Process ( executable ) ).first->Ptr();
}

void Process::Release( Process*& worker )
{
  g_Workers.erase( worker );
  worker = NULL;
}

void Process::ReleaseAll()
{
  g_Workers.clear();
}

Process::Process(const std::string& executable)
: m_Executable (executable)
, m_Handle (NULL)
, m_Connection (NULL)
, m_Killed (false)
{

}

Process::~Process()
{
  Kill();
}

bool Process::Start(int timeout)
{
  std::string str = m_Executable;
  str += " ";
  str += Nocturnal::CmdLineDelimiters[0];
  str += Worker::Args::Worker;

  // make our worker wait forever is we were asked to
  if (Nocturnal::GetCmdLineFlag( Worker::Args::Wait ))
  {
    str += " ";
    str += Nocturnal::CmdLineDelimiters[0];
    str += Worker::Args::Wait;
    timeout = -1;
  }

  STARTUPINFO startupInfo;
  memset( &startupInfo, 0, sizeof( startupInfo ) );
  startupInfo.cb = sizeof( startupInfo );

  PROCESS_INFORMATION procInfo;
  memset( &procInfo, 0, sizeof( procInfo ) );

  DWORD flags = 0;

#ifdef _DEBUG
  flags |= CREATE_NEW_CONSOLE;
#else
  flags |= CREATE_NO_WINDOW;
#endif

  // Start the child process.
  if( !Nocturnal::GetCmdLineFlag( Worker::Args::Debug ) && !::CreateProcess( NULL, (LPTSTR) str.c_str(), NULL, NULL, FALSE, flags, NULL, NULL, &startupInfo, &procInfo ) )
  {
    throw Nocturnal::Exception ( "Failed to run '%s' (%s)\n", str.c_str(), Platform::GetErrorString().c_str() );
  }
  else
  {
    // save this for query later
    m_Handle = procInfo.hProcess;

    // create the server side of the connection
    IPC::PipeConnection* connection = new IPC::PipeConnection ();

    // init pipe connection with background process' process id (hex)
    std::ostringstream stream;

    if (Nocturnal::GetCmdLineFlag( Worker::Args::Debug ))
    {
      stream << "worker_debug";
    }
    else
    {
      stream << "worker_" << std::hex << GetProcessId( m_Handle );
    }

    connection->Initialize(true, "Worker Process Connection", stream.str().c_str());

    // setup global connection
    m_Connection = connection;

    // release handles to our new process
    ::CloseHandle( procInfo.hThread );

    // mutex from kill
    Platform::TakeMutex mutex ( m_KillMutex );

    while ( timeout-- != 0 )
    {
      if ( m_Killed )
      {
        break;
      }

      if ( m_Connection && m_Connection->GetState() == IPC::ConnectionStates::Active )
      {
        break;
      }

      if ( m_Handle )
      {
        DWORD code = 0x0;
        ::GetExitCodeProcess( m_Handle, &code );
        if ( code != STILL_ACTIVE )
        {
          break;
        }
      }

      Sleep( 1 );
    }

    if ( m_Connection && m_Connection->GetState() != IPC::ConnectionStates::Active )
    {
      Kill();

      return false;
    }

    return true;
  }
}

IPC::Message* Process::Receive(bool wait)
{
  IPC::Message* msg = NULL;

  if (m_Connection)
  {
    m_Connection->Receive(&msg);

    if (!msg && wait)
    {
      while (m_Connection->GetState() == IPC::ConnectionStates::Active && !msg)
      {
        Sleep(1);
        
        m_Connection->Receive(&msg);
      }
    }
  }

  return msg;
}

bool Process::Send(u32 id, u32 size, const u8* data)
{
  // mutex from kill
  Platform::TakeMutex mutex ( m_KillMutex );

  if ( m_Connection && m_Connection->GetState() == IPC::ConnectionStates::Active )
  {
    IPC::Message* msg = m_Connection->CreateMessage(id, data ? size : 0);

    if (data && size)
    {
      memcpy(msg->GetData(), data, size);
    }

    if (m_Connection->Send(msg) == IPC::ConnectionStates::Active)
    {
      return true;
    }
    else
    {
      delete msg;
    }
  }

  return false;
}

bool Process::Running()
{
  if ( m_Handle )
  {
    return WaitForSingleObject( m_Handle, 0 ) == WAIT_TIMEOUT;
  }
  else
  {
    return Nocturnal::GetCmdLineFlag( Worker::Args::Debug );
  }
}

int Process::Finish()
{
  if ( m_Handle )
  {
    WaitForSingleObject( m_Handle, INFINITE );
    
    DWORD code;
    GetExitCodeProcess( m_Handle, &code );

    ::CloseHandle( m_Handle );
    m_Handle = NULL;

    delete m_Connection;
    m_Connection = NULL;

    return (int)code;
  }

  return Nocturnal::GetCmdLineFlag( Worker::Args::Debug ) ? 0 : -1;
}

void Process::Kill()
{
  if ( m_Handle )
  {
    Platform::TakeMutex mutex ( m_KillMutex );

    m_Killed = true;

    if ( m_Connection && m_Connection->GetState() == IPC::ConnectionStates::Active )
    {
      TerminateProcess( m_Handle, -1 );
    }

    ::CloseHandle( m_Handle );
    m_Handle = NULL;

    delete m_Connection;
    m_Connection = NULL;
  }
}