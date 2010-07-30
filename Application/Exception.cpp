#include "Exception.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sys/stat.h>

#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Console.h"
#include "Platform/Windows/Memory.h"
#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Debug;

bool g_EnableExceptionFilter = false;
u32 Debug::ExecuteHandler = EXCEPTION_EXECUTE_HANDLER;
u32 Debug::ContinueSearch = EXCEPTION_CONTINUE_SEARCH;
u32 Debug::ContinueExecution = EXCEPTION_CONTINUE_EXECUTION;

BreakpointSignature::Delegate Debug::g_BreakpointOccurred;
ExceptionSignature::Delegate Debug::g_ExceptionOccurred;
TerminateSignature::Event Debug::g_Terminating;

static LONG __stdcall ProcessFilteredException( LPEXCEPTION_POINTERS info )
{
  return Debug::ProcessException(info, Debug::GetExceptionBehavior(), true, true);
}

void Debug::EnableExceptionFilter(bool enable)
{
  g_EnableExceptionFilter = enable;

  // handles an exception occuring in the process not handled by a user exception handler
  SetUnhandledExceptionFilter( g_EnableExceptionFilter ? &ProcessFilteredException : NULL );
}

int Debug::GetExceptionBehavior()
{
  if (getenv( "HELIUM_CRASH_DONT_BLOCK" ) != NULL)
  {
    // do not propagate the exception up to the system (avoid dialog)
    return EXCEPTION_EXECUTE_HANDLER;
  }

  // handle exception transparently
  return EXCEPTION_CONTINUE_SEARCH;
}

void Debug::ProcessException(const Helium::Exception& exception, bool print, bool fatal)
{
  SetUnhandledExceptionFilter( NULL );

  ExceptionArgs args( ExceptionTypes::CPP, fatal );

  const char* cppClass = NULL;
  try
  {
    cppClass = typeid(exception).name();
  }
  catch (...)
  {
    cppClass = "Unknown";
  }

  bool converted = Helium::ConvertString( exception.What(), args.m_Message );
  HELIUM_ASSERT( converted );

  converted = Helium::ConvertString( cppClass, args.m_CPPClass );
  HELIUM_ASSERT( converted );

  args.m_State = Log::GetOutlineState();

  if (print)
  {
    Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "An exception has occurred\nType:    C++ Exception\n Class:   %s\n Message: %s\n" ), args.m_CPPClass.c_str(), args.m_Message.c_str() );
  }

  if ( g_ExceptionOccurred.Valid() )
  {
    g_ExceptionOccurred.Invoke( args );
  }

  if ( fatal )
  {
    if ( g_Terminating.Count() )
    {
      g_Terminating.Raise( TerminateArgs () );
    }

    EnableExceptionFilter( false );
  }
  else if ( g_EnableExceptionFilter )
  {
    SetUnhandledExceptionFilter( &ProcessFilteredException );
  }
}

void Debug::ProcessException(const std::exception& exception, bool print, bool fatal)
{
  SetUnhandledExceptionFilter( NULL );

  ExceptionArgs args( ExceptionTypes::CPP, fatal );

  const char* cppClass = NULL;
  try
  {
    cppClass = typeid(exception).name();
  }
  catch (...)
  {
    cppClass = "Unknown";
  }

  bool converted = Helium::ConvertString( exception.what(), args.m_Message );
  HELIUM_ASSERT( converted );

  converted = Helium::ConvertString( cppClass, args.m_CPPClass );
  HELIUM_ASSERT( converted );

  args.m_State = Log::GetOutlineState();

  if (print)
  {
    Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "An exception has occurred\nType:    C++ Exception\n Class:   %s\n Message: %s\n" ), args.m_CPPClass.c_str(), args.m_Message.c_str() );
  }

  if ( g_ExceptionOccurred.Valid() )
  {
    g_ExceptionOccurred.Invoke( args );
  }

  if ( fatal )
  {
    if ( g_Terminating.Count() )
    {
      g_Terminating.Raise( TerminateArgs () );
    }

    EnableExceptionFilter( false );
  }
  else if ( g_EnableExceptionFilter )
  {
    SetUnhandledExceptionFilter( &ProcessFilteredException );
  }
}

u32 Debug::ProcessException(LPEXCEPTION_POINTERS info, u32 ret_code, bool print, bool fatal)
{
  SetUnhandledExceptionFilter( NULL );

  // handle breakpoint exceptions outside the debugger
  if ( !::IsDebuggerPresent()
    && info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT
    && getenv( "HELIUM_CRASH_DONT_BLOCK" ) == NULL
    && g_BreakpointOccurred.Valid() )
  {
    ret_code = g_BreakpointOccurred.Invoke( BreakpointArgs (info, fatal) );
  }
  else
  {
    ExceptionArgs args ( ExceptionTypes::SEH, fatal );

    args.m_State = Log::GetOutlineState();

    Debug::GetExceptionDetails( info, args );
 
    if ( print )
    {
      Helium::Print( Helium::ConsoleColors::Red, stderr, TXT( "%s" ), GetExceptionInfo( info ).c_str() );
    }

    bool full = getenv( "HELIUM_CRASH_FULL_DUMP" ) != NULL;
    
    args.m_Dump = Debug::WriteDump(info, full);

    if ( g_ExceptionOccurred.Valid() )
    {
      g_ExceptionOccurred.Invoke( args );
    }

    if ( fatal )
    {
      if ( g_Terminating.Count() )
      {
        g_Terminating.Raise( TerminateArgs () );
      }

      EnableExceptionFilter( false );
    }
  }

  if ( g_EnableExceptionFilter )
  {
    SetUnhandledExceptionFilter( &ProcessFilteredException );
  }

  return ret_code;
}