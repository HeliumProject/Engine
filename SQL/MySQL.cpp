#include "stdafx.h"
#include "MySQLStmt.h"
#include "MySQL.h"

#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Windows/Error.h"
#include "Windows/Process.h"

// mysql
#include <mysql.h>
#include <errmsg.h>

using namespace SQL;


//
// Statics
//

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048

static const char *s_DefaultDateTimeFormat = "%M %D at %l:%i%p";

#define MYSQL_OK 0 


// Used in sqlite3_busy_timeout
// After BUSY_TIMEOUT_MS milliseconds (5 min) of sleeping, the handler returns 0 which causes sqlite3_exec() to return SQLITE_BUSY.
#define BUSY_TIMEOUT_MS 300000

static const char* s_BeginTransaction    = "START TRANSACTION;";
static const char* s_CommitTransaction   = "COMMIT;";
static const char* s_RollbackTransaction = "ROLLBACK;";

// The string "not an error" is returned when the most recent sqlite3 API call was successful.
static const char* s_MySQLMsgNoErr = "not an error";     

// determine if an occured because the last inset/update was not unique
static const char* s_NotUniqueErrMsg = " not unique";

static const char* s_MySQLMsgFormat = "MySQL [%s %d] -- %s";


/////////////////////////////////////////////////////////////////////////////
// Default constructor
//
MySQL::MySQL()
: DBManager()
, m_DB( NULL )
{
}


/////////////////////////////////////////////////////////////////////////////
// Default destructor
MySQL::~MySQL( )
{
  Close();
}


/////////////////////////////////////////////////////////////////////////////
// Opens the DB and prepares all of the statements.
//
void MySQL::Connect
( 
 const std::string& host,
 const std::string& user,
 const std::string& passwd,
 const std::string& db, 
 const u32 port,
 const i32 timeout
 )
{
  ThrowIfDBConnected( __FUNCTION__ );

  m_Hostname = host;
  m_Username = user;
  m_Password = passwd;
  m_Database = db;
  m_Port     = port;

  // m_DBFilename is used purely for debugging with MySQL
  // Example: tools@toolshed.insomniacgames.com:3306/ExceptionReports
  std::stringstream dbFilenameStrm;
  dbFilenameStrm << m_Username << "@" << m_Hostname << ":" << m_Port << "/" << m_Database;
  m_DBFilename = dbFilenameStrm.str();

  MYSQL* handle = mysql_init( NULL );

  if ( handle == NULL )
  {
    m_DB = handle;
    SetLastError( __FUNCTION__, "", "Call to mysql_init failed, could not init MySQL DB." );
    m_DB = NULL;
    handle = NULL;
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }
  else
  {
    if ( timeout >= 0 )
    {
      handle->options.connect_timeout = handle->options.read_timeout = handle->options.write_timeout = timeout;
    }

    // connect to the db
    m_DB = mysql_real_connect( handle,
      m_Hostname.c_str(), m_Username.c_str(),
      m_Password.c_str(), m_Database.c_str(),
      m_Port, NULL, 0 );

    // if a failure occured when connecting, then cleanup
    if ( !m_DB )
    {
      m_DB = handle;
      SetLastError( __FUNCTION__, "", "Call to mysql_real_connect failed, could not init MySQL DB." );
      mysql_close( handle );
      m_DB = NULL;
      handle = NULL;
      throw SQL::DBManagerException( this, __FUNCTION__ );
    }
  }
}



/////////////////////////////////////////////////////////////////////////////
// Closes the DB and cleans up all statement handles
//
void MySQL::Close()
{
  if( !IsConnected() )
  {
    return;
  }

  if ( m_IsTransOpen )
  {
    RollbackTrans();
  }

  // Finalize all open statements
  M_MySQLStmt::iterator itStmtHandles = m_StmtHandles.begin();
  M_MySQLStmt::iterator itEndStmtHandles = m_StmtHandles.end();
  for ( ; itStmtHandles != itEndStmtHandles ; ++itStmtHandles )
  {
    //if ( FinalizeStatement( (MySQLStmt) (*itStmtHandles).second ) != MYSQL_OK )
    //{
    //  throw SQL::DBManagerException( this, __FUNCTION__ );
    //}
  }

  // close the DB connection
  mysql_close( m_DB );
  m_DB = NULL;

  // Erase all the list of stmt handles:
  // - We don't erase them as they are finalized because that runs in linear time
  //   per call, better to erase them all at once.
  // - Also this is done AFTER mysql_close to ensure that the handles remain if
  //   the above call should fail.
  m_StmtHandles.clear();
  m_NumStmtHandles = 0;
}


/////////////////////////////////////////////////////////////////////////////
// test connection status (did we connect in Initialize())
bool MySQL::IsConnected() const
{
  return m_DB != NULL;
}



/////////////////////////////////////////////////////////////////////////////
// Execute a SQL statement on the DB and handle the errors
//
int MySQL::ExecSQL( const char* sql )
{
  ThrowIfDBNotConnected( __FUNCTION__ );

  int execResult = mysql_query( m_DB, sql );

  if ( execResult != MYSQL_OK )
  {
    SetLastError( __FUNCTION__, "", "Failed to execute SQL statement: \"%s\".", sql );
  }

  return execResult;
}

/////////////////////////////////////////////////////////////////////////////
// Gets the newly inserted Row ID
i64 MySQL::GetLastInsertRowId()
{
  i64 lastRowId = ( i64 ) mysql_insert_id( m_DB );

  if ( lastRowId <= 0 )
  {
    throw SQL::DBManagerException( this, __FUNCTION__, "No rows were inserted." );
  }
  return lastRowId;
}





