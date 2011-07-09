#pragma once

#include "Condition.h"

#ifdef HELIUM_OS_WIN
# include "Platform/PipeWin.h"
#else
# include "POSIX/Pipe.h"
#endif

const static uint32_t IPC_PIPE_BUFFER_SIZE = 8192;

namespace Helium
{
    HELIUM_PLATFORM_API bool InitializePipes();
    HELIUM_PLATFORM_API void CleanupPipes();

    HELIUM_PLATFORM_API bool CreatePipe(const tchar_t* name, Pipe& pipe);
    HELIUM_PLATFORM_API bool OpenPipe(const tchar_t* name, Pipe& pipe);
    HELIUM_PLATFORM_API void ClosePipe(Pipe& pipe);

    HELIUM_PLATFORM_API bool ConnectPipe(Pipe& pipe, Condition& terminate);
    HELIUM_PLATFORM_API void DisconnectPipe(Pipe& pipe);

    HELIUM_PLATFORM_API bool ReadPipe(Pipe& pipe, void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate);
    HELIUM_PLATFORM_API bool WritePipe(Pipe& pipe, void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate);
}
