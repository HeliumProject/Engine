#include "Platform/Windows/Windows.h"
#include "SQLiteStmt.h"
#include "SQLite.h"

#include "Foundation/Log.h"
#include "FileSystem/FileSystem.h"
#include "Foundation/Exception.h"
#include "Platform/Process.h"
#include <boost/regex.hpp>

#include "sqlite3.h"

#include <string>
#include <sstream>
#include <vector>

using namespace SQL;

//
// Statics
//

// Used in sqlite3_busy_timeout
// After BUSY_TIMEOUT_MS milliseconds (5 min) of sleeping, the handler returns 0 which causes sqlite3_exec() to return SQLITE_BUSY.
#define BUSY_TIMEOUT_MS 300000

static const char* s_BeginTransaction    = "BEGIN TRANSACTION;";
static const char* s_CommitTransaction   = "COMMIT TRANSACTION;";
static const char* s_RollbackTransaction = "ROLLBACK TRANSACTION;";

// http://www.sqlite.org/pragma.html#modify
static const char* s_Pragmas = 
"PRAGMA synchronous=OFF; \
PRAGMA temp_store=MEMORY;";

// determine if an occured because the last inset/update was not unique
static const char* s_NotUniqueErrMsg = " not unique";

// Dump Table Data SQL
static const char* s_TableDumpSQL = "SELECT sql FROM sqlite_master WHERE sql NOT NULL AND type IN ('index','trigger','view')";
static const char* s_DBSchemaDumpSQL = "SELECT name, type, sql FROM sqlite_master WHERE sql NOT NULL AND type=='table'";
static const char* s_TableSchemaDumpSQL = "SELECT name, type, sql FROM sqlite_master WHERE sql NOT NULL AND type=='table' AND name==? LIMIT 1";
static const char* s_InsertMasterSQL = "INSERT INTO sqlite_master (type,name,tbl_name,rootpage,sql) VALUES('table','%q','%q',0,'%q');";



/////////////////////////////////////////////////////////////////////////////
/// class SQLiteString
/////////////////////////////////////////////////////////////////////////////
SQLiteString::SQLiteString( const char* sql, ... )
: m_SqlString( NULL )
{
    va_list vmprintfArgs;
    va_start( vmprintfArgs, sql );

    m_SqlString = sqlite3_vmprintf( sql, vmprintfArgs );

    va_end( vmprintfArgs );
}

SQLiteString::SQLiteString( const char* sql, va_list vmprintfArgs )
: m_SqlString( NULL )
{
    m_SqlString = sqlite3_vmprintf( sql, vmprintfArgs );
}

SQLiteString::~SQLiteString()
{
    if ( m_SqlString )
    {
        sqlite3_free( m_SqlString );
        m_SqlString = NULL;
    }
}


/////////////////////////////////////////////////////////////////////////////
/// class SQLite
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// Default constructor
//
SQLite::SQLite( const char* friendlyName )
: DBManager( friendlyName )
, m_IsDBOpen( false )
{
    Log::Debug( Log::Levels::Verbose, "SQLite ThreadSafe: %d\n", sqlite3_threadsafe() );
}


/////////////////////////////////////////////////////////////////////////////
// Default destructor
SQLite::~SQLite( )
{
    Close();
}


