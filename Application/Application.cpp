#include "Application.h"

#include "Platform/Assert.h"
#include "Platform/Platform.h"
#include "Platform/Process.h"
#include "Platform/Debug.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/CommandLine/Utilities.h"

#include "Application/Exception.h"
#include "Application/Exceptions.h"
#include "Application/ExceptionListener.h"

#include <crtdbg.h>
#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

#include <sstream>
#include <algorithm>
#include <memory>
#include <sys/timeb.h>

using namespace Helium;

const tchar* Application::Args::Script = TXT( "script" );
const tchar* Application::Args::Attach = TXT( "attach" );
const tchar* Application::Args::Profile = TXT( "profile" );
const tchar* Application::Args::Memory = TXT( "memory" );
const tchar* Application::Args::Verbose = TXT( "verbose" );
const tchar* Application::Args::Extreme = TXT( "extreme" );
const tchar* Application::Args::Debug = TXT( "debug" );

#ifdef _DEBUG
const tchar* Application::Args::DisableDebugHeap = TXT( "no_debug_heap" );
const tchar* Application::Args::DisableLeakCheck = TXT( "no_leak_check" );
const tchar* Application::Args::CheckHeap = TXT( "check_heap" );
#endif

using namespace Helium;
using namespace Helium::Application;

// init time
_timeb g_StartTime;

// are we initialized
static i32 g_InitCount = 0;

// the event we raise when we shutdown
ShutdownSignature::Event Application::g_ShuttingDown;

// are we shutting down
bool g_ShutdownStarted = false;
bool g_ShutdownComplete = false;

// default to these streams for trace files, it is up to the app to ask for these, when creating a TraceFile
std::vector< tstring > g_TraceFiles;
Log::Stream g_TraceStreams  = Log::Streams::Normal | Log::Streams::Warning | Log::Streams::Error; 

// so you can set _crtBreakAlloc in the debugger (expression evaluator doesn't like it)
#ifdef _DEBUG
namespace Helium
{
    namespace Application
    {
        long& g_BreakOnAlloc (_crtBreakAlloc);
    }
}
#endif //_DEBUG

void Application::Startup( int argc, const tchar** argv )
{
    if ( ++g_InitCount == 1 )
    {
        InitializeStandardTraceFiles(); 

        //
        // Set our start time
        //

        _ftime( &g_StartTime ); 


        //
        // Init command line, wait for remote debugger
        //

        if ( argc )
        {
            Helium::SetCmdLine( argc, argv );
        }

        if ( Helium::GetCmdLineFlag( Application::Args::Attach ) )
        {
            i32 timeout = 300; // 5min

            Log::Print( TXT( "Waiting %d minutes for debugger to attach...\n" ), timeout / 60);

            while ( !Helium::IsDebuggerPresent() && timeout-- )
            {
                Sleep( 1000 );
            }

            if ( Helium::IsDebuggerPresent() )
            {
                Log::Print( TXT( "Debugger attached\n" ) );
                HELIUM_ISSUE_BREAK();
            }
        }


        //
        // Setup debug CRT
        //

#ifdef _DEBUG
        int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
        if (Helium::GetCmdLineFlag( Application::Args::DisableDebugHeap ))
        {
            flags = 0x0;
        }
        else if (Helium::GetCmdLineFlag( Application::Args::CheckHeap ))
        {
            flags |= (flags & 0x0000FFFF) | _CRTDBG_CHECK_ALWAYS_DF; // clear the upper 16 bits and OR in the desired freqency (always)
        }
        _CrtSetDbgFlag(flags);
#endif


        //
        // Only print startup summary if we are not in script mode
        //

        if ( !Helium::GetCmdLineFlag( Application::Args::Script ) )
        {
            //
            // Print project and version info
            //
            tchar module[MAX_PATH];
            GetModuleFileName( 0, module, MAX_PATH );

            tchar name[MAX_PATH];
            _tsplitpath( module, NULL, NULL, name, NULL );

            Log::Print( TXT( "Running %s\n" ), name );
            Log::Print( TXT( "Current Time: %s" ), _tctime64( &g_StartTime.time ) );
            Log::Print( TXT( "Command Line: %s\n" ), Helium::GetCmdLine() );
        }


        //
        // Setup Console
        //

        if ( Helium::GetCmdLineFlag( Application::Args::Extreme ) )
        {
            Log::SetLevel( Log::Levels::Extreme );
        }
        else if ( Helium::GetCmdLineFlag( Application::Args::Verbose ) )
        {
            Log::SetLevel( Log::Levels::Verbose );
        }

        Log::EnableStream( Log::Streams::Debug, Helium::GetCmdLineFlag( Application::Args::Debug ) );
        Log::EnableStream( Log::Streams::Profile, Helium::GetCmdLineFlag( Application::Args::Profile ) );

        if( Helium::GetCmdLineFlag( Application::Args::Debug ) )
        {
            // add the debug stream to the trace
            g_TraceStreams |= Log::Streams::Debug; 

            // dump env
            if ( Helium::GetCmdLineFlag( Application::Args::Verbose ) )
            {
                // get a pointer to the environment block. 
                const char* env = (const char*)GetEnvironmentStrings();

                // if the returned pointer is NULL, exit.
                if (env)
                {
                    Log::Debug( TXT( "\n" ) );
                    Log::Debug( TXT( "Environment:\n" ) );

                    // variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
                    for (const char* var = (const char*)env; *var; var++) 
                    {
                        if (*var != '=') // WTF?
                        {
                            Log::Debug( TXT( " %s\n" ), var );
                        }

                        while (*var)
                        {
                            var++;
                        }
                    }

                    FreeEnvironmentStrings((tchar*)env);
                }
            }
        }

        if( Helium::GetCmdLineFlag( Application::Args::Profile ) )
        {
            // init profiling
            Profile::Initialize(); 

            // add the profile stream to the trace
            g_TraceStreams |= Log::Streams::Profile; 

            // enable memory reports
            if ( Helium::GetCmdLineFlag( Application::Args::Memory ) )
            {
                Profile::Memory::Initialize();
            }
        }


        //
        // Setup exception handlers, do this last
        //

        // handle 'new' errors, invalid parameters, etc...
        Helium::Platform::Initialize();

        // init debug handling
        Helium::Debug::InitializeExceptionListener();

        // disable dialogs for main line error cases
        SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
    }
}

