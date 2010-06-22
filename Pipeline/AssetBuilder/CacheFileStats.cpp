#include "Platform/Windows/Windows.h"
#include "CacheFileStats.h"

#include "Application/Application.h"
#include "Foundation/Log.h"

#include <mysql.h>

#include "Foundation/Version.h"
#include "Foundation/Environment.h"

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

#define SQL_MYSQL_DEFAULT_TIMEOUT (30)

// The return code for mysql success
#define MYSQL_OK 0
//const u64 s_InvalidRowID = 0;

#define STATS_DB_NAME   "AssetBuilder"
#define TOOLS_DB_NAME   "Tools"
#define USERS_DB_NAME   "Users"

// for the sql statistics
static MYSQL*      g_MySQL        = NULL;
static u64         g_ComputerID;
static u64         g_VersionID;

static const u32   g_Port     = 3306;
static const char* g_UserName = "tools";
static const char* g_PassWord = "t0o1z7e4m!";
static const char* g_DataBase = STATS_DB_NAME;
static const i32   g_Timeout  = SQL_MYSQL_DEFAULT_TIMEOUT;

// User DB
const char* s_StartTransactionSQL    = "START TRANSACTION;";
const char* s_CommitTransactionSQL   = "COMMIT;";
const char* s_RollbackTransactionSQL = "ROLLBACK;";

const char* s_SelectUsersComputerIDSQL = "SELECT `id` FROM `"USERS_DB_NAME"`.`computers` WHERE `name`='%s';";
const char* s_InsertUsersComputerSQL = "INSERT INTO `"USERS_DB_NAME"`.`computers` (`name`) VALUES ('%s');";

// Tools DB
const char* s_SelectToolsVersionIDSQL = "SELECT `id` FROM `"TOOLS_DB_NAME"`.`versions` WHERE `version`='%s';";
const char* s_InsertToolsVersionSQL   = "INSERT INTO `"TOOLS_DB_NAME"`.`versions` (`version`) VALUES ('%s');";

// BuilderStatistics DB
const char* s_InsertCacheSQL =
"INSERT INTO `"STATS_DB_NAME"`.`cache_downloads` \
    SET `computer_id`=%I64u, \
    `tools_version_id`=%I64u, \
    `num_files`=%d, \
    `package_size`=%I64u, \
    `avg_speed`=%f, \
    `direction`='%s', \
    `duration`=%f";


/////////////////////////////////////////////////////////////////////////////
static inline bool InsertIfNotFound( const char* select, const char* insert, u64& rowID )
{
  NOC_ASSERT( select );

  if ( g_MySQL == NULL )
  {
    return false;
  }

  bool haveID = false;
  rowID = 0;

  char queryStr[MAX_QUERY_LENGTH];
  _snprintf( queryStr, sizeof(queryStr), "%s", select );
  queryStr[ sizeof(queryStr) - 1] = 0; 
  int execResult = mysql_query( g_MySQL, queryStr );

  if ( execResult == MYSQL_OK )
  {
    MYSQL_RES* sqlResult = mysql_store_result( g_MySQL );
    if ( sqlResult )
    { 
      int numFields = mysql_num_fields( sqlResult );
      if ( numFields > 0 )
      {
        MYSQL_ROW sqlRow = mysql_fetch_row( sqlResult );
        if ( sqlRow && sqlRow[0] != NULL )
        {
          rowID = (u64) _atoi64( sqlRow[0] );
          haveID = true;
        }
      }

      // free the result
      mysql_free_result( sqlResult );
    }
    else
    {
      Log::Warning( "No result for: %s", select );
      return false;
    }
  }
  else
  {
    Log::Warning( "Failed to execute SQL: %s", select );
    return false;
  }

  if ( !haveID && insert )
  {
    memset( queryStr, '\0', MAX_QUERY_LENGTH );
    _snprintf( queryStr, sizeof(queryStr), "%s", insert );
    queryStr[ sizeof(queryStr) - 1] = 0; 
    execResult = mysql_query( g_MySQL, queryStr );

    if ( execResult == MYSQL_OK )
    {
      // query was successful - the auto incremented value is the result
      rowID = ( u64 ) mysql_insert_id( g_MySQL );
      if ( rowID > 0 )
      {
        haveID = true;
      }
    }
    else
    {
      Log::Warning( "Failed to execute SQL: %s", insert );
      return false;
    }
  }

  return haveID;
}

