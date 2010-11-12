#include "Startup.h"

#include "Platform/Assert.h"
#include "Platform/PlatformUtility.h"
#include "Platform/Process.h"
#include "Platform/Debug.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/Exception.h"
#include "Foundation/ExceptionReport.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/Localization.h"

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

const tchar_t* StartupArgs::Script = TXT( "script" );
const tchar_t* StartupArgs::Attach = TXT( "attach" );
const tchar_t* StartupArgs::Profile = TXT( "profile" );
const tchar_t* StartupArgs::Memory = TXT( "memory" );
const tchar_t* StartupArgs::Verbose = TXT( "verbose" );
const tchar_t* StartupArgs::Extreme = TXT( "extreme" );
const tchar_t* StartupArgs::Debug = TXT( "debug" );

#ifdef _DEBUG
const tchar_t* StartupArgs::DisableDebugHeap = TXT( "no_debug_heap" );
const tchar_t* StartupArgs::DisableLeakCheck = TXT( "no_leak_check" );
const tchar_t* StartupArgs::CheckHeap = TXT( "check_heap" );
#endif

using namespace Helium;

// init time
_timeb g_StartTime;

// are we initialized
static int32_t g_InitCount = 0;

// the event we raise when we shutdown
ShutdownSignature::Event Helium::g_ShuttingDown;

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
    long& g_BreakOnAlloc (_crtBreakAlloc);
}
#endif //_DEBUG

namespace Helium
{
	static Localization::StringTable g_StringTable( "Helium" );
}

