#include "ApplicationPch.h"
#include "WorkerClient.h"

#include "Foundation/Log.h"
#include "Platform/Exception.h"
#include "Foundation/IPCPipe.h"
#include "Foundation/Exception.h"

#include "Application/CmdLine.h"
#include "Application/Startup.h"
#include "Application/WorkerProcess.h"

#include <sstream>

using namespace Helium;
using namespace Helium::Worker;

// the ipc connection for worker applications
IPC::Connection* g_Connection = NULL;

// Background processes hook functions for message emission
static void PrintedListener(Log::PrintedArgs& args)
{
    if (g_Connection && g_Connection->GetState() == IPC::ConnectionStates::Active)
    {
        uint32_t size = sizeof( Worker::ConsoleOutput ) + (uint32_t)args.m_Statement.m_String.length() + 1;

        IPC::Message* msg = g_Connection->CreateMessage( Worker::ConsoleOutputMessage, size );

        ConsoleOutput* output = (ConsoleOutput*)msg->GetData();
        output->m_Stream = args.m_Statement.m_Stream;
        output->m_Level = args.m_Statement.m_Level;
        output->m_Indent = args.m_Statement.m_Indent;
        memcpy(output->m_String, args.m_Statement.m_String.c_str(), args.m_Statement.m_String.length() + 1);

        if (g_Connection->Send(msg) != IPC::ConnectionStates::Active)
        {
            delete msg;
        }
    }
}

// Called from Application on Shutdown()
static void ShutdownListener(const Helium::ShutdownArgs& args)
{
    // release our connection to our manager process
    Client::Cleanup();
}

// Called from Debug if an exception occurs
static void TerminateListener(const Helium::TerminateArgs& args)
{
    // release our connection to our manager process
    Client::Cleanup();
}

bool Client::Initialize( bool debug, bool wait )
{
    IPC::PipeConnection* connection = new IPC::PipeConnection ();

    // init pipe connection with this process' process id (hex)
    tostringstream stream;

    if ( debug )
    {
        stream << TXT( "worker_debug" );
    }
    else
    {
        stream << TXT( "worker_" ) << std::hex << ::GetProcessId(GetCurrentProcess());
    }

    connection->Initialize(false, TXT( "Worker Process Connection" ), stream.str().c_str());

    // setup global connection
    g_Connection = connection;

    // wait a while for connection
    int timeout = DefaultWorkerTimeout;

    // wait forever is we are asked to
    if ( wait )
    {
        timeout = -1;
    }

    while ( timeout-- != 0 && g_Connection->GetState() != IPC::ConnectionStates::Active )
    {
        Sleep( 1 );
    }

    // error out with an exception if we didnt' connect
    if (g_Connection->GetState() != IPC::ConnectionStates::Active)
    {
        Log::Error( TXT( "Timeout connecting to manager process" ) );
        return false;
    }

    // hook up our handler to Console
    Log::AddPrintedListener( Log::PrintedSignature::Delegate (&PrintedListener) );

    // hook up our handler to Application
    Helium::g_ShuttingDown.Add( &ShutdownListener );

    // hook up our handler to debug
    Helium::g_Terminating.Add( &TerminateListener );

    return true;
}

void Client::Cleanup()
{
    if (g_Connection)
    {
        delete g_Connection;
        g_Connection = NULL;
    }
}

IPC::Message* Client::Receive(bool wait)
{
    IPC::Message* msg = NULL;

    if (g_Connection)
    {
        g_Connection->Receive(&msg);

        if (!msg && wait)
        {
            while (g_Connection->GetState() == IPC::ConnectionStates::Active && !msg)
            {
                Sleep(0);

                g_Connection->Receive(&msg);
            }
        }
    }

    return msg;
}

bool Client::Send(uint32_t id, uint32_t size, const uint8_t* data)
{
    if (g_Connection && g_Connection->GetState() == IPC::ConnectionStates::Active)
    {
        IPC::Message* msg = g_Connection->CreateMessage(id, data ? size : 0);

        if (data && size)
        {
            memcpy(msg->GetData(), data, size);
        }

        if (g_Connection->Send(msg) == IPC::ConnectionStates::Active)
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