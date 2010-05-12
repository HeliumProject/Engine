#include "Windows/Windows.h"
#include "Windows/Console.h"
#include "Windows/Error.h"

#include "AppUtils.h"
#include "ExceptionListener.h"
#include "ExceptionDB.h"
#include "ExceptionReport.h"

#include "Common/Version.h"
#include "Common/CommandLine.h"
#include "Common/Environment.h"
#include "Profile/Profile.h"
#include "Console/Console.h"
#include "Debug/Utils.h"
#include "Debug/Exception.h"
#include "FileSystem/FileSystem.h"
#include "SQL/MySQL.h"

#include <time.h>
#include <sstream>
#include <iomanip>

using namespace AppUtils;

static i32 g_InitCount = 0;

static void RecurseDirectories( const std::string& directory, std::string& result )
{
  if ( !result.empty() )
  {
    result += ";";
  }

  std::string win32Name( directory );
  FileSystem::Win32Name( win32Name );
  result += win32Name;

  V_string subDirs;
  FileSystem::GetDirectories( directory, subDirs );
  V_string::const_iterator dirItr = subDirs.begin();
  V_string::const_iterator dirEnd = subDirs.end();
  for ( ; dirItr != dirEnd; ++dirItr )
  {
    RecurseDirectories( *dirItr, result );
  }
}

static void CopyDump( ExceptionReport& report )
{
  if ( !report.m_Args.m_Fatal )
  {
    return;
  }

  if ( report.m_Args.m_Dump.empty() || !FileSystem::Exists( report.m_Args.m_Dump ) )
  {
    return;
  }

  Console::Debug("Writing dump to network...\n");

  const char* store = getenv( NOCTURNAL_STUDIO_PREFIX "TOOLS_CRASH_DUMP_STORE" );
  if ( store == NULL )
  {
    return;
  }

  time_t t;
  time( &t );

  tm* now = localtime( &t );

  std::ostringstream destination;
  destination << store 
    << "\\" << report.m_ToolsVersion
    << "\\" << report.m_UserName
    << "\\" << report.m_Computer
    << "\\" << report.m_ApplicationName
    << "\\" << now->tm_year + 1900
    << "-" << std::setfill('0') << std::setw(2) << now->tm_mon + 1
    << "-" << std::setfill('0') << std::setw(2) << now->tm_mday
    << "_" << std::setfill('0') << std::setw(2) << now->tm_hour
    << "." << std::setfill('0') << std::setw(2) << now->tm_min
    << "." << std::setfill('0') << std::setw(2) << now->tm_sec
    << "." << std::setfill('0') << std::setw(3) << t % 1000 << ".dmp";

  std::string dest = destination.str();

  FileSystem::CleanName( dest );
  FileSystem::MakePath( dest, true );

  if ( FALSE == ::CopyFile( report.m_Args.m_Dump.c_str(), destination.str().c_str(), FALSE ) )
  {
    Windows::Print(Windows::ConsoleColors::Red, stderr, "Failed to copy '%s' to '%s': %s\n", report.m_Args.m_Dump.c_str(), destination.str().c_str(), Windows::GetErrorString().c_str() );
  }
  else
  {
    report.m_Args.m_Dump = destination.str();
  }
}

