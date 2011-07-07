#include "FoundationPch.h"
#include "Exception.h"

#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Platform/Console.h"
#include "Platform/ProfileMemory.h"

#include "Foundation/Log.h"

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sys/stat.h>

using namespace Helium;
using namespace Helium::Debug;

BreakpointSignature::Delegate   Debug::g_BreakpointOccurred;
ExceptionSignature::Delegate    Debug::g_ExceptionOccurred;
TerminateSignature::Event       Debug::g_Terminating;

bool g_EnableExceptionFilter = false;

static LONG __stdcall ProcessFilteredException( LPEXCEPTION_POINTERS info )
{
    return Debug::ProcessException(info, true, true);
}

void Debug::EnableExceptionFilter(bool enable)
{
    g_EnableExceptionFilter = enable;

    // handles an exception occuring in the process not handled by a user exception handler
    SetUnhandledExceptionFilter( g_EnableExceptionFilter ? &ProcessFilteredException : NULL );
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

    g_ExceptionOccurred.Invoke( args );

    if ( fatal )
    {
        g_Terminating.Raise( TerminateArgs () );
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

    g_ExceptionOccurred.Invoke( args );

    if ( fatal )
    {
        g_Terminating.Raise( TerminateArgs () );
        EnableExceptionFilter( false );
    }
    else if ( g_EnableExceptionFilter )
    {
        SetUnhandledExceptionFilter( &ProcessFilteredException );
    }
}

uint32_t Debug::ProcessException(LPEXCEPTION_POINTERS info, bool print, bool fatal)
{
    SetUnhandledExceptionFilter( NULL );

    // handle breakpoint exceptions outside the debugger
    if ( !::IsDebuggerPresent()
        && info->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT
        && g_BreakpointOccurred.Valid() )
    {
        BreakpointArgs args (info, fatal);
        g_BreakpointOccurred.Invoke( args );
        return args.m_Result;
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

        args.m_Dump = Debug::WriteDump(info, false);

        g_ExceptionOccurred.Invoke( args );

        if ( fatal )
        {
            g_Terminating.Raise( TerminateArgs () );
            EnableExceptionFilter( false );
        }
    }

    if ( g_EnableExceptionFilter )
    {
        SetUnhandledExceptionFilter( &ProcessFilteredException );
    }

    return EXCEPTION_CONTINUE_SEARCH;
}