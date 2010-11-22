#pragma once

#include "Platform/API.h"
#include "Platform/Assert.h"
#include "Platform/Mutex.h"

/// @defgroup tracemacros Trace Macros
//@{

#if !defined( HELIUM_ENABLE_TRACE ) && !( HELIUM_RELEASE || HELIUM_PROFILE )
/// Defined and non-zero if tracing is enabled.
#define HELIUM_ENABLE_TRACE 1
#endif

#if defined( HELIUM_ENABLE_TRACE ) && !HELIUM_ENABLE_TRACE
#undef HELIUM_ENABLE_TRACE
#endif

#if HELIUM_ENABLE_TRACE

/// Write out a message to the trace output.
///
/// @param[in] LEVEL  Trace level.
/// @param[in] ...    Message.  This can be a "printf"-style format string and arguments or a Helium::String.
#define HELIUM_TRACE( LEVEL, ... ) Helium::g_Trace.Output( LEVEL, __VA_ARGS__ )

/// Write out a formatted message to the trace output using a variable argument list.
///
/// @param[in] LEVEL     Trace level.
/// @param[in] FORMAT    Format string.
/// @param[in] ARG_LIST  Variable argument list initialized to the format arguments (va_start() should have already been
///                      called.
#define HELIUM_TRACE_VA( LEVEL, FORMAT, ARG_LIST ) Helium::g_Trace.OutputVa( LEVEL, FORMAT, ARG_LIST )

/// Set the current trace level.
///
/// @param[in] LEVEL  Level to set.
///
/// @see HELIUM_TRACE_GET_LEVEL()
#define HELIUM_TRACE_SET_LEVEL( LEVEL ) Helium::g_Trace.SetLevel( LEVEL )

/// Get the current trace level.
///
/// @return  Current trace level.
///
/// @see HELIUM_TRACE_SET_LEVEL()
#define HELIUM_TRACE_GET_LEVEL() Helium::g_Trace.GetLevel()

namespace Helium
{
    /// Trace levels.  These are intentionally declared outside the scope of the Trace class and abbreviated for ease of
    /// use.
    enum ETraceLevel
    {
        TRACE_FIRST   =  0,
        TRACE_INVALID = -1,

        TRACE_DEBUG,    ///< Debug logging messages.
        TRACE_INFO,     ///< General info messages.
        TRACE_WARNING,  ///< Warning messages.
        TRACE_ERROR,    ///< Critical error messages.

        TRACE_MAX,
        TRACE_LAST = TRACE_MAX - 1
    };

    /// Trace interface.
    class PLATFORM_API Trace
    {
    public:
        /// Default size for formatted trace message buffers without requiring dynamic memory allocation.
        static const size_t DEFAULT_MESSAGE_BUFFER_SIZE = 1024;

        /// @name Construction/Destruction
        //@{
        Trace();
        virtual ~Trace();
        //@}

        /// @name Logging Interface
        //@{
        void SetLevel( ETraceLevel level );
        inline ETraceLevel GetLevel() const;

        void Output( ETraceLevel level, const tchar_t* pFormat, ... );
        void OutputVa( ETraceLevel level, const tchar_t* pFormat, va_list argList );
        //@}

    protected:
        /// Synchronization mutex.
        Mutex m_mutex;

        /// Current logging level.
        ETraceLevel m_level;

        /// Logging level of the last message.
        ETraceLevel m_lastMessageLevel;
        /// True if logging just started a fresh line.
        bool m_bNewLine;

        /// @name Logging Implementation
        //@{
        void OutputImplementation( const tchar_t* pMessage );
        //@}

        /// @name Static Utility Functions
        //@{
        static const tchar_t* GetLevelString( ETraceLevel level );
        //@}
    };

    /// Global trace instance.
    PLATFORM_API extern Trace g_Trace;
}

#else  // HELIUM_ENABLE_TRACE

/// Write out a message to the trace output.
///
/// @param[in] LEVEL  Trace level.
/// @param[in] ...    Message.  This can be a "printf"-style format string and arguments or a Helium::String.
#define HELIUM_TRACE( LEVEL, ... )

/// Write out a formatted message to the trace output using a variable argument list.
///
/// @param[in] LEVEL     Trace level.
/// @param[in] FORMAT    Format string.
/// @param[in] ARG_LIST  Variable argument list initialized to the format arguments (va_start() should have already been
///                      called.
#define HELIUM_TRACE_VA( LEVEL, FORMAT, ARG_LIST )

/// Set the current trace level.
///
/// @param[in] LEVEL  Level to set.
///
/// @see HELIUM_TRACE_GET_LEVEL()
#define HELIUM_TRACE_SET_LEVEL( LEVEL )

/// Get the current trace level.
///
/// @return  Current trace level.
///
/// @see HELIUM_TRACE_SET_LEVEL()
#define HELIUM_TRACE_GET_LEVEL() ( Helium::Trace::LEVEL_INVALID )

#endif  // HELIUM_ENABLE_TRACE

//@}

#include "Platform/Trace.inl"
