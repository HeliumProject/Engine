#pragma once

#ifdef WIN32
# include "Windows/Socket.h"
#else
# include "POSIX/Socket.h"
#endif

#include "Condition.h"

namespace Helium
{
    PLATFORM_API bool InitializeSockets();
    PLATFORM_API void CleanupSockets();
    PLATFORM_API void CleanupSocketThread();

    PLATFORM_API int GetSocketError();

    PLATFORM_API bool CreateSocket(Socket& socket);
    PLATFORM_API bool CloseSocket(Socket& socket);

    PLATFORM_API bool BindSocket(Socket& socket, uint16_t port);
    PLATFORM_API bool ListenSocket(Socket& socket);
    PLATFORM_API bool ConnectSocket(Socket& socket, sockaddr_in* service);
    PLATFORM_API bool AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info);

    PLATFORM_API int SelectSocket(int range, fd_set* read_set, fd_set* write_set, struct timeval* timeout);

    PLATFORM_API bool ReadSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate);
    PLATFORM_API bool WriteSocket(Socket& socket, void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate);
}
