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
        const static uint32_t IPC_TCP_BUFFER_SIZE = 32 << 10;

        class HELIUM_FOUNDATION_API TCPConnection : public Connection
        {
        private:
            tchar_t             m_IP[64];                       // ip of the server

            uint16_t               m_ReadPort;                     // port number for read operations
            Helium::Socket  m_ReadSocket;                   // socket used for read operations

            uint16_t               m_WritePort;                    // port number for write operations
            Helium::Socket  m_WriteSocket;                  // socket used for write operations

        public:
            TCPConnection();
            virtual ~TCPConnection();

        public:
            bool Initialize(bool server, const tchar_t* name, const tchar_t* server_ip, const uint16_t server_port_no);

        protected:
            void ServerThread();
            void ClientThread();

            virtual void CleanupThread();
            virtual bool ReadMessage(Message** msg);
            virtual bool WriteMessage(Message* msg);
            virtual bool Read(void* buffer, uint32_t bytes);
            virtual bool Write(void* buffer, uint32_t bytes);
        };
    }
}