void Helium::Startup( int argc, const tchar_t** argv )
{
    if ( ++g_InitCount == 1 )
    {

        g_StringTable.AddString( "en", "WaitingDebuggerAttach", TXT( "Waiting <MINUTES> minutes for debugger to attach...\n" ) );
        g_StringTable.AddString( "en", "DebuggerAttached", TXT( "Debugger attached\n" ) );
        g_StringTable.AddString( "en", "RunningApp", TXT( "Running <APPNAME>...\n" ) );
        g_StringTable.AddString( "en", "CurrentTime", TXT( "Current Time: <TIME>\n" ) );
        g_StringTable.AddString( "en", "CommandLine", TXT( "Command Line: <COMMANDLINE>\n" ) );

        Localization::GlobalLocalizer().RegisterTable( &g_StringTable );

#pragma TODO( "Set the language at some more appropriate point in the code?" )
        Localization::GlobalLocalizer().SetLanguageId( "en" );



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

        if ( Helium::GetCmdLineFlag( StartupArgs::Attach ) )
        {
            int32_t timeout = 300; // 5min

            Localization::Statement stmt( "Helium", "WaitingDebuggerAttach" );
            stmt.ReplaceKey( TXT( "MINUTES" ), timeout / 60 );
            Log::Print( stmt.Get().c_str() );

            while ( !Helium::IsDebuggerPresent() && timeout-- )
            {
                Sleep( 1000 );
            }

            if ( Helium::IsDebuggerPresent() )
            {
                Localization::Statement stmt( "Helium", "DebuggerAttached" );
                Log::Print( stmt.Get().c_str() );
                HELIUM_ISSUE_BREAK();
            }
        }


        //
        // Setup debug CRT
        //

#ifdef _DEBUG
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


        //
        // Only print startup summary if we are not in script mode
        //

        if ( !Helium::GetCmdLineFlag( StartupArgs::Script ) )
        {
            //
            // Print project and version info
            //
            tchar_t module[MAX_PATH];
            GetModuleFileName( 0, module, MAX_PATH );

            tchar_t name[MAX_PATH];
            _tsplitpath( module, NULL, NULL, name, NULL );

            Localization::Statement stmt( "Helium", "RunningApp" );
            stmt.ReplaceKey( TXT( "APPNAME" ), name );
            Log::Print( stmt.Get().c_str() );

            stmt.Set( "Helium", "CurrentTime" );
            stmt.ReplaceKey( TXT( "TIME" ), _tctime64( &g_StartTime.time ) );
            Log::Print( stmt.Get().c_str() );

            stmt.Set( "Helium", "CommandLine" );
            stmt.ReplaceKey( TXT( "COMMANDLINE" ), Helium::GetCmdLine() );
            Log::Print( stmt.Get().c_str() );
        }


        //
        // Setup Console
        //

        if ( Helium::GetCmdLineFlag( StartupArgs::Extreme ) )
        {
            Log::SetLevel( Log::Levels::Extreme );
        }
        else if ( Helium::GetCmdLineFlag( StartupArgs::Verbose ) )
        {
            Log::SetLevel( Log::Levels::Verbose );
        }

        Log::EnableStream( Log::Streams::Debug, Helium::GetCmdLineFlag( StartupArgs::Debug ) );
        Log::EnableStream( Log::Streams::Profile, Helium::GetCmdLineFlag( StartupArgs::Profile ) );

        if( Helium::GetCmdLineFlag( StartupArgs::Debug ) )
        {
            // add the debug stream to the trace
            g_TraceStreams |= Log::Streams::Debug; 

            // dump env
            if ( Helium::GetCmdLineFlag( StartupArgs::Verbose ) )
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

                    FreeEnvironmentStrings((tchar_t*)env);
                }
            }
        }

        if( Helium::GetCmdLineFlag( StartupArgs::Profile ) )
        {
            // init profiling
            Profile::Initialize(); 

            // add the profile stream to the trace
            g_TraceStreams |= Log::Streams::Profile; 

            // enable memory reports
            if ( Helium::GetCmdLineFlag( StartupArgs::Memory ) )
            {
                Profile::Memory::Initialize();
            }
        }


        //
        // Setup exception handlers, do this last
        //

        // handle invalid parameters, etc...
        Helium::EnableCPPErrorHandling( true );

        // init debug handling
        Helium::Debug::InitializeExceptionListener();

        // disable dialogs for main line error cases
        SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
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


        Localization::Cleanup();

        //
        // This should be done first, so that dynamic libraries to be freed in Cleanup()
        //  don't cause breakage in profile
        //

        if (Helium::GetCmdLineFlag( StartupArgs::Profile ))
        {
            Profile::Memory::Cleanup();
            Profile::Accumulator::ReportAll();
        }


        //
        // Only print shutdown summary if we are not in script mode
        //

        if ( !Helium::GetCmdLineFlag( StartupArgs::Script ) )
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
            tchar_t module[MAX_PATH];
            GetModuleFileName( 0, module, MAX_PATH );

            tchar_t name[MAX_PATH];
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
                tchar_t buf[80];
                _stprintf( buf, TXT( " %d error%s" ), Log::GetErrorCount(), Log::GetErrorCount() > 1 ? TXT( "s" ) : TXT( "" ) );
                Log::PrintString( buf, Log::Streams::Normal, Log::Levels::Default, Log::Colors::Red );
            }

            if (Log::GetWarningCount() && Log::GetErrorCount())
            {
                Log::Print( TXT( " and" ) );
            }

            if (Log::GetWarningCount())
            {
                tchar_t buf[80];
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


        //
        // Disable exception handling
        //

        Helium::Debug::CleanupExceptionListener();

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
    tchar_t module[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );

    tchar_t drive[MAX_PATH];
    tchar_t dir[MAX_PATH];
    tchar_t name[MAX_PATH];
    _tsplitpath( module, drive, dir, name, NULL );

    tstring path = drive;
    path += dir;
    path += name;

    g_TraceFiles.push_back( path + TXT( ".log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Helium::GetTraceStreams() );

    g_TraceFiles.push_back( path + TXT( "Warnings.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Warning );

    g_TraceFiles.push_back( path + TXT( "Errors.log" ) );
    Log::AddTraceFile( g_TraceFiles.back(), Log::Streams::Error );
}

void Helium::CleanupStandardTraceFiles()
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
            return Debug::ProcessException( info, false, false );
        }

        // this is not a nocturnal exception, so it will not be caught by the catch statements below, process it then execute the handler to unload the process
        return Debug::ProcessException( info, true, true );
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

void Helium::StandardThread( Helium::CallbackThread::Entry entry, void* param )
{
    if (Helium::IsDebuggerPresent())
    {
        StandardThreadEntry( entry, param );
    }
    else
    {
        Debug::InitializeExceptionListener();

        __try
        {
            StandardThreadEntry( entry, param );
        }
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( -1 );
        }

        Debug::CleanupExceptionListener();
    }
}

static int StandardMainTryExcept( int (*main)(int argc, const tchar_t** argv), int argc, const tchar_t** argv )
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

static int StandardMainTryCatch( int (*main)(int argc, const tchar_t** argv), int argc, const tchar_t** argv )
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

static int StandardMainEntry( int (*main)(int argc, const tchar_t** argv), int argc, const tchar_t** argv )
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

int Helium::StandardMain( int (*main)(int argc, const tchar_t** argv), int argc, const tchar_t** argv )
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
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( Helium::Shutdown( result ) );
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
    const tchar_t** argv = NULL;
    Helium::ProcessCmdLine( lpCmdLine, argc, argv );

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

namespace Helium
{
    FOUNDATION_API int StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ),
        HINSTANCE hInstance = ::GetModuleHandle(NULL),
        HINSTANCE hPrevInstance = NULL,
        LPTSTR lpCmdLine = ::GetCommandLine(),
        int nShowCmd = SW_SHOWNORMAL );
}

int Helium::StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nShowCmd )
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
        __except( ( g_ShutdownComplete || Helium::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), true, true ) )
        {
            ::ExitProcess( Helium::Shutdown( result ) );
        }

        Debug::CleanupExceptionListener();

        return result;
    }
}

#endif