/////////////////////////////////////////////////////////////////////////////
static inline bool SelectID( const char* select, const char* insert, const char* value, u64& rowID )
{
  char valueBuff[MAX_QUERY_LENGTH];
  mysql_real_escape_string( g_MySQL, valueBuff, value, (unsigned long) strlen( value ) );

  char selectBuff[MAX_QUERY_LENGTH];
  sprintf_s( selectBuff, sizeof( selectBuff ), select, valueBuff );

  char* insertDumbPtr = NULL;
  if ( insert )
  {
    char insertBuff[MAX_INSERT_LENGTH];
    sprintf_s( insertBuff, sizeof( insertBuff ), insert, value );
    insertDumbPtr = &insertBuff[0];
  }

  return InsertIfNotFound( &selectBuff[0], insertDumbPtr, rowID );
}

///////////////////////////////////////////////////////////////////////////////
static bool InitializeRowIDs()
{
  // Computer Name
  DWORD nameLength = MAX_COMPUTERNAME_LENGTH + 1;
  char computerName[MAX_COMPUTERNAME_LENGTH + 1];
  ::GetComputerName( computerName, &nameLength );

  if ( !SelectID( s_SelectUsersComputerIDSQL, s_InsertUsersComputerSQL, computerName, g_ComputerID ) )
  {
    return false;
  }

  // Tools Release Version
  if ( !SelectID( s_SelectToolsVersionIDSQL, s_InsertToolsVersionSQL, NOCTURNAL_VERSION_STRING, g_VersionID ) )
  {
    return false;
  }

  return true;
}    

///////////////////////////////////////////////////////////////////////////////
void CacheFileStats::Initialize()
{
  std::string host;
  if ( !Nocturnal::GetEnvVar( "NOC_DB_HOST", host ) )
  {
    Log::Warning( "No database host set in environment (%s).  Build statistics will not be reported.\n", "NOC_DB_HOST" );
    return;
  }

  // Database connection logic
  MYSQL* handle = mysql_init( NULL );
  if ( handle == NULL )
  {
    Log::Warning( "Call to mysql_init failed, could not init MySQL DB." );
    g_MySQL = NULL;
  }
  else
  {
    if ( g_Timeout >= 0 )
    {
      handle->options.connect_timeout = handle->options.read_timeout = handle->options.write_timeout = g_Timeout;
    }

    // connect to the g_DataBase engine
    g_MySQL = mysql_real_connect( handle,
      host.c_str(), g_UserName,
      g_PassWord, g_DataBase,
      g_Port, NULL, 0 );

    // if a failure occured when connecting, then cleanup
    if ( g_MySQL == NULL )
    {
      Log::Warning( "Call to mysql_real_connect failed, could not init MySQL DB." );
      mysql_close( handle );
      handle = NULL;
    }
    else if ( !InitializeRowIDs() )
    {
      CacheFileStats::Cleanup();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
void CacheFileStats::Cleanup()
{
  if ( g_MySQL )
  {
    mysql_close( g_MySQL );
    g_MySQL = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
bool CacheFileStats::IsConnected()
{
  return g_MySQL != NULL;
}

void CacheFileStats::BeginTransaction()
{
  mysql_query( g_MySQL, s_StartTransactionSQL );
}

void CacheFileStats::CommitTransaction()
{
  mysql_query( g_MySQL, s_CommitTransactionSQL );
}

void CacheFileStats::RollbackTransaction()
{
  mysql_query( g_MySQL, s_RollbackTransactionSQL );
}    

///////////////////////////////////////////////////////////////////////////////
bool CacheFileStats::UpdateStats( bool download, u64 size, u32 numFiles, f32 speed, f32 duration )
{
  if ( g_MySQL == NULL )
    return false;

  std::string directionString = download ? "Down" : "Up";

  static char insertCacheBuff[MAX_INSERT_LENGTH] = { '\0' };
  memset( insertCacheBuff, '\0', MAX_QUERY_LENGTH );
  sprintf_s( insertCacheBuff,
    sizeof( insertCacheBuff ),
    s_InsertCacheSQL,
    (u64) g_ComputerID,
    (u64) g_VersionID, 
    numFiles,
    (u64) size,
    (f32) speed,
    (char *) directionString.c_str(),
    duration );

  char queryStr[MAX_QUERY_LENGTH];
  _snprintf( queryStr, sizeof(queryStr), "%s", insertCacheBuff );
  queryStr[ sizeof(queryStr) - 1] = 0; 

  int execResult = mysql_query( g_MySQL, queryStr );
  if ( execResult != MYSQL_OK )
  {
    Log::Warning( "Failed to report cache file statistics, UpdateStats failed.\n" );
    return false;
  }

  return true;
}
