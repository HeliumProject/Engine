#pragma once

#include "API.h"
#include "Exceptions.h"
#include "DBManager.h"
#include "SQL.h"
#include "SQLiteStmt.h"

#include "Platform/Types.h"


// Each open sqlite database is represented by an instance of the
// following opaque structure.
typedef struct sqlite3 sqlite3;

// The type for a callback function.
typedef int (*sqlite3_callback)(void*,int,char**, char**);

#ifndef _SQLITE3_H_
/*
** CAPI3REF: Flags For File Open Operations {H10230} <H11120> <H12700>
**
** These bit values are intended for use in the
** 3rd parameter to the [sqlite3_open_v2()] interface and
** in the 4th parameter to the xOpen method of the
** [sqlite3_vfs] object.
*/
#define SQLITE_OPEN_READONLY         0x00000001  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_READWRITE        0x00000002  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_CREATE           0x00000004  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_DELETEONCLOSE    0x00000008  /* VFS only */
#define SQLITE_OPEN_EXCLUSIVE        0x00000010  /* VFS only */
#define SQLITE_OPEN_MAIN_DB          0x00000100  /* VFS only */
#define SQLITE_OPEN_TEMP_DB          0x00000200  /* VFS only */
#define SQLITE_OPEN_TRANSIENT_DB     0x00000400  /* VFS only */
#define SQLITE_OPEN_MAIN_JOURNAL     0x00000800  /* VFS only */
#define SQLITE_OPEN_TEMP_JOURNAL     0x00001000  /* VFS only */
#define SQLITE_OPEN_SUBJOURNAL       0x00002000  /* VFS only */
#define SQLITE_OPEN_MASTER_JOURNAL   0x00004000  /* VFS only */
#define SQLITE_OPEN_NOMUTEX          0x00008000  /* Ok for sqlite3_open_v2() */
#define SQLITE_OPEN_FULLMUTEX        0x00010000  /* Ok for sqlite3_open_v2() */

//
// Return values for sqlite3_exec() and sqlite3_step()
//
#define SQLITE_OK           0   // Successful result //
// beginning-of-error-codes //
#define SQLITE_ERROR        1   // SQL error or missing database //
#define SQLITE_INTERNAL     2   // NOT USED. Internal logic error in SQLite //
#define SQLITE_PERM         3   // Access permission denied //
#define SQLITE_ABORT        4   // Callback routine requested an abort //
#define SQLITE_BUSY         5   // The database file is locked //
#define SQLITE_LOCKED       6   // A table in the database is locked //
#define SQLITE_NOMEM        7   // A malloc() failed //
#define SQLITE_READONLY     8   // Attempt to write a readonly database //
#define SQLITE_INTERRUPT    9   // Operation terminated by sqlite3_interrupt()//
#define SQLITE_IOERR       10   // Some kind of disk I/O error occurred //
#define SQLITE_CORRUPT     11   // The database disk image is malformed //
#define SQLITE_NOTFOUND    12   // NOT USED. Table or record not found //
#define SQLITE_FULL        13   // Insertion failed because database is full //
#define SQLITE_CANTOPEN    14   // Unable to open the database file //
#define SQLITE_PROTOCOL    15   // Database lock protocol error //
#define SQLITE_EMPTY       16   // Database is empty //
#define SQLITE_SCHEMA      17   // The database schema changed //
#define SQLITE_TOOBIG      18   // NOT USED. Too much data for one row //
#define SQLITE_CONSTRAINT  19   // Abort due to contraint violation //
#define SQLITE_MISMATCH    20   // Data type mismatch //
#define SQLITE_MISUSE      21   // Library used incorrectly //
#define SQLITE_NOLFS       22   // Uses OS features not supported on host //
#define SQLITE_AUTH        23   // Authorization denied //
#define SQLITE_FORMAT      24   // Auxiliary database format error //
#define SQLITE_RANGE       25   // 2nd parameter to sqlite3_bind out of range //
#define SQLITE_NOTADB      26   // File opened that is not a database file //
#define SQLITE_ROW         100  // sqlite3_step() has another row ready //
#define SQLITE_DONE        101  // sqlite3_step() has finished executing //
// end-of-error-codes //
#endif


namespace SQL
{
  /////////////////////////////////////////////////////////////////////////////
  // Used to dump the database to file
  namespace DumpConfigs
  {
    enum DumpConfig
    {
      Schema            = 1 << 0,
      Data              = 1 << 1,
      CompleteInserts   = 1 << 2,
    };

    static const u32 Default = ( Schema | Data );
  }
  typedef SQL_API u32 DumpConfig;


  /////////////////////////////////////////////////////////////////////////////
  class SQL_API SQLiteString
  {
  public:
    SQLiteString( const char* sql, ... );
    SQLiteString( const char* sql, va_list vmprintfArgs );
    ~SQLiteString();

    const char* GetString() { return m_SqlString; }

  private:
    char* m_SqlString;
  };

