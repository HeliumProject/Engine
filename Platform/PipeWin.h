#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#define IPC_PIPE_ROOT TXT( "" )

namespace Helium
{
    struct HELIUM_PLATFORM_API Pipe
    {
        void* m_Handle;
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

        Pipe(int);
        ~Pipe();
    };
}
