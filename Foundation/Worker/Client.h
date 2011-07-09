#pragma once

#include "Platform/Types.h"

#include "Foundation/API.h"

namespace Helium
{
    namespace IPC
    {
        class Message;
    }

    namespace Worker
    {
        struct HELIUM_FOUNDATION_API Client
        {
            // initialize connection
            static bool Initialize( bool debug = false, bool wait = false );
            static void Cleanup();

            // you must delete the object this returns, if non-null
            static IPC::Message* Receive( bool wait = true );

            // a copy is made into the IPC connection system
            static bool Send(uint32_t id, uint32_t size = -1, const uint8_t* data = NULL);
        };
    }
}