  /////////////////////////////////////////////////////////////////////////////
  // Manages connections to a SQLite DB.
  // The SQLite is an implementation of the SQLite interface.
  //
  class SQL_API SQLite : public DBManager
  {
  private:
    //SQLite();
    SQLite( const SQLite& rhs );
    SQLite& operator=( const SQLite& rhs );

  public:
    SQLite( const char* friendlyName );
    virtual ~SQLite( );

    void                Open( const std::string& dbPath, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE );
    void                Close();

    virtual bool IsConnected() const NOC_OVERRIDE { return m_IsDBOpen; }
    
    void                Delete();
    static  void        Delete( const std::string& dbFile );

    static  bool        FromFile( const std::string& sqlFile, const std::string& dbFile, std::string& error, bool failIfExists = false );
    static  bool        ApplyFile( const std::string& sqlFile, const std::string& dbFile, std::string& error );

    //
    // Manage Statements
    //

    virtual StmtHandle  CreateStatement( const char* sql, const std::string& bindFormat = std::string("") ) NOC_OVERRIDE;    
    virtual int         ExecStatement( const StmtHandle handle, ... ) NOC_OVERRIDE; // does the bind, first step and reset
    virtual void        BindStatement( const StmtHandle handle, ... ) NOC_OVERRIDE;
    virtual int         StepStatement( const StmtHandle& handle, const bool autoReset = true ) NOC_OVERRIDE; // autoReset's the statement if the result != SQLITE_ROW
    virtual void        ResetStatement( const StmtHandle& handle ) NOC_OVERRIDE;
    virtual void        FinalizeStatement( const StmtHandle& handle ) NOC_OVERRIDE;

    //
    // Transactions
    //

    virtual void        BeginTrans() NOC_OVERRIDE;
    virtual void        CommitTrans() NOC_OVERRIDE;
    virtual void        RollbackTrans() NOC_OVERRIDE;

    int                 ExecSQL( const char* sql, sqlite3_callback callback = NULL, void* callback_arg = NULL );
    int                 ExecSQLVMPrintF( const char* sql, ... );
    int                 ExecSQLVMPrintFCallback( const char* sql, sqlite3_callback callback, void* callback_arg, ... );

    //
    // Transactions
    //

    virtual int         GetColumnCount( const StmtHandle handle ) NOC_OVERRIDE;
    virtual void        GetColumnName( const StmtHandle handle, const int columnIndex, std::string &name ) NOC_OVERRIDE;

    virtual void        GetColumnDouble( const StmtHandle handle, const int columnIndex, double &data ) NOC_OVERRIDE;
    virtual void        GetColumnInt( const StmtHandle handle, const int columnIndex, int &data ) NOC_OVERRIDE;
    virtual void        GetColumnI64( const StmtHandle handle, const int columnIndex, i64 &data ) NOC_OVERRIDE;
    virtual void        GetColumnText( const StmtHandle handle, const int columnIndex, std::string &data ) NOC_OVERRIDE;

    virtual i64         GetLastInsertRowId() NOC_OVERRIDE;
    bool                IsNotUniqueErr();


    //
    // Dump DB
    //

    void                DumpDB( std::ofstream& outFile, DumpConfig dumpConfig = DumpConfigs::Default );
    int                 DoSchemaDumpQuery( std::ofstream& outFile, DumpConfig dumpConfig = DumpConfigs::Default );
    int                 DumpTable( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig );
    int                 DumpTableData( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig );

  protected:

    //
    // Members
    //

    sqlite3      *m_DB;
    bool          m_IsDBOpen;

    typedef std::map< StmtHandle, SQLiteStmt > M_SQLiteStmt;
    M_SQLiteStmt  m_StmtHandles;


    //
    // Manage Statements
    //

    virtual bool ValidateBindFormat( const std::string& bindFormat ) NOC_OVERRIDE;

    SQLiteStmt& GetColumnStatement( const StmtHandle handle, const int columnIndex );

    SQLiteStmt& FindStatement( const StmtHandle handle );
    int         PrepareStatement( SQLiteStmt& stmt );
    int         StepStatement( SQLiteStmt& stmt, const bool autoReset = true );
    int         ResetStatement( SQLiteStmt& stmt );
    int         FinalizeStatement( SQLiteStmt& stmt );
    void        BindStatement( SQLiteStmt& stmt, va_list bindArgs );

    virtual const char* GetErrorString() NOC_OVERRIDE;
    virtual int  GetErrorCode() NOC_OVERRIDE;
    virtual void ErrorCodeToString( const int errorCode, std::string& errorString ) NOC_OVERRIDE;

    //
    // Dump DB
    //

    int         DumpTableDataQuery( std::ofstream& outFile, const std::string& stmtString, const std::string& tableComment = std::string( "" ) );
    int         DumpTable( std::ofstream& outFile, const StmtHandle stmt, DumpConfig dumpConfig = DumpConfigs::Default );
  };
}