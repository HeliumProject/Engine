#pragma once

#include <string>
#include <vector>
#include <exception>

#include "Platform/Exception.h"

#include "Foundation/API.h"
#include "Foundation/Event.h"

namespace Helium
{
    //
    // Filter exceptions via CRT
    //

    // installs unhandled exception filter into the C-runtime (for worker threads)
    HELIUM_FOUNDATION_API void EnableExceptionFilter(bool enable);

    //
    // Breakpoint Hit
    //

    struct BreakpointArgs
    {
        BreakpointArgs( LPEXCEPTION_POINTERS info, bool fatal )
            : m_Info( info )
            , m_Fatal( fatal )
            , m_Result( 0 )
        {

        }

        LPEXCEPTION_POINTERS    m_Info;
        bool                    m_Fatal;
        mutable int             m_Result;
    };
    typedef Helium::Signature< const BreakpointArgs& > BreakpointSignature;
    extern HELIUM_FOUNDATION_API BreakpointSignature::Delegate g_BreakpointOccurred;

    //
    // Exception Event (raised when an exception occurs)
    //

    typedef Helium::Signature< const ExceptionArgs& > ExceptionSignature;
    extern HELIUM_FOUNDATION_API ExceptionSignature::Delegate g_ExceptionOccurred;

    //
    // Termination Event (raised before process termination after a fatal exception)
    //

    struct HELIUM_FOUNDATION_API TerminateArgs {};
    typedef Helium::Signature< const TerminateArgs& > TerminateSignature;
    extern HELIUM_FOUNDATION_API TerminateSignature::Event g_Terminating;

    //
    // Exception reporting
    //

    // prepare and dispatch a report for a C++ exception
    HELIUM_FOUNDATION_API void ProcessException( const Helium::Exception& ex,
        bool print = false,
        bool fatal = false );

    // prepare and dispatch a report for a C++ exception
    HELIUM_FOUNDATION_API void ProcessException( const std::exception& ex,
        bool print = false,
        bool fatal = false );

    // prepare and dispatch a report for an SEH exception such as divide by zero, page fault from a invalid memory access, or even breakpoint instructions
    HELIUM_FOUNDATION_API uint32_t ProcessException( LPEXCEPTION_POINTERS info,
        bool print = false,
        bool fatal = false );
}
