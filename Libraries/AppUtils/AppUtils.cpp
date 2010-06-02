#include "Windows/Windows.h"
#include "AppUtils.h"

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

#include "Common/Assert.h"
#include "Common/Config.h"
#include "Common/Version.h"
#include "Common/Environment.h"
#include "Common/CommandLine.h"

#include "Exceptions.h"
#include "ExceptionListener.h"
#include "Version.h"

#include "Debug/Exception.h"
#include "FileSystem/FileSystem.h"
#include "Profile/Profile.h"
#include "Windows/Error.h"
#include "Windows/Process.h"
#include "Console/Console.h"
#include "Common/Config.h"

const char* AppUtils::Args::Script = "script";
const char* AppUtils::Args::Attach = "attach";
const char* AppUtils::Args::Profile = "profile";
const char* AppUtils::Args::Memory = "memory";
const char* AppUtils::Args::Verbose = "verbose";
const char* AppUtils::Args::Extreme = "extreme";
const char* AppUtils::Args::Debug = "debug";

#ifdef _DEBUG
const char* AppUtils::Args::DisableDebugHeap = "no_debug_heap";
const char* AppUtils::Args::DisableLeakCheck = "no_leak_check";
const char* AppUtils::Args::CheckHeap = "check_heap";
#endif

using namespace AppUtils;

// init time
_timeb g_StartTime;

// are we initialized
static i32 g_InitCount = 0;

// the event we raise when we shutdown
ShutdownSignature::Event AppUtils::g_ShuttingDown;

// are we shutting down
bool g_ShutdownStarted = false;
bool g_ShutdownComplete = false;

// default to these streams for trace files, it is up to the app to ask for these, when creating a TraceFile
V_string g_TraceFiles;
Console::Stream g_TraceStreams  = Console::Streams::Normal | Console::Streams::Warning | Console::Streams::Error; 

// so you can set _crtBreakAlloc in the debugger (expression evaluator doesn't like it)
#ifdef _DEBUG
namespace AppUtils
{
  long& g_BreakOnAlloc (_crtBreakAlloc);
}
#endif //_DEBUG

void AppUtils::Startup( int argc, const char** argv, bool checkVersion )
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
      Nocturnal::SetCmdLine( argc, argv );
    }

    if ( Nocturnal::GetCmdLineFlag( AppUtils::Args::Attach ) )
    {
      i32 timeout = 300; // 5min

      Console::Print("Waiting %d minutes for debugger to attach...\n", timeout / 60);

      while ( !AppUtils::IsDebuggerPresent() && timeout-- )
      {
        Sleep( 1000 );
      }

      if ( AppUtils::IsDebuggerPresent() )
      {
        Console::Print("Debugger attached\n");
        NOC_ISSUE_BREAK();
      }
    }


    //
    // Setup debug CRT
    //

#ifdef _DEBUG
    int flags = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    if (Nocturnal::GetCmdLineFlag( AppUtils::Args::DisableDebugHeap ))
    {
      flags = 0x0;
    }
    else if (Nocturnal::GetCmdLineFlag( AppUtils::Args::CheckHeap ))
    {
      flags |= (flags & 0x0000FFFF) | _CRTDBG_CHECK_ALWAYS_DF; // clear the upper 16 bits and OR in the desired freqency (always)
    }
    _CrtSetDbgFlag(flags);
