#pragma once

#include "Application/API.h"
#include "Application/SQL/Statement.h"

struct sqlite3_stmt;

namespace SQL
{  
  // The Statement is used to manage statement handles
  class SQLiteStmt : public Statement
  {
  public:
    sqlite3_stmt*  m_Stmt;
    int m_LastResultCode;

  public:
    SQLiteStmt();
    SQLiteStmt( const std::string& sql, const std::string& bindFormat = std::string( "" ) );
    virtual ~SQLiteStmt();

    virtual bool IsPrepared();
  };
}