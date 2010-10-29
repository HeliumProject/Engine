#include "Platform/API.h"
#include "TCP.h"

#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Foundation/Memory/Endian.h"

#include <string.h>
#include <algorithm>

using namespace Helium::IPC;

#define IPC_TCP_NO_DELAY

// {TIMEOUT_SECS, TIMEOUT_USECS}
struct timeval g_TimeoutWaiting = {0, 17000};

TCPConnection::TCPConnection()
: m_ReadPort (0)
, m_ReadSocket (0)
, m_WritePort (0)
, m_WriteSocket (0)
{
    m_IP[0] = '\0';
}

TCPConnection::~TCPConnection()
{
    // other threads still need our object's virtual functions, so call this in the derived destructor
    Cleanup();
}

bool TCPConnection::Initialize(bool server, const tchar* name, const tchar* server_ip, const uint16_t server_port)
{
    if (!Connection::Initialize( server, name ))
    {
        return false;
    }

    if (server_ip)
    {
        _tcscpy(m_IP, server_ip);
    }

    if (server)
    {
        m_ReadPort = server_port;
        m_WritePort = server_port + 1;
    }
    else
    {
        m_ReadPort = server_port + 1;
        m_WritePort = server_port;
    }

    SetState(ConnectionStates::Waiting);

    Helium::Thread::Entry serverEntry = Helium::Thread::EntryHelper<TCPConnection, &TCPConnection::ServerThread>;
    Helium::Thread::Entry clientEntry = Helium::Thread::EntryHelper<TCPConnection, &TCPConnection::ClientThread>;
    if (!m_ConnectThread.Create(server ? serverEntry : clientEntry, this, "IPC Connection Thread" ))
    {
        Helium::Print( TXT( "%s: Failed to create connect thread\n" ), m_Name);
        SetState(ConnectionStates::Failed);
        return false;  
    }

    return true;
}

void TCPConnection::ServerThread()
{
    Helium::InitializeSockets();

    Helium::Print( TXT( "%s: Starting TCP server (ports %d, %d)\n" ), m_Name, m_ReadPort, m_WritePort);

    Helium::Socket server_read_socket = 0;
    if (!Helium::CreateSocket(server_read_socket))
    {
        SetState(ConnectionStates::Failed);
        return;
    }

    Helium::Socket server_write_socket = 0;
    if (!Helium::CreateSocket(server_write_socket))
    {
        SetState(ConnectionStates::Failed);
        return;
    }

    if (!Helium::BindSocket(server_read_socket, m_ReadPort) || !Helium::ListenSocket(server_read_socket))
    {
        Helium::CloseSocket(server_read_socket);
        Helium::CloseSocket(server_write_socket);
        SetState(ConnectionStates::Failed);
        return;
    }

    if (!Helium::BindSocket(server_write_socket, m_WritePort) || !Helium::ListenSocket(server_write_socket))
    {
        Helium::CloseSocket(server_read_socket);
        Helium::CloseSocket(server_write_socket);
        SetState(ConnectionStates::Failed);
        return;
    }

    // while the server is still running, cycle through connections
    while (!m_Terminating)
    {
        Helium::Print( TXT( "%s: Ready for client\n" ), m_Name);

        // wait for a connection
        while (!m_Terminating)
        {
            fd_set read_fd_sock_set;
            FD_ZERO(&read_fd_sock_set);
            FD_SET(server_read_socket, &read_fd_sock_set);
            int readSelectResult = Helium::SelectSocket((int)server_read_socket + 1, &read_fd_sock_set, NULL, &g_TimeoutWaiting);

            fd_set write_fd_sock_set;
            FD_ZERO(&write_fd_sock_set);
            FD_SET(server_write_socket, &write_fd_sock_set);
            int writeSelectResult = Helium::SelectSocket((int)server_write_socket + 1, &write_fd_sock_set, NULL, &g_TimeoutWaiting);

            if (readSelectResult > 0 && writeSelectResult > 0)
            {
                HELIUM_ASSERT( FD_ISSET(server_read_socket, &read_fd_sock_set) );
                HELIUM_ASSERT( FD_ISSET(server_write_socket, &write_fd_sock_set) );
                break;
            }
            else
            {
                Helium::Sleep( 100 );
            }
        }

        if (!m_Terminating)
        {
            // we should have incoming data, accept the connection
            sockaddr_in client_info;
            if (!Helium::AcceptSocket(m_ReadSocket, server_read_socket, &client_info))
            {
                Helium::CloseSocket(server_read_socket);
                Helium::CloseSocket(server_write_socket);
                SetState(ConnectionStates::Failed);
                return;
            }
            if (!Helium::AcceptSocket(m_WriteSocket, server_write_socket, &client_info))
            {
                Helium::CloseSocket(server_read_socket);
                Helium::CloseSocket(server_write_socket);
                SetState(ConnectionStates::Failed);
                return;
            }

            int result;
            socklen_t buf_size = IPC_TCP_BUFFER_SIZE;
            socklen_t size_size = sizeof(IPC_TCP_BUFFER_SIZE);
            result = setsockopt(m_ReadSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_ReadSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_WriteSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_WriteSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&buf_size, size_size);
            Helium::Print( TXT( "%s: Accepted connection (%dk/%dk)\n" ), m_Name, IPC_TCP_BUFFER_SIZE >> 10, IPC_TCP_BUFFER_SIZE >> 10);

#ifdef IPC_TCP_NO_DELAY
            int flag = 1;
            result = setsockopt(m_ReadSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(int));
            result = setsockopt(m_WriteSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(int));
#endif

            // do connection
            ConnectThread();
        }

        if (!m_Terminating)
        {
            // reset back to waiting for connections
            SetState(ConnectionStates::Waiting);
        }
    }

    Helium::CloseSocket(server_read_socket);
    Helium::CloseSocket(server_write_socket);

    Helium::Print( TXT( "%s: Stopping TCP server (ports %d, %d)\n" ), m_Name, m_ReadPort, m_WritePort);

    CleanupThread();

    Helium::CleanupSockets();
}

