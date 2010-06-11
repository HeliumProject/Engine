#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Console.h"

#include "ExceptionDB.h"
#include "ExceptionReport.h"

#include "Debug/Exception.h"
#include "Foundation/Log.h"
#include "Foundation/Boost/Regex.h"
#include "Foundation/Environment.h"
#include "Platform/Mutex.h"
#include "SQL/MySQL.h"

#include <mysql.h>
#include <errmsg.h>
#include <sstream>

using namespace AppUtils;

//
// MySQL 
//

// main db handle
SQL::MySQL* g_DBHandle = NULL;

// Prevent multiple threads from accessing the DB at the same time.
Platform::Mutex g_ExceptionDBMutex;

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

// The return code for mysql success
#define MYSQL_OK 0
//const u64 s_InvalidRowID = 0;

#define REPORTS_DB_NAME "ExceptionReports"
#define TOOLS_DB_NAME   "Tools"
#define USERS_DB_NAME   "Users"

const u32   s_Port    = 3306;
const char* s_User    = "tools";
const char* s_Pass    = "t0o1z7e4m!";
const char* s_DBName  = REPORTS_DB_NAME;


// Users DB
const char* s_SelectUsersComputerIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`computers` WHERE `name`='%s';";
const char* s_InsertUsersComputerSQL = "INSERT INTO `"USERS_DB_NAME"`.`computers` (`name`) VALUES ('%s');";

const char* s_SelectUsersLoginIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`login_names` WHERE `login_name`='%s';";
const char* s_InsertUsersLoginSQL = "INSERT INTO `"USERS_DB_NAME"`.`login_names` (`login_name`) VALUES ('%s');";

const char* s_SelectUsersP4ClientIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`p4_clients` WHERE `name`='%s';";
const char* s_InsertUsersP4ClientSQL = "INSERT INTO `"USERS_DB_NAME"`.`p4_clients` (`name`) VALUES ('%s');";

const char* s_SelectUsersP4UserIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`p4_users` WHERE `username`='%s';";
const char* s_InsertUsersP4UserSQL = "INSERT INTO `"USERS_DB_NAME"`.`p4_users` (`username`) VALUES ('%s');";

// Tools DB
const char* s_SelectToolsAppIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`applications` WHERE `name`='%s';";
const char* s_InsertToolsAppSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`applications` (`name`) VALUES ('%s');";

const char* s_SelectToolsBranchIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`branches` WHERE `name`='%s';";
const char* s_InsertToolsBranchSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`branches` (`name`) VALUES ('%s');";

const char* s_SelectToolsBuildConfigIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`build_configs` WHERE `name`='%s';";
const char* s_InsertToolsBuildConfigSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`build_configs` (`name`) VALUES ('%s');";

const char* s_SelectToolsProjectIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`projects` WHERE `name`='%s';";
const char* s_InsertToolsProjectSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`projects` (`name`) VALUES ('%s');";

const char* s_SelectToolsReleaseIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`releases` WHERE `name`='%s';";
const char* s_InsertToolsReleaseSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`releases` (`name`) VALUES ('%s');";

const char* s_SelectToolsVersionIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`versions` WHERE `version`='%s';";
const char* s_InsertToolsVersionSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`versions` (`version`) VALUES ('%s');";

// Reports DB
const char* s_InsertReportsCommandSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`commands` (`application_id`,`application_path`,`arguments`,`inherited_args`) VALUES (%I64u,'%s','%s','%s');";
const char* s_InsertReportsEnvSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`environments` (`vars`) VALUES ('%s');";
const char* s_InsertReportsCallstackRawSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`callstacks` (`callstack`) VALUES ('%s');";

const char* s_InsertReportsOutlineStateSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`outline_states` (`state`) VALUES ('%s');";

const char* s_InsertReportsCallstackLineSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`callstack_lines` (`module`, `function`, `offset`, `file`, `line_number`) VALUES ('%s','%s','%s','%s',%I64u);";
const char* s_SelectReportsCallstackLineSQL = "SELECT `id` FROM `"REPORTS_DB_NAME"`.`callstack_lines` WHERE `module`='%s' AND `function`='%s' AND `offset`='%s' AND `file`='%s' AND `line_number`=%I64u;";

const char* s_InsertReportsCallstackXLinesSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`callstack_x_lines` (`report_id`, `callstack_line_id`, `line_number`) VALUES (%I64u, %I64u, %I64u)";

