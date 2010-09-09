#include "Platform/Socket.h"

#include "Platform/Platform.h"
#include "Platform/Assert.h"

using namespace Helium;

bool Helium::InitializeSockets()
{
    return true;
}

void Helium::CleanupSockets()
{

}

void Helium::CleanupSocketThread()
{
#ifdef PS3_POSIX
    // Cleaning up just a single thread doesn't seem to actually work
    i32 ret = sys_net_free_thread_context( 0, SYS_NET_THREAD_ALL );

    if ( ret < 0 )
    {
        Helium::Print( "TCP Support: Failed to cleanup thread context (%d)\n", Helium::GetSocketError() );
    }
#endif

    HELIUM_BREAK();
}

int Helium::GetSocketError()
{
#ifdef PS3_POSIX
    return sys_net_errno;
#endif

    HELIUM_BREAK();
    return -1;
}

bool Helium::CreateSocket(Socket& socket)
{
#ifdef PS3_POSIX
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (socket < 0)
    {
        Helium::Print("TCP Support: Failed to create socket %d (%d)\n", socket, Helium::GetSocketError());
        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Helium::CloseSocket(Socket& socket)
{
#ifdef PS3_POSIX
    // don't bother to check for errors here, as this socket may not have been communicated through yet
    shutdown(socket, SHUT_RDWR);

    if (socketclose(socket) < 0)
    {
        Helium::Print("TCP Support: Failed to close socket %d (%d)\n", socket, Helium::GetSocketError());
        return false;
    }

    return true;
#endif

    HELIUM_BREAK();
    return false;
}

bool Helium::BindSocket(Socket& socket, u16 port)
{
#ifdef PS3_POSIX
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);
    if ( ::bind(socket, (sockaddr*)&service, sizeof(sockaddr_in) ) < 0 )
    {
        Helium::Print("TCP Support: Failed to bind socket %d (%d)\n", socket, Helium::GetSocketError());

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

bool Helium::ListenSocket(Socket& socket)
{
#ifdef PS3_POSIX
    if (::listen(socket, 5) < 0)
    {
        Helium::Print("TCP Support: Failed to listen socket %d (%d)\n", socket, Helium::GetSocketError());

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

bool Helium::ConnectSocket(Socket& socket, sockaddr_in* service)
{
#ifdef PS3_POSIX
    return ::connect(socket, (struct sockaddr *)service, sizeof(sockaddr_in)) >= 0;
#endif

    HELIUM_BREAK();
    return false;
}

bool Helium::AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info)
{
#ifdef PS3_POSIX
    socklen_t lengthname = sizeof(sockaddr_in);

    socket = ::accept( server_socket, (struct sockaddr *)client_info, &lengthname );

    return socket > 0;
#endif

    HELIUM_BREAK();
    return false;
}

int Helium::SelectSocket(int range, fd_set* read_set, fd_set* write_set, struct timeval* timeout)
{
#ifdef PS3_POSIX
    return ::socketselect(range, read_set, write_set, 0, timeout);
#endif

    HELIUM_BREAK();
    return -1;
}

bool Helium::ReadSocket(Socket& socket, void* buffer, u32 bytes, u32& read, Condition& terminate)
{
#ifdef PS3_POSIX
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

bool Helium::WriteSocket(Socket& socket, void* buffer, u32 bytes, u32& wrote, Condition& terminate)
{
#ifdef PS3_POSIX
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
