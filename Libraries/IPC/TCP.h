#pragma once

#ifdef WIN32
# include "TCP_Win32.h"
#else
# include "TCP_PS3.h"
#endif

#include "Connection.h"

// Debug printing
//#define IPC_TCP_DEBUG_SOCKETS
//#define IPC_TCP_DEBUG_SOCKETS_CHUNKS

namespace IPC
{
  const static u32 IPC_TCP_BUFFER_SIZE = 32 << 10;

  class IPC_API TCPConnection : public Connection
  {
  private:
    char              m_IP[64];                       // ip of the server

    u16               m_ReadPort;                     // port number for read operations
    Socket            m_ReadSocket;                   // socket used for read operations

    u16               m_WritePort;                    // port number for write operations
    Socket            m_WriteSocket;                  // socket used for write operations

  public:
    TCPConnection();
    virtual ~TCPConnection();

  public:
    bool Initialize(bool server, const char* name, const char* server_ip, const u16 server_port_no);

  protected:
    void ServerThread();
    void ClientThread();

    virtual void CleanupThread();
    virtual bool ReadMessage(Message** msg);
    virtual bool WriteMessage(Message* msg);
    virtual bool Read(void* buffer, u32 bytes);
    virtual bool Write(void* buffer, u32 bytes);
  };
}

//
// Platform-specific prototypes
//

namespace Platform
{
  bool InitializeSockets();
  void CleanupSockets();
  void CleanupSocketThread();

  int GetSocketError();

  bool CreateSocket(IPC::Socket& socket);
  bool CloseSocket(IPC::Socket& socket);

  bool BindSocket(IPC::Socket& socket, u16 port);
  bool ListenSocket(IPC::Socket& socket);
  bool ConnectSocket(IPC::Socket& socket, sockaddr_in* service);
  bool AcceptSocket(IPC::Socket& socket, IPC::Socket& server_socket, sockaddr_in* client_info);

  int SelectSocket(int range, fd_set* read_set, fd_set* write_set, struct timeval* timeout);

  bool ReadSocket(IPC::Socket& socket, void* buffer, u32 bytes, u32& read, Event& terminate);
  bool WriteSocket(IPC::Socket& socket, void* buffer, u32 bytes, u32& wrote, Event& terminate);
}
