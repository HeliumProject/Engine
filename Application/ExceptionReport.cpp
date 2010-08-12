#include "ExceptionReport.h"

#include "Platform/Process.h"
#include "Platform/Environment.h"
#include "Platform/Windows/Memory.h"
#include "Platform/Windows/Windows.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"

#include <sstream>

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
        const tchar** cmdArgv = Helium::GetCmdLine( cmdArgc );
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
    const tchar* env = (const tchar*)::GetEnvironmentStrings();
    if ( env )
    {
        // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
        for (const tchar* var = (const tchar*)env; *var; var++) 
        {
            // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
            for (const tchar* var = (const tchar*)env; *var; var++) 
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

            ::FreeEnvironmentStrings((tchar*)env);
        }
    }
}