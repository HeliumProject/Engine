#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#define IPC_PIPE_ROOT TXT( "" )

namespace Platform
{
    struct PLATFORM_API Pipe
    {
        void* m_Handle;
        struct Overlapped
        {
            u32* Internal;
            u32* InternalHigh;
            union
            {
                struct
                {
                    u32 Offset;
                    u32 OffsetHigh;
                };

                void* Pointer;
            };
            void* hEvent;
        } m_Overlapped;

        Pipe(int);
        ~Pipe();
    };
}
