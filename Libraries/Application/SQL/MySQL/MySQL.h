#pragma once

#include "Application/API.h"
#include "Application/SQL/SQLExceptions.h"
#include "Application/SQL/DBManager.h"
#include "MySQLStmt.h"

#include "Platform/Types.h"

#ifndef _mysql_h
#define CR_UNKNOWN_ERROR                          2000 // Unknown MySQL error
#define CR_SOCKET_CREATE_ERROR                    2001 // Can't create UNIX socket (%d)
#define CR_CONNECTION_ERROR                       2002 // Can't connect to local MySQL server through socket '%s' (%d)
#define CR_CONN_HOST_ERROR                        2003 // Can't connect to MySQL server on '%s' (%d)
#define CR_IPSOCK_ERROR                           2004 // Can't create TCP/IP socket (%d)
#define CR_UNKNOWN_HOST                           2005 // Unknown MySQL server host '%s' (%d)
#define CR_SERVER_GONE_ERROR                      2006 // MySQL server has gone away
#define CR_VERSION_ERROR                          2007 // Protocol mismatch; server version = %d, client version = %d
#define CR_OUT_OF_MEMORY                          2008 // MySQL client ran out of memory
#define CR_WRONG_HOST_INFO                        2009 // Wrong host info
#define CR_LOCALHOST_CONNECTION                   2010 // Localhost via UNIX socket
#define CR_TCP_CONNECTION                         2011 // %s via TCP/IP
#define CR_SERVER_HANDSHAKE_ERR                   2012 // Error in server handshake
#define CR_SERVER_LOST                            2013 // Lost connection to MySQL server during query
#define CR_COMMANDS_OUT_OF_SYNC                   2014 // Commands out of sync; you can't run this command now
#define CR_NAMEDPIPE_CONNECTION                   2015 // Named pipe: %s
#define CR_NAMEDPIPEWAIT_ERROR                    2016 // Can't wait for named pipe to host: %s pipe: %s (%lu)
#define CR_NAMEDPIPEOPEN_ERROR                    2017 // Can't open named pipe to host: %s pipe: %s (%lu)
#define CR_NAMEDPIPESETSTATE_ERROR                2018 // Can't set state of named pipe to host: %s pipe: %s (%lu)
#define CR_CANT_READ_CHARSET                      2019 // Can't initialize character set %s (path: %s)
#define CR_NET_PACKET_TOO_LARGE                   2020 // Got packet bigger than 'max_allowed_packet' bytes
#define CR_EMBEDDED_CONNECTION                    2021 // Embedded server
#define CR_PROBE_SLAVE_STATUS                     2022 // Error on SHOW SLAVE STATUS:
#define CR_PROBE_SLAVE_HOSTS                      2023 // Error on SHOW SLAVE HOSTS:
#define CR_PROBE_SLAVE_CONNECT                    2024 // Error connecting to slave:
#define CR_PROBE_MASTER_CONNECT                   2025 // Error connecting to master:
#define CR_SSL_CONNECTION_ERROR                   2026 // SSL connection error
#define CR_MALFORMED_PACKET                       2027 // Malformed packet
#define CR_WRONG_LICENSE                          2028 // This client library is licensed only for use with MySQL servers having '%s' license

/* new 4.1 error codes */
#define CR_NULL_POINTER                           2029 // Invalid use of null pointer
#define CR_NO_PREPARE_STMT                        2030 // Statement not prepared
#define CR_PARAMS_NOT_BOUND                       2031 // No data supplied for parameters in prepared statement
#define CR_DATA_TRUNCATED                         2032 // Data truncated
#define CR_NO_PARAMETERS_EXISTS                   2033 // No parameters exist in the statement
#define CR_INVALID_PARAMETER_NO                   2034 // Invalid parameter number
#define CR_INVALID_BUFFER_USE                     2035 // Can't send long data for non-string/non-binary data types (parameter: %d)
#define CR_UNSUPPORTED_PARAM_TYPE                 2036 // Using unsupported buffer type: %d (parameter: %d)

#define CR_SHARED_MEMORY_CONNECTION               2037 // Shared memory: %s
#define CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR    2038 // Can't open shared memory; client could not create request event (%lu)
#define CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR     2039 // Can't open shared memory; no answer event received from server (%lu)
#define CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR   2040 // Can't open shared memory; server could not allocate file mapping (%lu)
#define CR_SHARED_MEMORY_CONNECT_MAP_ERROR        2041 // Can't open shared memory; server could not get pointer to file mapping (%lu)
#define CR_SHARED_MEMORY_FILE_MAP_ERROR           2042 // Can't open shared memory; client could not allocate file mapping (%lu)
#define CR_SHARED_MEMORY_MAP_ERROR                2043 // Can't open shared memory; client could not get pointer to file mapping (%lu)
#define CR_SHARED_MEMORY_EVENT_ERROR              2044 // Can't open shared memory; client could not create %s event (%lu)
#define CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR  2045 // Can't open shared memory; no answer from server (%lu)
#define CR_SHARED_MEMORY_CONNECT_SET_ERROR        2046 // Can't open shared memory; cannot send request event to server (%lu)
#define CR_CONN_UNKNOW_PROTOCOL                   2047 // Wrong or unknown protocol
#define CR_INVALID_CONN_HANDLE                    2048 // Invalid connection handle
#define CR_SECURE_AUTH                            2049 // Connection using old (pre-4.1.1) authentication protocol refused (client option 'secure_auth' enabled)
#define CR_FETCH_CANCELED                         2050 // Row retrieval was canceled by mysql_stmt_close() call
#define CR_NO_DATA                                2051 // Attempt to read column without prior row fetch
#define CR_NO_STMT_METADATA                       2052 // Prepared statement contains no metadata
#define CR_NO_RESULT_SET                          2053 // Attempt to read a row while there is no result set associated with the statement
#define CR_NOT_IMPLEMENTED                        2054 // This feature is not implemented yet
#define CR_SERVER_LOST_EXTENDED                   2055 // Lost connection to MySQL server at '%s', system error: %d
#endif