#endif


    //
    // Only print startup summary if we are not in script mode
    //

    if ( !Nocturnal::GetCmdLineFlag( AppUtils::Args::Script ) )
    {
      //
      // Print project and version info
      //
      char module[MAX_PATH];
      GetModuleFileName( 0, module, MAX_PATH );

      char name[MAX_PATH];
      _splitpath( module, NULL, NULL, name, NULL );

      Console::Print( "Running %s\n", name );
      Console::Print( "Tools: " NOCTURNAL_PROJECT_NAME " / " NOCTURNAL_VERSION_STRING "\n" );
      Console::Print( "Current Time: %s", ctime( &g_StartTime.time ) );
      Console::Print( "Command Line: %s\n", Nocturnal::GetCmdLine() );
    }


    //
    // Setup Console
    //

    if ( Nocturnal::GetCmdLineFlag( AppUtils::Args::Extreme ) )
    {
      Console::SetLevel( Console::Levels::Extreme );
    }
    else if ( Nocturnal::GetCmdLineFlag( AppUtils::Args::Verbose ) )
    {
      Console::SetLevel( Console::Levels::Verbose );
    }

    Console::EnableStream( Console::Streams::Debug, Nocturnal::GetCmdLineFlag( AppUtils::Args::Debug ) );
    Console::EnableStream( Console::Streams::Profile, Nocturnal::GetCmdLineFlag( AppUtils::Args::Profile ) );

    if( Nocturnal::GetCmdLineFlag( AppUtils::Args::Debug ) )
    {
      // add the debug stream to the trace
      g_TraceStreams |= Console::Streams::Debug; 

      // dump env
      if ( Nocturnal::GetCmdLineFlag( AppUtils::Args::Verbose ) )
      {
        // get a pointer to the environment block. 
        const char* env = (const char*)GetEnvironmentStrings();

        // if the returned pointer is NULL, exit.
        if (env)
        {
          Console::Debug("\n");
          Console::Debug("Environment:\n");

          // variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
          for (const char* var = (const char*)env; *var; var++) 
          {
            if (*var != '=') // WTF?
            {
              Console::Debug(" %s\n", var);
            }

            while (*var)
            {
              var++;
            }
          }

          FreeEnvironmentStrings((char*)env);
        }
      }
    }

    if( Nocturnal::GetCmdLineFlag( AppUtils::Args::Profile ) )
    {
      // init profiling
      Profile::Initialize(); 

      // add the profile stream to the trace
      g_TraceStreams |= Console::Streams::Profile; 

      // enable memory reports
      if ( Nocturnal::GetCmdLineFlag( AppUtils::Args::Memory ) )
      {
        Profile::Memory::Initialize();
      }
    }

    //
    // Version check
    //

    if ( checkVersion && !IsToolsBuilder() )
    {
      CheckVersion();
    }


    //
    // Report inherited args
    //

    const char* inherited = getenv( NOCTURNAL_STUDIO_PREFIX "CMD_ARGS" );
    if ( inherited )
    {
      Console::Print("Inheriting Args: %s\n", inherited);
    }


    //
    // Setup exception handlers, do this last
    //

    // init debug handling
    InitializeExceptionListener();

    // handle 'new' errors, invalid parameters, etc...
    Windows::EnableCPPErrorHandling(true);

    // disable dialogs for main line error cases
    SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX );
  }
}

int AppUtils::Shutdown( int code )
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

    if (Nocturnal::GetCmdLineFlag( AppUtils::Args::Profile ))
    {
      Profile::Memory::Cleanup();
      Profile::Accumulator::ReportAll();
    }


    //
    // Only print shutdown summary if we are not in script mode
    //

    if ( !Nocturnal::GetCmdLineFlag( AppUtils::Args::Script ) )
    {
      //
      // Print time usage
      //
      Console::Print( "\n" );

      _timeb endTime;
      _ftime(&endTime); 
      Console::Print( "Current Time: %s", ctime( &endTime.time ) );

      int time = (int) (((endTime.time*1000) + endTime.millitm) - ((g_StartTime.time*1000) +  g_StartTime.millitm));
      int milli = time % 1000; time /= 1000;
      int sec = time % 60; time /= 60;
      int min = time % 60; time /= 60;
      int hour = time;

      if (hour > 0)
      {
        Console::Print("Execution Time: %d:%02d:%02d.%02d hours\n", hour, min, sec, milli);
      }
      else
      {
        if (min > 0)
        {
          Console::Print("Execution Time: %d:%02d.%02d minutes\n", min, sec, milli);
        }
        else
        {
          if (sec > 0)
          {
            Console::Print("Execution Time: %d.%02d seconds\n", sec, milli);
          }
          else
          {
            if (milli > 0)
            {
              Console::Print("Execution Time: %02d milliseconds\n", milli);
            }
          }
        }
      }


      //
      // Print general success or failure, depends on the result code
      //
      char module[MAX_PATH];
      GetModuleFileName( 0, module, MAX_PATH );

      char name[MAX_PATH];
      _splitpath( module, NULL, NULL, name, NULL );

      Console::Print( "%s: ", name );
      Console::PrintString( code ? "Failed" : "Succeeeded", Console::Streams::Normal, Console::Levels::Default, code ? Console::Colors::Red : Console::Colors::Green );


      //
      // Print warning/error count
      //

      if (Console::GetWarningCount() || Console::GetErrorCount())
      {
        Console::Print(" with");
      }

      if (Console::GetErrorCount())
      {
        char buf[80];
        sprintf(buf, " %d error%s", Console::GetErrorCount(), Console::GetErrorCount() > 1 ? "s" : "");
        Console::PrintString( buf, Console::Streams::Normal, Console::Levels::Default, Console::Colors::Red );
      }

      if (Console::GetWarningCount() && Console::GetErrorCount())
      {
        Console::Print(" and");
      }

      if (Console::GetWarningCount())
      {
        char buf[80];
        sprintf(buf, " %d warning%s", Console::GetWarningCount(), Console::GetWarningCount() > 1 ? "s" : "");
        Console::PrintString( buf, Console::Streams::Normal, Console::Levels::Default, Console::Colors::Yellow );
      }

      Console::Print("\n");
    }


    //
    // Raise Shutdown Event
    //

    g_ShuttingDown.Raise( ShutdownArgs () );


    //
    // Setup debug CRT to dump memleaks to OutputDebugString and stderr
    //

