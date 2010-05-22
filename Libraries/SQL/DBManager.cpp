#include "DBManager.h"
#include "Exceptions.h"

#include "Console/Console.h"

using namespace SQL;

// Uncomment to turn on verbose DBManager LogPrints
//#define SQL_DBMANAGER_LOGPRINT

const static u32 MAX_PRINT_SIZE = 8192;

static const char* s_SQLErrorMsgFormat = "%s: %s [%s %d] -- %s";


DBManager::DBManager( const char* friendlyName )
: m_IsTransOpen( false )
, m_NumStmtHandles( 0 )
, m_FriendlyName( friendlyName )
{
}

DBManager::~DBManager()
{
}

//////////////////////////////////////////////////////////////////////////////

//void DBManager::ConvertAsteriskToPercent( std::string& argument )
//{
//  // Converts all wildcard asterisks '*' into percent symbols '%' for SQL LIKE expression
//  const boost::regex asterisk("[*]+"); 
//  argument = boost::regex_replace(argument, asterisk, "%"); 
//}
//
//void DBManager::CleanExpressionForSQL( std::string& argument )
//{ 
//  // convert asterisks to percent 
//  const boost::regex asterisk("[*]+"); 
//  argument = boost::regex_replace(argument, asterisk, "%"); 
//
//  // escape underscores
//  const boost::regex underscore("[_]"); 
//  argument = boost::regex_replace(argument, underscore, "@_"); 
//}

void DBManager::ThrowIfDBConnected( const char* function )
{
  if( IsConnected() ) throw SQL::DBManagerException( this, function, "A DB connection is already open!" );
}

void DBManager::ThrowIfDBNotConnected( const char* function )
{
  if( !IsConnected() ) throw SQL::DBManagerException( this, function, "There is no open DB connection!" );
}

void DBManager::ThrowIfMaxOpenTrans( const char* function )
{
  if ( m_IsTransOpen ) throw SQL::DBManagerException( this, function, "Maximum number of transactions exceeded." );
}

void DBManager::ThrowIfNoTransOpen( const char* function )
{
  if ( !m_IsTransOpen ) throw SQL::DBManagerException( this, function, "There is no open transaction!" );
}


/////////////////////////////////////////////////////////////////////////////
void DBManager::LogPrint( const char* function, Console::Level level, const char* fmt, ... )
{
  if ( level == Console::Levels::Extreme )
  {
#ifndef SQL_DBMANAGER_LOGPRINT
    return;
#endif
  }

  static char string[MAX_PRINT_SIZE];

  if ( strcmp( fmt, "" ) != 0 )
  {
    va_list args;
    va_start(args, fmt); 
    
    int size = _vsnprintf(string, sizeof(string), fmt, args);
    NOC_ASSERT(size >= 0);
    va_end(args); 

    string[ sizeof(string) - 1] = 0;

    Console::Print( level, "%s: %s - %s\n", m_FriendlyName, function, string );
  }
  else
  {
    Console::Print( level, "%s: %s\n", m_FriendlyName, function );
  }
}

/////////////////////////////////////////////////////////////////////////////
// Sets m_LastErrMsg with the pretty prints of the last error message,
// along with some additional debuging info in teh following format:
//
// "FriendlyName: <SQL_DB_TYPE> [<SQLITE_ERROR_CODE> <CODE>] -- <SQlite error message>[ in <FunctionName>][; <Additional message>]
//  (DB: <DB NAME>)";
// 
// Example:
//    AssetBuilder::DependenciesCacheDB: sqlite3 [SQLITE_MISUSE 21] -- library routine called out of sequence in ResetStatement;
//    Failed to execute SQL statement: ROLLBACK TRANSACTION;
//
void DBManager::SetLastError( const char* function, const char* errMsg, const char* msgFormat, ... )
{
  //////////////////////////////////////
  // Get the error code
  //
  m_LastErrCode = GetErrorCode();

  std::string errCodeString;
  ErrorCodeToString( m_LastErrCode, errCodeString );

  //////////////////////////////////////
  // Get the error message
  //
  char msgBuffer[Nocturnal::ERROR_STRING_BUF_SIZE];
  if ( strcmp( errMsg, "" ) != 0 )
  {
    _snprintf( msgBuffer, sizeof(msgBuffer), s_SQLErrorMsgFormat, m_FriendlyName, function, errCodeString.c_str(), m_LastErrCode, errMsg ); 
  }
  else
  {
    const char* dbErrMsg = GetErrorString();
    _snprintf( msgBuffer, sizeof(msgBuffer), s_SQLErrorMsgFormat, m_FriendlyName, function, errCodeString.c_str(), m_LastErrCode, dbErrMsg );
  }
  msgBuffer[ sizeof(msgBuffer) - 1] = 0; 
  m_LastErrMsg = msgBuffer;

  //////////////////////////////////////
  // Append any additional messages
  //
  if ( strcmp( msgFormat, "" ) != 0 )
  {
    memset( msgBuffer, '\0', sizeof(msgBuffer)); 

    va_list msgArgs;
    va_start( msgArgs, msgFormat );
    vsnprintf_s( msgBuffer, sizeof(msgBuffer), _TRUNCATE, msgFormat, msgArgs );
    msgBuffer[sizeof(msgBuffer) - 1] = 0; 
    va_end( msgArgs );


    m_LastErrMsg += "; ";
    m_LastErrMsg += msgBuffer;
  }

  // Append the DBFilename to the last error message
  m_LastErrMsg += "(DB: ";
  m_LastErrMsg += m_DBFilename;
  m_LastErrMsg += ")";

  LogPrint( __FUNCTION__, Console::Levels::Verbose, "%s", m_LastErrMsg.c_str() );
}