#pragma once

#include <winsock2.h>

#include "Platform/API.h"
#include "Platform/Types.h"

typedef int socklen_t;

namespace Helium
{
#pragma warning( push )
#pragma warning( disable : 4201 ) // warning C4201: nonstandard extension used : nameless struct/union
    struct HELIUM_PLATFORM_API Socket
    {
        SOCKET m_Handle;
        struct Overlapped
        {
            uint32_t* Internal;
            uint32_t* InternalHigh;
            union
            {
                struct
                {
                    uint32_t Offset;
                    uint32_t OffsetHigh;
                };

                void* Pointer;
            };
            void* hEvent;
        } m_Overlapped;
#pragma warning( pop )

        int m_Protocol;

        Socket(int);
        ~Socket();

        operator SOCKET()
        {
            return m_Handle;
        }
    };
}
