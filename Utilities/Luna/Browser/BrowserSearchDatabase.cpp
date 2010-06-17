#include "Precompile.h"

#include "BrowserSearchDatabase.h"
#include "Platform/Windows/Windows.h"
#include "Platform/Windows/Console.h"

#include "Foundation/Exception.h"
#include "Foundation/Log.h"

#include "BrowserSearchEnvironment.h"

#include "Foundation/Environment.h"

#include "Platform/Mutex.h"
#include "Application/SQL/MySQL/MySQL.h"

#include <mysql.h>
#include <errmsg.h>
#include <sstream>

namespace Luna
{

  namespace BrowserSearchDatabase
  {

    //
    // MySQL 
    //

    // main db handle
    SQL::MySQL* g_DBHandle = NULL;

    // Prevent multiple threads from accessing the DB at the same time.
    Platform::Mutex g_AssetSearchDBMutex;

    // max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

    // The return code for mysql success
#define MYSQL_OK 0


#define ASSET_BROWSER_DB_NAME "AssetBrowser"
#define TOOLS_DB_NAME   "Tools"
#define USERS_DB_NAME   "Users"

    const u32   s_Port    = 3306;
    const char* s_User    = "tools";
    const char* s_Pass    = "t0o1z7e4m!";
    const char* s_DBName  = ASSET_BROWSER_DB_NAME;

    int g_SearchDBInitCount = 0;


    // Users DB
    const char* s_SelectUsersComputerIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`computers` WHERE `name`='%s';";
    const char* s_InsertUsersComputerSQL = "INSERT INTO `"USERS_DB_NAME"`.`computers` (`name`) VALUES ('%s');";

    const char* s_SelectUsersLoginIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`login_names` WHERE `login_name`='%s';";
    const char* s_InsertUsersLoginSQL = "INSERT INTO `"USERS_DB_NAME"`.`login_names` (`login_name`) VALUES ('%s');";

    // AssetBrowser DB
    const char * s_SelectSearchQueryIDSQL = "SELECT `id` FROM `"ASSET_BROWSER_DB_NAME"`.`search_queries` WHERE `query_string`='%s';";
    const char * s_InsertSeqrchQuerySQL = "INSERT INTO `"ASSET_BROWSER_DB_NAME"`.`search_queries` (`query_string`) VALUES ('%s');";


    // Tools DB
    const char* s_SelectToolsAppIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`applications` WHERE `name`='%s';";
    const char* s_InsertToolsAppSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`applications` (`name`) VALUES ('%s');";

    const char* s_SelectToolsBranchIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`branches` WHERE `name`='%s';";
    const char* s_InsertToolsBranchSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`branches` (`name`) VALUES ('%s');";

    const char* s_SelectToolsBuildConfigIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`build_configs` WHERE `name`='%s';";
    const char* s_InsertToolsBuildConfigSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`build_configs` (`name`) VALUES ('%s');";

    const char* s_SelectToolsProjectIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`projects` WHERE `name`='%s';";
    const char* s_InsertToolsProjectSQL = "INSERT INTO `"TOOLS_DB_NAME"`.`projects` (`name`) VALUES ('%s');";

    const char * s_InsertSearchQuerySQL =
      "INSERT INTO `"ASSET_BROWSER_DB_NAME"`.`search_events` \
                                          SET `user_id`=%I64u, \
                                          `computer_id`=%I64u, \
                                          `search_id`=%I64u, \
                                          `project_id`=%I64u, \
                                          `branch_id`=%I64u, \
                                          `build_id`=%I64u,  \
                                          `application_id`=%I64u;" ;

    ////////////////////////////////////////////////////////////////////////////////////////
    //// Prototypes ////////////////////////////////////////////////////////////////////////

    static inline u64 InsertIfNotFound( const char* select, const char* insert );
    static inline u64 SelectID( const char* select, const char* insert, const char* value );
    static inline void ResetConnectionData( );

    /////////////////////////////////////////////////////////////////////////////////////////

    class BrowserSearchStaticData
    {
    public:
      u64 loginID;        //!< Users login ID
      u64 computerID;     //!< Users computer ID

      // Tools DB
      u64 projectID;      //!< Project ID : RCF2, resistance, ...
      u64 codeBranchID;   //!< Code brance ID : devel, stable, ..
      u64 buildConfigID;  //!< Build configuration : release, develop, ...
      u64 appID;          //!< Application ID : Luna, buildtool, ...
    };

    BrowserSearchStaticData * g_SearchData = NULL;


    /////////////////////////////////////////////////////////////////////////////
    void Luna::BrowserSearchDatabase::Initialize()
    {
      if( ++g_SearchDBInitCount == 1 )
      {
        g_DBHandle = new SQL::MySQL( "Luna-BrowserSearchDB" );

        ResetConnectionData( );
      }
    }

