#include "Platform/Windows/Windows.h"

#include "ExceptionReport.h"

#include "Foundation/Version.h"
#include "Foundation/CommandLine/Utilities.h"
#include "Foundation/File/Path.h"
#include "Foundation/String/Utilities.h"
#include "Platform/Windows/Memory.h"
#include "Platform/Process.h"

#include <sstream>

using namespace Application;

ExceptionReport::ExceptionReport( const Debug::ExceptionArgs& args )
: m_Args ( args )
, m_MemTotalReserve( 0 )
, m_MemTotalCommit( 0 )
, m_MemTotalFree( 0 )
, m_MemLargestFree( 0 )
{
  tchar buf[MAX_PATH];

  m_UserName.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( TXT( "USERNAME" ), buf, MAX_PATH );
  m_UserName = buf;

  m_Computer.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( TXT( "COMPUTERNAME" ), buf, MAX_PATH );
  m_Computer = buf;

  m_ApplicationPath.clear();
  m_ApplicationName.clear();
  GetModuleFileName( NULL, buf, MAX_PATH );
  m_ApplicationPath = buf;
  std::string processPath = buf;
  size_t idx = processPath.find_last_of( "\\" );
  if ( idx != processPath.npos )
  {
    m_ApplicationName = processPath.substr( idx+1 );
  }
  else
  {
    m_ApplicationName = processPath;
  }

  size_t firstDot = m_ApplicationName.find_last_of( '.' );
  m_ApplicationName = m_ApplicationName.substr( 0, firstDot );

  m_CmdLineArgs.clear();
  {
    int cmdArgc;
    const char** cmdArgv = Nocturnal::GetCmdLine( cmdArgc );
    if ( cmdArgc > 1 )
    {
      for ( int i = 1; i < cmdArgc; ++i )
      {
        if ( !m_CmdLineArgs.empty() )
        {
          m_CmdLineArgs += " ";
        }
        m_CmdLineArgs += cmdArgv[i];
      }
    }
  }

  m_InheritedArgs.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_CMD_ARGS", buf, MAX_PATH );
  m_InheritedArgs = buf;
  if ( m_InheritedArgs.empty() )
  {
    m_InheritedArgs = "";
  }

  m_AssetBranch.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_ASSETS_BRANCH_NAME", buf, MAX_PATH );
  m_AssetBranch = buf;
  Nocturnal::Path::Normalize( m_AssetBranch );

  m_CodeBranch.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_CODE_BRANCH_NAME", buf, MAX_PATH );
  m_CodeBranch = buf;
  Nocturnal::Path::Normalize( m_CodeBranch );

  m_ProjectName.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_PROJECT_NAME", buf, MAX_PATH );
  m_ProjectName = buf;

  m_IsToolsBuilder = false;
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_TOOLS_BUILDER", buf, MAX_PATH );
  if ( stricmp( buf, "1" ) == 0 )
  {
    m_IsToolsBuilder = true;
  }
  else
  {
    m_IsToolsBuilder = false;
  }

  m_IsSymbolBuilder = false;
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_SYMBOL_MODE", buf, MAX_PATH );
  if ( stricmp( buf, "BUILD" ) == 0 )
  {
    m_IsSymbolBuilder = true;
  }
  else
  {
    m_IsSymbolBuilder = false;
  }

  m_ToolsVersion = NOCTURNAL_VERSION_STRING;

#ifdef DEBUG
  m_ToolsBuildConfig = "Debug";
#else
  m_ToolsBuildConfig = "Release";
#endif

  m_ToolsReleaseName.clear();
  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( "NOC_TOOLS_RELEASE_NAME", buf, MAX_PATH );
  m_ToolsReleaseName = buf;

  // Memory
  Profile::MemoryStatus memory;
  Profile::GetMemoryStatus( &memory );
  m_MemTotalReserve = memory.m_TotalReserve;
  m_MemTotalCommit = memory.m_TotalCommit;
  m_MemTotalFree = memory.m_TotalFree;
  m_MemLargestFree = memory.m_LargestFree;

  m_Environment.clear();
  // Get a pointer to the environment block. 
  const char* env = (const char*)::GetEnvironmentStrings();
  if ( env )
  {
    // Variable strings are separated by NULL byte, and the block is terminated by a NULL byte. 
    for (const char* var = (const char*)env; *var; var++) 
    {
      if (*var != '=')
      {
        m_Environment += var;
        m_Environment += "\n";
      }

      while (*var)
      {
        var++;
      }
    }

    ::FreeEnvironmentStrings((char*)env);
  }
}