#pragma once

#include "Platform/Types.h"
#include "Platform/Mutex.h"

#include "Foundation/Log.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/API.h"

namespace Helium
{
    namespace IPC
    {
        class Message;
        class Connection;
    }

    namespace Worker
    {
        // This is the allotted time (in millis) that:
        //  - Manager's Process::Create() will wait for IPC connection from the running Worker process
        //  - Worker's Worker::Initialize function will wait for a connection with the Manager process
        const static int DefaultWorkerTimeout = 10000;

        struct FOUNDATION_API Args
        {
            static const tchar_t* Worker;
            static const tchar_t* Debug;
            static const tchar_t* Wait;
        };

#pragma warning ( disable: 4200 )
        struct ConsoleOutput
        {
            Log::Stream m_Stream;
            Log::Level  m_Level;
            int         m_Indent;
            tchar_t       m_String[0];
        };

        const static uint32_t ConsoleOutputMessage = 0;
#pragma warning ( default: 4200 )

        class FOUNDATION_API Process : public Helium::RefCountBase
        {
        private:
            // the file to execute
            tstring m_Executable;

            // the process handle
            void* m_Handle;

            // the process IPC connection
            IPC::Connection* m_Connection;

            // mutex to handles killing the process
            Helium::Mutex m_KillMutex;

            // this process was killed
            bool m_Killed;
            bool m_Debug; // we're debugging?
            bool m_Wait;

        public:
            // process tracking support
            static Process* Create( const tstring& executable, bool debug, bool wait );
            static void Release( Process*& worker );
            static void ReleaseAll();

        private:
            // protect constructor so all worker processes are tracked
            Process( const tstring& executable, bool debug, bool wait );

        public:
            // will destroy process if its not over or killed
            ~Process();

            // create process
            bool Start( int timeout = DefaultWorkerTimeout );

            // you must delete the object this returns, if non-null
            IPC::Message* Receive( bool wait = true );

            // a copy is made into the IPC connection system
            bool Send(uint32_t id, uint32_t size = -1, const uint8_t* data = NULL);

            // test to see if its still running
            bool Running();

            // wait for exit and return success/fail
            int Finish();

            // destroy the process
            void Kill();
        };
    }
}