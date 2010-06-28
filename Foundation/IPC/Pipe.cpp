#include "Pipe.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/Endian.h"

#include <stdio.h>
#include <string.h>
#include <algorithm>

using namespace IPC;

PipeConnection::PipeConnection()
: m_ReadPipe (0)
, m_WritePipe (0)
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

bool PipeConnection::Initialize(bool server, const tchar* name, const tchar* pipe_name, const tchar* server_name)
{
    if (!Connection::Initialize(server, name))
    {
        return false;
    }

    if (pipe_name && pipe_name[0] != '\0')
    {
        _tcscpy(m_PipeName, pipe_name);
    }

    if (server_name && server_name[0] != '\0')
    {
        _tcscpy(m_ServerName, server_name);
    }

    if (server)
    {
        // a server cannot be on a remote machine so we simply ignore the server name
        _stprintf(m_ReadName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "server_read");
        _stprintf(m_WriteName, TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "client_read");
    }
    else
    {
        // if the server name is null then create the pipe on the local machine, if a server name
        // is specified then it names the remote machine on which to connect to.
        if (server_name && _tcslen(server_name) != 0)
        {
            _stprintf(m_ReadName, IPC_PIPE_ROOT TXT( "\\\\%s\\pipe\\%s_%s" ), m_ServerName, m_PipeName, "client_read");
            _stprintf(m_WriteName, IPC_PIPE_ROOT TXT( "\\\\%s\\pipe\\%s_%s" ), m_ServerName, m_PipeName, "server_read");
        }
        else
        {
            _stprintf(m_ReadName, IPC_PIPE_ROOT TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "client_read");
            _stprintf(m_WriteName, IPC_PIPE_ROOT TXT( "\\\\.\\pipe\\%s_%s" ), m_PipeName, "server_read");
        }
    }

    SetState(ConnectionStates::Waiting);

    Platform::Thread::Entry serverEntry = Platform::Thread::EntryHelper<PipeConnection, &PipeConnection::ServerThread>;
    Platform::Thread::Entry clientEntry = Platform::Thread::EntryHelper<PipeConnection, &PipeConnection::ClientThread>;
    if (!m_ConnectThread.Create(server ? serverEntry : clientEntry, this, TXT( "IPC Connection Thread" ) ))
    {
        Platform::Print( TXT( "%s: Failed to create connect thread\n" ), m_Name);
        SetState(ConnectionStates::Failed);
        return false;  
    }

    return true;
}

void PipeConnection::ServerThread()
{
    Platform::Print( TXT( "%s: Starting pipe server '%s'\n" ), m_Name, m_PipeName);

    // while the server is still running, cycle through connections
    while (!m_Terminating)
    {
        if ( !Platform::CreatePipe( m_ReadName, m_ReadPipe ) )
        {
            SetState(ConnectionStates::Failed);
            return;
        }

        if ( !Platform::CreatePipe( m_WriteName, m_WritePipe ) )
        {
            SetState(ConnectionStates::Failed);
            return;
        }

        Platform::Print( TXT( "%s: Ready for client\n" ), m_Name, m_PipeName);

        // wait for the connection
        while (!m_Terminating)
        {
            bool readConnect = Platform::ConnectPipe( m_ReadPipe, m_Terminate );
            bool writeConnect = Platform::ConnectPipe( m_WritePipe, m_Terminate );

            if (readConnect && writeConnect)
            {
                break;
            }
            else
            {
                Platform::Sleep(100);
            }
        }

        if (!m_Terminating)
        {
            // do connection
            ConnectThread();
        }

        // force disconnect of the client
        Platform::DisconnectPipe(m_ReadPipe);
        Platform::DisconnectPipe(m_WritePipe);

        // release the handles to the pipes
        Platform::ClosePipe(m_ReadPipe);
        Platform::ClosePipe(m_WritePipe);

        if (!m_Terminating)
        {
            // reset back to waiting for connections
            SetState(ConnectionStates::Waiting);
        }
    }

    Platform::Print( TXT( "%s: Stopping pipe server '%s'\n" ), m_Name, m_PipeName);
}

