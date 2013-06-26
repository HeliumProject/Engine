#include "ApplicationPch.h"
#include "WorkerProcess.h"

#include "Platform/Exception.h"

#include "Foundation/Log.h"
#include "Foundation/IPCPipe.h"
#include "Foundation/Exception.h"

#include "Application/CmdLine.h"
#include "Application/Startup.h"

#include <sstream>

using namespace Helium;
using namespace Helium::Worker;

const char* Worker::Args::Worker  = TXT( "worker" );
const char* Worker::Args::Debug   = TXT( "worker_debug" );
const char* Worker::Args::Wait    = TXT( "worker_wait" );

// the worker processes for a master application
std::set< Helium::SmartPtr< Worker::Process > > g_Workers;

// Called from Debug if an exception occurs
static void TerminateListener(const Helium::TerminateArgs& args)
{
    // murder!
    Process::ReleaseAll();
}

// Called from Application on Shutdown()
static void ShutdownListener(const Helium::ShutdownArgs& args)
{
    // murder!
    Process::ReleaseAll();
}

Process* Process::Create( const std::string& executable, bool debug, bool wait )
{
    static bool firstCreate = true;

    if (firstCreate)
    {
        firstCreate = false;

        // hook up our handler to Application
        Helium::g_ShuttingDown.Add( &ShutdownListener );

        // hook up our handler to debug
        Helium::g_Terminating.Add( &TerminateListener );
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

Process::Process( const std::string& executable, bool debug, bool wait )
: m_Executable (executable)
, m_Handle ( InvalidProcessHandle )
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
    std::string str = m_Executable;
    str += TXT( " " );
    str += Helium::CmdLineDelimiters[0];
    str += Worker::Args::Worker;

    // make our worker wait forever is we were asked to
    if ( m_Wait )
    {
        str += TXT( " " );
        str += Helium::CmdLineDelimiters[0];
        str += Worker::Args::Wait;
        timeout = -1;
    }

    // Start the child process.
    if( !m_Debug )
    {
        m_Handle = Spawn( str.c_str() );
        if ( m_Handle == 0 )
        {
            throw Helium::Exception( TXT( "Failed to run '%s' (%s)\n" ), str.c_str(), Helium::GetErrorString().c_str() );
        }

        // create the server side of the connection
        IPC::PipeConnection* connection = new IPC::PipeConnection ();

        // init pipe connection with background process' process id (hex)
        std::ostringstream stream;

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

        // mutex from kill
        Helium::MutexScopeLock mutex ( m_KillMutex );

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
                if ( !SpawnRunning( m_Handle ) )
                {
                    break;
                }
            }

            Thread::Yield();
        }

        if ( m_Connection && m_Connection->GetState() != IPC::ConnectionStates::Active )
        {
            Kill();

            return false;
        }

        return true;
    }

    return false;
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
                Thread::Yield();

                m_Connection->Receive(&msg);
            }
        }
    }

    return msg;
}

bool Process::Send(uint32_t id, uint32_t size, const uint8_t* data)
{
    // mutex from kill
    Helium::MutexScopeLock mutex ( m_KillMutex );

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
        return SpawnRunning( m_Handle );
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
        int code = SpawnResult( m_Handle );

        delete m_Connection;
        m_Connection = NULL;

        return code;
    }

    return m_Debug ? 0 : -1;
}

void Process::Kill()
{
    if ( m_Handle )
    {
        Helium::MutexScopeLock mutex ( m_KillMutex );

        m_Killed = true;

        if ( m_Connection && m_Connection->GetState() == IPC::ConnectionStates::Active )
        {
            SpawnKill( m_Handle );
        }

        delete m_Connection;
        m_Connection = NULL;
    }
}