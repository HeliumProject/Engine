#include "ApplicationPch.h"
#include "Startup.h"

#include "Platform/Assert.h"
#include "Platform/Encoding.h"
#include "Platform/Exception.h"
#include "Platform/Process.h"
#include "Platform/Runtime.h"
#include "Platform/Thread.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/Exception.h"

#include "Application/CmdLine.h"
#include "Application/ExceptionReport.h"

#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>

#include <memory>
#include <sstream>
#include <algorithm>

#if HELIUM_OS_WIN
#include <crtdbg.h>
#include <malloc.h>
#endif

using namespace Helium;

const char* StartupArgs::Script = TXT( "script" );
const char* StartupArgs::Attach = TXT( "attach" );
const char* StartupArgs::Profile = TXT( "profile" );
const char* StartupArgs::Verbose = TXT( "verbose" );
const char* StartupArgs::Extreme = TXT( "extreme" );
const char* StartupArgs::Debug = TXT( "debug" );

#ifdef _DEBUG
const char* StartupArgs::DisableDebugHeap = TXT( "no_debug_heap" );
const char* StartupArgs::DisableLeakCheck = TXT( "no_leak_check" );
const char* StartupArgs::CheckHeap = TXT( "check_heap" );
#endif

using namespace Helium;

// are we initialized
static int32_t g_InitCount = 0;

// the event we raise when we shutdown
ShutdownSignature::Event Helium::g_ShuttingDown;

// are we shutting down
bool g_ShutdownStarted = false;
bool g_ShutdownComplete = false;

// default to these streams for trace files, it is up to the app to ask for these, when creating a TraceFile
std::vector< std::string > g_TraceFiles;
Log::Stream g_TraceStreams  = Log::Streams::Normal | Log::Streams::Warning | Log::Streams::Error; 

// so you can set _crtBreakAlloc in the debugger (expression evaluator doesn't like it)
#if HELIUM_OS_WIN
#ifdef _DEBUG
namespace Helium
{
    long& g_BreakOnAlloc (_crtBreakAlloc);
}
#endif // _DEBUG
#endif // HELIUM_OS_WIN

void Helium::Startup( int argc, const char** argv )
{
    if ( ++g_InitCount == 1 )
    {
        InitializeStandardTraceFiles(); 

        // Init command line, wait for remote debugger
        if ( argc )
        {
            Helium::SetCmdLine( argc, argv );
        }

        if ( Helium::GetCmdLineFlag( StartupArgs::Attach ) )
        {
            int32_t timeout = 300; // 5min
            Log::Print( TXT( "Waiting %d minutes for debugger to attach...\n" ), timeout / 60 );

            while ( !Helium::IsDebuggerPresent() && timeout-- )
            {
                Thread::Sleep( 1000 );
            }

            if ( Helium::IsDebuggerPresent() )
            {
                Log::Print( TXT( "Debugger attached\n" ) );
                HELIUM_ISSUE_BREAK();
            }
        }

        // Setup debug CRT
#if HELIUM_OS_WIN && defined( _DEBUG )
        int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        if (Helium::GetCmdLineFlag( StartupArgs::DisableDebugHeap ))
        {
            flags = 0x0;
        }
        else if (Helium::GetCmdLineFlag( StartupArgs::CheckHeap ))
        {
            flags |= (flags & 0x0000FFFF) | _CRTDBG_CHECK_ALWAYS_DF; // clear the upper 16 bits and OR in the desired freqency (always)
        }
        _CrtSetDbgFlag(flags);
#endif

        // Setup Console
        if ( Helium::GetCmdLineFlag( StartupArgs::Extreme ) )
        {
            Log::SetLevel( Log::Levels::Extreme );
        }
        else if ( Helium::GetCmdLineFlag( StartupArgs::Verbose ) )
        {
            Log::SetLevel( Log::Levels::Verbose );
        }

#ifdef HELIUM_DEBUG
        Log::EnableStream( Log::Streams::Debug, true );
#else
        Log::EnableStream( Log::Streams::Debug, Helium::GetCmdLineFlag( StartupArgs::Debug ) );
#endif

#ifdef HELIUM_PROFILE
        Log::EnableStream( Log::Streams::Profile, true );
#else
        Log::EnableStream( Log::Streams::Profile, Helium::GetCmdLineFlag( StartupArgs::Profile ) );
#endif

        if( Helium::GetCmdLineFlag( StartupArgs::Debug ) )
        {
            // add the debug stream to the trace
            g_TraceStreams |= Log::Streams::Debug; 
        }

        if( Helium::GetCmdLineFlag( StartupArgs::Profile ) )
        {
            // init profiling
            Profile::Initialize(); 

            // add the profile stream to the trace
            g_TraceStreams |= Log::Streams::Profile; 
        }

        // handle invalid parameters, etc...
        Helium::EnableCPPErrorHandling( true );

        // init debug handling
        Helium::InitializeExceptionListener();

#if HELIUM_OS_WIN
        // disable dialogs for main line error cases
        SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
#endif
    }
}

