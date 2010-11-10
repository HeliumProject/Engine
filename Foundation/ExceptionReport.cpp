#include "ExceptionReport.h"

#include "Platform/Process.h"
#include "Platform/Exception.h"
#include "Platform/Environment.h"
#include "Platform/Windows/MemoryProfiler.h"
#include "Platform/Windows/Console.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/Log.h"
#include "Foundation/Profile.h"
#include "Foundation/File/Path.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/String/Utilities.h"

#include <sstream>
#include <time.h>
#include <sstream>
#include <iomanip>

using namespace Helium;
using namespace Helium::Debug;

ExceptionReport::ExceptionReport( const ExceptionArgs& args )
: m_Args ( args )
, m_MemTotalReserve( 0 )
, m_MemTotalCommit( 0 )
, m_MemTotalFree( 0 )
, m_MemLargestFree( 0 )
{
    m_UserName.clear();
    Helium::GetUsername( m_UserName );

    m_Computer.clear();
    Helium::GetComputer( m_Computer );

    m_ApplicationPath = Helium::GetProcessPath();
    m_ApplicationName = Helium::GetProcessName();

    size_t firstDot = m_ApplicationName.find_last_of( '.' );
    m_ApplicationName = m_ApplicationName.substr( 0, firstDot );

    m_CmdLineArgs.clear();
    {
        int cmdArgc;
        const tchar_t** cmdArgv = Helium::GetCmdLine( cmdArgc );
        if ( cmdArgc > 1 )
        {
            for ( int i = 1; i < cmdArgc; ++i )
            {
                if ( !m_CmdLineArgs.empty() )
                {
                    m_CmdLineArgs += TXT( " " );
                }
                m_CmdLineArgs += cmdArgv[i];
            }
        }
    }

#ifdef DEBUG
# ifdef UNICODE
    m_BuildConfig = TXT( "Debug Unicode" );
# else
    m_BuildConfig = TXT( "Debug" );
# endif
#else
# ifdef UNICODE
    m_BuildConfig = TXT( "Release Unicode" );
# else
    m_BuildConfig = TXT( "Release" );
# endif
#endif

    // Memory
    Profile::MemoryStatus memory;
    Profile::GetMemoryStatus( &memory );
    m_MemTotalReserve = memory.m_TotalReserve;
    m_MemTotalCommit = memory.m_TotalCommit;
    m_MemTotalFree = memory.m_TotalFree;
    m_MemLargestFree = memory.m_LargestFree;

    m_Environment.clear();
    // Get a pointer to the environment block. 
    const tchar_t* env = (const tchar_t*)::GetEnvironmentStrings();
    if ( env )
    {
        // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
        for (const tchar_t* var = (const tchar_t*)env; *var; var++) 
        {
            // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
            for (const tchar_t* var = (const tchar_t*)env; *var; var++) 
            {
                if (*var != '=')
                {
                    m_Environment += var;
                    m_Environment += TXT( "\n" );
                }

                while (*var)
                {
                    var++;
                }
            }

            ::FreeEnvironmentStrings((tchar_t*)env);
        }
    }
}

static void CopyDump( ExceptionReport& report )
{
    if ( !report.m_Args.m_Fatal )
    {
        return;
    }

    if ( report.m_Args.m_Dump.empty() || !Helium::Path( report.m_Args.m_Dump ).Exists() )
    {
        return;
    }

    Log::Debug( TXT( "Writing dump to network...\n" ) );

    const char* store = getenv( "HELIUM_TOOLS_CRASH_DUMP_STORE" );
    if ( store == NULL )
    {
        return;
    }

    time_t t;
    time( &t );

    tm* now = localtime( &t );

    tostringstream destination;
    destination << store 
        << TXT( "\\" ) << report.m_UserName
        << TXT( "\\" ) << report.m_Computer
        << TXT( "\\" ) << report.m_ApplicationName
        << TXT( "\\" ) << now->tm_year + 1900
        << TXT( "-" ) << std::setfill( TXT( '0' ) ) << std::setw(2) << now->tm_mon + 1
        << TXT( "-" ) << std::setfill( TXT( '0' ) ) << std::setw(2) << now->tm_mday
        << TXT( "_" ) << std::setfill( TXT( '0' ) ) << std::setw(2) << now->tm_hour
        << TXT( "." ) << std::setfill( TXT( '0' ) ) << std::setw(2) << now->tm_min
        << TXT( "." ) << std::setfill( TXT( '0' ) ) << std::setw(2) << now->tm_sec
        << TXT( "." ) << std::setfill( TXT( '0' ) ) << std::setw(3) << t % 1000 << TXT( ".dmp" );

    Helium::Path dest( destination.str() );
    dest.MakePath();

    if ( FALSE == ::CopyFile( report.m_Args.m_Dump.c_str(), destination.str().c_str(), FALSE ) )
    {
        Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "Failed to copy '%s' to '%s': %s\n" ), report.m_Args.m_Dump.c_str(), destination.str().c_str(), Helium::GetErrorString().c_str() );
    }
    else
    {
        report.m_Args.m_Dump = destination.str();
    }
}

