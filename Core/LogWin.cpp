//----------------------------------------------------------------------------------------------------------------------
// LogWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Platform/Platform.h"

#if HELIUM_OS_WIN

#include "Core/Log.h"

namespace Lunar
{
    /// Constructor.
    WinDebugLog::WinDebugLog()
    {
    }

    /// Destructor.
    WinDebugLog::~WinDebugLog()
    {
    }

    /// Write out a message to this log.
    ///
    /// @param[in] pMessage  Message text.
    void WinDebugLog::OutputImpl( const tchar_t* pMessage )
    {
        HELIUM_ASSERT( pMessage );

#if HELIUM_UNICODE
        OutputDebugStringW( pMessage );
#else
        OutputDebugStringA( pMessage );
#endif
    }

#if L_ENABLE_LOGGING
    /// Get the global logger instance.
    ///
    /// @return  Reference to the global logger.
    WinDebugLog& GetLog()
    {
        static WinDebugLog log;

        return log;
    }
#endif
}

#endif  // HELIUM_OS_WIN
