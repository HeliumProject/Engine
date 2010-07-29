#include "Platform/Socket.h"

#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Platform;

bool Platform::InitializeSockets()
{
    return true;
}

void Platform::CleanupSockets()
{

}

void Platform::CleanupSocketThread()
{
#if 0
    // Cleaning up just a single thread doesn't seem to actually work
    i32 ret = sys_net_free_thread_context( 0, SYS_NET_THREAD_ALL );

    if ( ret < 0 )
    {
        Platform::Print( "TCP Support: Failed to cleanup thread context (%d)\n", Platform::GetSocketError() );
    }
#endif

    HELIUM_BREAK();
}

int Platform::GetSocketError()
{
#if 0
    return sys_net_errno;
#endif

    HELIUM_BREAK();
    return -1;
}

bool Platform::CreateSocket(Socket& socket)
{
#if 0
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket < 0)
    {
        Platform::Print("TCP Support: Failed to create socket %d (%d)\n", socket, Platform::GetSocketError());
        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::CloseSocket(Socket& socket)
{
#if 0
    // don't bother to check for errors here, as this socket may not have been communicated through yet
    shutdown(socket, SHUT_RDWR);

    if (socketclose(socket) < 0)
    {
        Platform::Print("TCP Support: Failed to close socket %d (%d)\n", socket, Platform::GetSocketError());
        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::BindSocket(Socket& socket, u16 port)
{
#if 0
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);
    if ( ::bind(socket, (sockaddr*)&service, sizeof(sockaddr_in) ) < 0 )
    {
        Platform::Print("TCP Support: Failed to bind socket %d (%d)\n", socket, Platform::GetSocketError());

        if (shutdown(socket, SHUT_RDWR) < 0)
        {
            HELIUM_BREAK();
        }
        if (socketclose(socket) < 0) {
            HELIUM_BREAK();
        }

        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::ListenSocket(Socket& socket)
{
#if 0
    if (::listen(socket, 5) < 0)
    {
        Platform::Print("TCP Support: Failed to listen socket %d (%d)\n", socket, Platform::GetSocketError());

        if (shutdown(socket, SHUT_RDWR) < 0)
        {
            HELIUM_BREAK();
        }
        if (socketclose(socket) < 0)
        {
            HELIUM_BREAK();
        }

        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::ConnectSocket(Socket& socket, sockaddr_in* service)
{
#if 0
    return ::connect(socket, (struct sockaddr *)service, sizeof(sockaddr_in)) >= 0;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info)
{
#if 0
    socklen_t lengthname = sizeof(sockaddr_in);

    socket = ::accept( server_socket, (struct sockaddr *)client_info, &lengthname );

    return socket > 0;
#endif

    HELIUM_BREAK();
    return false;
}

int Platform::SelectSocket(int range, fd_set* read_set, fd_set* write_set, struct timeval* timeout)
{
#if 0
    return ::socketselect(range, read_set, write_set, 0, timeout);
#endif

    HELIUM_BREAK();
    return -1;
}

bool Platform::ReadSocket(Socket& socket, void* buffer, u32 bytes, u32& read, Event& terminate)
{
#if 0
    i32 local_read = ::recv( socket, (tchar*)buffer, bytes, 0 );

    if (local_read < 0)
    {
        return false;
    }

    read = local_read;

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Platform::WriteSocket(Socket& socket, void* buffer, u32 bytes, u32& wrote, Event& terminate)
{
#if 0
    i32 local_wrote = ::send( socket, (tchar*)buffer, bytes, 0 );

    if (local_wrote < 0)
    {
        return false;
    }

    wrote = local_wrote;

    return true;
#endif

    HELIUM_BREAK();
    return false;
}
