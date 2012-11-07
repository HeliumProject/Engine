#pragma once

#include <string>
#include <vector>

#include "Application/API.h"
#include "Preferences.h"

#include "Platform/Types.h"
#include "Platform/Thread.h"
#include "Foundation/Event.h"
#include "Foundation/Log.h"

namespace Helium
{
    //
    // Top level pervasive command line args
    //

    struct HELIUM_APPLICATION_API StartupArgs
    {
        static const tchar_t* Script;
        static const tchar_t* Attach;
        static const tchar_t* Profile;
        static const tchar_t* Memory;
        static const tchar_t* Verbose;
        static const tchar_t* Extreme;
        static const tchar_t* Debug;

#ifdef _DEBUG
        static const tchar_t* DisableDebugHeap;
        static const tchar_t* DisableLeakCheck;
        static const tchar_t* CheckHeap;
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
    HELIUM_APPLICATION_API void Startup( int argc = 0, const tchar_t** argv = NULL );

    // Shutdown your application
    //  * Cleans up global initializer stack to release memory so its not reported as memory leaks
    //  * Report profiling summary
    //  * Report execution time
    //  * Invoke exit() (if console app)
    HELIUM_APPLICATION_API int Shutdown( int code = 0 );


    //
    // Shutdown Event
    //

    struct ShutdownArgs
    {
        int m_Code;
    };
    typedef Helium::Signature< const ShutdownArgs&> ShutdownSignature;
    extern HELIUM_APPLICATION_API ShutdownSignature::Event g_ShuttingDown;

    //
    // Trace Files
    //
    HELIUM_APPLICATION_API void InitializeStandardTraceFiles();
    HELIUM_APPLICATION_API void CleanupStandardTraceFiles();


    //
    // Application behavior
    //

    // Which streams should go into the trace file
    HELIUM_APPLICATION_API Log::Stream GetTraceStreams(); 


    //
    // Main wrapper functions
    //
    // Use StandardMain for:
    //  * Breakpoint instruction handling - GUI handler makes breakpoints outside the debugger continuable/disableable
    //  * SEH Exception (crash) handling - email reports for illegal instructions or unhandled c++ exceptions
    //  * Top level C++ exception handler that catches Helium::Exceptions and reports them as errors to the user
    //  * Automatic calling of Startup and Shutdown
    //  * Automatic trace file output
    //

    //
    // Thread entry wrapper
    //  void thread()
    //  {
    //    StandardThread( &::Thread, param );
    //  }
    //

    HELIUM_APPLICATION_API void StandardThread( Helium::CallbackThread::Entry entry, void* param );

    //
    // Main wrappers for console applications
    //  int main(int argc, const char** argv)
    //  {
    //    return StandardMain( &::Main, argc, argv );
    //  }
    //

    HELIUM_APPLICATION_API int StandardMain( int (*main)(int argc, const tchar_t** argv), int argc, const tchar_t** argv  );

    //
    // Main wrappers for windows applications
    //  int WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
    //  {
    //    return ProtectedWinMain( &WinMain, hInstance, hPrevInstance, lpCmdLine, int nShowCmd );
    //  }
    //  
    //  or, for windows applications running with the console subsystem set:
    //
    //  int main()
    //  {
    //    return StandardWinMain( &::WinMain );
    //  }
    //

#if HELIUM_OS_WIN
	typedef int (*WinMainFunc)( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd );

    HELIUM_APPLICATION_API int StandardWinMain( WinMainFunc winMain, HINSTANCE hInstance = ::GetModuleHandle(NULL), HINSTANCE hPrevInstance = NULL, LPWSTR lpCmdLine = ::GetCommandLineW(), int nShowCmd = SW_SHOWNORMAL );
#endif
}