const char* s_InsertReportsDumpSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`dumps` SET `path`='%s';";
const char* s_SelectReportsDumpSQL = "SELECT `id` from `"REPORTS_DB_NAME"`.`dumps` WHERE `path`='%s'";

const char* s_InsertReportsMemorySQL = "INSERT INTO `"REPORTS_DB_NAME"`.`memory` (`total_reserved`,`total_commit`,`total_free`,`largest_free`) VALUES (%I64u,%I64u,%I64u,%I64u);";
const char* s_InsertReportsCPPSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`cpp_exceptions` (`report_id`,`exception_message`,`exception`) VALUES (%I64u,'%s','%s');";
const char* s_InsertReportsSEHSQL = "INSERT INTO `"REPORTS_DB_NAME"`.`seh_exceptions` (`report_id`,`exception_message`,`control_registers`,`integer_registers`) VALUES (%I64u,'%s','%s','%s');";

const char* s_InsertReportSQL =
"INSERT INTO `"REPORTS_DB_NAME"`.`reports` \
SET `login_name_id`=%I64u, \
    `computer_id`=%I64u, \
    `project_id`=%I64u, \
    `tools_version_id`=%I64u, \
    `tools_release_id`=%I64u, \
    `build_config_id`=%I64u, \
    `is_tools_builder`=%d, \
    `is_symbol_builder`=%d, \
    `code_branch_id`=%I64u, \
    `assets_branch_id`=%I64u, \
    `environment_id`=%I64u, \
    `memory_id`=%I64u, \
    `command_id`=%I64u, \
    `callstack_id`=%I64u, \
    `outline_state_id`=%I64u, \
    `is_fatal`=%d, \
    `dump_id`=%I64u;";

/////////////////////////////////////////////////////////////////////////////
bool AppUtils::InitializeExceptionDB()
{
  g_DBHandle = new SQL::MySQL( "AppUtils-ExceptionDB" );
  return true;
}

/////////////////////////////////////////////////////////////////////////////
void AppUtils::CleanupExceptionDB()
{
  delete g_DBHandle;
  g_DBHandle = NULL;
}

/////////////////////////////////////////////////////////////////////////////
static inline u64 InsertIfNotFound( const char* select, const char* insert )
{
  NOC_ASSERT( select );

  if ( !g_DBHandle->IsConnected() )
  {
    throw Nocturnal::Exception( "Lost DB connection" );
  }

  bool haveID = false;
  u64 id = 0;
  int execResult = g_DBHandle->ExecSQL( select );
  if ( execResult == MYSQL_OK )
  {
    MYSQL_RES* sqlResult = mysql_store_result( g_DBHandle->GetDBHandle() );
    if ( sqlResult )
    { 
      int numFields = mysql_num_fields( sqlResult );
      if ( numFields > 0 )
      {
        MYSQL_ROW sqlRow = mysql_fetch_row( sqlResult );
        if ( sqlRow && sqlRow[0] != NULL )
        {
          id = (u64) _atoi64( sqlRow[0] );
          haveID = true;
        }
      }

      // free the result
      mysql_free_result( sqlResult );
    }
    else
    {
      throw Nocturnal::Exception( "No result for: %s", select );
    }
  }
  else
  {
    throw Nocturnal::Exception( "Failed to execute SQL: %s", select );
  }

  if ( !haveID && insert )
  {
    execResult = g_DBHandle->ExecSQLSNPrintF( insert, id );
    if ( execResult == MYSQL_OK )
    {
      id = g_DBHandle->GetLastInsertRowId();
    }
    else
    {
      throw Nocturnal::Exception( "Failed to execute SQL: %s", insert );
    }
  }
  return id;
}

/////////////////////////////////////////////////////////////////////////////
static inline u64 SelectID( const char* select, const char* insert, const char* value )
{
  char valueBuff[MAX_QUERY_LENGTH];
  mysql_real_escape_string( g_DBHandle->GetDBHandle(), valueBuff, value, (unsigned long) strlen( value ) );

  char selectBuff[MAX_QUERY_LENGTH];
  sprintf_s( selectBuff, sizeof( selectBuff ), select, valueBuff );

  char* insertDumbPtr = NULL;
  if ( insert )
  {
    char insertBuff[MAX_INSERT_LENGTH];
    sprintf_s( insertBuff, sizeof( insertBuff ), insert, value );
    insertDumbPtr = &insertBuff[0];
  }

  return InsertIfNotFound( &selectBuff[0], insertDumbPtr );
}

