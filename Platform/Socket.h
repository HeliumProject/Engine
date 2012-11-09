#pragma once

#ifdef HELIUM_OS_WIN
# include "Platform/SocketWin.h"
#else
# include "POSIX/Socket.h"
#endif

#include "Platform/Condition.h"

namespace Helium
{
    namespace SocketProtocols
    {
        enum SocketProtocol
        {
            Tcp,
            Udp
        };
    }
    typedef SocketProtocols::SocketProtocol SocketProtocol;

    // Call to initiate/shutdown the subsystem
    HELIUM_PLATFORM_API bool InitializeSockets();
    HELIUM_PLATFORM_API void CleanupSockets();
    HELIUM_PLATFORM_API void CleanupSocketThread();

    // Get the most recent socket error
    HELIUM_PLATFORM_API int GetSocketError();

    // Create/close sockets
    HELIUM_PLATFORM_API bool CreateSocket(Socket& socket, SocketProtocol protocol = SocketProtocols::Tcp);
    HELIUM_PLATFORM_API bool CloseSocket(Socket& socket);

    // Associate the socket with a particular port
    HELIUM_PLATFORM_API bool BindSocket(Socket& socket, uint16_t port);

    // These functions are invalid for connectionless protocols such as UDP
    HELIUM_PLATFORM_API bool ListenSocket(Socket& socket);
    HELIUM_PLATFORM_API bool ConnectSocket(Socket& socket, sockaddr_in* service);
    HELIUM_PLATFORM_API bool AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info);

    // Poll the state of a socket
    HELIUM_PLATFORM_API int SelectSocket(int range, fd_set* read_set, fd_set* write_set, struct timeval* timeout);

    // Use the socket to communicate on a connection based protocol
    HELIUM_PLATFORM_API bool ReadSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate, sockaddr_in *peer = 0);
    HELIUM_PLATFORM_API bool WriteSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate, sockaddr_in *peer = 0);
}
