#pragma once

#include "Platform/API.h"

#include "Platform/Types.h"

#ifdef WIN32
# include "Platform/Windows/File.h"
#else
# include "Platform/POSIX/File.h"
#endif

namespace Helium
{
    bool PLATFORM_API CloseHandle( Handle& handle );
    bool PLATFORM_API ReadFile( Handle& handle, void* buffer, uint32_t numberOfBytesToRead, uint32_t* numberOfBytesRead = NULL );
}