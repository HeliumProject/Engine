#include "SQLiteStmt.h"

#include "SQLite.h"

#include "sqlite3.h"

using namespace SQL;

SQLiteStmt::SQLiteStmt()
: Statement()
, m_Stmt( NULL )
, m_LastResultCode( SQLITE_OK )
{

}

SQLiteStmt::SQLiteStmt( const std::string& sql, const std::string& bindFormat )
: Statement( sql, bindFormat )
, m_Stmt( NULL )
, m_LastResultCode( SQLITE_OK )
{

}

SQLiteStmt::~SQLiteStmt()
{

}

bool SQLiteStmt::IsPrepared()
{
  return ( ( m_IsPrepared == true ) && ( sqlite3_expired( m_Stmt ) == 0 ) );
}
