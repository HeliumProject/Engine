#pragma once

#include "API.h"

namespace Helium
{
    /// @defgroup debugutility Debug Utility Functions
    //@{
    // Detects if a debugger is attached to the process
    PLATFORM_API bool IsDebuggerPresent();

#if !HELIUM_RELEASE && !HELIUM_PROFILE
    PLATFORM_API size_t GetStackTrace( void** ppStackTraceArray, size_t stackTraceArraySize, size_t skipCount = 1 );
#pragma TODO( "LUNAR MERGE - Replace tstring usage below with String once merged from Core." )
    PLATFORM_API void GetAddressSymbol( tstring& rSymbol, void* pAddress );

    PLATFORM_API void DebugLog( const tchar_t* pMessage );
#endif
    //@}
}