#pragma TODO("Convert to MySQL queries")
/////////////////////////////////////////////////////////////////////////////
// Uses snprintf to populate the given sql string with variable
// arguments; then calls ExecSQL.
//
// 
int MySQL::ExecSQLSNPrintF( const char* sql, ... )
{
  ThrowIfDBNotConnected( "ExecSQLSNPrintF" );

  va_list sqlArgs;
  va_start( sqlArgs, sql );

  static char queryStr[MAX_QUERY_LENGTH];
  memset( queryStr, '\0', sizeof(queryStr)); 

  _vsnprintf( queryStr, sizeof(queryStr), sql, sqlArgs );
  queryStr[ sizeof(queryStr) - 1] = 0; 

  int execResult = MYSQL_OK;
  try
  {
    execResult = mysql_query( m_DB, queryStr );
    //Windows::Print( Windows::ConsoleColors::Red, stderr, "%s\n", queryStr );
  }
  catch( ... )
  {
    va_end( sqlArgs );
    throw;
  }

  va_end( sqlArgs );

  return execResult;
}


/////////////////////////////////////////////////////////////////////////////
// Begins a new transaction; this wrapper ensures that there is only one
// open transaction at all times.
//
void MySQL::BeginTrans()
{
  ThrowIfMaxOpenTrans( "MySQL::BeginTrans" );

  if ( ExecSQL( s_BeginTransaction ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  m_IsTransOpen = true;
}



/////////////////////////////////////////////////////////////////////////////
// Commits the existing transaction; this wrapper ensures that there is
// only one open transaction at all times.
//
void MySQL::CommitTrans()
{
  ThrowIfNoTransOpen( "MySQL::CommitTrans" );

  if ( ExecSQL( s_CommitTransaction ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  m_IsTransOpen = false;
}



/////////////////////////////////////////////////////////////////////////////
// RollBack the existing transaction; this wrapper ensures that there is
// only one open transaction at all times.
//
void MySQL::RollbackTrans()
{
  ThrowIfNoTransOpen( "MySQL::RollbackTrans" );

  if ( ExecSQL( s_RollbackTransaction ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  m_IsTransOpen = false;
}


/////////////////////////////////////////////////////////////////////////////
// The bindFormat string may contain any combination (of zero or more
// characters) from the following list, where each character specifies the
// type (and order) of the arguments expected by the BindStatement function:
//
// c - signed char   MYSQL_TYPE_TINY        TINYINT     1
// s - short int     MYSQL_TYPE_SHORT       SMALLINT    2
// i - int           MYSQL_TYPE_LONG        INT         4
// l - long long int MYSQL_TYPE_LONGLONG    BIGINT      8
// f - float         MYSQL_TYPE_FLOAT       FLOAT       4
// d - double        MYSQL_TYPE_DOUBLE      DOUBLE      8

// ? - MYSQL_TIME    MYSQL_TYPE_DATE        DATE        sizeof(MYSQL_TIME)
// ? - MYSQL_TIME    MYSQL_TYPE_DATETIME    DATETIME    sizeof(MYSQL_TIME)
// ? - MYSQL_TIME    MYSQL_TYPE_TIME        TIME        sizeof(MYSQL_TIME)
// ? - MYSQL_TIME    MYSQL_TYPE_TIMESTAMP   TIMESTAMP   sizeof(MYSQL_TIME)

// t - char[]        MYSQL_TYPE_STRING      TEXT        data_length         (TEXT, CHAR, VARCHAR; for non-binary data)
// b - char[]        MYSQL_TYPE_BLOB        BLOB        data_length         (BLOB, BINARY, VARBINARY; for binary data)
// n -               MYSQL_TYPE_NULL        NULL
//
bool MySQL::ValidateBindFormat( const std::string& bindFormat )
{
  if ( !bindFormat.empty() )
  {
    // validate that bindFormat only contains letters "cslLfdtbn"
    // Pattern used to validate statement bind types string
    const boost::regex s_AcceptedBindTypes( "^[csilfdtbn]*$" );

    boost::smatch resultBindFormat; 
    if( !boost::regex_match(bindFormat, resultBindFormat, s_AcceptedBindTypes))
    {
      return false;
    }
  }

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// Creates a MySQL statement handle.
//
StmtHandle MySQL::CreateStatement( const char* sql, const std::string &bindFormat )
{
  ThrowIfDBNotConnected( "CreateStatement" );

  StmtHandle handle = SQL::NullStatement;
  MySQLStmt stmt( sql, bindFormat );

  if ( !ValidateBindFormat( bindFormat ) )
  {
    throw SQL::DBManagerException( this, __FUNCTION__, "Invalid bind format (%s), bindFormat may only contain letters \"bdilntT\". Failed to create statement: \"%s\"",
      bindFormat.c_str(),
      stmt.m_SQL.c_str() );
  }

  // Prepare the statement
  if ( PrepareStatement( stmt ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  // Validate the bindFormat string length
  int bindParamCount = mysql_stmt_param_count( stmt.m_Stmt );
  if ( stmt.m_BindFormat.length() != bindParamCount )
  {
    FinalizeStatement( stmt );
    throw SQL::DBManagerException( this, __FUNCTION__, "Invalid bind format (%s), wrong number of parameters given (expects %d). Failed to create statement: \"%s\"",
      stmt.m_BindFormat.c_str(),
      bindParamCount,
      stmt.m_SQL.c_str() );
  }

  // store it in the map and set handle
  handle = GetNewStamtentHandle();
  m_StmtHandles[handle] = stmt;

  return handle;
}


/////////////////////////////////////////////////////////////////////////////
MySQLStmt& MySQL::FindStatement( const StmtHandle handle )
{
  M_MySQLStmt::iterator it = m_StmtHandles.find( handle );
  if ( it == m_StmtHandles.end() )
  {
    throw SQL::StmtHandleNotFoundException( this, __FUNCTION__ );
  }

  return (*it).second;
}


/////////////////////////////////////////////////////////////////////////////
int MySQL::PrepareStatement( MySQLStmt& stmt )
{ 
  stmt.m_Stmt = mysql_stmt_init( m_DB );
  if ( !stmt.m_Stmt )
  {
    SetLastError( __FUNCTION__, "Out of memory.", "Call to mysql_stmt_init failed. Failed to create statement: \"%s\".", stmt.m_SQL.c_str() );
    return CR_OUT_OF_MEMORY;
  }

  int prepareResult = mysql_stmt_prepare( stmt.m_Stmt, stmt.m_SQL.c_str(), (u32) stmt.m_SQL.size() );
  if ( prepareResult != MYSQL_OK )
  {
    SetLastError( __FUNCTION__, "", "Call to mysql_stmt_prepare failed. Failed to create statement: \"%s\".", stmt.m_SQL.c_str() );
    return prepareResult;
  }

  stmt.m_IsPrepared = true;

  return prepareResult;
}

/////////////////////////////////////////////////////////////////////////////
void MySQL::FinalizeStatement( const StmtHandle &handle )
{
  ThrowIfDBNotConnected( __FUNCTION__ );

  // Locate the handle
  MySQLStmt& stmt = FindStatement( handle );

  if ( FinalizeStatement( stmt ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }
}


/////////////////////////////////////////////////////////////////////////////
int MySQL::FinalizeStatement( MySQLStmt& stmt )
{
  if ( !stmt.m_IsPrepared || !stmt.m_Stmt )
  {
    return MYSQL_OK;
  }

  int finalizeResult = mysql_stmt_close( stmt.m_Stmt );
  if ( finalizeResult != MYSQL_OK )
  {
    SetLastError( __FUNCTION__, "", "Failed to finalize statement, with mysql_stmt_close: \"%s\".", stmt.m_SQL );
    return finalizeResult;
  }

  stmt.m_Stmt = NULL;
  stmt.m_IsPrepared = false;

  return finalizeResult;
}


/////////////////////////////////////////////////////////////////////////////
// Locate and step the given statement handle.
int MySQL::StepStatement( const StmtHandle& handle, const bool autoReset )
{
  ThrowIfDBNotConnected( __FUNCTION__ );

  // Locate the handle
  MySQLStmt& stmt = FindStatement( handle );

  // make sure it's prepared and does not need to be recompiled
  if ( !stmt.IsPrepared() )
  {
    SetLastError( __FUNCTION__, "", "Cannot step uncompiled statement: \"%s\".", stmt.m_SQL );
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  // Execute the statement handle
  int stepResult = mysql_stmt_execute( stmt.m_Stmt );
  if ( stepResult != MYSQL_OK  )
  {
    ResetStatement( handle );
  }
  else if ( autoReset )
  {
#pragma TODO("Convert to MySQL queries")
    // autoReset's the statement if the result != SQLITE_ROW
    //if ( stepResult != SQLITE_ROW )
    //{
    //  ResetStatement( stmt );
    //}
  }

  return stepResult;
}


/////////////////////////////////////////////////////////////////////////////
// Resets the statement handle
//
//void MySQL::ResetStatement( const StmtHandle handle )
//{
//  ThrowIfDBNotConnected( __FUNCTION__ );
//
//  // Locate the handle
//  MySQLStmt& stmt = FindStatement( handle );
//
//  // make sure it's prepared and does not need to be recompiled
//  if ( !stmt.IsPrepared() )
//  {
//    SetLastError( __FUNCTION__, "", "Cannot reset uncompiled statement: \"%s\".", stmt.m_SQL );
//    throw SQL::DBManagerException( this, __FUNCTION__ );
//  }
//
//  if ( ResetStatement( stmt ) != MYSQL_OK )
//  {
//    throw SQL::DBManagerException( this, __FUNCTION__ );
//  }
//}


/////////////////////////////////////////////////////////////////////////////
void MySQL::ResetStatement( const StmtHandle& handle )
{
  // Locate the handle
  MySQLStmt& stmt = FindStatement( handle );

  int resetResult = mysql_stmt_reset( stmt.m_Stmt );

  if ( resetResult != MYSQL_OK )
  {
    SetLastError( __FUNCTION__, "", "Reset statement failed for: \"%s\".", stmt.m_SQL );
  }

  stmt.m_IsBound = false;
}


/////////////////////////////////////////////////////////////////////////////
// Binds a list of arguments to the given sqlite statement using
// sqlite3_bind* functions.
//
void MySQL::BindStatement( const StmtHandle handle, ... )
{
  ThrowIfDBNotConnected( __FUNCTION__ );

  // Locate the handle
  MySQLStmt& stmt = FindStatement( handle );

  // Prepare the stmt if it: is not prepared, or has expired
  if ( !stmt.IsPrepared() && PrepareStatement( stmt ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  va_list bindArgs;
  va_start( bindArgs, handle );
  BindStatement( stmt, bindArgs );
  va_end( bindArgs );
}


/////////////////////////////////////////////////////////////////////////////
void MySQL::BindStatement( MySQLStmt& stmt, va_list bindArgs )
{
  if ( stmt.m_BindFormat.length() < 1 )
  {
    return;
  }

  stmt.m_Strings.clear();

  bool callMySQLBind = false;

  if ( !stmt.m_InputBinds )
  {
    stmt.m_InputBindCount = ( int ) stmt.m_BindFormat.length();
    stmt.m_InputBinds = new MYSQL_BIND[ stmt.m_InputBindCount ];
    stmt.m_InputBindables = new U_DBRowType[ stmt.m_InputBindCount ];
    stmt.m_InputBindLengths = new u32[ stmt.m_InputBindCount ];
    callMySQLBind = true;
  }

  u32 index = 0;
  for (  ; index < stmt.m_InputBindCount && stmt.m_BindFormat[index] != '\0' ; ++index )
  {
    memset( &stmt.m_InputBinds[ index ], 0, sizeof( MYSQL_BIND ) );
    memset( &stmt.m_InputBindables[ index ], 0, sizeof( U_DBRowType ) );
    stmt.m_InputBindLengths[ index ] = 0;

    switch( stmt.m_BindFormat[index] )
    {
    default:
      throw SQL::DBManagerException( this, __FUNCTION__, "Bad input specified for BindStatement." );
      break;

    case 'c': // _tinyint
      stmt.m_InputBindables[ index ]._tinyint = va_arg( bindArgs, i32 );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_TINY;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._tinyint; 
      break;

    case 's': // _i16
      stmt.m_InputBindables[ index ]._i16     = va_arg( bindArgs, i32 );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_SHORT;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._i16;
      break;

    case 'i': // _int
      stmt.m_InputBindables[ index ]._int     = va_arg( bindArgs, int );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_LONG;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._int;
      //stmt.m_InputBinds[ index ].is_null     = 0;
      break;

    case 'l': // _i64
      stmt.m_InputBindables[ index ]._i64     = va_arg( bindArgs, i64 );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_LONGLONG;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._i64;
      break;

    case 'f': // _float
      stmt.m_InputBindables[ index ]._float   = (f32) va_arg( bindArgs, double );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_FLOAT;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._float;
      break;

    case 'd': // _double
      stmt.m_InputBindables[ index ]._double  = va_arg( bindArgs, double );
      stmt.m_InputBinds[ index ].buffer_type  = MYSQL_TYPE_DOUBLE;
      stmt.m_InputBinds[ index ].buffer       = (char*)&stmt.m_InputBindables[ index ]._double;
      break;

    //case '?': // _date
    //  Bindable._date = va_arg( bindArgs, MYSQL_TIME );
    //  stmt.m_InputBinds[ index ].buffer_type = MYSQL_TYPE_DATE;
    //  stmt.m_InputBinds[ index ].buffer      = (char*)&stmt.m_InputBindables[ index ]._date;
    //  break;

    //case '?': // _datetime
    //  Bindable._datetime = va_arg( bindArgs, MYSQL_TIME );
    //  stmt.m_InputBinds[ index ].buffer_type = MYSQL_TYPE_DATETIME;
    //  stmt.m_InputBinds[ index ].buffer      = (char*)&stmt.m_InputBindables[ index ]._datetime;
    //  break;

    //case '?': // _time
    //  Bindable._time = va_arg( bindArgs, MYSQL_TIME );
    //  stmt.m_InputBinds[ index ].buffer_type = MYSQL_TYPE_TIME;
    //  stmt.m_InputBinds[ index ].buffer      = (char*)&stmt.m_InputBindables[ index ]._time;
    //  break;

    //case '?': // _timestamp
    //  Bindable._timestamp = va_arg( bindArgs, MYSQL_TIME );
    //  stmt.m_InputBinds[ index ].buffer_type = MYSQL_TYPE_TIMESTAMP;
    //  stmt.m_InputBinds[ index ].buffer      = (char*)&stmt.m_InputBindables[ index ]._timestamp;
    //  break;

    case 't': // _text
      {
        stmt.m_Strings.push_back( va_arg( bindArgs, char* ) );

        stmt.m_InputBindables[ index ]._text     = (*stmt.m_Strings.rbegin()).c_str();
        stmt.m_InputBinds[ index ].buffer_type   = MYSQL_TYPE_STRING;
        stmt.m_InputBinds[ index ].buffer        = (void*) stmt.m_InputBindables[ index ]._text;
        stmt.m_InputBindLengths[ index ]         = (unsigned long) strlen( stmt.m_InputBindables[ index ]._text ); 
        stmt.m_InputBinds[ index ].buffer_length = stmt.m_InputBindLengths[ index ];
        stmt.m_InputBinds[ index ].length        = (unsigned long*) &stmt.m_InputBindLengths[ index ];
      }
      break;

    case 'b': // _blob
      {
        throw SQL::DBManagerException( this, __FUNCTION__, "Blob not currently supported." );
        /*
        void* incoming = va_arg( bindArgs, void* );

        stmt.m_InputBindables[ index ]._blob     = new char[ sizeof( incoming ) ];
        memcpy_s( stmt.m_InputBindables[ index ]._blob, sizeof( stmt.m_InputBindables[ index ]._blob ), incoming, sizeof( incoming ) );

        stmt.m_InputBinds[ index ].buffer_type   = MYSQL_TYPE_BLOB;
        stmt.m_InputBinds[ index ].buffer        = stmt.m_InputBindables[ index ]._blob;
        stmt.m_InputBindLengths[ index ]         = (unsigned long) sizeof( stmt.m_InputBindables[ index ]._blob ); 
        stmt.m_InputBinds[ index ].buffer_length = stmt.m_InputBindLengths[ index ];
        stmt.m_InputBinds[ index ].length        = (unsigned long*) &stmt.m_InputBindLengths[ index ];
        */
      }
      break;

    //case 'n': // _null
    //  Bindable._null = va_arg( bindArgs, void* );
    //  stmt.m_InputBinds[ index ].buffer_type   = MYSQL_TYPE_NULL;
    //  stmt.m_InputBinds[ index ].buffer        = (char*)&Bindable._null;
    //  break;
    }
  }

  if ( callMySQLBind )
  {
    if ( mysql_stmt_bind_param( stmt.m_Stmt, stmt.m_InputBinds )   != MYSQL_OK )
    {
      SetLastError( __FUNCTION__, "", "Failed to bind argument (index %d in bind format %s) for statement: \"%s\".", index, stmt.m_BindFormat.c_str(), stmt.m_SQL );
      throw SQL::DBManagerException( this, __FUNCTION__ );
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
int MySQL::ExecStatement( const StmtHandle handle, ... )
{
  ThrowIfDBNotConnected( "ExecStatement" );

  // Locate the handle
  MySQLStmt& stmt = FindStatement( handle );

  // Prepare the stmt if it: is not prepared, or has expired
  if ( !stmt.IsPrepared() && PrepareStatement( stmt ) != MYSQL_OK )
  {
    throw SQL::DBManagerException( this, __FUNCTION__ );
  }

  va_list bindArgs;
  va_start( bindArgs, handle );
  BindStatement( stmt, bindArgs );
  va_end( bindArgs );

  int stepResult = StepStatement( handle );

  return stepResult; 
}

/////////////////////////////////////////////////////////////////////////////
// Helper funciton for GetColumn* functions: Finds the statement by the
// given handle and validates the requested columnIndex.
//
//MySQLStmt& MySQL::GetColumnStatement( const StmtHandle handle, const int columnIndex )
//{     
//  ThrowIfDBNotConnected( __FUNCTION__ );
//
//  // Locate the handle
//  MySQLStmt& stmt = FindStatement( handle );
//
//  // make sure it's already prepared
//  if ( !stmt.IsPrepared() )
//  {
//    throw SQL::DBManagerException( this, __FUNCTION__, "Could not get column data from unprepared statement: \"%s\".", stmt.m_SQL );
//  }
//
//  // they are requesting a column not in the result set
//  if ( columnIndex >= (int) mysql_stmt_field_count( stmt.m_Stmt ) )
//  {
//    throw SQL::DBManagerException( this, __FUNCTION__, "Could not get column data, column index [%d] out of bounds for statement: \"%s\".", columnIndex, stmt.m_SQL );
//  }
//
//  return stmt;
//}

/////////////////////////////////////////////////////////////////////////////
// Return the number of columns in the result set returned by the prepared
// SQL statement. This routine returns 0 if pStmt is an SQL statement that
// does not return data (for example an UPDATE).
//
//int MySQL::GetColumnCount( const StmtHandle handle )
//{
//  ThrowIfDBNotConnected( __FUNCTION__ );
//
//  // Locate the handle
//  MySQLStmt& stmt = FindStatement( handle );
//
//  // make sure it's already prepared
//  if ( !stmt.IsPrepared() )
//  {
//    throw SQL::DBManagerException( this, __FUNCTION__, "Cannot get column data from unprepared statement!" );
//  }
//
//  int colCount = ( int ) mysql_stmt_field_count( stmt.m_Stmt );
//
//  // check for errors
//  u32 sqlErrNum = mysql_errno( m_DB );
//  if ( sqlErrNum != MYSQL_OK )
//  {
//    SetLastError( __FUNCTION__, "", "GetColumnCount call failed when trying to retrieve column data from statement: \"%s\".", stmt.m_SQL );
//    throw SQL::DBManagerException( this, __FUNCTION__ );
//  }
//
//  return colCount;
//}

/////////////////////////////////////////////////////////////////////////////
// 
//void MySQL::GetColumnName( const StmtHandle handle, const int columnIndex, std::string &name )
//{
//  ThrowIfDBNotConnected( "GetColumnName" );
//#pragma TODO("Convert GetColumn to MySQL queries")
//  //MySQLStmt& stmt = GetColumnStatement( handle, columnIndex );
//
//  //char* tmpData = (char*) sqlite3_column_name( stmt.m_Stmt, columnIndex );
//  //if ( tmpData )
//  //{
//  //  name = tmpData;
//  //}
//  //else
//  //{
//  //  name.clear();
//  //}
//
//  //// check for errors
//  //std::string sqlErr = ( char*  ) sqlite3_errmsg( m_DB );
//  //if ( sqlErr.compare( s_MySQLMsgNoErr ) != 0 )
//  //{
//  //  SetLastError( __FUNCTION__, sqlErr.c_str(), "GetColumnName call failed when trying to retrieve column %d name from statement: \"%s\".", columnIndex, stmt.m_SQL );
//  //  throw SQL::DBManagerException( this, __FUNCTION__ );
//  //}
//
//}


/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as double.
//
//void MySQL::GetColumnDouble( const StmtHandle handle, const int columnIndex, double &data )
//{
//  MySQLStmt& stmt = GetColumnStatement( handle, columnIndex );
//#pragma TODO("Convert GetColumn to MySQL queries")
//  //data = ( double ) sqlite3_column_double( stmt.m_Stmt, columnIndex );
//
//  //// check for errors
//  //std::string sqlErr = ( char*  ) sqlite3_errmsg( m_DB );
//  //if ( sqlErr.compare( s_MySQLMsgNoErr ) != 0 )
//  //{
//  //  SetLastError( __FUNCTION__, sqlErr.c_str(), "GetColumnDouble call failed when trying to retrieve column data from statement: \"%s\".", stmt.m_SQL );
//  //  throw SQL::DBManagerException( this, __FUNCTION__ );
//  //}
//
//}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as int.
//
//void MySQL::GetColumnInt( const StmtHandle handle, const int columnIndex, int &data )
//{
//  MySQLStmt& stmt = GetColumnStatement( handle, columnIndex );
//#pragma TODO("Convert GetColumn to MySQL queries")
//  //data = ( int ) sqlite3_column_int( stmt.m_Stmt, columnIndex );
//
//  //// check for errors
//  //std::string sqlErr = ( char*  ) sqlite3_errmsg( m_DB );
//  //if ( sqlErr.compare( s_MySQLMsgNoErr ) != 0 )
//  //{
//  //  SetLastError( __FUNCTION__, sqlErr.c_str(), "GetColumnInt call failed when trying to retrieve column data from statement: \"%s\".", stmt.m_SQL );
//  //  throw SQL::DBManagerException( this, __FUNCTION__ );
//  //}
//
//}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as i64.
//
//void MySQL::GetColumnI64( const StmtHandle handle, const int columnIndex, i64 &data )
//{
//  MySQLStmt& stmt = GetColumnStatement( handle, columnIndex );
//#pragma TODO("Convert GetColumn to MySQL queries")
//  //data = ( i64 ) sqlite3_column_int64( stmt.m_Stmt, columnIndex );
//
//  //// check for errors
//  //std::string sqlErr = ( char*  ) sqlite3_errmsg( m_DB );
//  //if ( sqlErr.compare( s_MySQLMsgNoErr ) != 0 )
//  //{
//  //  SetLastError( __FUNCTION__, sqlErr.c_str(), "GetColumnI64 call failed when trying to retrieve column data from statement: \"%s\".", stmt.m_SQL );
//  //  throw SQL::DBManagerException( this, __FUNCTION__ );
//  //}
//}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as std::string.
//
//void MySQL::GetColumnText( const StmtHandle handle, const int columnIndex, char* *data )
//void MySQL::GetColumnText( const StmtHandle handle, const int columnIndex, std::string &data )
//{
//  MySQLStmt& stmt = GetColumnStatement( handle, columnIndex );
//#pragma TODO("Convert GetColumn to MySQL queries")
//  //char* tmpData = (char*)sqlite3_column_text( stmt.m_Stmt, columnIndex );
//  //if ( tmpData )
//  //{
//  //  data = tmpData;
//  //}
//  //else
//  //{
//  //  data.clear();
//  //}
//
//  //// check for errors
//  //std::string sqlErr = ( char*  ) sqlite3_errmsg( m_DB );
//  //if ( sqlErr.compare( s_MySQLMsgNoErr ) != 0 )
//  //{
//  //  SetLastError( __FUNCTION__, sqlErr.c_str(), "GetColumnText call failed when trying to retrieve column data from statement: \"%s\".", stmt.m_SQL );
//  //  throw SQL::DBManagerException( this, __FUNCTION__ );3
//  //}
//}


/////////////////////////////////////////////////////////////////////////////
// Sets m_LastErrMsg with the pretty prints of the last MySQL error message,
// along with some additional debuging info in teh following format:
//
// "sqlite3 [<SQLITE_ERROR_CODE> <CODE>] -- <SQlite error message>[ in <FunctionName>][; <Additional message>]";
// 
// Example:
//    MySQL [SQLITE_MISUSE 21] -- library routine called out of sequence in ResetStatement;
//    Failed to execute SQL statement: ROLLBACK TRANSACTION;
//
void MySQL::SetLastError( const char* function, const char* errMsg, const char* msgFormat, ... )
{
  //////////////////////////////////////
  // Get the error code
  //
  m_LastErrCode = mysql_errno( m_DB );


  //////////////////////////////////////
  // Get the error message
  //
  char msgBuffer[Nocturnal::ERROR_STRING_BUF_SIZE];
  if ( strcmp( errMsg, "" ) != 0 )
  {
    _snprintf( msgBuffer, sizeof(msgBuffer), s_MySQLMsgFormat, MySQLErrCodeToString( m_LastErrCode ).c_str(), m_LastErrCode, errMsg ); 
  }
  else
  {
    const char* dbErrMsg = mysql_error( m_DB );
    _snprintf( msgBuffer, sizeof(msgBuffer), s_MySQLMsgFormat, MySQLErrCodeToString( m_LastErrCode ).c_str(), m_LastErrCode, dbErrMsg );
  }
  msgBuffer[ sizeof(msgBuffer) - 1] = 0; 

  m_LastErrMsg = msgBuffer;


  //////////////////////////////////////
  // Append the function name
  //
  if ( strcmp( function, "" ) != 0 )
  {
    m_LastErrMsg += " in ";
    m_LastErrMsg += function;
  }

  //////////////////////////////////////
  // Append any additional messages
  //
  if ( strcmp( msgFormat, "" ) != 0 )
  {
    // reuse of msgBuffer requires clear (why is that? i don't know)
    // 
    memset( msgBuffer, '\0', sizeof(msgBuffer)); 

    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    vsnprintf_s( msgBuffer, sizeof(msgBuffer), _TRUNCATE, msgFormat, msgArgs );
    msgBuffer[ sizeof(msgBuffer)] = 0; 
    va_end( msgArgs );


    m_LastErrMsg += "; ";
    m_LastErrMsg += msgBuffer;
  }

  // Append the DBFilename and the function to the last error message
  m_LastErrMsg += "(DB: ";
  m_LastErrMsg += m_DBFilename;
  m_LastErrMsg += ")";

  Console::Debug( "SetLastError called:\n  %s\n", m_LastErrMsg.c_str() );
}


/////////////////////////////////////////////////////////////////////////////
//  \#define {[A-Z_]+}{[ ]+}{:z+}{ \/\/ }{.*}
//  case \1:\2\/\/ \3 \5\nerrCodeString = "\1";\nbreak;\n
std::string SQL::MySQLErrCodeToString( int errCode )
{
  std::string errCodeString = "";
  switch( errCode )
  {
  default:
    break;

  case MYSQL_OK:                                  // 0    Successful result
    errCodeString = "MYSQL_OK";
    break;

  case CR_UNKNOWN_ERROR:                          // 2000 Unknown MySQL error
    errCodeString = "CR_UNKNOWN_ERROR";
    break;

  case CR_SOCKET_CREATE_ERROR:                    // 2001 Can't create UNIX socket (%d)
    errCodeString = "CR_SOCKET_CREATE_ERROR";
    break;

  case CR_CONNECTION_ERROR:                       // 2002 Can't connect to local MySQL server through socket '%s' (%d)
    errCodeString = "CR_CONNECTION_ERROR";
    break;

  case CR_CONN_HOST_ERROR:                        // 2003 Can't connect to MySQL server on '%s' (%d)
    errCodeString = "CR_CONN_HOST_ERROR";
    break;

  case CR_IPSOCK_ERROR:                           // 2004 Can't create TCP/IP socket (%d)
    errCodeString = "CR_IPSOCK_ERROR";
    break;

  case CR_UNKNOWN_HOST:                           // 2005 Unknown MySQL server host '%s' (%d)
    errCodeString = "CR_UNKNOWN_HOST";
    break;

  case CR_SERVER_GONE_ERROR:                      // 2006 MySQL server has gone away
    errCodeString = "CR_SERVER_GONE_ERROR";
    break;

  case CR_VERSION_ERROR:                          // 2007 Protocol mismatch; server version = %d, client version = %d
    errCodeString = "CR_VERSION_ERROR";
    break;

  case CR_OUT_OF_MEMORY:                          // 2008 MySQL client ran out of memory
    errCodeString = "CR_OUT_OF_MEMORY";
    break;

  case CR_WRONG_HOST_INFO:                        // 2009 Wrong host info
    errCodeString = "CR_WRONG_HOST_INFO";
    break;

  case CR_LOCALHOST_CONNECTION:                   // 2010 Localhost via UNIX socket
    errCodeString = "CR_LOCALHOST_CONNECTION";
    break;

  case CR_TCP_CONNECTION:                         // 2011 %s via TCP/IP
    errCodeString = "CR_TCP_CONNECTION";
    break;

  case CR_SERVER_HANDSHAKE_ERR:                   // 2012 Error in server handshake
    errCodeString = "CR_SERVER_HANDSHAKE_ERR";
    break;

  case CR_SERVER_LOST:                            // 2013 Lost connection to MySQL server during query
    errCodeString = "CR_SERVER_LOST";
    break;

  case CR_COMMANDS_OUT_OF_SYNC:                   // 2014 Commands out of sync; you can't run this command now
    errCodeString = "CR_COMMANDS_OUT_OF_SYNC";
    break;

  case CR_NAMEDPIPE_CONNECTION:                   // 2015 Named pipe: %s
    errCodeString = "CR_NAMEDPIPE_CONNECTION";
    break;

  case CR_NAMEDPIPEWAIT_ERROR:                    // 2016 Can't wait for named pipe to host: %s pipe: %s (%lu)
    errCodeString = "CR_NAMEDPIPEWAIT_ERROR";
    break;

  case CR_NAMEDPIPEOPEN_ERROR:                    // 2017 Can't open named pipe to host: %s pipe: %s (%lu)
    errCodeString = "CR_NAMEDPIPEOPEN_ERROR";
    break;

  case CR_NAMEDPIPESETSTATE_ERROR:                // 2018 Can't set state of named pipe to host: %s pipe: %s (%lu)
    errCodeString = "CR_NAMEDPIPESETSTATE_ERROR";
    break;

  case CR_CANT_READ_CHARSET:                      // 2019 Can't initialize character set %s (path: %s)
    errCodeString = "CR_CANT_READ_CHARSET";
    break;

  case CR_NET_PACKET_TOO_LARGE:                   // 2020 Got packet bigger than 'max_allowed_packet' bytes
    errCodeString = "CR_NET_PACKET_TOO_LARGE";
    break;

  case CR_EMBEDDED_CONNECTION:                    // 2021 Embedded server
    errCodeString = "CR_EMBEDDED_CONNECTION";
    break;

  case CR_PROBE_SLAVE_STATUS:                     // 2022 Error on SHOW SLAVE STATUS:
    errCodeString = "CR_PROBE_SLAVE_STATUS";
    break;

  case CR_PROBE_SLAVE_HOSTS:                      // 2023 Error on SHOW SLAVE HOSTS:
    errCodeString = "CR_PROBE_SLAVE_HOSTS";
    break;

  case CR_PROBE_SLAVE_CONNECT:                    // 2024 Error connecting to slave:
    errCodeString = "CR_PROBE_SLAVE_CONNECT";
    break;

  case CR_PROBE_MASTER_CONNECT:                   // 2025 Error connecting to master:
    errCodeString = "CR_PROBE_MASTER_CONNECT";
    break;

  case CR_SSL_CONNECTION_ERROR:                   // 2026 SSL connection error
    errCodeString = "CR_SSL_CONNECTION_ERROR";
    break;

  case CR_MALFORMED_PACKET:                       // 2027 Malformed packet
    errCodeString = "CR_MALFORMED_PACKET";
    break;

  case CR_WRONG_LICENSE:                          // 2028 This client library is licensed only for use with MySQL servers having '%s' license
    errCodeString = "CR_WRONG_LICENSE";
    break;

    /* new 4.1 error codes */
  case CR_NULL_POINTER:                           // 2029 Invalid use of null pointer
    errCodeString = "CR_NULL_POINTER";
    break;

  case CR_NO_PREPARE_STMT:                        // 2030 Statement not prepared
    errCodeString = "CR_NO_PREPARE_STMT";
    break;

  case CR_PARAMS_NOT_BOUND:                       // 2031 No data supplied for parameters in prepared statement
    errCodeString = "CR_PARAMS_NOT_BOUND";
    break;

  case CR_DATA_TRUNCATED:                         // 2032 Data truncated
    errCodeString = "CR_DATA_TRUNCATED";
    break;

  case CR_NO_PARAMETERS_EXISTS:                   // 2033 No parameters exist in the statement
    errCodeString = "CR_NO_PARAMETERS_EXISTS";
    break;

  case CR_INVALID_PARAMETER_NO:                   // 2034 Invalid parameter number
    errCodeString = "CR_INVALID_PARAMETER_NO";
    break;

  case CR_INVALID_BUFFER_USE:                     // 2035 Can't send long data for non-string/non-binary data types (parameter: %d)
    errCodeString = "CR_INVALID_BUFFER_USE";
    break;

  case CR_UNSUPPORTED_PARAM_TYPE:                 // 2036 Using unsupported buffer type: %d (parameter: %d)
    errCodeString = "CR_UNSUPPORTED_PARAM_TYPE";
    break;


  case CR_SHARED_MEMORY_CONNECTION:               // 2037 Shared memory: %s
    errCodeString = "CR_SHARED_MEMORY_CONNECTION";
    break;

  case CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR:    // 2038 Can't open shared memory; client could not create request event (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR";
    break;

  case CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR:     // 2039 Can't open shared memory; no answer event received from server (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR";
    break;

  case CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR:   // 2040 Can't open shared memory; server could not allocate file mapping (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR";
    break;

  case CR_SHARED_MEMORY_CONNECT_MAP_ERROR:        // 2041 Can't open shared memory; server could not get pointer to file mapping (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_MAP_ERROR";
    break;

  case CR_SHARED_MEMORY_FILE_MAP_ERROR:           // 2042 Can't open shared memory; client could not allocate file mapping (%lu)
    errCodeString = "CR_SHARED_MEMORY_FILE_MAP_ERROR";
    break;

  case CR_SHARED_MEMORY_MAP_ERROR:                // 2043 Can't open shared memory; client could not get pointer to file mapping (%lu)
    errCodeString = "CR_SHARED_MEMORY_MAP_ERROR";
    break;

  case CR_SHARED_MEMORY_EVENT_ERROR:              // 2044 Can't open shared memory; client could not create %s event (%lu)
    errCodeString = "CR_SHARED_MEMORY_EVENT_ERROR";
    break;

  case CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR:  // 2045 Can't open shared memory; no answer from server (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR";
    break;

  case CR_SHARED_MEMORY_CONNECT_SET_ERROR:        // 2046 Can't open shared memory; cannot send request event to server (%lu)
    errCodeString = "CR_SHARED_MEMORY_CONNECT_SET_ERROR";
    break;

  case CR_CONN_UNKNOW_PROTOCOL:                   // 2047 Wrong or unknown protocol
    errCodeString = "CR_CONN_UNKNOW_PROTOCOL";
    break;

  case CR_INVALID_CONN_HANDLE:                    // 2048 Invalid connection handle
    errCodeString = "CR_INVALID_CONN_HANDLE";
    break;

  case CR_SECURE_AUTH:                            // 2049 Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)
    errCodeString = "CR_SECURE_AUTH";
    break;

  case CR_FETCH_CANCELED:                         // 2050 Row retrieval was canceled by mysql_stmt_close() call
    errCodeString = "CR_FETCH_CANCELED";
    break;

  case CR_NO_DATA:                                // 2051 Attempt to read column without prior row fetch
    errCodeString = "CR_NO_DATA";
    break;

  case CR_NO_STMT_METADATA:                       // 2052 Prepared statement contains no metadata
    errCodeString = "CR_NO_STMT_METADATA";
    break;

  //case CR_NO_RESULT_SET:                          // 2053 Attempt to read a row while there is no result set associated with the statement
  //  errCodeString = "CR_NO_RESULT_SET";
  //  break;

  //case CR_NOT_IMPLEMENTED:                        // 2054 This feature is not implemented yet
  //  errCodeString = "CR_NOT_IMPLEMENTED";
  //  break;

  //case CR_SERVER_LOST_EXTENDED:                   // 2055 Lost connection to MySQL server at '%s', system error: %d
  //  errCodeString = "CR_SERVER_LOST_EXTENDED";
  //  break;
  }

  return errCodeString;
}