int Application::Shutdown( int code )
{
    if ( --g_InitCount == 0 )
    {
        if ( g_ShutdownStarted )
        {
            return code;
        }

        // signal our shutdown has begun
        g_ShutdownStarted = true;


        //
        // This should be done first, so that dynamic libraries to be freed in Cleanup()
        //  don't cause breakage in profile
        //

        if (Helium::GetCmdLineFlag( Application::Args::Profile ))
        {
            Profile::Memory::Cleanup();
            Profile::Accumulator::ReportAll();
        }


        //
        // Only print shutdown summary if we are not in script mode
        //

        if ( !Helium::GetCmdLineFlag( Application::Args::Script ) )
        {
            //
            // Print time usage
            //
            Log::Print( TXT( "\n" ) );

            _timeb endTime;
            _ftime(&endTime); 
            Log::Print( TXT( "Current Time: %s" ), _tctime64( &endTime.time ) );

            int time = (int) (((endTime.time*1000) + endTime.millitm) - ((g_StartTime.time*1000) +  g_StartTime.millitm));
            int milli = time % 1000; time /= 1000;
            int sec = time % 60; time /= 60;
            int min = time % 60; time /= 60;
            int hour = time;

            if (hour > 0)
            {
                Log::Print( TXT( "Execution Time: %d:%02d:%02d.%02d hours\n" ), hour, min, sec, milli);
            }
            else
            {
                if (min > 0)
                {
                    Log::Print( TXT( "Execution Time: %d:%02d.%02d minutes\n" ), min, sec, milli);
                }
                else
                {
                    if (sec > 0)
                    {
                        Log::Print( TXT( "Execution Time: %d.%02d seconds\n" ), sec, milli);
                    }
                    else
                    {
                        if (milli > 0)
                        {
                            Log::Print( TXT( "Execution Time: %02d milliseconds\n" ), milli);
                        }
                    }
                }
            }


            //
            // Print general success or failure, depends on the result code
            //
            tchar module[MAX_PATH];
            GetModuleFileName( 0, module, MAX_PATH );

            tchar name[MAX_PATH];
            _tsplitpath( module, NULL, NULL, name, NULL );

            Log::Print( TXT( "%s: " ), name );
            Log::PrintString( code ? TXT( "Failed" ) : TXT( "Succeeeded" ), Log::Streams::Normal, Log::Levels::Default, code ? Log::Colors::Red : Log::Colors::Green );


            //
            // Print warning/error count
            //

            if (Log::GetWarningCount() || Log::GetErrorCount())
            {
                Log::Print( TXT( " with" ) );
            }

            if (Log::GetErrorCount())
            {
                tchar buf[80];
                _stprintf( buf, TXT( " %d error%s" ), Log::GetErrorCount(), Log::GetErrorCount() > 1 ? TXT( "s" ) : TXT( "" ) );
                Log::PrintString( buf, Log::Streams::Normal, Log::Levels::Default, Log::Colors::Red );
            }

            if (Log::GetWarningCount() && Log::GetErrorCount())
            {
                Log::Print( TXT( " and" ) );
            }

            if (Log::GetWarningCount())
            {
                tchar buf[80];
                _stprintf(buf, TXT( " %d warning%s" ), Log::GetWarningCount(), Log::GetWarningCount() > 1 ? TXT( "s" ) : TXT( "" ) );
                Log::PrintString( buf, Log::Streams::Normal, Log::Levels::Default, Log::Colors::Yellow );
            }

            Log::Print( TXT( "\n" ) );
        }


        //
        // Raise Shutdown Event
        //

        g_ShuttingDown.Raise( ShutdownArgs () );


        //
        // Setup debug CRT to dump memleaks to OutputDebugString and stderr
        //

#ifdef _DEBUG
        if ( !Helium::GetCmdLineFlag( Application::Args::DisableDebugHeap ) && !Helium::GetCmdLineFlag( Application::Args::DisableLeakCheck ) )
        {
            int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
            _CrtSetDbgFlag( flags | _CRTDBG_LEAK_CHECK_DF );
            _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
        }
#endif


        //
        // Disable exception handling
        //

        Helium::Debug::CleanupExceptionListener();

        if (Helium::GetCmdLineFlag( Application::Args::Profile ))
        {
            Profile::Cleanup(); 
        }

        CleanupStandardTraceFiles();

        Helium::ReleaseCmdLine();

        g_ShutdownComplete = true;
    }

    return code;
}

