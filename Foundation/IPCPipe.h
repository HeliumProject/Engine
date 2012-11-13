#pragma once

#include "Platform/Pipe.h"

#include "Foundation/IPC.h"

// Debug printing
//#define IPC_PIPE_DEBUG_PIPES
//#define IPC_PIPE_DEBUG_PIPES_CHUNKS

namespace Helium
{
    namespace IPC
    {
        class HELIUM_FOUNDATION_API PipeConnection : public Connection
        {
        private:
            tchar_t              m_PipeName[256];                // name of the pipe passed in by the user
            tchar_t              m_ServerName[256];              // name of the server passed in by the user

            tchar_t              m_ReadName[256];                // name of the pipe
            Helium::Pipe    m_ReadPipe;                     // handle of the pipe

            tchar_t              m_WriteName[256];               // name of the pipe
            Helium::Pipe    m_WritePipe;                    // handle of the pipe

        public:
            PipeConnection();
            virtual ~PipeConnection();

        public:
            bool Initialize(bool server, const tchar_t* name, const tchar_t* pipe_name, const tchar_t* server_name = 0);

        protected:
            void ServerThread();
            void ClientThread();

            virtual bool ReadMessage(Message** msg);
            virtual bool WriteMessage(Message* msg);
            virtual bool Read(void* buffer, uint32_t bytes);
            virtual bool Write(void* buffer, uint32_t bytes);
        };
    }
}