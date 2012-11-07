#include "FoundationPch.h"
#include "IPCPipe.h"

#include "Platform/Assert.h"
#include "Platform/Print.h"

#include "Foundation/Endian.h"
#include "Foundation/String.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

using namespace Helium;
using namespace Helium::IPC;

PipeConnection::PipeConnection()
{
    m_PipeName[0] = '\0';
    m_ServerName[0] = '\0';

    m_ReadName[0] = '\0';
    m_WriteName[0] = '\0';
}

PipeConnection::~PipeConnection()
{
    // other threads still need our object's virtual functions, so call this in the derived destructor
    Cleanup();
}

bool PipeConnection::Initialize(bool server, const tchar_t* name, const tchar_t* pipe_name, const tchar_t* server_name)
{
    if (!Connection::Initialize(server, name))
    {
        return false;
    }

    if (pipe_name && pipe_name[0] != '\0')
    {
        CopyString(m_PipeName, pipe_name);
    }

    if (server_name && server_name[0] != '\0')
    {
        CopyString(m_ServerName, server_name);
    }

    if (server)
    {
        // a server cannot be on a remote machine so we simply ignore the server name
        StringPrint(m_ReadName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "server_read");
        StringPrint(m_WriteName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "client_read");
    }
    else
    {
        // if the server name is null then create the pipe on the local machine, if a server name
        // is specified then it names the remote machine on which to connect to.
        if (server_name && StringLength(server_name) != 0)
        {
            StringPrint(m_ReadName, TXT( "\\\\%s\\pipe\\%s_%s" ), m_ServerName, m_PipeName, "client_read");
            StringPrint(m_WriteName, TXT( "\\\\%s\\pipe\\%s_%s" ), m_ServerName, m_PipeName, "server_read");
        }
        else
        {
            StringPrint(m_ReadName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "client_read");
            StringPrint(m_WriteName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "server_read");
        }
    }

    SetState(ConnectionStates::Waiting);

    Helium::CallbackThread::Entry serverEntry = Helium::CallbackThread::EntryHelper<PipeConnection, &PipeConnection::ServerThread>;
    Helium::CallbackThread::Entry clientEntry = Helium::CallbackThread::EntryHelper<PipeConnection, &PipeConnection::ClientThread>;
    if (!m_ConnectThread.Create(server ? serverEntry : clientEntry, this, TXT("IPC Connection Thread")))
    {
        Helium::Print( TXT( "%s: Failed to create connect thread\n" ), m_Name);
        SetState(ConnectionStates::Failed);
        return false;  
    }

    return true;
}

void PipeConnection::ServerThread()
{
    Helium::Print( TXT( "%s: Starting pipe server '%s'\n" ), m_Name, m_PipeName);

    // while the server is still running, cycle through connections
    while (!m_Terminating)
    {
        if ( !m_ReadPipe.Create( m_ReadName ) )
        {
            SetState(ConnectionStates::Failed);
            return;
        }

        if ( !m_WritePipe.Create( m_WriteName ) )
        {
            SetState(ConnectionStates::Failed);
            return;
        }

        Helium::Print( TXT( "%s: Ready for client\n" ), m_Name, m_PipeName);

        // wait for the connection
        while (!m_Terminating)
        {
            bool readConnect = m_ReadPipe.Connect( m_Terminate );
            bool writeConnect = m_WritePipe.Connect( m_Terminate );

            if (readConnect && writeConnect)
            {
                break;
            }
            else
            {
                Thread::Sleep(100);
            }
        }

        if (!m_Terminating)
        {
            // do connection
            ConnectThread();
        }

        // force disconnect of the client
        m_ReadPipe.Disconnect();
        m_WritePipe.Disconnect();

        // release the handles to the pipes
        m_ReadPipe.Close();
        m_WritePipe.Close();

        if (!m_Terminating)
        {
            // reset back to waiting for connections
            SetState(ConnectionStates::Waiting);
        }
    }

    Helium::Print( TXT( "%s: Stopping pipe server '%s'\n" ), m_Name, m_PipeName);
}

void PipeConnection::ClientThread()
{
    Helium::Print( TXT( "%s: Starting pipe client '%s'\n" ), m_Name, m_PipeName);

    while (!m_Terminating)
    {
        Helium::Print( TXT( "%s: Ready for server\n" ), m_Name, m_PipeName);

        // wait for write pipe creation
        while (!m_Terminating)
        {
            bool writeOpen = m_WritePipe.Open( m_WriteName );
            bool readOpen = m_ReadPipe.Open( m_ReadName );

            if (writeOpen && readOpen)
            {
                break;
            }
            else
            {
                Thread::Sleep(100);
            }
        }

        if (!m_Terminating)
        {
            // do connection
            ConnectThread();
        }

        // close the handles to the pipes
        m_ReadPipe.Close();
		m_WritePipe.Close();

        if (!m_Terminating)
        {
            // return to waiting
            SetState(ConnectionStates::Waiting);
        }
    }

    Helium::Print( TXT( "%s: Stopping pipe client '%s'\n" ), m_Name, m_PipeName);
}