void TCPConnection::ClientThread()
{
    Helium::InitializeSockets();

    Helium::Print( TXT( "%s: Starting TCP client (%s: %d, %d)\n" ), m_Name, m_IP, m_ReadPort, m_WritePort);

    std::string ip;
    bool converted = Helium::ConvertString( m_IP, ip );
    HELIUM_ASSERT( converted );

    Helium::Print( TXT( "%s: Resolving '%s'\n" ), m_Name, ip.c_str());
    hostent* hostInfo = gethostbyname(ip.c_str());
    if ( hostInfo )
    {
        sockaddr_in sockAddr;
        memcpy(&sockAddr.sin_addr, hostInfo->h_addr, hostInfo->h_length);
        ip = inet_ntoa(sockAddr.sin_addr);
        Helium::Print( TXT( "%s: IP is '%s'\n" ), m_Name, ip.c_str());
    }

    sockaddr_in client_service;
    client_service.sin_family = AF_INET;
    client_service.sin_addr.s_addr = inet_addr(ip.c_str());

    while (!m_Terminating)
    {
        Helium::Print( TXT( "%s: Ready for server\n" ), m_Name);

        bool socketsCreated = false;

        while (!m_Terminating)
        {
            if (!Helium::CreateSocket(m_WriteSocket))
            {
                SetState(ConnectionStates::Failed);
                return;
            }

            if (!Helium::CreateSocket(m_ReadSocket))
            {
                SetState(ConnectionStates::Failed);
                return;
            }

            socketsCreated = true;

            client_service.sin_port = htons(m_WritePort);
            bool connectWrite = Helium::ConnectSocket(m_WriteSocket, &client_service);

            client_service.sin_port = htons(m_ReadPort);
            bool connectRead = Helium::ConnectSocket(m_ReadSocket, &client_service);

            if (connectWrite && connectRead)
            {
                break;
            }
            else
            {
                Helium::CloseSocket(m_WriteSocket);
                Helium::CloseSocket(m_ReadSocket);
                Helium::Sleep( 100 );
                socketsCreated = false;
            }
        }

        if (!m_Terminating)
        {
            int result;
            socklen_t buf_size = IPC_TCP_BUFFER_SIZE;
            socklen_t size_size = sizeof(IPC_TCP_BUFFER_SIZE);
            result = setsockopt(m_ReadSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_ReadSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_WriteSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&buf_size, size_size);
            result = setsockopt(m_WriteSocket, SOL_SOCKET, SO_SNDBUF, (const char*)&buf_size, size_size);
            Helium::Print( TXT( "%s: Connection established (%dk/%dk)\n" ), m_Name, IPC_TCP_BUFFER_SIZE / 1024, IPC_TCP_BUFFER_SIZE / 1024);

#ifdef IPC_TCP_NO_DELAY
            int flag = 1;
            result = setsockopt(m_ReadSocket, IPPROTO_TCP, TCP_NODELAY, (const char*) &flag, sizeof(int));
            result = setsockopt(m_WriteSocket, IPPROTO_TCP, TCP_NODELAY, (const char*) &flag, sizeof(int));
#endif

            // do connection
            ConnectThread();
        }

        if (socketsCreated)
        {
            Helium::CloseSocket(m_WriteSocket);
            Helium::CloseSocket(m_ReadSocket);
        }

        if (!m_Terminating)
        {
            // reset back to waiting for connections
            SetState(ConnectionStates::Waiting);
        }
    }

    Helium::Print( TXT( "%s: Stopping TCP client (%s: %d, %d)\n" ), m_Name, m_IP, m_ReadPort, m_WritePort);

    CleanupThread();

    Helium::CleanupSockets();
}

