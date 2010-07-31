#pragma once

#include "Platform/Socket.h"

#include "IPC.h"
#include "Connection.h"

// Debug printing
//#define IPC_TCP_DEBUG_SOCKETS
//#define IPC_TCP_DEBUG_SOCKETS_CHUNKS

namespace Helium
{
    namespace IPC
    {
        const static u32 IPC_TCP_BUFFER_SIZE = 32 << 10;

        class FOUNDATION_API TCPConnection : public Connection
        {
        private:
            tchar             m_IP[64];                       // ip of the server

            u16               m_ReadPort;                     // port number for read operations
            Helium::Socket  m_ReadSocket;                   // socket used for read operations

            u16               m_WritePort;                    // port number for write operations
            Helium::Socket  m_WriteSocket;                  // socket used for write operations

        public:
            TCPConnection();
            virtual ~TCPConnection();

        public:
            bool Initialize(bool server, const tchar* name, const tchar* server_ip, const u16 server_port_no);

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
}