bool PipeConnection::ReadMessage(Message** msg)
{
    IPC_SCOPE_TIMER("");

    {
        IPC_SCOPE_TIMER("Read Message Header");

        if ( !Read(&m_ReadHeader,sizeof(m_ReadHeader)) )
        {
            return false;
        }

#ifdef WIN32
        if ( m_RemotePlatform != (Helium::Platform::Type)-1 )
        {
            m_ReadHeader.m_ID = ConvertEndian(m_ReadHeader.m_ID, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_ReadHeader.m_TRN = ConvertEndian(m_ReadHeader.m_TRN, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_ReadHeader.m_Size = ConvertEndian(m_ReadHeader.m_Size, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_ReadHeader.m_Type = ConvertEndian(m_ReadHeader.m_Type, m_RemotePlatform != Helium::Platform::Types::Windows);
        }
#endif
    }

    // make a message with the received m_ReadHeader and fill it
    Message* message = CreateMessage(m_ReadHeader.m_ID,m_ReadHeader.m_Size,m_ReadHeader.m_TRN, m_ReadHeader.m_Type);

    // out of memory condition
    if ( message == NULL )
    {
        return true;
    }

    uint8_t* data = message->GetData();

    // out of memory condition #2
    if ( m_ReadHeader.m_Size > 0 && data == NULL )
    {
        delete message;
        message = NULL;
        return true;
    }

    {
        IPC_SCOPE_TIMER("Read Message Data");

        if ( !Read(data, m_ReadHeader.m_Size) )
        {
            delete message;
            message = NULL;
            return false;
        }
    }

    *msg = message;

    return true;
}

bool PipeConnection::WriteMessage(Message* msg)
{
    IPC_SCOPE_TIMER("");

    {
        IPC_SCOPE_TIMER("Write Message Header");

        m_WriteHeader.m_ID = msg->GetID();
        m_WriteHeader.m_TRN = msg->GetTransaction();
        m_WriteHeader.m_Size = msg->GetSize();
        m_WriteHeader.m_Type = msg->GetType();

#ifdef WIN32
        if ( m_RemotePlatform != (Helium::Platform::Type)-1 )
        {
            m_WriteHeader.m_ID = ConvertEndian(m_WriteHeader.m_ID, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_WriteHeader.m_TRN = ConvertEndian(m_WriteHeader.m_TRN, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_WriteHeader.m_Size = ConvertEndian(m_WriteHeader.m_Size, m_RemotePlatform != Helium::Platform::Types::Windows);
            m_WriteHeader.m_Type = ConvertEndian(m_WriteHeader.m_Type, m_RemotePlatform != Helium::Platform::Types::Windows);
        }
#endif

        // write the m_WriteHeader
        if ( !Write(&m_WriteHeader, sizeof(m_WriteHeader)) )
        {
            return false;
        }
    }

    {
        IPC_SCOPE_TIMER("Write Message Data");

        // write the data
        if ( !Write(msg->GetData(), msg->GetSize()) )
        {
            return false;
        }
    }

    return true; 
}

bool PipeConnection::Read(void* buffer, uint32_t bytes)
{  
#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Helium::Print("%s: Starting read of %d bytes\n", m_Name, bytes);
#endif

    uint32_t bytes_left = bytes;
    uint32_t bytes_got = 0;

    while (bytes_left>0)
    {
        uint32_t count = std::min(bytes_left, IPC_PIPE_BUFFER_SIZE);

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Helium::Print(" %s: Receiving %d bytes...\n", m_Name, count);
#endif

        if (!m_ReadPipe.Read(buffer, count, bytes_got, m_Terminate))
        {
            return false;
        }

        if (m_Terminating)
        {
            return false;
        }

        bytes_left -= bytes_got;
        buffer = ((uint8_t*)buffer) + bytes_got;

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Helium::Print(" %s: Got %d bytes, %d bytes to go\n", m_Name, bytes_got, bytes_left);
#endif
    }

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Helium::Print("%s: Completed read of %d bytes\n", m_Name, bytes);
#endif

    return true;
}

bool PipeConnection::Write(void* buffer, uint32_t bytes)
{
#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Helium::Print("%s: Starting write of %d bytes\n", m_Name, bytes);
#endif

    uint32_t bytes_left = bytes;
    uint32_t bytes_put = 0;

    while (bytes_left>0)
    {
        uint32_t count = std::min(bytes_left, IPC_PIPE_BUFFER_SIZE);

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Helium::Print(" %s: Sending %d bytes...\n", m_Name, count);
#endif

        if (!m_WritePipe.Write(buffer, count, bytes_put, m_Terminate))
        {
            return false;
        }

        if (m_Terminating)
        {
            return false;
        }

        bytes_left -= bytes_put;
        buffer = ((uint8_t*)buffer) + bytes_put;

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Helium::Print(" %s: Put %d bytes, %d bytes to go\n", m_Name, bytes_put, bytes_left);
#endif
    }

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Helium::Print("%s: Completed write of %d bytes\n", m_Name, bytes);
#endif

    return true;
}
