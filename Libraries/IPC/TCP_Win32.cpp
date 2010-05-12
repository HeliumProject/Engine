#include "Windows/Windows.h"
#include "Windows/Error.h"

#include "TCP.h"
#include "Common/Assert.h"

#include <mstcpip.h>

using namespace IPC;

// in milliseconds
#define KEEPALIVE_TIMEOUT 10000
#define KEEPALIVE_INTERVAL 1000

// globals
i32 g_Count = 0;
WSADATA g_WSAData;

bool Platform::InitializeSockets()
{
  if ( ++g_Count == 1 )
  {
    int result = WSAStartup(MAKEWORD(2,2), &g_WSAData);
    if (result != NO_ERROR)
    {
      Platform::Print("TCP Support: Error initializing socket layer (%d)\n", WSAGetLastError());
      return false;
    }
  }

  return true;
}

void Platform::CleanupSockets()
{
  if ( --g_Count == 0 )
  {
    int result = WSACleanup();
    if (result != NO_ERROR)
    {
      Platform::Print("TCP Support: Error cleaning up socket layer (%d)\n", WSAGetLastError());
    }
  }
}

void Platform::CleanupSocketThread()
{

}

int Platform::GetSocketError()
{
  return WSAGetLastError();
}

bool Platform::CreateSocket(Socket& socket)
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
    Platform::Print( "TCP Support: Error setting keep alive on socket (%d)\n", WSAGetLastError() );
  }

  return true;
}

bool Platform::CloseSocket(Socket& socket)
{
  return ::closesocket(socket) != SOCKET_ERROR;
}

bool Platform::BindSocket(Socket& socket, u16 port)
{
  sockaddr_in service;
  service.sin_family = AF_INET;
  service.sin_addr.s_addr = INADDR_ANY;
  service.sin_port = htons(port);
  if ( ::bind(socket, (sockaddr*)&service, sizeof(sockaddr_in) ) == SOCKET_ERROR )
  {
    Platform::Print("TCP Support: Failed to bind socket (%d)\n", WSAGetLastError());
    closesocket(socket);
    return false;
  }

  return true;
}

bool Platform::ListenSocket(Socket& socket)
{
  if ( ::listen(socket, SOMAXCONN) == SOCKET_ERROR)
  {
    Platform::Print("TCP Support: Failed to listen socket (%d)\n", WSAGetLastError());
    closesocket(socket);
    return false;
  }

  return true;
}

bool Platform::ConnectSocket(Socket& socket, sockaddr_in* service)
{
  return ::connect(socket, (SOCKADDR*)service, sizeof(sockaddr_in)) != SOCKET_ERROR;
}

bool Platform::AcceptSocket(Socket& socket, Socket& server_socket, sockaddr_in* client_info)
{
  int lengthname = sizeof(sockaddr_in);

  socket.m_Handle = ::accept( server_socket, (struct sockaddr *)client_info, &lengthname);

  return socket != SOCKET_ERROR;
}

int Platform::SelectSocket(int range, fd_set* read_set, fd_set* write_set,struct timeval* timeout)
{
  return ::select(range, read_set, write_set, 0, timeout);
}

bool Platform::ReadSocket(Socket& socket, void* buffer, u32 bytes, u32& read, Event& terminate)
{
  if (bytes == 0)
  {
    return true;
  }

  WSABUF buf;
  buf.buf = (char*)buffer;
  buf.len = bytes;

  DWORD flags = 0;
  DWORD read_local = 0;
  if ( ::WSARecv(socket.m_Handle, &buf, 1, &read_local, &flags, &socket.m_Overlapped, NULL) != 0 )
  {
    if ( WSAGetLastError() != WSA_IO_PENDING )
    {
#ifdef IPC_TCP_DEBUG_SOCKETS
      Platform::Print("TCP Support: Failed to initiate overlapped read (%s)\n", Windows::GetErrorString().c_str());
#endif
      return false;
    }
    else
    {
      HANDLE events[] = { terminate.GetHandle(), socket.m_Overlapped.hEvent };
      DWORD result = ::WSAWaitForMultipleEvents(2, events, FALSE, INFINITE, FALSE);

      NOC_ASSERT( result != WAIT_FAILED );

      if ( (result - WAIT_OBJECT_0) == 0 )
      {
#ifdef IPC_TCP_DEBUG_SOCKETS
        Platform::Print("TCP Support: Terminating read\n");
#endif
        return false;
      }

      if ( !::WSAGetOverlappedResult(socket.m_Handle, &socket.m_Overlapped, &read_local, false, &flags) )
      {
#ifdef IPC_TCP_DEBUG_SOCKETS
        Platform::Print("TCP Support: Failed read (%s)\n", Windows::GetErrorString().c_str());
#endif
        return false;
      }
    }
  }

  if (read_local == 0)
  {
    return false;
  }

  read = (u32)read_local;

  return true;
}

bool Platform::WriteSocket(Socket& socket, void* buffer, u32 bytes, u32& wrote, Event& terminate)
{
  if (bytes == 0)
  {
    return true;
  }

  WSABUF buf;
  buf.buf = (char*)buffer;
  buf.len = bytes;

  DWORD flags = 0;
  DWORD wrote_local = 0;
  if ( ::WSASend(socket.m_Handle, &buf, 1, &wrote_local, 0, &socket.m_Overlapped, NULL) != 0 )
  {
    if ( WSAGetLastError() != WSA_IO_PENDING )
    {
#ifdef IPC_TCP_DEBUG_SOCKETS
      Platform::Print("TCP Support: Failed to initiate overlapped write (%s)\n", Windows::GetErrorString().c_str());
#endif
      return false;
    }
    else
    {
      HANDLE events[] = { terminate.GetHandle(), socket.m_Overlapped.hEvent };
      DWORD result = ::WSAWaitForMultipleEvents(2, events, FALSE, INFINITE, FALSE);

      NOC_ASSERT( result != WAIT_FAILED );

      if ( (result - WAIT_OBJECT_0) == 0 )
      {
#ifdef IPC_TCP_DEBUG_SOCKETS
        Platform::Print("TCP Support: Terminating write\n");
#endif
        return false;
      }

      if ( !::WSAGetOverlappedResult(socket.m_Handle, &socket.m_Overlapped, &wrote_local, false, &flags) )
      {
#ifdef IPC_TCP_DEBUG_SOCKETS
        Platform::Print("TCP Support: Failed write (%s)\n", Windows::GetErrorString().c_str());
#endif
        return false;
      }
    }
  }

  if (wrote_local == 0)
  {
    return false;
  }

  wrote = (u32)wrote_local;

  return true;
}