#ifdef _DEBUG
    if ( !Nocturnal::GetCmdLineFlag( AppUtils::Args::DisableDebugHeap ) && !Nocturnal::GetCmdLineFlag( AppUtils::Args::DisableLeakCheck ) )
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

    CleanupExceptionListener();

    if (Nocturnal::GetCmdLineFlag( AppUtils::Args::Profile ))
    {
      Profile::Cleanup(); 
    }

    CleanupStandardTraceFiles();

    Nocturnal::ReleaseCmdLine();

    g_ShutdownComplete = true;
  }

  return code;
}

Console::Stream AppUtils::GetTraceStreams()
{
  return g_TraceStreams; 
}

void AppUtils::InitializeStandardTraceFiles()
{
  char module[MAX_PATH];
  GetModuleFileName( 0, module, MAX_PATH );

  char name[MAX_PATH];
  _splitpath( module, NULL, NULL, name, NULL );

  const char* logVar = getenv( NOCTURNAL_STUDIO_PREFIX"PROJECT_LOG" );
  std::string logFolder = logVar ? logVar : "";
  if ( !logFolder.empty() && *logFolder.rbegin() != '/' && *logFolder.rbegin() != '\\' )
  {
    logFolder += "/";
  }
  
  g_TraceFiles.push_back( logFolder + name + ".log" );
  Console::AddTraceFile( g_TraceFiles.back(), AppUtils::GetTraceStreams() );
  
  g_TraceFiles.push_back( logFolder + name + "Warnings.log" );
  Console::AddTraceFile( g_TraceFiles.back(), Console::Streams::Warning );

  g_TraceFiles.push_back( logFolder + name + "Errors.log" );
  Console::AddTraceFile( g_TraceFiles.back(), Console::Streams::Error );
}

void AppUtils::CleanupStandardTraceFiles()
{
  for ( V_string::const_iterator itr = g_TraceFiles.begin(), 
    end = g_TraceFiles.begin(); itr != end; ++itr )
  {
    Console::RemoveTraceFile( *itr );
  }

  g_TraceFiles.clear();
}

bool AppUtils::IsToolsBuilder()
{
  bool toolsBuilder = false;
  Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX "TOOLS_BUILDER", toolsBuilder );
  return toolsBuilder;
}

bool AppUtils::IsDebuggerPresent()
{
  return ::IsDebuggerPresent() != 0;
}

static DWORD ProcessUnhandledCxxException( LPEXCEPTION_POINTERS info )
{
  if ( info->ExceptionRecord->ExceptionCode == 0xE06D7363 )
  {
    std::exception* cppException = Debug::GetCxxException( info->ExceptionRecord->ExceptionInformation[1] );

    if ( cppException )
    {
      Nocturnal::Exception* nocException = dynamic_cast<Nocturnal::Exception*>( cppException );

      if ( nocException )
      {
        // process this as a non-fatal C++ exception, but via the SEH handler so we get the callstack at the throw() statment
        return Debug::ProcessException( info, Debug::ContinueSearch, false, false );
      }
    }

    // this is not a nocturnal exception, so it will not be caught by the catch statements below, process it then execute the handler to unload the process
    return Debug::ProcessException( info, Debug::ExecuteHandler, true, true );
  }

  return Debug::ContinueSearch;
}