int Helium::Shutdown( int code )
{
    if ( --g_InitCount == 0 )
    {
        if ( g_ShutdownStarted )
        {
            return code;
        }

        // signal our shutdown has begun
        g_ShutdownStarted = true;

        // This should be done first, so that dynamic libraries to be freed in Cleanup() don't cause breakage in profile
        if (Helium::GetCmdLineFlag( StartupArgs::Profile ))
        {
            Profile::Accumulator::ReportAll();
        }

        // Only print shutdown summary if we are not in script mode
        if ( !Helium::GetCmdLineFlag( StartupArgs::Script ) )
        {
            //
            // Print time usage
            //
            Log::Print( TXT( "\n" ) );

            // Print general success or failure, depends on the result code
            Log::Print( TXT( "%s: " ), GetProcessName().c_str() );
            Log::PrintString( code ? TXT( "Failed" ) : TXT( "Succeeeded" ), Log::Streams::Normal, Log::Levels::Default, code ? ConsoleColors::Red : ConsoleColors::Green );

            // Print warning/error count
            if (Log::GetWarningCount() || Log::GetErrorCount())
            {
                Log::Print( TXT( " with" ) );
            }

            if (Log::GetErrorCount())
            {
                char buf[80];
                StringPrint( buf, TXT( " %d error%s" ), Log::GetErrorCount(), Log::GetErrorCount() > 1 ? TXT( "s" ) : TXT( "" ) );
                Log::PrintString( buf, Log::Streams::Normal, Log::Levels::Default, ConsoleColors::Red );
            }

            if (Log::GetWarningCount() && Log::GetErrorCount())
            {
                Log::Print( TXT( " and" ) );
            }

            if (Log::GetWarningCount())
            {
                char buf[80];
                StringPrint(buf, TXT( " %d warning%s" ), Log::GetWarningCount(), Log::GetWarningCount() > 1 ? TXT( "s" ) : TXT( "" ) );
                Log::PrintString( buf, Log::Streams::Normal, Log::Levels::Default, ConsoleColors::Yellow );
            }

            Log::Print( TXT( "\n" ) );
        }

        // Raise Shutdown Event
        g_ShuttingDown.Raise( ShutdownArgs () );


        // Setup debug CRT to dump memleaks to OutputDebugString and stderr
#if HELIUM_OS_WIN && defined( _DEBUG )
        if ( !Helium::GetCmdLineFlag( StartupArgs::DisableDebugHeap ) && !Helium::GetCmdLineFlag( StartupArgs::DisableLeakCheck ) )
        {
            int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
            _CrtSetDbgFlag( flags | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
            _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
        }
#endif

        // Disable exception handling
        Helium::CleanupExceptionListener();

        if (Helium::GetCmdLineFlag( StartupArgs::Profile ))
        {
            Profile::Cleanup(); 
        }

        CleanupStandardTraceFiles();

        Helium::ReleaseCmdLine();

        g_ShutdownComplete = true;
    }

    return code;
}

Log::Stream Helium::GetTraceStreams()
{
    return g_TraceStreams; 
}

void Helium::InitializeStandardTraceFiles()
{
    std::string path = GetProcessPath();
    g_TraceFiles.push_back( path + TXT( ".log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Helium::GetTraceStreams() );

    g_TraceFiles.push_back( path + TXT( "Warnings.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Warning );

    g_TraceFiles.push_back( path + TXT( "Errors.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Error );
}

void Helium::CleanupStandardTraceFiles()
{
    for ( std::vector< std::string >::const_iterator itr = g_TraceFiles.begin(), end = g_TraceFiles.begin(); itr != end; ++itr )
    {
        Log::RemoveTraceFile( *itr );
    }

    g_TraceFiles.clear();
}

#if HELIUM_OS_WIN

static DWORD ProcessUnhandledCxxException( LPEXCEPTION_POINTERS info )
{
    if ( info->ExceptionRecord->ExceptionCode == 0xE06D7363 )
    {
        Helium::Exception* ex = Helium::GetHeliumException( info->ExceptionRecord->ExceptionInformation[1] );
        if ( ex )
        {
            // process this as a non-fatal C++ exception, but via the SEH handler so we get the callstack at the throw() statment
            return Helium::ProcessException( info, false, false );
        }

        // this is not a nocturnal exception, so it will not be caught by the catch statements below, process it then execute the handler to unload the process
        return Helium::ProcessException( info, true, true );
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

static void StandardThreadTryExcept( Helium::CallbackThread::Entry entry, void* param )
{
    if (Helium::IsDebuggerPresent())
    {
        entry( param );
    }
    else
    {
        __try
        {
            entry( param );
        }
        __except( ProcessUnhandledCxxException( GetExceptionInformation() ) )
        {
            ::ExitProcess( -1 ); // propagating the exception up doesn't lead to a good situation, just shut down
        }
    }
}

static void StandardThreadTryCatch( Helium::CallbackThread::Entry entry, void* param )
{
    if ( Helium::IsDebuggerPresent() )
    {
        StandardThreadTryExcept( entry, param );
    }
    else
    {
        try
        {
            StandardThreadTryExcept( entry, param );
        }
        catch ( const Helium::Exception& ex )
        {
            Log::Error( TXT( "%s\n" ), ex.What() );

            ::ExitProcess( -1 );
        }
    }
}

static void StandardThreadEntry( Helium::CallbackThread::Entry entry, void* param )
{
    // any normal thread startup work would go here
    StandardThreadTryCatch( entry, param );
}

#endif

void Helium::StandardThread( Helium::CallbackThread::Entry entry, void* param )
{
#if HELIUM_OS_WIN
    if (Helium::IsDebuggerPresent())
    {
        StandardThreadEntry( entry, param );
    }
    else
    {
        Helium::InitializeExceptionListener();

        __try
        {
            StandardThreadEntry( entry, param );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Helium::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( -1 );
        }

        Helium::CleanupExceptionListener();
    }
#else
    entry( param );
#endif
}

#if HELIUM_OS_WIN

static int StandardMainTryExcept( int (*main)(int argc, const char** argv), int argc, const char** argv )
{
    if (Helium::IsDebuggerPresent())
    {
        return main(argc, argv);
    }
    else
    {
        __try
        {
            return main(argc, argv);
        }
        __except( ProcessUnhandledCxxException( GetExceptionInformation() ) )
        {
            ::ExitProcess( -1 ); // propagating the exception up doesn't lead to a good situation, just shut down
        }

        return -1;
    }
}

static int StandardMainTryCatch( int (*main)(int argc, const char** argv), int argc, const char** argv )
{
    if ( Helium::IsDebuggerPresent() )
    {
        return StandardMainTryExcept( main, argc, argv );
    }
    else
    {
        int result = -1;

        try
        {
            result = StandardMainTryExcept( main, argc, argv );
        }
        catch ( const Helium::Exception& ex )
        {
            Log::Error( TXT( "%s\n" ), ex.What() );

            ::ExitProcess( -1 );
        }

        return result;
    }
}

static int StandardMainEntry( int (*main)(int argc, const char** argv), int argc, const char** argv )
{
    int result = 0; 

#if VERSION_CHECK
    try
#endif
    {
        Helium::Startup(argc, argv);
    }
#if VERSION_CHECK
    catch ( const Helium::CheckVersionException& ex )
    {
        Log::Error( TXT( "%s\n" ), ex.What() );
        result = 1;
    }
#endif

    if ( result == 0 )
    {
        result = StandardMainTryCatch( main, argc, argv );
    }

    return Helium::Shutdown( result );
}

#endif

int Helium::StandardMain( int (*main)(int argc, const char** argv), int argc, const char** argv )
{
#if HELIUM_OS_WIN
    if (Helium::IsDebuggerPresent())
    {
        return StandardMainEntry( main, argc, argv );
    }
    else
    {
        int result = -1;

        Helium::InitializeExceptionListener();

        __try
        {
            result = StandardMainEntry( main, argc, argv );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Helium::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( Helium::Shutdown( result ) );
        }

        Helium::CleanupExceptionListener();

        return result;
    }
#else
    return main(argc, argv);
#endif
}

#if HELIUM_OS_WIN

static int StandardWinMainTryExcept( WinMainFunc winMain, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    if (Helium::IsDebuggerPresent())
    {
        return winMain( hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }
    else
    {
        __try
        {
            return winMain( hInstance, hPrevInstance, lpCmdLine, nShowCmd );
        }
        __except( ProcessUnhandledCxxException( GetExceptionInformation() ) )
        {
            ::ExitProcess( -1 ); // propagating the exception up doesn't lead to a good situation, just shut down
        }

        return -1;
    }
}

static void ShowErrorDialog( const char* error )
{
	HELIUM_TCHAR_TO_WIDE( error, convertedError );
	::MessageBoxW(NULL, convertedError, L"Error", MB_OK|MB_ICONEXCLAMATION);
}

static int StandardWinMainTryCatch( WinMainFunc winMain, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    if ( Helium::IsDebuggerPresent() )
    {
        return StandardWinMainTryExcept( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }
    else
    {
        int result = -1;

        try
        {
            result = StandardWinMainTryExcept( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
        }
        catch ( const Helium::Exception& ex )
        {
            Log::Error( TXT( "%s\n" ) , ex.What() );
			ShowErrorDialog( ex.What() ); // b/c of not being able to call alloca inside catch
            ::ExitProcess( -1 );
        }

        return result;
    }
}

static int StandardWinMainEntry( WinMainFunc winMain, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
	HELIUM_WIDE_TO_TCHAR( lpCmdLine, convertedCmdLine );

    int argc = 0;
    const char** argv = NULL;
    Helium::ProcessCmdLine( convertedCmdLine, argc, argv );

    int result = 0;

#if VERSION_CHECK
    try
#endif
    {
        Helium::Startup(argc, argv);
    }
#if VERSION_CHECK
    catch ( const Helium::CheckVersionException& ex )
    {
        result = 1;
        Log::Error( TXT( "%s\n" ), ex.What() );
        MessageBox(NULL, ex.What(), TXT( "Fatal Error" ), MB_OK|MB_ICONEXCLAMATION);
    }
#endif

    if ( result == 0 )
    {
        result = StandardWinMainTryCatch( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }

    Helium::Shutdown( result );

    delete[] argv;

    return result;
}

int Helium::StandardWinMain( WinMainFunc winMain, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd )
{
    if (Helium::IsDebuggerPresent())
    {
        return StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }
    else
    {
        int result = -1;

        Helium::InitializeExceptionListener();

        __try
        {
            result = StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Helium::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( Helium::Shutdown( result ) );
        }

        Helium::CleanupExceptionListener();

        return result;
    }
}

#endif