static void SendMail( ExceptionReport& report )
{
    Log::Debug( TXT( "Sending email report...\n" ) );

    tstring subject;
    if ( report.m_Args.m_Fatal )
    {
        subject += TXT( "Fatal " );
    }
    subject += Debug::ExceptionTypes::Strings[ report.m_Args.m_Type ];
    subject += TXT( " Exception: " ) + report.m_ApplicationName + TXT( " " ) + report.m_UserName + TXT( "@" ) + report.m_Computer;

    tstringstream body;
    body << "Username: " << report.m_UserName << std::endl;
    body << "Computer: " << report.m_Computer << std::endl;
    body << "Build Config: " << report.m_BuildConfig << std::endl;
    body << "Command Line: " << Helium::GetCmdLine() << std::endl;

    body << std::endl;
    body << "Memory:" << std::endl;
    body << "Total Reserved: " << (report.m_MemTotalReserve>>10) << "K bytes" << std::endl;
    body << "Total Commit: " << (report.m_MemTotalCommit>>10) << "K bytes" << std::endl;
    body << "Total Free: " << (report.m_MemTotalFree>>10) << ( "K bytes" ) << std::endl;
    body << "Largest Free: " << (report.m_MemLargestFree>>10) << ( "K bytes" ) << std::endl;

    if ( !report.m_Args.m_State.empty() )
    {
        body << std::endl;
        body << TXT( "Outline State:   " ) << std::endl;
        body << report.m_Args.m_State << std::endl;
    }

    if ( !report.m_Args.m_Dump.empty() )
    {
        body << std::endl;
        body << "Dump File:" << std::endl;
        body << report.m_Args.m_Dump << std::endl;
    }

    body << std::endl;
    body << "Type: " << Debug::ExceptionTypes::Strings[ report.m_Args.m_Type ] << std::endl;
    switch ( report.m_Args.m_Type )
    {
    case Debug::ExceptionTypes::SEH:
        {
            body << TXT( "Code: 0x" ) << std::hex << std::setfill( TXT( '0' ) ) << std::setw(8) << report.m_Args.m_SEHCode << std::endl;
            body << TXT( "Class: " ) << report.m_Args.m_SEHClass << std::endl;

            if ( !report.m_Args.m_Message.empty() )
            {
                body << std::endl;
                body << TXT( "Message: " ) << std::endl;
                body << report.m_Args.m_Message << std::endl;
            }

            body << std::endl << report.m_Args.m_SEHControlRegisters;
            body << std::endl << report.m_Args.m_SEHIntegerRegisters;

            break;
        }

    case Debug::ExceptionTypes::CPP:
        {
            body << "Class: " << report.m_Args.m_CPPClass << std::endl;

            if ( !report.m_Args.m_Message.empty() )
            {
                body << std::endl;
                body << "Message: " << std::endl;
                body << report.m_Args.m_Message << std::endl;
            }

            break;
        }
    }

    if ( !report.m_Args.m_Callstack.empty() )
    {
        body << std::endl;
        body << "Callstack:" << std::endl;
        body << report.m_Args.m_Callstack << std::endl;
    }

    std::vector< tstring >::const_iterator itr = report.m_Args.m_Threads.begin();
    std::vector< tstring >::const_iterator end = report.m_Args.m_Threads.end();
    for ( ; itr != end; ++itr )
    {
        body << std::endl << *itr << std::endl;
    }

    if ( !report.m_Environment.empty() )
    {
        body << std::endl;
        body << "Environment:" << std::endl;
        body << report.m_Environment << std::endl;
    }

#pragma TODO("Send email?")
}

static void HandleException( const Helium::Debug::ExceptionArgs& args )
{
    ExceptionReport report ( args );

    try
    {
        CopyDump( report );

        SendMail( report );
    }
    catch ( Helium::Exception& ex )
    {
        Helium::Print(Helium::ConsoleColors::Red, stderr, TXT( "%s\n" ), ex.What() );
    }
}

static int32_t g_InitCount = 0;

void Debug::InitializeExceptionListener()
{
    // init counting this API seems kind of silly, but we can actually get initialized from several places
    if ( ++g_InitCount == 1 )
    {
        tchar_t module[MAX_PATH];
        tchar_t drive[MAX_PATH];
        tchar_t path[MAX_PATH];
        GetModuleFileName( 0, module, MAX_PATH );
        _tsplitpath( module, drive, path, NULL, NULL );

        // Symbol path always starts with module directory
        tstring symbolPath( tstring( drive ) + tstring( path ) );

        // initialize debug symbols
        Debug::Initialize( symbolPath );

        // from here on out, submit crash reports
        Debug::EnableExceptionFilter(true);

        // wait for an exception
        Debug::g_ExceptionOccurred.Set( &HandleException );
    }
}

void Debug::CleanupExceptionListener()
{
    if ( --g_InitCount == 0 )
    {
        // stop waiting for exception
        Debug::g_ExceptionOccurred.Clear();

        // uninstall the exception filter function
        Debug::EnableExceptionFilter(false);
    }
}
