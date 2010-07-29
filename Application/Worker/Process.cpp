#include "Platform/Windows/Windows.h"
#include "Process.h"

#include <sstream>

#include "Foundation/Log.h"
#include "Platform/Exception.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/IPC/Pipe.h"

#include "Application/Application.h"
#include "Application/Exception.h"

using namespace Worker;

const tchar* Worker::Args::Worker  = TXT( "worker" );
const tchar* Worker::Args::Debug   = TXT( "worker_debug" );
const tchar* Worker::Args::Wait    = TXT( "worker_wait" );

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

Process* Process::Create( const tstring& executable, bool debug, bool wait )
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

    return g_Workers.insert( new Process ( executable, debug, wait ) ).first->Ptr();
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

Process::Process( const tstring& executable, bool debug, bool wait )
: m_Executable (executable)
, m_Handle (NULL)
, m_Connection (NULL)
, m_Killed (false)
, m_Debug( debug )
, m_Wait( wait )
{

}

Process::~Process()
{
    Kill();
}

bool Process::Start( int timeout )
{
    tstring str = m_Executable;
    str += TXT( " " );
    str += Nocturnal::CmdLineDelimiters[0];
    str += Worker::Args::Worker;

    // make our worker wait forever is we were asked to
    if ( m_Wait )
    {
        str += TXT( " " );
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
    if( !m_Debug && !::CreateProcess( NULL, (LPTSTR) str.c_str(), NULL, NULL, FALSE, flags, NULL, NULL, &startupInfo, &procInfo ) )
    {
        throw Nocturnal::Exception( TXT( "Failed to run '%s' (%s)\n" ), str.c_str(), Platform::GetErrorString().c_str() );
    }
    else
    {
        // save this for query later
        m_Handle = procInfo.hProcess;

        // create the server side of the connection
        IPC::PipeConnection* connection = new IPC::PipeConnection ();

        // init pipe connection with background process' process id (hex)
        tostringstream stream;

        if ( m_Debug )
        {
            stream << TXT( "worker_debug" );
        }
        else
        {
            stream << TXT( "worker_" ) << std::hex << GetProcessId( m_Handle );
        }

        connection->Initialize(true, TXT( "Worker Process Connection" ), stream.str().c_str());

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
        return m_Debug;
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

    return m_Debug ? 0 : -1;
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