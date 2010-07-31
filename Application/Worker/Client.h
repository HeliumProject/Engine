#pragma once

#include "Platform/Types.h"

#include "Application/API.h"

namespace Helium
{
    namespace IPC
    {
        class Message;
    }

    namespace Worker
    {
        struct APPLICATION_API Client
        {
            // initialize connection
            static bool Initialize( bool debug = false, bool wait = false );
            static void Cleanup();

            // you must delete the object this returns, if non-null
            static IPC::Message* Receive( bool wait = true );

            // a copy is made into the IPC connection system
            static bool Send(u32 id, u32 size = -1, const u8* data = NULL);
        };
    }
}