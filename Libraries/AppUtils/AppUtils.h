#pragma once

#include <string>
#include <vector>

#include "API.h"
#include "Exceptions.h"
#include "Preferences.h"

#include "Platform/Types.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Log.h"
#include "Debug/Symbols.h"

#ifdef _WINDOWS_
#include "Platform/Thread.h"
#endif

namespace IPC
{
  class Message;
}

namespace AppUtils
{
  //
  // Top level pervasive command line args
  //

  struct APPUTILS_API Args
  {
    static const char* Script;
    static const char* Attach;
    static const char* Profile;
    static const char* Memory;
    static const char* Verbose;
    static const char* Extreme;
    static const char* Debug;

#ifdef _DEBUG
    static const char* DisableDebugHeap;
    static const char* DisableLeakCheck;
    static const char* CheckHeap;
#endif
  };


  //
  // Application Startup/Shutdown
  //

  // Initialize AppUtils support in your application.  Call once
  //  * Setup debug heap checking
  //  * Disable Message-Box based error checking for console applications
  //  * Perform version checking (to validate execution)
  //  * Initialize Console printing system
  //  * Inherit automatic command line args from the environment
  APPUTILS_API void Startup( int argc = 0, const char** argv = NULL, bool checkVersion = true );

  // Shutdown your application
  //  * Cleans up global initializer stack to release memory so its not reported as memory leaks
  //  * Report profiling summary
  //  * Report execution time
  //  * Invoke exit() (if console app)
  APPUTILS_API int Shutdown( int code = 0 );


  //
  // Shutdown Event
  //

  struct ShutdownArgs
  {
    int m_Code;
  };
  typedef Nocturnal::Signature<void, const ShutdownArgs&> ShutdownSignature;
  extern APPUTILS_API ShutdownSignature::Event g_ShuttingDown;

  //
  // Trace Files
  //
  APPUTILS_API void InitializeStandardTraceFiles();
  APPUTILS_API void CleanupStandardTraceFiles();


  //
  // Application behavior
  //

  // Did you make these tools, or did you download a release
  APPUTILS_API bool IsToolsBuilder();

  // Detects if a debugger is attached to the process
  APPUTILS_API bool IsDebuggerPresent();

  // Which streams should go into the trace file
  APPUTILS_API ::Log::Stream GetTraceStreams(); 


  //
  // Main wrapper functions
  //
  // Use StandardMain for:
  //  * Breakpoint instruction handling - GUI handler makes breakpoints outside the debugger continuable/disableable
  //  * SEH Exception (crash) handling - email reports for illegal instructions or unhandled c++ exceptions
  //  * Top level C++ exception handler that catches Nocturnal::Exceptions and reports them as errors to the user
  //  * Automatic calling of AppUtils::Startup and AppUtils::Shutdown
  //  * Automatic trace file output
  //

  //
  // Thread entry wrapper
  //  void thread()
  //  {
  //    return AppUtils::StandardThread( &::Thread, param );
  //  }
  //

#ifdef _WINDOWS_
  APPUTILS_API Platform::Thread::Return StandardThread( Platform::Thread::Entry entry, Platform::Thread::Param param );
#endif

  //
  // Main wrappers for console applications
  //  int main(int argc, const char** argv)
  //  {
  //    return AppUtils::StandardMain( &::Main, argc, argv );
  //  }
  //

  APPUTILS_API int StandardMain( int (*main)(int argc, const char** argv), int argc, const char** argv, bool checkVersion = true );

  //
  // Main wrappers for windows applications
  //  int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
  //  {
  //    return AppUtils::ProtectedWinMain( &WinMain, hInstance, hPrevInstance, lpCmdLine, int nShowCmd );
  //  }
  //  
  //  or, for windows applications running with the console subsystem set:
  //
  //  int main()
  //  {
  //    return AppUtils::StandardWinMain( &::WinMain );
  //  }
  //

#ifdef _WINDOWS_
  APPUTILS_API int StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ),
                                    HINSTANCE hInstance = ::GetModuleHandle(NULL),
                                    HINSTANCE hPrevInstance = NULL,
                                    LPSTR lpCmdLine = ::GetCommandLine(),
                                    int nShowCmd = SW_SHOWNORMAL,
                                    bool checkVersion = true );
#endif
}
