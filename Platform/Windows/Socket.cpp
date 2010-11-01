#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Socket.h"
#include "Platform/Socket.h"
#include "Platform/Assert.h"
#include "Platform/Platform.h"

#include <mstcpip.h>

using namespace Helium;

HELIUM_COMPILE_ASSERT( sizeof( uint32_t* ) == sizeof( SOCKET ) );
HELIUM_COMPILE_ASSERT( sizeof( Socket::Overlapped ) == sizeof( OVERLAPPED ) );

// in milliseconds
#define KEEPALIVE_TIMEOUT 10000
#define KEEPALIVE_INTERVAL 1000

// globals
int32_t g_Count = 0;
WSADATA g_WSAData;

Socket::Socket(int)
: m_Handle (0)
{
    memset(&m_Overlapped, 0, sizeof(m_Overlapped));
    m_Overlapped.hEvent = ::CreateEvent(0, true, false, 0);
}

Socket::~Socket()
{
    ::CloseHandle( m_Overlapped.hEvent );
}

bool Helium::InitializeSockets()
{
    if ( ++g_Count == 1 )
    {
        int result = WSAStartup(MAKEWORD(2,2), &g_WSAData);
        if (result != NO_ERROR)
        {
            Helium::Print(TXT("TCP Support: Error initializing socket layer (%d)\n"), WSAGetLastError());
            return false;
        }
    }

    return true;
}

void Helium::CleanupSockets()
{
    if ( --g_Count == 0 )
    {
        int result = WSACleanup();
        if (result != NO_ERROR)
        {
            Helium::Print(TXT("TCP Support: Error cleaning up socket layer (%d)\n"), WSAGetLastError());
        }
    }
}

void Helium::CleanupSocketThread()
{

}

int Helium::GetSocketError()
{
    return WSAGetLastError();
}

bool Helium::CreateSocket(Socket& socket)
{
    socket.m_Handle = ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (socket == INVALID_SOCKET)
    {
        return false;
    }

    // I hate Winsock.  This sets a keepalive timeout so that it can detect when
    // a connection is abnormally terminated (like, say, when you reset your ps3!)
    // Otherwise, it never registers a disconnect.
    struct tcp_keepalive keepalive;
    keepalive.onoff = 1;
    keepalive.keepalivetime = KEEPALIVE_TIMEOUT;
    keepalive.keepaliveinterval = KEEPALIVE_INTERVAL;

    DWORD returned;
    int result = ::WSAIoctl( socket, SIO_KEEPALIVE_VALS, &keepalive, sizeof( keepalive ), NULL, 0, &returned, NULL, NULL );
    if ( result == SOCKET_ERROR )
    {
        Helium::Print( TXT("TCP Support: Error setting keep alive on socket (%d)\n"), WSAGetLastError() );
    }

    return true;
}

bool Helium::CloseSocket(Socket& socket)
{
    return ::closesocket(socket) != SOCKET_ERROR;
}

bool Helium::BindSocket(Socket& socket, uint16_t port)
{
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);
    if ( ::bind(socket, (sockaddr*)&service, sizeof(sockaddr_in) ) == SOCKET_ERROR )
    {
        Helium::Print(TXT("TCP Support: Failed to bind socket (%d)\n"), WSAGetLastError());
        closesocket(socket);
        return false;
    }

    return true;
}

bool Helium::ListenSocket(Socket& socket)
{
    if ( ::listen(socket, SOMAXCONN) == SOCKET_ERROR)
    {
        Helium::Print(TXT("TCP Support: Failed to listen socket (%d)\n"), WSAGetLastError());
        closesocket(socket);
        return false;
    }

    return true;
}

bool Helium::ConnectSocket(Socket& socket, sockaddr_in* service)
{
    return ::connect(socket, (SOCKADDR*)service, sizeof(sockaddr_in)) != SOCKET_ERROR;
}

bool Helium::AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info)
{
    int lengthname = sizeof(sockaddr_in);

    socket.m_Handle = ::accept( server_socket, (struct sockaddr *)client_info, &lengthname);

    return socket != SOCKET_ERROR;
}

int Helium::SelectSocket(int range, fd_set* read_set, fd_set* write_set,struct timeval* timeout)
{
    return ::select(range, read_set, write_set, 0, timeout);
}

bool Helium::ReadSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    WSABUF buf;
    buf.buf = (CHAR*)buffer;
    buf.len = bytes;

    DWORD flags = 0;
    DWORD read_local = 0;
    if ( ::WSARecv(socket.m_Handle, &buf, 1, &read_local, &flags, (OVERLAPPED*)&socket.m_Overlapped, NULL) != 0 )
    {
        if ( WSAGetLastError() != WSA_IO_PENDING )
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("TCP Support: Failed to initiate overlapped read (%s)\n", Helium::GetErrorString().c_str());
#endif
            return false;
        }
        else
        {
            HANDLE events[] = { terminate.GetHandle(), socket.m_Overlapped.hEvent };
            DWORD result = ::WSAWaitForMultipleEvents(2, events, FALSE, INFINITE, FALSE);

            HELIUM_ASSERT( result != WAIT_FAILED );

            if ( (result - WAIT_OBJECT_0) == 0 )
            {
#ifdef IPC_TCP_DEBUG_SOCKETS
                Helium::Print("TCP Support: Terminating read\n");
#endif
                return false;
            }

            if ( !::WSAGetOverlappedResult(socket.m_Handle, (OVERLAPPED*)&socket.m_Overlapped, &read_local, false, &flags) )
            {
#ifdef IPC_TCP_DEBUG_SOCKETS
                Helium::Print("TCP Support: Failed read (%s)\n", Helium::GetErrorString().c_str());
#endif
                return false;
            }
        }
    }

    if (read_local == 0)
    {
        return false;
    }

    read = (uint32_t)read_local;

    return true;
}

bool Helium::WriteSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate)
{
    if (bytes == 0)
    {
        return true;
    }

    WSABUF buf;
    buf.buf = (CHAR*)buffer;
    buf.len = bytes;

    DWORD flags = 0;
    DWORD wrote_local = 0;
    if ( ::WSASend(socket.m_Handle, &buf, 1, &wrote_local, 0, (OVERLAPPED*)&socket.m_Overlapped, NULL) != 0 )
    {
        if ( WSAGetLastError() != WSA_IO_PENDING )
        {
#ifdef IPC_TCP_DEBUG_SOCKETS
            Helium::Print("TCP Support: Failed to initiate overlapped write (%s)\n", Helium::GetErrorString().c_str());
#endif
            return false;
        }
        else
        {
            HANDLE events[] = { terminate.GetHandle(), socket.m_Overlapped.hEvent };
            DWORD result = ::WSAWaitForMultipleEvents(2, events, FALSE, INFINITE, FALSE);

            HELIUM_ASSERT( result != WAIT_FAILED );

            if ( (result - WAIT_OBJECT_0) == 0 )
            {
#ifdef IPC_TCP_DEBUG_SOCKETS
                Helium::Print("TCP Support: Terminating write\n");
#endif
                return false;
            }

            if ( !::WSAGetOverlappedResult(socket.m_Handle, (OVERLAPPED*)&socket.m_Overlapped, &wrote_local, false, &flags) )
            {
#ifdef IPC_TCP_DEBUG_SOCKETS
                Helium::Print("TCP Support: Failed write (%s)\n", Helium::GetErrorString().c_str());
#endif
                return false;
            }
        }
    }

    if (wrote_local == 0)
    {
        return false;
    }

    wrote = (uint32_t)wrote_local;

    return true;
}