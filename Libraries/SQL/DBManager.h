#pragma once

#include "API.h"
#include "SQL.h"

#include "Common/Types.h"
#include "Common/Compiler.h"
#include "Console/Console.h"


namespace SQL
{
  /////////////////////////////////////////////////////////////////////////////
  //  Manages connections to a SQLite DB.
  //  The SQLite is an implementation of the DBManager interface.
  //  
  class SQL_API DBManager NOC_ABSTRACT
  {
  public:
    DBManager( const char* friendlyName );
    virtual ~DBManager();

    //virtual void        Open( const std::string& dbPath ) = 0;
    //virtual void        Close() = 0;
    //virtual void        Delete() = 0;
    virtual bool IsConnected() const = 0;

    //
    // Manage Statements
    //

    virtual StmtHandle  CreateStatement( const char* sql, const std::string& bindFormat = std::string("") ) = 0;    
    virtual int         ExecStatement( const StmtHandle handle, ... ) = 0;
    virtual void        BindStatement( const StmtHandle handle, ... ) = 0;
    virtual int         StepStatement( const StmtHandle& handle, const bool autoReset = true ) = 0;
    virtual void        ResetStatement( const StmtHandle& handle ) = 0;
    virtual void        FinalizeStatement( const StmtHandle& handle ) = 0;

    //
    // Transactions
    //

    virtual void        BeginTrans() = 0;
    virtual void        CommitTrans() = 0;
    virtual void        RollbackTrans() = 0;
    virtual bool        IsTransOpen() const { return m_IsTransOpen; }

    //virtual int         ExecSQL( const char* sql ) = 0;
    //virtual int         ExecSQL( const char* sql, sqlite3_callback callback, void *callback_arg = NULL ) = 0;
    //virtual int         ExecSQLVMPrintF( const char* sql, ... ) = 0;
    //virtual int         ExecSQLVMPrintFCallback( const char* sql, sqlite3_callback callback, void *callback_arg, ... ) = 0;

    virtual int         GetColumnCount( const StmtHandle handle ) = 0;
    virtual void        GetColumnName( const StmtHandle handle, const int columnIndex, std::string &name ) = 0;

    virtual void        GetColumnDouble( const StmtHandle handle, const int columnIndex, double &data ) = 0;
    virtual void        GetColumnInt( const StmtHandle handle, const int columnIndex, int &data ) = 0;
    virtual void        GetColumnI64( const StmtHandle handle, const int columnIndex, i64 &data ) = 0;
    virtual void        GetColumnText( const StmtHandle handle, const int columnIndex, std::string &data ) = 0;

    virtual i64         GetLastInsertRowId() = 0;

    virtual int         GetLastErrCode() { return m_LastErrCode; }
    virtual std::string GetLastErrMsg() { return m_LastErrMsg; }

    const std::string&  GetDBFilename() const { return m_DBFilename; }
    
    const char*         GetFriendlyName() const { return m_FriendlyName; }

    //
    // Dump DB
    //

    //virtual void        DumpDB( std::ofstream& outFile, DumpConfig dumpConfig = DumpConfigs::Default );
    //virtual int         DoSchemaDumpQuery( std::ofstream& outFile, DumpConfig dumpConfig = DumpConfigs::Default );
    //virtual int         DumpTable( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig );
    //virtual int         DumpTableData( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig );

  protected:

    //
    // Members
    //

    char          m_FriendlyName[256];
    std::string   m_DBFilename;
    bool          m_IsTransOpen;
    int           m_NumStmtHandles;
    int           m_LastErrCode;
    std::string   m_LastErrMsg;

    //
    // Transaction
    //

    void ThrowIfMaxOpenTrans( const char* function );

    void ThrowIfNoTransOpen( const char* function );

    //
    // Connection
    //

    void ThrowIfDBConnected( const char* function );

    void ThrowIfDBNotConnected( const char* function );

    //
    // Manage Statements
    //

    virtual bool        ValidateBindFormat( const std::string& bindFormat ) = 0;

    virtual StmtHandle  GetNewStamtentHandle() { return ++m_NumStmtHandles; }
    //virtual Statement&  GetColumnStatement( const StmtHandle handle, const int columnIndex ) = 0;
    //virtual Statement&  FindStatement( const StmtHandle handle ) = 0;
    //virtual int         PrepareStatement( Statement& stmt ) = 0;
    //virtual int         StepStatement( Statement& stmt, const bool autoReset = true ) = 0;
    //virtual int         ResetStatement( Statement& stmt ) = 0;
    //virtual int         FinalizeStatement( Statement& stmt ) = 0;
    //virtual void        BindStatement( Statement& stmt, va_list bindArgs ) = 0;

    virtual void        LogPrint( const char* function, Console::Level level = Console::Levels::Verbose, const char* fmt = "", ... );

    virtual void        SetLastError( const char* function, const char* errMsg = "", const char* msgFormat = "", ... );
    virtual const char* GetErrorString() = 0;
    virtual int         GetErrorCode() = 0;
    virtual void        ErrorCodeToString( const int errorCode, std::string& errorString ) = 0;

    //
    // Dump DB
    //

    //virtual int         DumpTableDataQuery( std::ofstream& outFile, const std::string& stmtString, const std::string& tableComment = std::string( "" ) );
    //virtual int         DumpTable( std::ofstream& outFile, const StmtHandle stmt, DumpConfig dumpConfig = DumpConfigs::Default );
    
  };
}