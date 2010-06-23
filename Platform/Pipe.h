#pragma once

#include "Event.h"

#ifdef WIN32
# include "Windows/Pipe.h"
#else
# include "POSIX/Pipe.h"
#endif

const static u32 IPC_PIPE_BUFFER_SIZE = 8192;

namespace Platform
{
    PLATFORM_API bool InitializePipes();
    PLATFORM_API void CleanupPipes();

    PLATFORM_API bool CreatePipe(const tchar* name, Pipe& pipe);
    PLATFORM_API bool OpenPipe(const tchar* name, Pipe& pipe);
    PLATFORM_API void ClosePipe(Pipe& pipe);

    PLATFORM_API bool ConnectPipe(Pipe& pipe, Event& terminate);
    PLATFORM_API void DisconnectPipe(Pipe& pipe);

    PLATFORM_API bool ReadPipe(Pipe& pipe, void* buffer, u32 bytes, u32& read, Event& terminate);
    PLATFORM_API bool WritePipe(Pipe& pipe, void* buffer, u32 bytes, u32& wrote, Event& terminate);
}
