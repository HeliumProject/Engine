#pragma once

#include "Platform/API.h"
#include "Platform/Types.h"

#ifdef HELIUM_OS_WIN
# include "Platform/DebugWin.h"
#endif

namespace Helium
{
    /// @defgroup debugutility Debug Utility Functions
    //@{
    // Detects if a debugger is attached to the process
    HELIUM_PLATFORM_API bool IsDebuggerPresent();

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    HELIUM_PLATFORM_API size_t GetStackTrace( void** ppStackTraceArray, size_t stackTraceArraySize, size_t skipCount = 1 );
    HELIUM_PLATFORM_API void GetAddressSymbol( tstring& rSymbol, void* pAddress );
    HELIUM_PLATFORM_API void DebugLog( const tchar_t* pMessage );
#endif
    //@}
}