/////////////////////////////////////////////////////////////////////////////
// Opens the DB and prepares all of the statements.
//
void SQLite::Open( const std::string& dbPath, int flags )
{
    Profile::Timer timer;

    LogPrint( __FUNCTION__, Log::Levels::Verbose, "Connection settings: %s", dbPath.c_str() );

    ThrowIfDBConnected( __FUNCTION__ );

    NOC_ASSERT( !dbPath.empty() && !IsConnected() );
    m_DBFilename = dbPath;

    if ( m_DBFilename.empty() )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "No database file path was specified." );
    }

    FileSystem::MakePath( m_DBFilename, true );

    std::string dbWin32Name = m_DBFilename;
    FileSystem::Win32Name( dbWin32Name );
    if ( sqlite3_open_v2( m_DBFilename.c_str(), &m_DB, flags, 0 ) != SQLITE_OK )
    {
        SetLastError( __FUNCTION__, "", "Could not connect to SQLite DB file: \"%s\".", m_DBFilename.c_str() );
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    m_IsDBOpen = true;

    sqlite3_busy_timeout( m_DB, BUSY_TIMEOUT_MS );

    // http://www.sqlite.org/pragma.html#modify
    if ( ExecSQL( s_Pragmas ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    LogPrint( __FUNCTION__, Log::Levels::Verbose, "DB opened in %.2fms: %s", timer.Elapsed(), m_DBFilename.c_str() );
}



/////////////////////////////////////////////////////////////////////////////
// Closes the DB and cleans up all statement handles
//
void SQLite::Close()
{
    Profile::Timer timer;

    if( !m_IsDBOpen )
    {
        return;
    }

    LogPrint( __FUNCTION__, Log::Levels::Verbose );

    if ( m_IsTransOpen )
        RollbackTrans();

    // Finalize all open statements
    M_SQLiteStmt::iterator itStmtHandles = m_StmtHandles.begin();
    M_SQLiteStmt::iterator itEndStmtHandles = m_StmtHandles.end();
    for ( ; itStmtHandles != itEndStmtHandles ; ++itStmtHandles )
    {
        if ( FinalizeStatement( (SQLiteStmt) (*itStmtHandles).second ) != SQLITE_OK )
        {
            throw SQL::DBManagerException( this, __FUNCTION__ );
        }
    }

    if ( sqlite3_close( m_DB ) != SQLITE_OK )
    {
        SetLastError( __FUNCTION__, "", "Failed to close the DB connection \"%s\".", m_DBFilename.c_str() );
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
    m_IsDBOpen = false;

    // Erase all the list of stmt handles:
    // - We don't erase them as they are finalized because that runs in linear time
    //   per call, better to erase them all at once.
    // - Also this is done AFTER sqlite3_close to ensure that the handles remain if
    //   the above call should fail.
    m_StmtHandles.clear();
    m_NumStmtHandles = 0;

    LogPrint( __FUNCTION__, Log::Levels::Verbose, "DB closed in %.2fms: %s", timer.Elapsed(), m_DBFilename.c_str() );
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the  DB and handled events list
void SQLite::Delete()
{
    Close();

    std::string win32Name;
    FileSystem::Win32Name( m_DBFilename, win32Name );

    if ( FileSystem::Exists( m_DBFilename )
        && !DeleteFile( win32Name.c_str() ) )
    {
        throw Nocturnal::PlatformException( "Could not delete database file %s, Error: %s",
            m_DBFilename.c_str(),
            Platform::GetErrorString().c_str() );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Deletes the  DB and handled events list
void SQLite::Delete( const std::string& dbFile )
{  
    std::string win32Name;
    FileSystem::Win32Name( dbFile, win32Name );

    if ( FileSystem::Exists( dbFile )
        && !::DeleteFile( win32Name.c_str() ) )
    {
        throw Nocturnal::PlatformException( "Could not delete database file %s, Error: %s",
            dbFile.c_str(),
            Platform::GetErrorString().c_str() );
    }
}

/////////////////////////////////////////////////////////////////////////////
// Loads the DB from the given file path
//
bool SQLite::FromFile( const std::string& sqlFile, const std::string& dbFile, std::string& error, bool failIfExists )
{  
    bool result = false;

    if ( sqlFile.empty() || dbFile.empty() )
    {
        error = std::string( "Could not create DB from file, sqlFile and/or dbFile are invalid path(s)" );
        return result;
    }

    if ( FileSystem::Exists( dbFile ) )
    {
        if ( failIfExists )
        {
            error = std::string( "Could not create DB, DB file already exists: " ) +  dbFile ;
            return result;
        }

        Delete( dbFile );
    }

    if ( !FileSystem::Exists( sqlFile ) )
    {
        error = std::string( "DB create script does not exists: " ) +  sqlFile ;
        return result;
    }

    FileSystem::MakePath( dbFile, true );

    std::string sqliteCmd = std::string ("sqlite3 -init \"") + sqlFile + std::string ("\" \"") + dbFile + std::string ("\" .quit");
    result = ( Platform::Execute( sqliteCmd, false, true ) == 0 );

    if ( !result || !FileSystem::Exists( dbFile ) )
    {
        error = std::string( "Could not create DB from file: " ) +  sqlFile ;
        return false;
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////
// Applies SQL to the database from a file or string
bool SQLite::ApplyFile( const std::string& sqlFile, const std::string& dbFile, std::string& error )
{   
    bool result = false;

    if ( sqlFile.empty() || dbFile.empty() )
    {
        error = std::string( "Could not apply sql file to DB, sqlFile and/or dbFile are invalid path(s)" );
        return result;
    }

    if ( !FileSystem::Exists( sqlFile ) )
    {
        error = std::string( "SQL file does not exist: " ) + sqlFile;
        return result;
    }

    char tempDir[ MAX_PATH ];
    DWORD tempPathResult = GetTempPath( MAX_PATH, tempDir );
    if ( tempPathResult == 0 || tempPathResult > MAX_PATH )
    {
        throw Nocturnal::Exception( "Unable to get the temporary directory on this computer." );
    }

    std::string tempBat = tempDir;
    FileSystem::MakePath( tempBat );

    FileSystem::AppendPath( tempBat, "tempSQL.bat" );
    FileSystem::Win32Name( tempBat );

    std::ofstream tempSQLFile( tempBat.c_str(), std::ios::out | std::ios::trunc );
    if ( !tempSQLFile.is_open() )
    {
        error = std::string( "Could not open file temporary batch file for writing: " ) + tempBat;
        return result;
    }

    tempSQLFile << "@echo off\n";
    tempSQLFile << "echo Applying sql file to database: " << sqlFile << "...\n";
    tempSQLFile << "call sqlite3 \"";
    tempSQLFile << dbFile;
    tempSQLFile << "\" < \"";
    tempSQLFile << sqlFile;
    tempSQLFile << "\"";
    tempSQLFile.close();

    result = ( Platform::Execute( tempBat, false, true ) == 0 );

    if ( !result )
    {
        error = std::string( "Could not apply sql file: " ) + sqlFile;
    }

    ::DeleteFile( tempBat.c_str() );

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// Execute a SQL statement on the DB and handle the errors
//
int SQLite::ExecSQL( const char* sql, sqlite3_callback callback, void *callbacs_arg )
{
    LogPrint( __FUNCTION__, Log::Levels::Extreme, "SQL: %s", sql );

    ThrowIfDBNotConnected( __FUNCTION__ );

    char* sqlErr;
    int result = sqlite3_exec( m_DB, sql, callback, callbacs_arg, &sqlErr );

    if ( sqlErr != NULL )
    {
        SetLastError( __FUNCTION__, sqlErr, "Failed to execute SQL statement: \"%s\".", sql );
    }

    sqlite3_free( sqlErr );

    return result;
}

/////////////////////////////////////////////////////////////////////////////
// http://www.sqlite.org/capi3ref.html#sqlite3_last_insert_rowid
i64 SQLite::GetLastInsertRowId()
{
    i64 lastRowId = ( i64 ) sqlite3_last_insert_rowid( m_DB );

    if ( lastRowId <= 0 )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "No rows were inserted." );
    }
    return lastRowId;
}

/////////////////////////////////////////////////////////////////////////////
// Uses sqlite3_vmprintf to populate the given sql string with variable
// arguments; then calls ExecSQL.
//
int SQLite::ExecSQLVMPrintFCallback( const char* sql, sqlite3_callback callback, void *callbacs_arg, ... )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    va_list vmprintfArgs;
    va_start( vmprintfArgs, sql );

    SQLiteString sqlString( sql, vmprintfArgs );

    va_end( vmprintfArgs );

    int result = SQLITE_ERROR;
    try
    {
        result = ExecSQL( sqlString.GetString(), callback, callbacs_arg );
    }
    catch( ... )
    {
        throw;
    }

    return result;
}



/////////////////////////////////////////////////////////////////////////////
// Uses sqlite3_vmprintf to populate the given sql string with variable
// arguments; then calls ExecSQL.
//
// NOTE: The only difference between ExecSQLVMPrintF 
//   and ExecSQLVMPrintFCallback, is that ExecSQLVMPrintFCallback takes two 
//   additional params - sqlite3_callback callback and void *callbacs_arg - which
//   are passed along to ExecSQL. The functions were not abstracted to save
//   time/memory.
// 
int SQLite::ExecSQLVMPrintF( const char* sql, ... )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    va_list vmprintfArgs;
    va_start( vmprintfArgs, sql );

    SQLiteString sqlString( sql, vmprintfArgs );

    va_end( vmprintfArgs );

    int result = SQLITE_ERROR;
    try
    {
        result = ExecSQL( sqlString.GetString() );
    }
    catch( ... )
    {
        throw;
    }

    return result;
}


/////////////////////////////////////////////////////////////////////////////
// Begins a new transaction; this wrapper ensures that there is only one
// open transaction at all times.
//
void SQLite::BeginTrans()
{
    LogPrint( __FUNCTION__, Log::Levels::Verbose );

    ThrowIfMaxOpenTrans( __FUNCTION__ );

    if ( ExecSQL( s_BeginTransaction ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    m_IsTransOpen = true;
}



/////////////////////////////////////////////////////////////////////////////
// Commits the existing transaction; this wrapper ensures that there is
// only one open transaction at all times.
//
void SQLite::CommitTrans()
{
    LogPrint( __FUNCTION__, Log::Levels::Verbose );

    ThrowIfNoTransOpen( __FUNCTION__ );

    if ( ExecSQL( s_CommitTransaction ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    m_IsTransOpen = false;
}



/////////////////////////////////////////////////////////////////////////////
// RollBack the existing transaction; this wrapper ensures that there is
// only one open transaction at all times.
//
void SQLite::RollbackTrans()
{
    LogPrint( __FUNCTION__, Log::Levels::Verbose );

    ThrowIfNoTransOpen( __FUNCTION__ );

    if ( ExecSQL( s_RollbackTransaction ) != SQLITE_OK )
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
//  b = blob
//  d = double
//  i = int
//  l = int64
//  n = null
//  t = text
//  T = text16
//
bool SQLite::ValidateBindFormat( const std::string& bindFormat )
{
    if ( !bindFormat.empty() )
    {
        // validate that bindFormat only contains letters "bdilntT"
        // Pattern used to validate statement bind types string
        const boost::regex s_AcceptedBindTypes( "^[bdilntT]*$" );

        boost::smatch resultBindFormat; 
        if( !boost::regex_match(bindFormat, resultBindFormat, s_AcceptedBindTypes))
        {
            return false;
        }
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////
// Creates a SQLite statement handle.
StmtHandle SQLite::CreateStatement( const char* sql, const std::string &bindFormat )
{
    LogPrint( __FUNCTION__, Log::Levels::Extreme, "Bind format: \"%s\";\nSQL: %s", bindFormat.c_str(), sql );

    ThrowIfDBNotConnected( __FUNCTION__ );

    StmtHandle handle = SQL::NullStatement;
    SQLiteStmt stmt( sql, bindFormat );

    if ( !ValidateBindFormat( bindFormat ) )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "Invalid bind format (%s), bindFormat may only contain letters \"bdilntT\". Failed to create statement: \"%s\"",
            bindFormat.c_str(),
            stmt.m_SQL.c_str() );
    }

    // Prepare the statement
    if ( PrepareStatement( stmt ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    // Validate the bindFormat string length
    int bindParamCount = sqlite3_bind_parameter_count( stmt.m_Stmt );
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
SQLiteStmt& SQLite::FindStatement( const StmtHandle handle )
{
    M_SQLiteStmt::iterator it = m_StmtHandles.find( handle );
    if ( it == m_StmtHandles.end() )
    {
        throw SQL::StmtHandleNotFoundException( this, __FUNCTION__ );
    }

    return (*it).second;
}

/////////////////////////////////////////////////////////////////////////////
int SQLite::PrepareStatement( SQLiteStmt& stmt )
{
    int prepareResult = sqlite3_prepare_v2( m_DB, stmt.m_SQL.c_str(), -1, &stmt.m_Stmt, NULL );

    if ( prepareResult != SQLITE_OK )
    {
        SetLastError( __FUNCTION__, "", "Call to sqlite3_prepare_v2 failed. Failed to prepare statement: \"%s\".", stmt.m_SQL.c_str() );
    }
    else
    {
        stmt.m_IsPrepared = true;
    }

    stmt.m_LastResultCode = prepareResult;
    return prepareResult;
}

/////////////////////////////////////////////////////////////////////////////
void SQLite::FinalizeStatement( const StmtHandle &handle )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    if ( FinalizeStatement( stmt ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
}


/////////////////////////////////////////////////////////////////////////////
int SQLite::FinalizeStatement( SQLiteStmt& stmt )
{
    int finalizeResult = SQLITE_OK;
    if ( stmt.m_IsPrepared )
    {
        finalizeResult = sqlite3_finalize( stmt.m_Stmt );

        if ( finalizeResult != SQLITE_OK )
        {
            SetLastError( __FUNCTION__, "", "Failed to finalize statement: \"%s\".", stmt.m_SQL );
        }
    }

    stmt.m_IsPrepared = false;

    stmt.m_LastResultCode = finalizeResult;
    return finalizeResult;
}

/////////////////////////////////////////////////////////////////////////////
// Locate and step the given statement handle.
int SQLite::StepStatement( const StmtHandle& handle, const bool autoReset )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    return StepStatement( stmt, autoReset );
}

/////////////////////////////////////////////////////////////////////////////
// Step the given statement handle.
int SQLite::StepStatement( SQLiteStmt& stmt, const bool autoReset )
{
    LogPrint( __FUNCTION__, Log::Levels::Extreme, "SQL: %s", stmt.m_SQL.c_str() );

    // make sure it's prepared and does not need to be recompiled
    if ( !stmt.IsPrepared() )
    {
        SetLastError( __FUNCTION__, "", "Cannot step uncompiled statement: \"%s\".", stmt.m_SQL );
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    // Step the statement handle
    // See: http://www.sqlite.org/capi3ref.html#sqlite3_step
    int stepResult = sqlite3_step( stmt.m_Stmt );

    // autoReset's the statement if the result != SQLITE_ROW
    if ( autoReset && stepResult != SQLITE_ROW )
    {

        ResetStatement( stmt );
    }

    stmt.m_LastResultCode = stepResult;
    return stepResult;
}

/////////////////////////////////////////////////////////////////////////////
// Resets the statement handle
//
void SQLite::ResetStatement( const StmtHandle& handle )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    // make sure it's prepared and does not need to be recompiled
    if ( !stmt.IsPrepared() )
    {
        SetLastError( __FUNCTION__, "", "Cannot reset uncompiled statement: \"%s\".", stmt.m_SQL );
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    if ( ResetStatement( stmt ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
}


/////////////////////////////////////////////////////////////////////////////
int SQLite::ResetStatement( SQLiteStmt& stmt )
{
    int resetResult = sqlite3_reset( stmt.m_Stmt );

    if ( resetResult != SQLITE_OK )
    {
        SetLastError( __FUNCTION__, "", "Reset statement failed for: \"%s\".", stmt.m_SQL );
    }

    stmt.m_IsBound = false;

    stmt.m_LastResultCode = resetResult;
    return resetResult;
}


/////////////////////////////////////////////////////////////////////////////
// Binds a list of arguments to the given sqlite statement using
// sqlite3_bind* functions.
//
void SQLite::BindStatement( const StmtHandle handle, ... )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    // Prepare the stmt if it: is not prepared, or has expired
    if ( !stmt.IsPrepared() && PrepareStatement( stmt ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    va_list bindArgs;
    va_start( bindArgs, handle );
    BindStatement( stmt, bindArgs );
    va_end( bindArgs );
}


/////////////////////////////////////////////////////////////////////////////
void SQLite::BindStatement( SQLiteStmt& stmt, va_list bindArgs )
{
    // Step through the arguments list, while isSQLiteOK is true
    if ( stmt.m_BindFormat.length() > 0 )
    {
        int numBindArgs = ( int ) stmt.m_BindFormat.length();

        bool isSQLiteOK = true;

        int index = 0;
        for (  ; isSQLiteOK && index < numBindArgs && stmt.m_BindFormat[index] != '\0' ; ++index )
        {
            /// Contains all posible datatypes that SQLite rows can return
            union U_DBRowType
            {
                void*          _blob;   // b - blob
                double         _double; // d - double
                int            _int;    // i - int
                i64            _i64;    // l - i64
                int            _null;   // n - null
                char*          _text;   // t - text
                void*          _text16; // T - text16
            } Bindable;

            switch( stmt.m_BindFormat[index] )
            {
            default:
                throw SQL::DBManagerException( this, __FUNCTION__, "Bad input specified for BindStatement." );
                break;

            case 'b': // _blob
                Bindable._blob = va_arg( bindArgs, void* );
                isSQLiteOK &= ( sqlite3_bind_blob( stmt.m_Stmt, ( index + 1 ), Bindable._blob, -1, SQLITE_TRANSIENT ) == SQLITE_OK );
                break;

            case 'd': // _double
                Bindable._double = va_arg( bindArgs, double );
                isSQLiteOK &= ( sqlite3_bind_double( stmt.m_Stmt, ( index + 1 ), Bindable._double ) == SQLITE_OK );
                break;

            case 'i': // _int
                Bindable._int = va_arg( bindArgs, int );
                isSQLiteOK &= ( sqlite3_bind_int( stmt.m_Stmt, ( index + 1 ), Bindable._int ) == SQLITE_OK );
                break;

            case 'l': // _i64
                Bindable._i64 = va_arg( bindArgs, i64 );
                isSQLiteOK &= ( sqlite3_bind_int64( stmt.m_Stmt, ( index + 1 ), Bindable._i64 ) == SQLITE_OK );
                break;

            case 'n': // null
                isSQLiteOK &= ( sqlite3_bind_null( stmt.m_Stmt, ( index + 1 ) ) == SQLITE_OK );
                break;

            case 't': // _text
                Bindable._text = va_arg( bindArgs, char* );
                isSQLiteOK &= ( sqlite3_bind_text( stmt.m_Stmt, ( index + 1 ), Bindable._text, -1, SQLITE_TRANSIENT ) == SQLITE_OK );
                break;

            case 'T': // _text16
                Bindable._text16 = va_arg( bindArgs, void* );
                isSQLiteOK &= ( sqlite3_bind_text16( stmt.m_Stmt, ( index + 1 ), Bindable._text16, -1, SQLITE_TRANSIENT ) == SQLITE_OK );
                break;
            }
        }

        if ( !isSQLiteOK )
        {
            SetLastError( __FUNCTION__, "", "Failed to bind argument (index %d in bind format %s) for statement: \"%s\".", index, stmt.m_BindFormat.c_str(), stmt.m_SQL );
            throw SQL::DBManagerException( this, __FUNCTION__ );
        }

        stmt.m_IsBound = true;
        stmt.m_LastResultCode = SQLITE_OK;
    }

}

/////////////////////////////////////////////////////////////////////////////
int SQLite::ExecStatement( const StmtHandle handle, ... )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    // Prepare the stmt if it: is not prepared, or has expired
    if ( !stmt.IsPrepared() && PrepareStatement( stmt ) != SQLITE_OK )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }

    if ( !stmt.m_IsBound )
    {
        va_list bindArgs;
        va_start( bindArgs, handle );
        BindStatement( stmt, bindArgs );
        va_end( bindArgs );
    }

    return StepStatement( stmt );
}

/////////////////////////////////////////////////////////////////////////////
// Helper funciton for GetColumn* functions: Finds the statement by the
// given handle and validates the requested columnIndex.
//
SQLiteStmt& SQLite::GetColumnStatement( const StmtHandle handle, const int columnIndex )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    // make sure it's already prepared
    if ( !stmt.IsPrepared() )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "Could not get column data from unprepared statement: \"%s\".", stmt.m_SQL );
    }

    // make sure it's got a ROW
    if ( stmt.m_LastResultCode != SQLITE_ROW )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "No rows were returned for statement: \"%s\".", stmt.m_SQL );
    }

    // they are requesting a column not in the result set
    if ( columnIndex >= sqlite3_column_count( stmt.m_Stmt ) )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "Could not get column data, column index [%d] out of bounds for statement: \"%s\".", columnIndex, stmt.m_SQL );
    }

    return stmt;
}

/////////////////////////////////////////////////////////////////////////////
// Return the number of columns in the result set returned by the prepared
// SQL statement. This routine returns 0 if pStmt is an SQL statement that
// does not return data (for example an UPDATE).
//
int SQLite::GetColumnCount( const StmtHandle handle )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    // Locate the handle
    SQLiteStmt& stmt = FindStatement( handle );

    // make sure it's already prepared
    if ( !stmt.IsPrepared() )
    {
        throw SQL::DBManagerException( this, __FUNCTION__, "Cannot get column data from unprepared statement!" );
    }

    int colCount = ( int ) sqlite3_column_count( stmt.m_Stmt );
    return colCount;
}

/////////////////////////////////////////////////////////////////////////////
// 
void SQLite::GetColumnName( const StmtHandle handle, const int columnIndex, std::string &name )
{
    ThrowIfDBNotConnected( __FUNCTION__ );

    SQLiteStmt& stmt = GetColumnStatement( handle, columnIndex );

    char* tmpData = (char*) sqlite3_column_name( stmt.m_Stmt, columnIndex );
    if ( tmpData )
    {
        name = tmpData;
    }
    else
    {
        name.clear();
    }
}


/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as double.
//
void SQLite::GetColumnDouble( const StmtHandle handle, const int columnIndex, double &data )
{
    SQLiteStmt& stmt = GetColumnStatement( handle, columnIndex );

    data = ( double ) sqlite3_column_double( stmt.m_Stmt, columnIndex );
}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as int.
//
void SQLite::GetColumnInt( const StmtHandle handle, const int columnIndex, int &data )
{
    SQLiteStmt& stmt = GetColumnStatement( handle, columnIndex );

    data = ( int ) sqlite3_column_int( stmt.m_Stmt, columnIndex );
}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as i64.
//
void SQLite::GetColumnI64( const StmtHandle handle, const int columnIndex, i64 &data )
{
    SQLiteStmt& stmt = GetColumnStatement( handle, columnIndex );

    data = ( i64 ) sqlite3_column_int64( stmt.m_Stmt, columnIndex );
}

/////////////////////////////////////////////////////////////////////////////
// GetColumn* functions return the contents of a given column. 
// This funciton is overloaded to accept void, double, int, i64, and char data
// pointers which are then cast as std::string.
//
//void SQLite::GetColumnText( const StmtHandle handle, const int columnIndex, char* *data )
void SQLite::GetColumnText( const StmtHandle handle, const int columnIndex, std::string &data )
{
    SQLiteStmt& stmt = GetColumnStatement( handle, columnIndex );

    char* tmpData = (char*)sqlite3_column_text( stmt.m_Stmt, columnIndex );
    if ( tmpData )
    {
        data = tmpData;
    }
    else
    {
        data.clear();
    }
}


/////////////////////////////////////////////////////////////////////////////
bool SQLite::IsNotUniqueErr()
{
    return ( ( m_LastErrCode == SQLITE_CONSTRAINT ) &&
        ( strstr( m_LastErrMsg.c_str(), s_NotUniqueErrMsg ) != NULL ) );
}

/////////////////////////////////////////////////////////////////////////////
const char* SQLite::GetErrorString()
{
    return sqlite3_errmsg( m_DB );
}

/////////////////////////////////////////////////////////////////////////////
int SQLite::GetErrorCode()
{
    return sqlite3_errcode( m_DB );
}

/////////////////////////////////////////////////////////////////////////////
void SQLite::ErrorCodeToString( const int errorCode, std::string& errorString )
{
    switch( errorCode )
    {
    default:
        break;

    case SQLITE_OK           :   /* 0   Successful result */
        errorString = "SQLITE_OK";
        break;

    case SQLITE_ERROR        :   /* 1   SQL error or missing database */
        errorString = "SQLITE_ERROR";
        break;

    case SQLITE_INTERNAL     :   /* 2   An internal logic error in SQLite */
        errorString = "SQLITE_INTERNAL";
        break;

    case SQLITE_PERM         :   /* 3   Access permission denied */
        errorString = "SQLITE_PERM";
        break;

    case SQLITE_ABORT        :   /* 4   Callback routine requested an abort */
        errorString = "SQLITE_ABORT";
        break;

    case SQLITE_BUSY         :   /* 5   The database file is locked */
        errorString = "SQLITE_BUSY";
        break;

    case SQLITE_LOCKED       :   /* 6   A table in the database is locked */
        errorString = "SQLITE_LOCKED";
        break;

    case SQLITE_NOMEM        :   /* 7   A malloc() failed */
        errorString = "SQLITE_NOMEM";
        break;

    case SQLITE_READONLY     :   /* 8   Attempt to write a readonly database */
        errorString = "SQLITE_READONLY";
        break;

    case SQLITE_INTERRUPT    :   /* 9   Operation terminated by sqlite_interrupt() */
        errorString = "SQLITE_INTERRUPT";
        break;

    case SQLITE_IOERR        :   /* 10  Some kind of disk I/O error occurred */
        errorString = "SQLITE_IOERR";
        break;

    case SQLITE_CORRUPT      :   /* 11  The database disk image is malformed */
        errorString = "SQLITE_CORRUPT";
        break;

    case SQLITE_NOTFOUND     :   /* 12  (Internal Only) Table or record not found */
        errorString = "SQLITE_NOTFOUND";
        break;

    case SQLITE_FULL         :   /* 13  Insertion failed because database is full */
        errorString = "SQLITE_FULL";
        break;

    case SQLITE_CANTOPEN     :   /* 14  Unable to open the database file */
        errorString = "SQLITE_CANTOPEN";
        break;

    case SQLITE_PROTOCOL     :   /* 15  Database lock protocol error */
        errorString = "SQLITE_PROTOCOL";
        break;

    case SQLITE_EMPTY        :   /* 16  (Internal Only) Database table is empty */
        errorString = "SQLITE_EMPTY";
        break;

    case SQLITE_SCHEMA       :   /* 17  The database schema changed */
        errorString = "SQLITE_SCHEMA";
        break;

    case SQLITE_TOOBIG       :   /* 18  Too much data for one row of a table */
        errorString = "SQLITE_TOOBIG";
        break;

    case SQLITE_CONSTRAINT   :   /* 19  Abort due to constraint violation */
        errorString = "SQLITE_CONSTRAINT";
        break;

    case SQLITE_MISMATCH     :   /* 20  Data type mismatch */
        errorString = "SQLITE_MISMATCH";
        break;

    case SQLITE_MISUSE       :   /* 21  Library used incorrectly */
        errorString = "SQLITE_MISUSE";
        break;

    case SQLITE_NOLFS        :   /* 22  Uses OS features not supported on host */
        errorString = "SQLITE_NOLFS";
        break;

    case SQLITE_AUTH         :   /* 23  Authorization denied */
        errorString = "SQLITE_AUTH";
        break;

    case SQLITE_ROW          :   /* 100 sqlite_step() has another row ready */
        errorString = "SQLITE_ROW";
        break;

    case SQLITE_DONE         :   /* 101 sqlite_step() has finished executing */
        errorString = "SQLITE_DONE";
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////
void SQLite::DumpDB( std::ofstream& outFile, DumpConfig dumpConfig  )
{
    BeginTrans();

    DoSchemaDumpQuery( outFile, dumpConfig );
    DumpTableDataQuery( outFile, s_TableDumpSQL );

    CommitTrans();
}


/////////////////////////////////////////////////////////////////////////////
int SQLite::DoSchemaDumpQuery( std::ofstream& outFile, DumpConfig dumpConfig  )
{
    StmtHandle stmt = CreateStatement( s_DBSchemaDumpSQL );
    int sqlResult = ExecStatement( stmt );

    if ( sqlResult == SQLITE_CORRUPT )
    {
        std::string tryAgain = s_DBSchemaDumpSQL;
        tryAgain += " ORDER BY rowid DESC";

        stmt = CreateStatement( tryAgain.c_str() );
        sqlResult = ExecStatement( stmt );
    }

    if ( sqlResult == SQLITE_DONE )
    {
        return sqlResult;
    }
    else if ( sqlResult != SQLITE_ROW )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
    else if ( sqlResult == SQLITE_ROW )
    {
        while ( sqlResult == SQLITE_ROW && !outFile.fail() )
        {
            DumpTable( outFile, stmt, dumpConfig );

            sqlResult = StepStatement( stmt );
        }
        ResetStatement( stmt );
    }

    return sqlResult;
}

/////////////////////////////////////////////////////////////////////////////
// This is a different callback routine used for dumping the database.
// Each row received by this callback consists of a table name,
// the table type ("index" or "table") and SQL to create the table.
// This routine should print text sufficient to recreate the table.
//
int SQLite::DumpTable( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig )
{
    StmtHandle stmt = CreateStatement( s_TableSchemaDumpSQL, "t" );
    int sqlResult = ExecStatement( stmt, tableName.c_str() );

    if ( sqlResult == SQLITE_DONE )
    {
        return sqlResult;
    }
    else if ( sqlResult != SQLITE_ROW )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
    else if ( sqlResult == SQLITE_ROW )
    {
        DumpTable( outFile, stmt, dumpConfig );
        ResetStatement( stmt );
    }

    return sqlResult;
}

/////////////////////////////////////////////////////////////////////////////
//
// This is a different callback routine used for dumping the database.
// Each row received by this callback consists of a table name,
// the table type ("index" or "table") and SQL to create the table.
// This routine should print text sufficient to recreate the table.
//
int SQLite::DumpTable( std::ofstream& outFile, const StmtHandle stmt, DumpConfig dumpConfig )
{
    if ( GetColumnCount( stmt ) != 3 )
    {
        return 0;
    }

    std::string name, type, sql;
    GetColumnText( stmt, 0, name );
    GetColumnText( stmt, 1, type );
    GetColumnText( stmt, 2, sql );

    if ( ( dumpConfig & DumpConfigs::Schema ) == DumpConfigs::Schema )
    {
        if ( name.compare( "sqlite_sequence") == 0 )
        {
            outFile << "DELETE FROM sqlite_sequence;\n" ;
        }
        else if ( name.compare( "sqlite_stat1" ) == 0 )
        {
            outFile << "ANALYZE sqlite_master;\n" ;
        }
        else if ( name.compare( 0, 7, "sqlite_" ) == 0 )
        {
            return 0;
        }
        else if ( name.compare( 0, 20, "CREATE VIRTUAL TABLE" ) == 0 )
        {    
            char* sqlInsertMaster;
            sqlInsertMaster = sqlite3_mprintf( s_InsertMasterSQL, name, name, sql );

            outFile << sqlInsertMaster << "\n";

            sqlite3_free( sqlInsertMaster );

            return 0;
        }
        else
        {
            outFile << "/********************************************************\n";
            outFile << "* " << name << "\n";
            outFile << "*********************************************************/\n";

            outFile << sql << "\n\n";
        }
    }
    else if ( ( name.compare( 0, 7, "sqlite_" ) == 0 )
        || ( name.compare( 0, 20, "CREATE VIRTUAL TABLE" ) == 0 ) )
    {
        return 0;
    }


    if ( ( ( dumpConfig & DumpConfigs::Data ) == DumpConfigs::Data )
        && ( type.compare( "table" ) == 0 ) )
    {
        return DumpTableData( outFile, name, dumpConfig );
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// 
int SQLite::DumpTableData( std::ofstream& outFile, const std::string& tableName, DumpConfig dumpConfig )
{
    std::stringstream selectInsertSQL;
    selectInsertSQL << "SELECT 'INSERT INTO ' || ";
    selectInsertSQL << '\'' << '"' << tableName << '"' << '\'';

    std::stringstream selectValuesSQL;
    selectValuesSQL << " || ' VALUES(' || ";

    std::string getTableInfoSQL = "PRAGMA table_info(" ;
    getTableInfoSQL += tableName + ");";

    StmtHandle tableInfoStmt = CreateStatement( getTableInfoSQL.c_str() );
    int stepResult = ExecStatement( tableInfoStmt );

    bool completeInsert = ( ( dumpConfig & DumpConfigs::CompleteInserts ) == DumpConfigs::CompleteInserts );

    if ( completeInsert )
    {
        selectInsertSQL << " (";
    }

    while( stepResult == SQLITE_ROW )
    {      
        std::string value;
        GetColumnText( tableInfoStmt, 0, value );

        if ( completeInsert )
        {
            selectInsertSQL << value;
        }

        selectValuesSQL << "quote(" << '"' << value << '"' << ")" ;

        stepResult = StepStatement( tableInfoStmt );

        if( stepResult == SQLITE_ROW )
        {
            if ( completeInsert )
            {
                selectInsertSQL << ",";
            }

            selectValuesSQL << " || ',' || ";
        }
        else
        {
            selectValuesSQL << " ";
        }
    }

    if ( completeInsert )
    {
        selectInsertSQL << ")";
    }

    selectInsertSQL << selectValuesSQL.str();

    selectInsertSQL << "|| ')' FROM  " << tableName ;

    int dumpTableResult = DumpTableDataQuery( outFile, selectInsertSQL.str(), tableName + std::string( " data" ) );

    if( dumpTableResult == SQLITE_CORRUPT )
    {
        selectInsertSQL << " ORDER BY rowid DESC";
        dumpTableResult = DumpTableDataQuery( outFile, selectInsertSQL.str(), tableName + std::string( " data" ) );
    }

    return dumpTableResult;

}

/////////////////////////////////////////////////////////////////////////////
// 
int SQLite::DumpTableDataQuery( std::ofstream& outFile, const std::string& stmtString, const std::string& tableComment )
{
    StmtHandle stmt = CreateStatement( stmtString.c_str() );

    int sqlResult = ExecStatement( stmt );

    if ( sqlResult == SQLITE_DONE )
    {
        return sqlResult;
    }
    else if ( sqlResult != SQLITE_ROW )
    {
        throw SQL::DBManagerException( this, __FUNCTION__ );
    }
    else if ( sqlResult == SQLITE_ROW )
    {
        if ( !tableComment.empty() )
        {
            outFile << "/********************************************************\n";
            outFile << "* " << tableComment << "\n";
            outFile << "*********************************************************/\n";
        }

        while ( sqlResult == SQLITE_ROW && !outFile.fail() )
        {
            std::string row;
            GetColumnText( stmt, 0, row );

            outFile << row << ";\n";

            sqlResult = StepStatement( stmt );
        }
        ResetStatement( stmt );

        if ( !tableComment.empty() )
        {
            outFile << "\n\n";
        }
    }

    return sqlResult;
}