void PipeConnection::ClientThread()
{
    Platform::Print( TXT( "%s: Starting pipe client '%s'\n" ), m_Name, m_PipeName);

    while (!m_Terminating)
    {
        Platform::Print( TXT( "%s: Ready for server\n" ), m_Name, m_PipeName);

        // wait for write pipe creation
        while (!m_Terminating)
        {
            bool writeOpen = Platform::OpenPipe( m_WriteName, m_WritePipe );
            bool readOpen = Platform::OpenPipe( m_ReadName, m_ReadPipe );

            if (writeOpen && readOpen)
            {
                break;
            }
            else
            {
                Platform::Sleep(100);
            }
        }

        if (!m_Terminating)
        {
            // do connection
            ConnectThread();
        }

        // close the handles to the pipes
        Platform::ClosePipe(m_ReadPipe);
        Platform::ClosePipe(m_WritePipe);

        if (!m_Terminating)
        {
            // return to waiting
            SetState(ConnectionStates::Waiting);
        }
    }

    Platform::Print( TXT( "%s: Stopping pipe client '%s'\n" ), m_Name, m_PipeName);
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
        if ( m_RemotePlatform != (Platform::Type)-1 )
        {
            m_ReadHeader.m_ID = ConvertEndian(m_ReadHeader.m_ID, m_RemotePlatform != Platform::Types::Windows);
            m_ReadHeader.m_TRN = ConvertEndian(m_ReadHeader.m_TRN, m_RemotePlatform != Platform::Types::Windows);
            m_ReadHeader.m_Size = ConvertEndian(m_ReadHeader.m_Size, m_RemotePlatform != Platform::Types::Windows);
            m_ReadHeader.m_Type = ConvertEndian(m_ReadHeader.m_Type, m_RemotePlatform != Platform::Types::Windows);
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

    u8* data = message->GetData();

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
        if ( m_RemotePlatform != (Platform::Type)-1 )
        {
            m_WriteHeader.m_ID = ConvertEndian(m_WriteHeader.m_ID, m_RemotePlatform != Platform::Types::Windows);
            m_WriteHeader.m_TRN = ConvertEndian(m_WriteHeader.m_TRN, m_RemotePlatform != Platform::Types::Windows);
            m_WriteHeader.m_Size = ConvertEndian(m_WriteHeader.m_Size, m_RemotePlatform != Platform::Types::Windows);
            m_WriteHeader.m_Type = ConvertEndian(m_WriteHeader.m_Type, m_RemotePlatform != Platform::Types::Windows);
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

bool PipeConnection::Read(void* buffer, u32 bytes)
{  
#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Platform::Print("%s: Starting read of %d bytes\n", m_Name, bytes);
#endif

    u32 bytes_left = bytes;
    u32 bytes_got = 0;

    while (bytes_left>0)
    {
        u32 count = std::min(bytes_left, IPC_PIPE_BUFFER_SIZE);

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Platform::Print(" %s: Receiving %d bytes...\n", m_Name, count);
#endif

        if (!Platform::ReadPipe(m_ReadPipe, buffer, count, bytes_got, m_Terminate))
        {
            return false;
        }

        if (m_Terminating)
        {
            return false;
        }

        bytes_left -= bytes_got;
        buffer = ((u8*)buffer) + bytes_got;

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Platform::Print(" %s: Got %d bytes, %d bytes to go\n", m_Name, bytes_got, bytes_left);
#endif
    }

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Platform::Print("%s: Completed read of %d bytes\n", m_Name, bytes);
#endif

    return true;
}

bool PipeConnection::Write(void* buffer, u32 bytes)
{
#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Platform::Print("%s: Starting write of %d bytes\n", m_Name, bytes);
#endif

    u32 bytes_left = bytes;
    u32 bytes_put = 0;

    while (bytes_left>0)
    {
        u32 count = std::min(bytes_left, IPC_PIPE_BUFFER_SIZE);

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Platform::Print(" %s: Sending %d bytes...\n", m_Name, count);
#endif

        if (!Platform::WritePipe(m_WritePipe, buffer, count, bytes_put, m_Terminate))
        {
            return false;
        }

        if (m_Terminating)
        {
            return false;
        }

        bytes_left -= bytes_put;
        buffer = ((u8*)buffer) + bytes_put;

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
        Platform::Print(" %s: Put %d bytes, %d bytes to go\n", m_Name, bytes_put, bytes_left);
#endif
    }

#ifdef IPC_PIPE_DEBUG_PIPES_CHUNKS
    Platform::Print("%s: Completed write of %d bytes\n", m_Name, bytes);
#endif

    return true;
}