    /////////////////////////////////////////////////////////////////////////////
    void Luna::BrowserSearchDatabase::Cleanup()
    {
      if( --g_SearchDBInitCount == 0 )
      {
        g_DBHandle->Close();

        delete g_DBHandle;
        g_DBHandle = NULL;

        delete g_SearchData;
        g_SearchData = NULL;
      }
    }

    static inline void ResetConnectionData( )
    {
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

      Log::Debug("Updating AssetBrowser search_events database...\n");

      if( !g_SearchData )
      {
        g_SearchData = new BrowserSearchStaticData();

        g_DBHandle->BeginTrans();

        try
        {
          Luna::BrowserSearchDatabase::EnvironmentData envData;

          // Users DB
          g_SearchData->loginID       = Luna::BrowserSearchDatabase::SelectID( s_SelectUsersLoginIDSQL, s_InsertUsersLoginSQL, envData.m_UserName.c_str() );
          g_SearchData->computerID    = SelectID( s_SelectUsersComputerIDSQL, s_InsertUsersComputerSQL, envData.m_Computer.c_str() );

          // Tools DB
          g_SearchData->projectID     = SelectID( s_SelectToolsProjectIDSQL, s_InsertToolsProjectSQL, envData.m_ProjectName.c_str() );
          g_SearchData->codeBranchID  = SelectID( s_SelectToolsBranchIDSQL, s_InsertToolsBranchSQL, envData.m_CodeBranch.c_str() );
          g_SearchData->buildConfigID = SelectID( s_SelectToolsBuildConfigIDSQL, s_InsertToolsBuildConfigSQL, envData.m_ToolsBuildConfig.c_str() );
          g_SearchData->appID         = SelectID( s_SelectToolsAppIDSQL, s_InsertToolsAppSQL, envData.m_ApplicationName.c_str() );

        }
        catch( const Nocturnal::Exception& ex)
        {
          const char* dbErrMsg = mysql_error( g_DBHandle->GetDBHandle() );
          Platform::Print( Platform::ConsoleColors::Red, stderr, "%s; %s\n", ex.what(), dbErrMsg );

          g_DBHandle->RollbackTrans();  // roll back the transaction

          delete g_SearchData;          // delete the information that we tried to acquire
          g_SearchData = NULL;

          return;
        }

        g_DBHandle->CommitTrans();  // successfully gathered all of the data, commit
      }
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
        sprintf_s( insertBuff, sizeof( insertBuff ), insert, valueBuff );
        insertDumbPtr = &insertBuff[0];
      }

      return InsertIfNotFound( &selectBuff[0], insertDumbPtr );
    }

    /////////////////////////////////////////////////////////////////////////////
    void Luna::BrowserSearchDatabase::UpdateSearchEvents( const std::string& query )
    {
      // We lost the connection, or it was never established
      if ( !g_DBHandle->IsConnected() || !g_SearchData )
      {
        Platform::Print( Platform::ConsoleColors::Red, stderr, "Lost DB connection: Unable to save search query\n" );

        // Try once to reconnect it
        ResetConnectionData( );

        // if it is still not successful return so that we dont busy loop
        if( !g_DBHandle->IsConnected() || !g_SearchData )
        {
          return;
        }        
      }

      Platform::TakeMutex mutex( g_AssetSearchDBMutex );

      if( !g_SearchData )
      {
        Platform::Print( Platform::ConsoleColors::Red, stderr, "Lost DB connection: AssetBrowser Search Data failed to initialize\n" );
        return;
      }

      Log::Debug("Updating AssetBrowser search_events database...\n");

      g_DBHandle->BeginTrans();

      try
      {
        // AssetBrowser DB
        u64 searchID      = SelectID( s_SelectSearchQueryIDSQL, s_InsertSeqrchQuerySQL, query.c_str() );

        u64 eventID;
        {
          int execResult = g_DBHandle->ExecSQLSNPrintF( s_InsertSearchQuerySQL,
            g_SearchData->loginID,
            g_SearchData->computerID,
            searchID,
            g_SearchData->projectID,
            g_SearchData->codeBranchID,
            g_SearchData->buildConfigID,
            g_SearchData->appID );

          if ( execResult == MYSQL_OK )
          {
            eventID = g_DBHandle->GetLastInsertRowId();
          }
          else
          {
            throw Nocturnal::Exception( "Failed to insert search event" );
          }
        }
      }
      catch( const Nocturnal::Exception& ex )
      {
        const char* dbErrMsg = mysql_error( g_DBHandle->GetDBHandle() );
        Platform::Print( Platform::ConsoleColors::Red, stderr, "%s; %s\n", ex.what(), dbErrMsg );
        g_DBHandle->RollbackTrans();    // failed to do it, rollback the transaction
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
   
    }
  }
}