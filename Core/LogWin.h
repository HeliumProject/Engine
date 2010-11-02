//----------------------------------------------------------------------------------------------------------------------
// LogWin.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_LOG_WIN_H
#define LUNAR_CORE_LOG_WIN_H

#include "Core/Core.h"

#if L_OS_WIN

namespace Lunar
{
    /// Windows debug output logger.
    class LUNAR_CORE_API WinDebugLog : public Log
    {
    public:
        /// @name Construction/Destruction
        //@{
        WinDebugLog();
        virtual ~WinDebugLog();
        //@}

    protected:
        /// @name Logging Implementation
        //@{
        virtual void OutputImpl( const tchar_t* pMessage );
        //@}
    };

#if L_ENABLE_LOGGING
    LUNAR_CORE_API WinDebugLog& GetLog();
#endif
}

#endif  // L_OS_WIN

#endif  // LUNAR_CORE_LOG_WIN_H
