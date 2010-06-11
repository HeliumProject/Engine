#pragma once

#include <string>
#include <vector>
#include <exception>

#include "API.h"
#include "Symbols.h"

#include "Foundation/Exception.h"
#include "Foundation/Automation/Event.h"

namespace Debug
{
  //
  // Filter exceptions via CRT
  //

  // installs unhandled exception filter into the C-runtime (for worker threads)
  DEBUG_API void EnableExceptionFilter(bool enable);

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
  DEBUG_API extern BreakpointSignature::Delegate g_BreakpointOccurred;

  //
  // Exception Event (raised when an exception occurs)
  //

  namespace ExceptionTypes
  {
    enum ExceptionType
    {
      SEH,
      CPP
    };

    static const char* Strings[] =
    {
      "SEH",
      "C++",
    };
  }
  typedef ExceptionTypes::ExceptionType ExceptionType;

  struct DEBUG_API ExceptionArgs
  {
    ExceptionType   m_Type;
    bool            m_Fatal;
    std::string     m_Message;
    std::string     m_Callstack;
    V_string        m_Threads;
    std::string     m_State;
    std::string     m_Dump;

    // SEH-specific info
    DWORD           m_SEHCode;
    std::string     m_SEHClass;
    std::string     m_SEHControlRegisters;
    std::string     m_SEHIntegerRegisters;

    // CPP-specific info
    std::string     m_CPPClass;

    ExceptionArgs( ExceptionType type, bool fatal )
      : m_Type( type )
      , m_Fatal( fatal )
      , m_SEHCode( -1 )
    {
    };
  };

  typedef Nocturnal::Signature<void, const ExceptionArgs&> ExceptionSignature;
  extern DEBUG_API ExceptionSignature::Delegate g_ExceptionOccurred;

  //
  // Termination Event (raised before process termination after a fatal exception)
  //

  struct DEBUG_API TerminateArgs
  {
  };
  typedef Nocturnal::Signature<void, const TerminateArgs&> TerminateSignature;
  extern DEBUG_API TerminateSignature::Event g_Terminating;

  //
  // Exception reporting
  //

  // some constants
  extern DEBUG_API DWORD ExecuteHandler;    // execute the __except statement for this __try
  extern DEBUG_API DWORD ContinueSearch;    // continue up the stack and look for another __except to handle this exception
  extern DEBUG_API DWORD ContinueExecution; // continue execution at the exception point (ex. after you changed some instructions or mapped some memory)

  // get environment-driven handling behavior
  DEBUG_API int GetExceptionBehavior();

  // prepare and dispatch a report for a C++ exception
  DEBUG_API void ProcessException( const std::exception& ex,
                                   bool print = false,
                                   bool fatal = false );

  // prepare and dispatch a report for an SEH exception such as divide by zero, page fault from a invalid memory access, or even breakpoint instructions
  DEBUG_API DWORD ProcessException( LPEXCEPTION_POINTERS info,
                                    DWORD ret_code = GetExceptionBehavior(),
                                    bool print = false,
                                    bool fatal = false );
};