Log::Stream Application::GetTraceStreams()
{
    return g_TraceStreams; 
}

void Application::InitializeStandardTraceFiles()
{
    tchar module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    tchar drive[MAX_PATH];
    tchar dir[MAX_PATH];
    tchar name[MAX_PATH];
    _tsplitpath( module, drive, dir, name, NULL );

    tstring path = drive;
    path += dir;
    path += name;

    g_TraceFiles.push_back( path + TXT( ".log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Application::GetTraceStreams() );

    g_TraceFiles.push_back( path + TXT( "Warnings.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Warning );

    g_TraceFiles.push_back( path + TXT( "Errors.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Error );
}

void Application::CleanupStandardTraceFiles()
{
    for ( std::vector< tstring >::const_iterator itr = g_TraceFiles.begin(), end = g_TraceFiles.begin(); itr != end; ++itr )
    {
        Log::RemoveTraceFile( *itr );
    }

    g_TraceFiles.clear();
}

static DWORD ProcessUnhandledCxxException( LPEXCEPTION_POINTERS info )
{
    if ( info->ExceptionRecord->ExceptionCode == 0xE06D7363 )
    {
        Helium::Exception* nocturnalException = Debug::GetHeliumException( info->ExceptionRecord->ExceptionInformation[1] );

        if ( nocturnalException )
        {
            // process this as a non-fatal C++ exception, but via the SEH handler so we get the callstack at the throw() statment
            return Debug::ProcessException( info, Debug::ContinueSearch, false, false );
        }

        // this is not a nocturnal exception, so it will not be caught by the catch statements below, process it then execute the handler to unload the process
        return Debug::ProcessException( info, Debug::ExecuteHandler, true, true );
    }

    return Debug::ContinueSearch;
}

static Helium::Thread::Return StandardThreadTryExcept( Helium::Thread::Entry entry, Helium::Thread::Param param )
{
    if (Helium::IsDebuggerPresent())
    {
        return entry( param );
    }
    else
    {
        __try
        {
            return entry( param );
        }
        __except( ProcessUnhandledCxxException( GetExceptionInformation() ) )
        {
            ::ExitProcess( -1 ); // propagating the exception up doesn't lead to a good situation, just shut down
        }

        return -1;
    }
}

static Helium::Thread::Return StandardThreadTryCatch( Helium::Thread::Entry entry, Helium::Thread::Param param )
{
    if ( Helium::IsDebuggerPresent() )
    {
        return StandardThreadTryExcept( entry, param );
    }
    else
    {
        try
        {
            return StandardThreadTryExcept( entry, param );
        }
        catch ( const Helium::Exception& ex )
        {
            Log::Error( TXT( "%s\n" ), ex.What() );

            ::ExitProcess( -1 );
        }
    }
}

static Helium::Thread::Return StandardThreadEntry( Helium::Thread::Entry entry, Helium::Thread::Param param )
{
    // any normal thread startup work would go here
    return StandardThreadTryCatch( entry, param );
}

Helium::Thread::Return Application::StandardThread( Helium::Thread::Entry entry, Helium::Thread::Param param )
{
    if (Helium::IsDebuggerPresent())
    {
        return StandardThreadEntry( entry, param );
    }
    else
    {
        Debug::InitializeExceptionListener();

        Helium::Thread::Return result = -1;

        __try
        {
            result = StandardThreadEntry( entry, param );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
        {
            ::ExitProcess( -1 );
        }

        Debug::CleanupExceptionListener();

        return result;
    }
}

static int StandardMainTryExcept( int (*main)(int argc, const tchar** argv), int argc, const tchar** argv )
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

static int StandardMainTryCatch( int (*main)(int argc, const tchar** argv), int argc, const tchar** argv )
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

static int StandardMainEntry( int (*main)(int argc, const tchar** argv), int argc, const tchar** argv )
{
    int result = 0; 

    try
    {
        Application::Startup(argc, argv);
    }
    catch ( const Application::CheckVersionException& ex )
    {
        Log::Error( TXT( "%s\n" ), ex.What() );
        result = 1;
    }

    if ( result == 0 )
    {
        result = StandardMainTryCatch( main, argc, argv );
    }

    return Application::Shutdown( result );
}

int Application::StandardMain( int (*main)(int argc, const tchar** argv), int argc, const tchar** argv )
{
    if (Helium::IsDebuggerPresent())
    {
        return StandardMainEntry( main, argc, argv );
    }
    else
    {
        int result = -1;

        Debug::InitializeExceptionListener();

        __try
        {
            result = StandardMainEntry( main, argc, argv );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
        {
            ::ExitProcess( Application::Shutdown( result ) );
        }

        Debug::CleanupExceptionListener();

        return result;
    }
}

#if defined( WIN32 ) && defined ( _WINDOWS_ )

static int StandardWinMainTryExcept( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
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

static int StandardWinMainTryCatch( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd)
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
            MessageBox(NULL, ex.What(), TXT( "Error" ), MB_OK|MB_ICONEXCLAMATION);

            ::ExitProcess( -1 );
        }

        return result;
    }
}

static int StandardWinMainEntry( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd )
{
    int argc = 0;
    const tchar** argv = NULL;
    Helium::ProcessCmdLine( lpCmdLine, argc, argv );

    int result = 0;

    try
    {
        Application::Startup(argc, argv);
    }
    catch ( const Application::CheckVersionException& ex )
    {
        result = 1;
        Log::Error( TXT( "%s\n" ), ex.What() );
        MessageBox(NULL, ex.What(), TXT( "Fatal Error" ), MB_OK|MB_ICONEXCLAMATION);
    }

    if ( result == 0 )
    {
        result = StandardWinMainTryCatch( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }

    Application::Shutdown( result );

    delete[] argv;

    return result;
}

namespace Helium
{
    namespace Application
    {
        APPLICATION_API int StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ),
            HINSTANCE hInstance = ::GetModuleHandle(NULL),
            HINSTANCE hPrevInstance = NULL,
            LPTSTR lpCmdLine = ::GetCommandLine(),
            int nShowCmd = SW_SHOWNORMAL );
    }
}

int Application::StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd )
{
    if (Helium::IsDebuggerPresent())
    {
        return StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
    }
    else
    {
        int result = -1;

        Debug::InitializeExceptionListener();

        __try
        {
            result = StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
        {
            ::ExitProcess( Application::Shutdown( result ) );
        }

        Debug::CleanupExceptionListener();

        return result;
    }
}

#endif