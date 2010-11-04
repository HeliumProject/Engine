//----------------------------------------------------------------------------------------------------------------------
// Log.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_CORE_LOG_H
#define LUNAR_CORE_LOG_H

#include "Core/Core.h"
#include "Core/String.h"
#include "Core/Threading.h"

/// @defgroup logmacros Logging Macros
//@{

#if !defined( L_ENABLE_LOGGING ) && !L_RELEASE
/// Defined and non-zero if logging is enabled.
#define L_ENABLE_LOGGING 1
#endif

#if defined( L_ENABLE_LOGGING ) && !L_ENABLE_LOGGING
#undef L_ENABLE_LOGGING
#endif

#if L_ENABLE_LOGGING

/// Write out a message to the output log.
///
/// @param[in] LEVEL  Logging level.
/// @param[in] ...    Message.  This can be a "printf"-style format string and arguments or a Lunar::String.
#define L_LOG( LEVEL, ... ) Lunar::GetLog().Output( LEVEL, __VA_ARGS__ )

/// Write out a formatted message to the output log using a variable argument list.
///
/// @param[in] LEVEL     Logging level.
/// @param[in] FORMAT    Format string.
/// @param[in] ARG_LIST  Variable argument list initialized to the format arguments (va_start() should have already been
///                      called.
#define L_LOG_VA( LEVEL, FORMAT, ARG_LIST ) Lunar::GetLog().OutputVa( LEVEL, FORMAT, ARG_LIST )

/// Set the current logging level.
///
/// @param[in] LEVEL  Level to set.
///
/// @see L_LOG_GET_LEVEL()
#define L_LOG_SET_LEVEL( LEVEL ) Lunar::GetLog().SetLevel( LEVEL )

/// Get the current logging level.
///
/// @return  Current logging level.
///
/// @see L_LOG_SET_LEVEL()
#define L_LOG_GET_LEVEL() Lunar::GetLog().GetLevel()

#else  // L_ENABLE_LOGGING

/// Write out a message to the output log.
///
/// @param[in] LEVEL  Logging level.
/// @param[in] ...    Message.  This can be a "printf"-style format string and arguments or a Lunar::String.
#define L_LOG( LEVEL, ... )

/// Write out a formatted message to the output log using a variable argument list.
///
/// @param[in] LEVEL     Logging level.
/// @param[in] FORMAT    Format string.
/// @param[in] ARG_LIST  Variable argument list initialized to the format arguments (va_start() should have already been
///                      called.
#define L_LOG_VA( LEVEL, FORMAT, ARG_LIST )

/// Set the current logging level.
///
/// @param[in] LEVEL  Level to set.
///
/// @see L_LOG_GET_LEVEL()
#define L_LOG_SET_LEVEL( LEVEL )

/// Get the current logging level.
///
/// @return  Current logging level.
///
/// @see L_LOG_SET_LEVEL()
#define L_LOG_GET_LEVEL() ( Lunar::Log::LEVEL_INVALID )

#endif  // L_ENABLE_LOGGING

//@}

namespace Lunar
{
    /// Logging levels.  These are intentionally declared outside the scope of the Log class and abbreviated for ease of
    /// use.
    enum ELogLevel
    {
        LOG_FIRST   =  0,
        LOG_INVALID = -1,

        LOG_DEBUG,    ///< Debug logging messages.
        LOG_INFO,     ///< General info messages.
        LOG_WARNING,  ///< Warning messages.
        LOG_ERROR,    ///< Critical error messages.

        LOG_MAX,
        LOG_LAST = LOG_MAX - 1
    };

    /// Logging interface.
    class LUNAR_CORE_API Log
    {
    public:
        /// Default size for formatted log message buffers without requiring dynamic memory allocation.
        static const size_t DEFAULT_MESSAGE_BUFFER_SIZE = 1024;

        /// @name Construction/Destruction
        //@{
        Log();
        virtual ~Log();
        //@}

        /// @name Logging Interface
        //@{
        void SetLevel( ELogLevel level );
        inline ELogLevel GetLevel() const;

        void Output( ELogLevel level, const tchar_t* pFormat, ... );
        void Output( ELogLevel level, const String& rMessage );
        void OutputVa( ELogLevel level, const tchar_t* pFormat, va_list argList );
        //@}

    protected:
        /// Synchronization mutex.
        LwMutex m_mutex;

        /// Current logging level.
        ELogLevel m_level;

        /// Logging level of the last message.
        ELogLevel m_lastMessageLevel;
        /// True if logging just started a fresh line.
        bool m_bNewLine;

        /// @name Logging Implementation
        //@{
        virtual void OutputImpl( const tchar_t* pMessage ) = 0;
        //@}

        /// @name Static Utility Functions
        //@{
        static const tchar_t* GetLevelString( ELogLevel level );
        //@}
    };
}

#if HELIUM_OS_WIN
#include "Core/LogWin.h"
#endif

#include "Core/Log.inl"

#endif  // LUNAR_CORE_LOG_H