#define SQL_MYSQL_DEFAULT_TIMEOUT (30)

typedef struct st_mysql MYSQL;

namespace SQL
{
  /////////////////////////////////////////////////////////////////////////////
  // Manages connections to a MySQL DB.
  // The MySQL is an implementation of the MySQL interface.
  //
  class APPLICATION_API MySQL : public DBManager
  {
  public:
    MySQL( const char* friendlyName );
    virtual ~MySQL( );

    void        Connect( const std::string& host, const std::string& user, const std::string& passwd, const std::string& db, const u32 port, const i32 timeout = SQL_MYSQL_DEFAULT_TIMEOUT );
    //void        Reconnect();
    void        Close();

    virtual bool IsConnected() const NOC_OVERRIDE;

    //
    // Manage Statements
    //

    virtual StmtHandle  CreateStatement( const char* sql, const std::string& bindFormat = std::string("") ) NOC_OVERRIDE;
    virtual int         ExecStatement( const StmtHandle handle, ... ) NOC_OVERRIDE;
    virtual void        BindStatement( const StmtHandle handle, ... ) NOC_OVERRIDE;
    virtual int         StepStatement( const StmtHandle& handle, const bool autoReset = true ) NOC_OVERRIDE;
    virtual void        ResetStatement( const StmtHandle& handle ) NOC_OVERRIDE;
    virtual void        FinalizeStatement( const StmtHandle& handle ) NOC_OVERRIDE;

    //
    // Transactions
    //

    virtual void        BeginTrans() NOC_OVERRIDE;
    virtual void        CommitTrans() NOC_OVERRIDE;
    virtual void        RollbackTrans() NOC_OVERRIDE;

    int                 ExecSQL( const char* sql );
    virtual int         ExecSQLSNPrintF( const char* sql, ... );

    //
    // Transactions
    //

    virtual int         GetColumnCount( const StmtHandle handle ) NOC_OVERRIDE { return 2000; }
    virtual void        GetColumnName( const StmtHandle handle, const int columnIndex, std::string &name ) NOC_OVERRIDE {}

    virtual void        GetColumnDouble( const StmtHandle handle, const int columnIndex, double &data ) NOC_OVERRIDE {}
    virtual void        GetColumnInt( const StmtHandle handle, const int columnIndex, int &data ) NOC_OVERRIDE {}
    virtual void        GetColumnI64( const StmtHandle handle, const int columnIndex, i64 &data ) NOC_OVERRIDE {}
    virtual void        GetColumnText( const StmtHandle handle, const int columnIndex, std::string &data ) NOC_OVERRIDE {}

    virtual i64         GetLastInsertRowId() NOC_OVERRIDE;

    MYSQL*              GetDBHandle() { return m_DB; }


  protected:

    //
    // Members
    //

    MYSQL*        m_DB;

    std::string   m_Hostname;
    std::string   m_Username;
    std::string   m_Password;
    std::string   m_Database;
    u32           m_Port;
    std::string   m_UnixSocket;
    u32           m_ClientFlag;

    typedef std::map< StmtHandle, MySQLStmt > M_MySQLStmt;
    M_MySQLStmt  m_StmtHandles;


    //
    // Manage Statements
    //

    virtual bool ValidateBindFormat( const std::string& bindFormat ) NOC_OVERRIDE;

    //MySQLStmt&  GetColumnStatement( const StmtHandle handle, const int columnIndex ) {}

    MySQLStmt&  FindStatement( const StmtHandle handle );
    int         PrepareStatement( MySQLStmt& stmt );
    int         StepStatement( MySQLStmt& stmt, const bool autoReset = true );
    int         ResetStatement( MySQLStmt& stmt );
    int         FinalizeStatement( MySQLStmt& stmt );
    void        BindStatement( MySQLStmt& stmt, va_list bindArgs );

    virtual const char* GetErrorString() NOC_OVERRIDE;
    virtual int  GetErrorCode() NOC_OVERRIDE;
    virtual void ErrorCodeToString( const int errorCode, std::string& errorString ) NOC_OVERRIDE;

  };
}