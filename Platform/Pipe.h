#pragma once

#include "Condition.h"

#ifdef __GNUC__
# include "POSIX/Pipe.h"
#elif defined( WIN32 )
# include "Windows/Pipe.h"
#else
#  pragma TODO( "Emit an error here..." )
#endif

const static uint32_t IPC_PIPE_BUFFER_SIZE = 8192;

namespace Helium
{
    PLATFORM_API bool InitializePipes();
    PLATFORM_API void CleanupPipes();

    PLATFORM_API bool CreatePipe(const tchar_t* name, Pipe& pipe);
    PLATFORM_API bool OpenPipe(const tchar_t* name, Pipe& pipe);
    PLATFORM_API void ClosePipe(Pipe& pipe);

    PLATFORM_API bool ConnectPipe(Pipe& pipe, Condition& terminate);
    PLATFORM_API void DisconnectPipe(Pipe& pipe);

    PLATFORM_API bool ReadPipe(Pipe& pipe, void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate);
    PLATFORM_API bool WritePipe(Pipe& pipe, void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate);
}