static void SendMail( ExceptionReport& report )
{
  if ( getenv( NOCTURNAL_STUDIO_PREFIX "CRASH_DONT_EMAIL" ) != NULL )
  {
    return;
  }

  Console::Debug("Sending email report...\n");

  std::string subject;
  if ( report.m_Args.m_Fatal )
  {
    subject += "Fatal ";
  }
  subject += Debug::ExceptionTypes::Strings[ report.m_Args.m_Type ];
  subject += " Exception: " + report.m_ApplicationName + "(v" + report.m_ToolsVersion + ") " + report.m_UserName + "@" + report.m_Computer;

  std::stringstream body;
  body << "Username: " << report.m_UserName << std::endl;
  body << "Computer: " << report.m_Computer << std::endl;
  body << "Tools Version: " << report.m_ToolsVersion << std::endl;
  body << "Tools Release: " << report.m_ToolsReleaseName << std::endl;
  body << "Tools Build Config: " << report.m_ToolsBuildConfig << std::endl;
  body << "Command Line: " << Nocturnal::GetCmdLine() << std::endl;

  body << std::endl;
  body << "Memory:" << std::endl;
  body << "Total Reserved: " << (report.m_MemTotalReserve>>10) << "K bytes" << std::endl;
  body << "Total Commit: " << (report.m_MemTotalCommit>>10) << "K bytes" << std::endl;
  body << "Total Free: " << (report.m_MemTotalFree>>10) << "K bytes" << std::endl;
  body << "Largest Free: " << (report.m_MemLargestFree>>10) << "K bytes" << std::endl;

  if ( !report.m_Args.m_State.empty() )
  {
    body << std::endl;
    body << "Outline State:   " << std::endl;
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
      body << "Code: 0x" << std::hex << std::setfill('0') << std::setw(8) << report.m_Args.m_SEHCode << std::endl;
      body << "Class: " << report.m_Args.m_SEHClass << std::endl;

      if ( !report.m_Args.m_Message.empty() )
      {
        body << std::endl;
        body << "Message: " << std::endl;
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

  V_string::const_iterator itr = report.m_Args.m_Threads.begin();
  V_string::const_iterator end = report.m_Args.m_Threads.end();
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
}

static void ProcessException( const Debug::ExceptionArgs& args )
{
  Debug::EnableTranslator<Debug::TranslateException> translate;

  AppUtils::ExceptionReport report ( args );

  try
  {
    CopyDump( report );

    SendMail( report );

    if ( !AppUtils::IsToolsBuilder() )
    {
      UpdateExceptionDB( report );
    }
  }
  catch ( Nocturnal::Exception& ex )
  {
    Windows::Print(Windows::ConsoleColors::Red, stderr, "%s\n", ex.what() );
  }
}

void AppUtils::InitializeExceptionListener()
{
  // init counting this API seems kind of silly, but we can actually get initialized from several places
  if ( ++g_InitCount == 1 )
  {
    char module[MAX_PATH];
    char drive[MAX_PATH];
    char path[MAX_PATH];
    GetModuleFileName( 0, module, MAX_PATH );
    _splitpath( module, drive, path, NULL, NULL );

    // Symbol path always starts with module directory
    std::string symbolPath( std::string( drive ) + std::string( path ) );

    if ( !AppUtils::IsToolsBuilder() )
    {
      // Search for network symbol storage
      const char* symbolStoreVar = getenv( NOCTURNAL_STUDIO_PREFIX"TOOLS_SYMBOLS_STORE" );
      std::string symbolStore = symbolStoreVar ? symbolStoreVar : "";
      if ( !symbolStore.empty() && FileSystem::Exists( symbolStore ) )
      {
        symbolStore += "\\"; 
        symbolStore += getenv( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME" );
        symbolStore += "\\"; 
        symbolStore += getenv( NOCTURNAL_STUDIO_PREFIX"CODE_BRANCH_NAME" ); 
        symbolStore += "\\";
        symbolStore += getenv( NOCTURNAL_STUDIO_PREFIX"TOOLS_RELEASE_NAME" );
        symbolStore += "\\";
        symbolStore += NOCTURNAL_VERSION_STRING;

        // Add subdirectories
        RecurseDirectories( symbolStore, symbolPath );
      }
    }

    // initialize debug symbols
    Debug::Initialize( symbolPath );

    // init db objects
    InitializeExceptionDB();

    // from here on out, submit crash reports
    Debug::EnableExceptionFilter(true);

    // wait for an exception
    Debug::g_ExceptionOccurred.Set( &ProcessException );
  }
}

void AppUtils::CleanupExceptionListener()
{
  if ( --g_InitCount == 0 )
  {
    // stop waiting for exception
    Debug::g_ExceptionOccurred.Clear();

    // uninstall the exception filter function
    Debug::EnableExceptionFilter(false);

    // unload db objects
    CleanupExceptionDB();
  }
}
