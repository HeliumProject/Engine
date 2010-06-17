#pragma once

#include <string>
#include <vector>
#include <exception>

#include "Platform/Windows/Debug.h"

#include "Foundation/API.h"
#include "Foundation/Exception.h"
#include "Foundation/Automation/Event.h"

namespace Debug
{
  //
  // Filter exceptions via CRT
  //

  // installs unhandled exception filter into the C-runtime (for worker threads)
  FOUNDATION_API void EnableExceptionFilter(bool enable);

  //
  // Breakpoint Hit
  //

  struct BreakpointArgs
  {
    LPEXCEPTION_POINTERS  m_Info;
    bool                  m_Fatal;

    BreakpointArgs( LPEXCEPTION_POINTERS info, bool fatal )
      : m_Info( info )
      , m_Fatal( fatal )
    {

    }
  };
  typedef Nocturnal::Signature<int, const BreakpointArgs&> BreakpointSignature;
  FOUNDATION_API extern BreakpointSignature::Delegate g_BreakpointOccurred;

  //
  // Exception Event (raised when an exception occurs)
  //

  typedef Nocturnal::Signature<void, const ExceptionArgs&> ExceptionSignature;
  extern FOUNDATION_API ExceptionSignature::Delegate g_ExceptionOccurred;

  //
  // Termination Event (raised before process termination after a fatal exception)
  //

  struct FOUNDATION_API TerminateArgs
  {
  };
  typedef Nocturnal::Signature<void, const TerminateArgs&> TerminateSignature;
  extern FOUNDATION_API TerminateSignature::Event g_Terminating;

  //
  // Exception reporting
  //

  // some constants
  extern FOUNDATION_API DWORD ExecuteHandler;    // execute the __except statement for this __try
  extern FOUNDATION_API DWORD ContinueSearch;    // continue up the stack and look for another __except to handle this exception
  extern FOUNDATION_API DWORD ContinueExecution; // continue execution at the exception point (ex. after you changed some instructions or mapped some memory)

  // get environment-driven handling behavior
  FOUNDATION_API int GetExceptionBehavior();

  // prepare and dispatch a report for a C++ exception
  FOUNDATION_API void ProcessException( const std::exception& ex,
                                   bool print = false,
                                   bool fatal = false );

  // prepare and dispatch a report for an SEH exception such as divide by zero, page fault from a invalid memory access, or even breakpoint instructions
  FOUNDATION_API DWORD ProcessException( LPEXCEPTION_POINTERS info,
                                    DWORD ret_code = GetExceptionBehavior(),
                                    bool print = false,
                                    bool fatal = false );
};