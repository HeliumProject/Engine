//----------------------------------------------------------------------------------------------------------------------
// LogWin.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/Platform.h"

#if L_OS_WIN

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
        L_ASSERT( pMessage );

#if L_UNICODE
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

#endif  // L_OS_WIN