static Platform::Thread::Return StandardThreadTryExcept( Platform::Thread::Entry entry, Platform::Thread::Param param )
{
  if (AppUtils::IsDebuggerPresent())
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

static Platform::Thread::Return StandardThreadTryCatch( Platform::Thread::Entry entry, Platform::Thread::Param param )
{
  if ( AppUtils::IsDebuggerPresent() )
  {
    return StandardThreadTryExcept( entry, param );
  }
  else
  {
    try
    {
      return StandardThreadTryExcept( entry, param );
    }
    catch ( const Nocturnal::Exception& ex )
    {
      Console::Error( "%s\n", ex.what() );

      ::ExitProcess( -1 );
    }
  }
}

static Platform::Thread::Return StandardThreadEntry( Platform::Thread::Entry entry, Platform::Thread::Param param )
{
  // any normal thread startup work would go here
  return StandardThreadTryCatch( entry, param );
}

Platform::Thread::Return AppUtils::StandardThread( Platform::Thread::Entry entry, Platform::Thread::Param param )
{
  if (AppUtils::IsDebuggerPresent())
  {
    return StandardThreadEntry( entry, param );
  }
  else
  {
    InitializeExceptionListener();

    Platform::Thread::Return result = -1;

    __try
    {
      result = StandardThreadEntry( entry, param );
    }
    __except( ( g_ShutdownComplete || AppUtils::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
    {
      ::ExitProcess( -1 );
    }

    CleanupExceptionListener();

    return result;
  }
}

static int StandardMainTryExcept( int (*main)(int argc, const char** argv), int argc, const char** argv )
{
  if (AppUtils::IsDebuggerPresent())
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
  if ( AppUtils::IsDebuggerPresent() )
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
    catch ( const Nocturnal::Exception& ex )
    {
      Console::Error( "%s\n", ex.what() );

      ::ExitProcess( -1 );
    }

    return result;
  }
}

static int StandardMainEntry( int (*main)(int argc, const char** argv), int argc, const char** argv, bool checkVersion )
{
  int result = 0; 

  try
  {
    AppUtils::Startup(argc, argv, checkVersion);
  }
  catch ( const AppUtils::CheckVersionException& ex )
  {
    Console::Error( "%s\n", ex.what() );
    result = 1;
  }

  if ( result == 0 )
  {
    result = StandardMainTryCatch( main, argc, argv );
  }

  return AppUtils::Shutdown( result );
}

int AppUtils::StandardMain( int (*main)(int argc, const char** argv), int argc, const char** argv, bool checkVersion )
{
  if (AppUtils::IsDebuggerPresent())
  {
    return StandardMainEntry( main, argc, argv, checkVersion );
  }
  else
  {
    int result = -1;

    InitializeExceptionListener();

    __try
    {
      result = StandardMainEntry( main, argc, argv, checkVersion );
    }
    __except( ( g_ShutdownComplete || AppUtils::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
    {
      ::ExitProcess( AppUtils::Shutdown( result ) );
    }

    CleanupExceptionListener();

    return result;
  }
}

static int StandardWinMainTryExcept( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  if (AppUtils::IsDebuggerPresent())
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

static int StandardWinMainTryCatch( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
  if ( AppUtils::IsDebuggerPresent() )
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
    catch ( const Nocturnal::Exception& ex )
    {
      Console::Error( "%s\n", ex.what() );
      MessageBox(NULL, ex.what(), "Error", MB_OK|MB_ICONEXCLAMATION);

      ::ExitProcess( -1 );
    }

    return result;
  }
}

static int StandardWinMainEntry( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd, bool checkVersion )
{
  int argc = 0;
  const char** argv = NULL;
  Nocturnal::ProcessCmdLine( lpCmdLine, argc, argv );

  int result = 0;

  try
  {
    AppUtils::Startup(argc, argv, checkVersion);
  }
  catch ( const AppUtils::CheckVersionException& ex )
  {
    result = 1;
    Console::Error( "%s\n", ex.what() );
    MessageBox(NULL, ex.what(), "Fatal Error", MB_OK|MB_ICONEXCLAMATION);
  }

  if ( result == 0 )
  {
    result = StandardWinMainTryCatch( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd );
  }

  AppUtils::Shutdown( result );

  delete[] argv;

  return result;
}

int AppUtils::StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ), HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd, bool checkVersion )
{
  if (AppUtils::IsDebuggerPresent())
  {
    return StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd, checkVersion );
  }
  else
  {
    int result = -1;

    InitializeExceptionListener();

    __try
    {
      result = StandardWinMainEntry( winMain, hInstance, hPrevInstance, lpCmdLine, nShowCmd, checkVersion );
    }
    __except( ( g_ShutdownComplete || AppUtils::IsDebuggerPresent() ) ? EXCEPTION_CONTINUE_SEARCH : Debug::ProcessException( GetExceptionInformation(), Debug::GetExceptionBehavior(), true, true ) )
    {
      ::ExitProcess( AppUtils::Shutdown( result ) );
    }

    CleanupExceptionListener();

    return result;
  }
}
