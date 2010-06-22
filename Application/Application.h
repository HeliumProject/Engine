#pragma once

#include <string>
#include <vector>

#include "Application/API.h"
#include "Exceptions.h"
#include "Preferences.h"

#include "Platform/Types.h"
#include "Platform/Thread.h"
#include "Foundation/Automation/Event.h"
#include "Foundation/Log.h"

namespace Application
{
  //
  // Top level pervasive command line args
  //

  struct APPLICATION_API Args
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

  // Initialize Application support in your application.  Call once
  //  * Setup debug heap checking
  //  * Disable Message-Box based error checking for console applications
  //  * Perform version checking (to validate execution)
  //  * Initialize Console printing system
  //  * Inherit automatic command line args from the environment
  APPLICATION_API void Startup( int argc = 0, const char** argv = NULL, bool checkVersion = true );

  // Shutdown your application
  //  * Cleans up global initializer stack to release memory so its not reported as memory leaks
  //  * Report profiling summary
  //  * Report execution time
  //  * Invoke exit() (if console app)
  APPLICATION_API int Shutdown( int code = 0 );


  //
  // Shutdown Event
  //

  struct ShutdownArgs
  {
    int m_Code;
  };
  typedef Nocturnal::Signature<void, const ShutdownArgs&> ShutdownSignature;
  extern APPLICATION_API ShutdownSignature::Event g_ShuttingDown;

  //
  // Trace Files
  //
  APPLICATION_API void InitializeStandardTraceFiles();
  APPLICATION_API void CleanupStandardTraceFiles();


  //
  // Application behavior
  //

  // Detects if a debugger is attached to the process
  APPLICATION_API bool IsDebuggerPresent();

  // Which streams should go into the trace file
  APPLICATION_API ::Log::Stream GetTraceStreams(); 


  //
  // Main wrapper functions
  //
  // Use StandardMain for:
  //  * Breakpoint instruction handling - GUI handler makes breakpoints outside the debugger continuable/disableable
  //  * SEH Exception (crash) handling - email reports for illegal instructions or unhandled c++ exceptions
  //  * Top level C++ exception handler that catches Nocturnal::Exceptions and reports them as errors to the user
  //  * Automatic calling of Application::Startup and Application::Shutdown
  //  * Automatic trace file output
  //

  //
  // Thread entry wrapper
  //  void thread()
  //  {
  //    return Application::StandardThread( &::Thread, param );
  //  }
  //

  APPLICATION_API Platform::Thread::Return StandardThread( Platform::Thread::Entry entry, Platform::Thread::Param param );

  //
  // Main wrappers for console applications
  //  int main(int argc, const char** argv)
  //  {
  //    return Application::StandardMain( &::Main, argc, argv );
  //  }
  //

  APPLICATION_API int StandardMain( int (*main)(int argc, const char** argv), int argc, const char** argv, bool checkVersion = true );

  //
  // Main wrappers for windows applications
  //  int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
  //  {
  //    return Application::ProtectedWinMain( &WinMain, hInstance, hPrevInstance, lpCmdLine, int nShowCmd );
  //  }
  //  
  //  or, for windows applications running with the console subsystem set:
  //
  //  int main()
  //  {
  //    return Application::StandardWinMain( &::WinMain );
  //  }
  //

#if defined( WIN32 ) && defined ( _WINDOWS_ )
  APPLICATION_API int StandardWinMain( int (*winMain)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd ),
                                    HINSTANCE hInstance = ::GetModuleHandle(NULL),
                                    HINSTANCE hPrevInstance = NULL,
                                    LPSTR lpCmdLine = ::GetCommandLine(),
                                    int nShowCmd = SW_SHOWNORMAL,
                                    bool checkVersion = true );
#endif
}