void TCPConnection::CleanupThread()
{
    Helium::CleanupSocketThread();
}

bool TCPConnection::ReadMessage(Message** msg)
{
    IPC_SCOPE_TIMER("");

    {
        IPC_SCOPE_TIMER("Read Message Header");

        if (!Read(&m_ReadHeader,sizeof(m_ReadHeader)))
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("%s: Failed to read header from socket (%d)\n", m_Name, Helium::GetSocketError());
#endif
            return false;
        }
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

    IPC::Message* message = CreateMessage(m_ReadHeader.m_ID,m_ReadHeader.m_Size,m_ReadHeader.m_TRN, m_ReadHeader.m_Type);

    // out of memory condition
    if ( message == NULL )
    {
        Helium::Print( TXT( "%s: Failed to allocate memory for message\n" ), m_Name);
        return false;
    }

    uint8_t* data = message->GetData();

    // out of memory condition #2
    if ( m_ReadHeader.m_Size > 0 && data == NULL )
    {
        Helium::Print( TXT( "%s: Failed to allocate memory for message data\n" ), m_Name);
        delete message;
        return false;
    }

    {
        IPC_SCOPE_TIMER("Read Message Data");

        if (!Read(data,m_ReadHeader.m_Size))
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("%s: Failed to read message data (%d)\n", m_Name, Helium::GetSocketError());
#endif

            delete message;
            return false;
        }
    }

    *msg = message;

    return true;
}

bool TCPConnection::WriteMessage(Message* msg)
{
    IPC_SCOPE_TIMER("");

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

    {
        IPC_SCOPE_TIMER("Write Message Header");
        if (!Write( &m_WriteHeader, sizeof( m_WriteHeader ) ))
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("%s: Failed to write message header\n", m_Name);
#endif

            return false;
        }
    }

    {
        IPC_SCOPE_TIMER("Write Message Data");
        if (!Write( msg->GetData(), msg->GetSize() ))
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("%s::Failed to write message data\n", m_Name);
#endif

            return false;
        }
    }

    return true;
}

bool TCPConnection::Read(void* buffer, uint32_t bytes)
{  
#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
    Helium::Print("%s: Starting read of %d bytes\n", m_Name, bytes);
#endif

    uint32_t bytes_left = bytes;
    uint32_t bytes_got = 0;

    while (bytes_left > 0)
    {
        uint32_t count = std::min<uint32_t>(bytes_left, IPC_TCP_BUFFER_SIZE);

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
        Helium::Print(" %s: Receiving %d bytes...\n", m_Name, count);
#endif

        if (!Helium::ReadSocket( m_ReadSocket, buffer, count, bytes_got, m_Terminate ))
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print( "%s: ReadSocket failed\n", m_Name );
#endif
            return false;
        }

        if (m_Terminating || bytes_got == 0)
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print( "%s: Terminating or 0 bytes read\n", m_Name );
#endif
            return false;
        }

        bytes_left -= bytes_got;
        buffer = ((uint8_t*)buffer) + bytes_got;

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
        Helium::Print(" %s: Got %d bytes, %d bytes to go\n", m_Name, bytes_got, bytes_left);
#endif
    }

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
    Helium::Print("%s: Completed read of %d bytes\n", m_Name, bytes);
#endif

    return true;
}

bool TCPConnection::Write(void* buffer, uint32_t bytes)
{
#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
    Helium::Print("%s: Starting write of %d bytes\n", m_Name, bytes);
#endif

    uint32_t bytes_left = bytes;
    uint32_t bytes_put = 0;

    while (bytes_left > 0)
    {
        uint32_t count = std::min<uint32_t>(bytes_left, IPC_TCP_BUFFER_SIZE);

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
        Helium::Print(" %s: Sending %d bytes...\n", m_Name, count);
#endif

        if (!Helium::WriteSocket( m_WriteSocket, buffer, count, bytes_put, m_Terminate ))
        {
            return false;
        }

        if (m_Terminating)
        {
            return false;
        }

        bytes_left -= bytes_put;
        buffer = ((uint8_t*)buffer) + bytes_put;

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
        Helium::Print(" %s: Put %d bytes, %d bytes to go\n", m_Name, bytes_put, bytes_left);
#endif
    }

#ifdef IPC_TCP_DEBUG_SOCKETS_CHUNKS
    Helium::Print("%s: Completed write of %d bytes\n", m_Name, bytes);
#endif

    return true;
}
