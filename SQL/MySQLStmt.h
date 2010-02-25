#pragma once

#include "API.h"
#include "Statement.h"

#include "Common/Types.h"

//predeclare some mysql
typedef struct st_mysql_stmt MYSQL_STMT;
typedef struct st_mysql_bind MYSQL_BIND;

namespace SQL
{  
  union U_DBRowType
  {
    i8      _tinyint;   // c - signed char   MYSQL_TYPE_TINY        TINYINT     1
    i16     _i16;       // s - short int     MYSQL_TYPE_SHORT       SMALLINT    2
    i32     _int;       // i - int           MYSQL_TYPE_LONG        INT         4
    i64     _i64;       // l - long long int MYSQL_TYPE_LONGLONG    BIGINT      8
    f32     _float;     // f - float         MYSQL_TYPE_FLOAT       FLOAT       4
    double  _double;    // d - double        MYSQL_TYPE_DOUBLE      DOUBLE      8

    //char*   _date;      // ? - MYSQL_TIME    MYSQL_TYPE_DATE        DATE        sizeof(MYSQL_TIME)
    //char*   _datetime;  // ? - MYSQL_TIME    MYSQL_TYPE_DATETIME    DATETIME    sizeof(MYSQL_TIME)
    //char*   _time;      // ? - MYSQL_TIME    MYSQL_TYPE_TIME        TIME        sizeof(MYSQL_TIME)
    //char*   _timestamp; // ? - MYSQL_TIME    MYSQL_TYPE_TIMESTAMP   TIMESTAMP   sizeof(MYSQL_TIME)

    const char*   _text;      // t - char[]        MYSQL_TYPE_STRING      TEXT        data_length         (TEXT, CHAR, VARCHAR; for non-binary data)
    const void*   _blob;      // b - char[]        MYSQL_TYPE_BLOB        BLOB        data_length         (BLOB, BINARY, VARBINARY; for binary data)
    int     _null;      // n -               MYSQL_TYPE_NULL        NULL

  };

  // The Statement is used to manage statement handles
  class MySQLStmt : public Statement
  {
  public:
    MYSQL_STMT*  m_Stmt;

    u32          m_InputBindCount;
    MYSQL_BIND*  m_InputBinds;
    U_DBRowType* m_InputBindables;
    u32*         m_InputBindLengths;

    u32          m_OutputBindCount;
    MYSQL_BIND*  m_OutputBinds;
    U_DBRowType* m_OutputBindables;
    u32*         m_OutputBindLengths;

    std::vector< std::string > m_Strings;

    MySQLStmt()
      : Statement()
      , m_Stmt( NULL )
      , m_InputBindCount( 0 )
      , m_InputBinds( NULL )
      , m_InputBindables( NULL )
      , m_InputBindLengths( NULL )
      , m_OutputBindCount( 0 )
      , m_OutputBinds( NULL )
      , m_OutputBindables( NULL )
      , m_OutputBindLengths( NULL )
    {}

    MySQLStmt( const std::string& sql, const std::string& bindFormat = std::string( "" ) )
      : Statement( sql, bindFormat )
      , m_Stmt( NULL )
      , m_InputBindCount( 0 )
      , m_InputBinds( NULL )
      , m_InputBindables( NULL )
      , m_InputBindLengths( NULL )
      , m_OutputBindCount( 0 )
      , m_OutputBinds( NULL )
      , m_OutputBindables( NULL )
      , m_OutputBindLengths( NULL )
    {}
    
    virtual ~MySQLStmt();

    virtual bool IsPrepared() { return true; } //( ( m_IsPrepared == true ) && ( sqlite3_expired( m_Stmt ) == 0 ) ); }
  };
}