/////////////////////////////////////////////////////////////////////////////
void AppUtils::UpdateExceptionDB( const ExceptionReport& report )
{
  Platform::TakeMutex mutex( g_ExceptionDBMutex );

  std::string host;
  if ( !Nocturnal::GetEnvVar( "NOC_DB_HOST", host ) )
  {
    std::string error = std::string( "No exception db set in environment.  Please set the " ) + "NOC_DB_HOST" + " environment variable.\n";
    Platform::Print( Platform::ConsoleColors::Red, stderr, error.c_str() );
    return;
  }

  try
  {
    g_DBHandle->Connect( host.c_str(), s_User, s_Pass, s_DBName, s_Port );
  }
  catch( const Nocturnal::Exception& ex )
  {
    Platform::Print( Platform::ConsoleColors::Red, stderr, "Failed to connect to tools MySQL database @ '%s', reason: %s\n", host.c_str(), ex.what() );
  }

  if ( !g_DBHandle->IsConnected() )
  {
    Platform::Print( Platform::ConsoleColors::Red, stderr, "Lost DB connection\n" );
    return;
  }

  Log::Debug("Updating report database...\n");

  g_DBHandle->BeginTrans();

  try
  {
    // Users DB
    u64 computerID    = SelectID( s_SelectUsersComputerIDSQL, s_InsertUsersComputerSQL, report.m_Computer.c_str() );
    u64 loginID       = SelectID( s_SelectUsersLoginIDSQL, s_InsertUsersLoginSQL, report.m_UserName.c_str() );
    //u64 p4ClientID    = SelectID( s_SelectUsersP4ClientIDSQL, s_InsertUsersP4ClientSQL, report..c_str() );
    //u64 p4UserID      = SelectID( s_SelectUsersP4UserIDSQL, s_InsertUsersP4UserSQL, report..c_str() );

    // Tools DB
    u64 appID         = SelectID( s_SelectToolsAppIDSQL, s_InsertToolsAppSQL, report.m_ApplicationName.c_str() );
    u64 codeBranchID  = SelectID( s_SelectToolsBranchIDSQL, s_InsertToolsBranchSQL, report.m_CodeBranch.c_str() );
    u64 assetBranchID = SelectID( s_SelectToolsBranchIDSQL, s_InsertToolsBranchSQL, report.m_AssetBranch.c_str() );
    u64 buildConfigID = SelectID( s_SelectToolsBuildConfigIDSQL, s_InsertToolsBuildConfigSQL, report.m_ToolsBuildConfig.c_str() );
    u64 projectID     = SelectID( s_SelectToolsProjectIDSQL, s_InsertToolsProjectSQL, report.m_ProjectName.c_str() );
    u64 releaseID     = SelectID( s_SelectToolsReleaseIDSQL, s_InsertToolsReleaseSQL, report.m_ToolsReleaseName.c_str() );
    u64 toolsVerID    = SelectID( s_SelectToolsVersionIDSQL, s_InsertToolsVersionSQL, report.m_ToolsVersion.c_str() );

    // Reports DB
    int execResult = MYSQL_OK;
    static char buff[MAX_QUERY_LENGTH];
    memset( buff, '\0', MAX_QUERY_LENGTH );

    u64 commandID;
    {
      memset( buff, '\0', MAX_QUERY_LENGTH );
      mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_ApplicationPath.c_str(), (unsigned long) report.m_ApplicationPath.length() );
      std::string applicationPath = buff;

      memset( buff, '\0', MAX_QUERY_LENGTH );
      mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_CmdLineArgs.c_str(), (unsigned long) report.m_CmdLineArgs.length() );
      std::string cmdLineArgs = buff;

      memset( buff, '\0', MAX_QUERY_LENGTH );
      mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_InheritedArgs.c_str(), (unsigned long) report.m_InheritedArgs.length() );
      std::string inheritedArgs = buff;

      execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsCommandSQL, appID, applicationPath.c_str(), cmdLineArgs.c_str(), inheritedArgs.c_str() );  
      if ( execResult == MYSQL_OK )
      {
        commandID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        throw Nocturnal::Exception( "Failed to insert commandline" );
      }
    }

    u64 environmentID;
    {
      static char queryStr[1024 * 32];

      if ( sizeof( queryStr ) > report.m_Environment.length() * 2 )
      {
        static char envBuff[ sizeof( queryStr ) ];
        memset( envBuff, '\0', sizeof( queryStr ) );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), envBuff, report.m_Environment.c_str(), (unsigned long) report.m_Environment.length() );
        sprintf_s( queryStr, sizeof(queryStr), s_InsertReportsEnvSQL, envBuff );
      }
      else
      {
        sprintf_s( queryStr, sizeof(queryStr), s_InsertReportsEnvSQL, "Environment too large" );
      }

      execResult = mysql_query( g_DBHandle->GetDBHandle(), queryStr );
      if ( execResult == MYSQL_OK )
      {
        environmentID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        throw Nocturnal::Exception( "Failed to insert environment" );
      }
    }

    u64 memoryID;
    {
      execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsMemorySQL,
        report.m_MemTotalReserve,
        report.m_MemTotalCommit,
        report.m_MemTotalFree,
        report.m_MemLargestFree );  
      if ( execResult == MYSQL_OK )
      {
        memoryID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        throw Nocturnal::Exception( "Failed to insert memory row" );
      }
    }

    u64 callstackID;
    {
      static char queryStr[1024 * 32];

      if ( sizeof( queryStr ) > report.m_Args.m_Callstack.length() * 2 )
      {
        static char envBuff[ sizeof( queryStr ) ];
        memset( envBuff, '\0', sizeof( queryStr ) );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), envBuff, report.m_Args.m_Callstack.c_str(), (unsigned long) report.m_Args.m_Callstack.length() );
        sprintf_s( queryStr, sizeof(queryStr), s_InsertReportsCallstackRawSQL, envBuff );
      }
      else
      {
        sprintf_s( queryStr, sizeof(queryStr), s_InsertReportsCallstackRawSQL, "Callstack too large" );
      }

      execResult = mysql_query( g_DBHandle->GetDBHandle(), queryStr );
      if ( execResult == MYSQL_OK )
      {
        callstackID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        throw Nocturnal::Exception( "Failed to insert callstack" );
      }
    }

    u64 stateID = 1; // First entry in table is pre-populated as a NULL entry
    if ( !report.m_Args.m_State.empty() )
    {
      memset( buff, '\0', MAX_QUERY_LENGTH );
      mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_State.c_str(), (unsigned long) report.m_Args.m_State.length() );

      execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsOutlineStateSQL, buff );  
      if ( execResult == MYSQL_OK )
      {
        stateID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        Log::Error( "Failed to insert outline state.\n" );
      }
    }

    u64 dumpID;
    {
      memset( buff, '\0', MAX_QUERY_LENGTH );
      mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_Dump.c_str(), (unsigned long) report.m_Args.m_Dump.length() );

      static char selectBuff[MAX_QUERY_LENGTH] = { '\0' };
      sprintf_s( selectBuff, sizeof( selectBuff ), s_SelectReportsDumpSQL, buff );

      static char insertBuff[MAX_INSERT_LENGTH] = { '\0' };
      sprintf_s( insertBuff, sizeof( insertBuff ), s_InsertReportsDumpSQL, buff );

      dumpID = InsertIfNotFound( selectBuff, insertBuff );
    }

    //TODO: What do I do with: std::string m_State;

    u64 reportID;
    {
      execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportSQL,
        loginID,
        computerID,
        projectID,
        toolsVerID,
        releaseID,
        buildConfigID,
        ( report.m_IsToolsBuilder ? 1 : 0 ),
        ( report.m_IsSymbolBuilder ? 1 : 0 ),
        codeBranchID,
        assetBranchID,
        environmentID,
        memoryID,
        commandID,
        callstackID,
        stateID,
        ( report.m_Args.m_Fatal ? 1 : 0 ),
        dumpID ); 
      if ( execResult == MYSQL_OK )
      {
        reportID = g_DBHandle->GetLastInsertRowId();
      }
      else
      {
        throw Nocturnal::Exception( "Failed to insert report" );
      }
    }

    // Parsed callstack
    {
      // address, module, function_parse, function, offset, file_parse, file, fileLineNumber
      const boost::regex callStackPattern( "^(0[xX]{1}[0-9a-fA-F]+) \\- ([^,]*?)(, ([^\\+]*?) \\+ (0[xX]{1}[0-9a-fA-F]+)){0,1}( \\: ([^\\(]*?)\\((\\d+?)\\)){0,1}$" );
      const boost::regex lineBreakPattern( "\\n|\\r" ); 
      boost::sregex_token_iterator lineItr( report.m_Args.m_Callstack.begin(), report.m_Args.m_Callstack.end(), lineBreakPattern, -1 ); 
      boost::sregex_token_iterator lineEnd; 
      u64 callstackLineNumber = 0;
      for ( ; lineItr != lineEnd; ++lineItr )
      {
        if ( ( *lineItr ).matched )
        {
          const std::string& temp = *lineItr;
          boost::smatch results;
          if ( boost::regex_search( temp, results, callStackPattern ) )
          {
            std::string module    = results[2].matched ? ResultAsString( results, 2 ) : "";
            std::string function  = results[4].matched ? ResultAsString( results, 4 ) : "";
            std::string offset    = results[5].matched ? ResultAsString( results, 5 ) : "";
            std::string file      = results[7].matched ? ResultAsString( results, 7 ) : "";
            u64 lineNumber        = results[8].matched ? ResultAsU64( results, 8 ) : 0;

            memset( buff, '\0', MAX_QUERY_LENGTH );
            mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, module.c_str(), (unsigned long) module.length() );
            module = buff;

            memset( buff, '\0', MAX_QUERY_LENGTH );
            mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, function.c_str(), (unsigned long) function.length() );
            function = buff;

            memset( buff, '\0', MAX_QUERY_LENGTH );
            mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, offset.c_str(), (unsigned long) offset.length() );
            offset = buff;

            memset( buff, '\0', MAX_QUERY_LENGTH );
            mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, file.c_str(), (unsigned long) file.length() );
            file = buff;

            char queryStr[MAX_QUERY_LENGTH];
            sprintf_s( queryStr, sizeof( queryStr ), s_SelectReportsCallstackLineSQL, module.c_str(), function.c_str(), offset.c_str(), file.c_str(), lineNumber );

            char insertStr[MAX_INSERT_LENGTH];
            sprintf_s( insertStr, sizeof( insertStr ), s_InsertReportsCallstackLineSQL, module.c_str(), function.c_str(), offset.c_str(), file.c_str(), lineNumber );

            u64 callstackLineID = InsertIfNotFound( queryStr, insertStr );
            execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsCallstackXLinesSQL, 
              reportID, 
              callstackLineID, 
              callstackLineNumber );

            if ( execResult != MYSQL_OK )
            {
              Log::Error( "Failed to insert into callstack_x_lines\n" );
            }
          }

          ++callstackLineNumber;
        }
      }
    }

    switch ( report.m_Args.m_Type )
    {
    case Debug::ExceptionTypes::SEH:
      {
        memset( buff, '\0', MAX_QUERY_LENGTH );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_Message.c_str(), (unsigned long) report.m_Args.m_Message.length() );
        std::string msg = buff;

        memset( buff, '\0', MAX_QUERY_LENGTH );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_SEHControlRegisters.c_str(), (unsigned long) report.m_Args.m_SEHControlRegisters.length() );
        std::string controlReg = buff;

        memset( buff, '\0', MAX_QUERY_LENGTH );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_SEHIntegerRegisters.c_str(), (unsigned long) report.m_Args.m_SEHIntegerRegisters.length() );
        std::string intReg = buff;

        execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsSEHSQL, reportID, msg.c_str(), controlReg.c_str(), intReg.c_str() );  
        if ( execResult != MYSQL_OK )
        {
          throw Nocturnal::Exception( "Failed to insert SEH exception" );
        }

        break;
      }

    case Debug::ExceptionTypes::CPP:
      {
        memset( buff, '\0', MAX_QUERY_LENGTH );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_Message.c_str(), (unsigned long) report.m_Args.m_Message.length() );
        std::string msg = buff;

        memset( buff, '\0', MAX_QUERY_LENGTH );
        mysql_real_escape_string( g_DBHandle->GetDBHandle(), buff, report.m_Args.m_CPPClass.c_str(), (unsigned long) report.m_Args.m_CPPClass.length() );
        std::string cppClass = buff;

        execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertReportsCPPSQL, reportID, msg.c_str(), cppClass.c_str() );  
        if ( execResult != MYSQL_OK )
        {
          throw Nocturnal::Exception( "Failed to insert CPP exception" );
        }

        break;
      }
    }
  }
  catch( const Nocturnal::Exception& ex )
  {
    const char* dbErrMsg = mysql_error( g_DBHandle->GetDBHandle() );
    Platform::Print( Platform::ConsoleColors::Red, stderr, "%s; %s\n", ex.what(), dbErrMsg );
    g_DBHandle->RollbackTrans();
    g_DBHandle->Close();
    return;
  }
  catch( ... )
  {
    g_DBHandle->RollbackTrans();
    g_DBHandle->Close();
    throw;
  }

  g_DBHandle->CommitTrans();
  g_DBHandle->Close();
}
