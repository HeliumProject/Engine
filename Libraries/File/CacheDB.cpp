#include "Windows/Windows.h"

#include "CacheDB.h"

#include "Common/Boost/Regex.h" 
#include "Common/String/Tokenize.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ProjectSpecs.h"
#include "SQL/SQLite.h"
#include "Windows/Thread.h"

#include <sqlite3.h>

using namespace File;

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

const char* CacheDB::s_FileResolverDBVersion = "7.0";

///////////////////////////////////////////////
// File Table

// Used to INSERT into Cache DB with sqlite3_mprintf OR sqlite3_vmprintf,
// takes arguments for (in order): created, modified, id, filePath
static const char* s_InsertFileSQL = 
"INSERT INTO file (created,modified,id,path,user_id) \
VALUES ('%lld','%lld','%lld','%q','%d');";

// Used to UPDATE Cache DB with sqlite3_mprintf OR sqlite3_vmprintf
// takes arguments for (in order): modified, filePath, id
static const char* s_UpdateFileSQL = 
"UPDATE file \
SET modified='%lld', \
path='%q', \
user_id='%d', \
was_deleted='0' \
WHERE id='%lld';";

// Used to remove a single entry from the file table
static const char* s_ObliterateFileSQL = "DELETE FROM file WHERE id=?;";

// Used to remove a single entry from the file table
static const char* s_MarkFileDeletedSQL = "UPDATE file SET was_deleted='1',user_id=? WHERE id=?;";

// Used to restore a single entry in the file table
static const char* s_RestoreFileSQL = "UPDATE file SET was_deleted='0',user_id=? WHERE id=?;";

// Used to SELECT an entry from the file table, by id
static const char* s_SelectFileByIdSQL = 
"SELECT file.* \
FROM file \
WHERE id=? \
AND was_deleted<=?;";

// Used to SELECT one or more rows from the file table, by path
static const char* s_SelectFileByPathSQL = 
"SELECT DISTINCT file.* \
FROM file \
WHERE path LIKE ? ESCAPE '@' \
AND was_deleted<=?;";

// Used to SELECT only one row from the file table, by path
static const char* s_SelectOneFileByPathSQL = "SELECT DISTINCT * FROM file WHERE path=? AND was_deleted<=? LIMIT 1;";

// just determines if the id is in the db
static const char *s_ContainsIdSQL = "SELECT COUNT(*) FROM file WHERE id=? AND was_deleted<=?;";


///////////////////////////////////////////////
// File ID Lookup table

// INSERT a file look up to the table
static const char* s_ReplaceLookupFileIDSQL = "REPLACE INTO file_id_lookup (duplicate_file_id,existing_file_id) VALUES ('%lld','%lld');";

// UPDATE takes: NEW existing_file_id, OLD existing_file_id
static const char* s_UpdateLookupFileIDSQL = "UPDATE file_id_lookup SET existing_file_id='%lld' WHERE existing_file_id='%lld';";

static const char* s_SelectLookupFileByIDSQL=
"SELECT file.* \
FROM file \
JOIN file_id_lookup ON file_id_lookup.existing_file_id = file.id \
WHERE file_id_lookup.duplicate_file_id=? \
AND was_deleted<=?;";



///////////////////////////////////////////////
// Histoy Table

// INSERT into the history table
static const char* s_InsertFileHistorySQL = "INSERT INTO history (file_id,operation,modified,user_id,data_id) VALUES ('%lld','%q','%lld','%d','%d');";

// SELECT an enty's file history from the history table, by ID
static const char* s_SelectFileHistorySQL = 
"SELECT history.operation, history.modified, user.username, history_data.data \
FROM history \
LEFT JOIN user ON history.user_id = user.id \
LEFT JOIN history_data ON history.data_id = history_data.id  \
WHERE file_id=?;";

// SELECT file by the user that modified it
static const char* s_SelectFileByHistorySQL =
"SELECT file.* \
FROM file \
LEFT JOIN history ON history.file_id = file.id \
LEFT JOIN user ON history.user_id = user.id \
WHERE file.path LIKE ? ESCAPE '@' \
AND user.username LIKE ? ESCAPE '@' \
AND history.operation=? \
AND file.was_deleted<=?;";

// SELECT file by path rename history
static const char* s_SelectFileByHistoryDataSQL =
"SELECT file.* \
FROM file \
LEFT JOIN history ON history.file_id = file.id \
LEFT JOIN user ON history.user_id = user.id \
LEFT JOIN history_data ON history.data_id = history_data.id  \
WHERE history_data.data LIKE ? ESCAPE '@' \
AND user.username LIKE ? ESCAPE '@' \
AND history.operation=? \
AND file.was_deleted<=?;";


///////////////////////////////////////////////
// History Data Table
static const char* s_InsertFileHistoryDataSQL = "INSERT INTO history_data (data) VALUES ('%q');";


///////////////////////////////////////////////
// Handled Events Table
static const char* s_InsertHandledEventSQL = "INSERT INTO handled_events (id,created,username,data) VALUES ('%lld','%lld','%q','%q');";
static const char* s_SelectHandledEventsSQL = "SELECT DISTINCT id FROM handled_events;";
static const char* s_DeleteHandledEventsSQL = "DELETE FROM handled_events;";


///////////////////////////////////////////////
// User Table

static const char* s_SelectUsernameIDSQL = "SELECT id FROM user WHERE username=?;";
static const char* s_InsertUsernameSQL = "INSERT INTO user (username) VALUES ('%s');";

// SELECT an asset's file history from the history table, by ID
static const char* s_SelectUsernameByIDSQL = "SELECT username FROM user WHERE id=?;";



/////////////////////////////////////////////////////////////////////////////
// Ctor - Initialiaze the CacheDb
CacheDB::CacheDB( const char* friendlyName )
: SQL::SQLiteDB( friendlyName )
, m_NeedsToFlushEvents( false )
{
}


/////////////////////////////////////////////////////////////////////////////
// Dtor - Closes the CacheDB
CacheDB::~CacheDB()
{
}


/////////////////////////////////////////////////////////////////////////////
// Prepares all of the statements.
void CacheDB::PrepareStatements()
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  m_SqlSelectIdStatementHandle        = m_DBManager->CreateStatement( s_SelectFileByIdSQL, "li" );
  m_SqlSelectPathStatementHandle      = m_DBManager->CreateStatement( s_SelectFileByPathSQL, "ti" );
  m_SqlSelectOnePathStatementHandle   = m_DBManager->CreateStatement( s_SelectOneFileByPathSQL, "ti" );
  m_SqlContainsIdStatementHandle      = m_DBManager->CreateStatement( s_ContainsIdSQL, "li" );
  m_SqlObliterateStatementHandle      = m_DBManager->CreateStatement( s_ObliterateFileSQL, "l" );
  m_SqlMarkDeletedStatementHandle     = m_DBManager->CreateStatement( s_MarkFileDeletedSQL, "il" );
  m_SqlRestoreStatementHandle         = m_DBManager->CreateStatement( s_RestoreFileSQL, "il" );

  m_SqlLookupFileByIDStatementHandle  = m_DBManager->CreateStatement( s_SelectLookupFileByIDSQL, "li" );

  m_SqlSelectFileHistoryHandle        = m_DBManager->CreateStatement( s_SelectFileHistorySQL, "l" );
  m_SqlSelectFileByHistoryHandle      = m_DBManager->CreateStatement( s_SelectFileByHistorySQL, "ttti" );
  m_SqlSelectFileByHistoryDataHandle  = m_DBManager->CreateStatement( s_SelectFileByHistoryDataSQL, "ttti" );

  m_SqlSelectHandledEventsHandle      = m_DBManager->CreateStatement( s_SelectHandledEventsSQL );

  m_SqlSelectUsernameIDHandle         = m_DBManager->CreateStatement( s_SelectUsernameIDSQL, "t" );
  m_SqlSelectUsernameByIDHandle       = m_DBManager->CreateStatement( s_SelectUsernameByIDSQL, "i" );
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the Cache DB and handled events list
void CacheDB::Delete()
{
  FILE_SCOPE_TIMER((""));

  __super::Delete();

  m_NeedsToFlushEvents = true;
}



/////////////////////////////////////////////////////////////////////////////
// Queries the DB using a select statement that has a single text value
// Note: If it is not in the DB it and an insert statement has been passed in
//        this function will insert the value into the DB
//
u64 CacheDB::SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert )
{
  FILE_SCOPE_TIMER((""));

  NOC_ASSERT( select );

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  if ( !m_DBManager->IsConnected() )
  {
    throw Nocturnal::Exception( "Lost DB connection" );
  }

  u64 id = 0;
  int execResult = m_DBManager->ExecStatement( select, value );
  if ( execResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnI64( select, 0, ( i64& ) id );
    m_DBManager->ResetStatement( select );
  }
  else if ( execResult != SQLITE_DONE && execResult != SQLITE_OK )
  {
    throw Nocturnal::Exception( "Failed to execute SQL: %s", select );
  }
  else if ( insert )
  {
    char* insertDumbPtr = NULL;

    char insertBuff[MAX_INSERT_LENGTH];
    sprintf_s( insertBuff, sizeof( insertBuff ), insert, value );
    insertDumbPtr = &insertBuff[0];

    execResult = m_DBManager->ExecSQLVMPrintF( insertDumbPtr, id );
    if ( execResult == SQLITE_OK )
    {
      id = m_DBManager->GetLastInsertRowId();
    }
    else
    {
      throw Nocturnal::Exception( "Failed to execute SQL: %s", insert );
    }
  }


  return id;
  //return InsertIfNotFound( select, value, insertDumbPtr );
}

//*************************************************************************//
//
//  USER TABLE FUNCTIONS
//
//*************************************************************************//


/////////////////////////////////////////////////////////////////////////////
u32 CacheDB::SelectUsernameID( const std::string& username )
{
  FILE_SCOPE_TIMER((""));

  return (u32) SelectIDByName( m_SqlSelectUsernameIDHandle, username.c_str(), s_InsertUsernameSQL );
}


/////////////////////////////////////////////////////////////////////////////
// Gets the file's modified history from the history table
void CacheDB::SelectUsernameByID( const u32 userID, std::string& username )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectUsernameByIDHandle, (i32) userID );

  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // Step through one or more entries, and pull the data from the columns
  if ( sqlResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnText( m_SqlSelectUsernameByIDHandle,  0, username );
  }
}

//*************************************************************************//
//
//  HANDLED EVENTS TABLE FUNCTIONS
//
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
void CacheDB::InsertHandledEvent( const tuid id, const u64 created, const std::string& username, const std::string& data )
{
  FILE_SCOPE_TIMER("");

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  if ( m_DBManager->ExecSQLVMPrintF(
    s_InsertHandledEventSQL,
    ( i64 ) id,
    ( i64 ) created,
    username.c_str(),
    data.c_str() ) != SQLITE_OK )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}

/////////////////////////////////////////////////////////////////////////////
void CacheDB::SelectHandledEvents( S_tuid& eventIDs )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectHandledEventsHandle );
  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }

  if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // Step through one or more entries, and pull the data from the columns
  while ( sqlResult == SQLITE_ROW )
  {
    tuid eventID;
    m_DBManager->GetColumnI64( m_SqlSelectHandledEventsHandle, 0, ( i64& ) eventID );
    eventIDs.insert( eventID );

    sqlResult = m_DBManager->StepStatement( m_SqlSelectHandledEventsHandle );
  }

  m_DBManager->ResetStatement( m_SqlSelectHandledEventsHandle );
}

/////////////////////////////////////////////////////////////////////////////
void CacheDB::DeleteHandledEvents()
{
  FILE_SCOPE_TIMER("");

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  if ( m_DBManager->ExecSQL( s_DeleteHandledEventsSQL ) != SQLITE_OK )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}


//*************************************************************************//
//
//  API FUNCTIONS
//
//*************************************************************************//

/////////////////////////////////////////////////////////////////////////////
// single utility function to do all the cleaning we need 
// 
void CacheDB::CleanExpressionForSQL( std::string& argument )
{ 
  // convert asterisks to percent 
  const boost::regex asterisk("[*]+"); 
  argument = boost::regex_replace(argument, asterisk, "%"); 

  // escape underscores
  const boost::regex underscore("[_]"); 
  argument = boost::regex_replace(argument, underscore, "@_"); 
}


/////////////////////////////////////////////////////////////////////////////
// Insert a single file table row into the Cache DB, returns the tuid of the 
// entry
//
tuid CacheDB::InsertFile
(
 const tuid id,
 const u64 creationTime,
 const u64 modifiedTime,
 const std::string& filePath,
 const u32 userID
 )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  //////////////////////////
  // try to find the existing file by id first
  ManagedFilePtr file = SelectFileByID( id, true );
  if ( file )
  {
    if ( file->m_Path == filePath )
    {
      // retore the existing file
      if ( file->m_WasDeleted )
      {
        RestoreFile( file->m_Id, modifiedTime, userID );
      }

      return file->m_Id;
    }
    else
    {
      // There is no use case for inserting an existing ID with a different path,
      // it would mean that there was an ID collision, this is very rare
      // and Andy claims this case is nearly impossible!
      throw DuplicateEntryException( id, filePath.c_str() );
    }
  }


  //////////////////////////
  // try to find the existing file by path
  file = SelectFileByPath( filePath, true );
  if ( file )
  {
    // if the entry is different from the expected value
    if ( file->m_Id != id )
    {
      Console::Debug( "Attempted to insert duplicate entry (already exists with id: "TUID_HEX_FORMAT"):\n  TUID: "TUID_HEX_FORMAT"\n  path: %s\n",
        file->m_Id, id, filePath.c_str() ); 

      // add it to the lookup table and update the existing look up pairs (if there are any)
      if ( ( m_DBManager->ExecSQLVMPrintF( s_ReplaceLookupFileIDSQL, ( i64 ) id, ( i64 ) file->m_Id ) != SQLITE_OK )
        || ( m_DBManager->ExecSQLVMPrintF( s_UpdateLookupFileIDSQL, ( i64 ) file->m_Id, ( i64 ) id ) != SQLITE_OK ) )
      {
        throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
      }
    }

    // retore the existing file
    if ( file->m_WasDeleted )
    {
      RestoreFile( file->m_Id, modifiedTime, userID );
    }

    // either way early out and return the existing TUID
    return file->m_Id;
  }

  {
    FILE_SCOPE_TIMER(("Adding file to DB"));
    //////////////////////////
    // otherwise, add the new file to the DB
    if ( m_DBManager->ExecSQLVMPrintF(
      s_InsertFileSQL,
      ( i64 ) creationTime,
      ( i64 ) modifiedTime,
      ( i64 ) id,
      filePath.c_str(),
      ( i32 ) userID ) != SQLITE_OK )
    {
      throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
    }
  }

  // update the history
  InsertFileHistory( id, PatchOperations::Insert, modifiedTime, userID );

  return id;
}


/////////////////////////////////////////////////////////////////////////////
// Replace a single file table row into the Cache DB
tuid CacheDB::UpdateFile
(
 const tuid id,
 const u64 modifiedTime,
 const std::string& filePath,
 const u32 userID
 )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  std::string oldFilePath;

  //////////////////////////
  // try to find the existing file by id
  ManagedFilePtr foundFile = SelectFileByID( id, true );
  if ( foundFile )
  {
    // early out, the path is up-to-date
    if ( foundFile->m_Path == filePath )
    {
      // retore the existing file
      if ( foundFile->m_WasDeleted )
      {
        RestoreFile( foundFile->m_Id, modifiedTime, userID );
      }

      return foundFile->m_Id;
    }

    oldFilePath = foundFile->m_Path;
  }

  //////////////////////////
  // try to find the existing file by path
  foundFile = SelectFileByPath( filePath, true );
  if ( foundFile )
  {
    // if the found file was deleted, replace it
    if ( foundFile->m_WasDeleted )
    {
      Console::Debug( "Deleting an entry with an existing path (completely deleting existing id: "TUID_HEX_FORMAT"):\n Updaing TUID: "TUID_HEX_FORMAT"\n  to path: %s\n",
        foundFile->m_Id, id, filePath.c_str() ); 

      // really remove the deleted file, so that the other one can be updated w/o path collisions 
      ObliterateFile( foundFile->m_Id, modifiedTime, userID );
    }
    // otherwise, the found file still exists so add an entry to the file id lookup table
    else
    {
      Console::Debug( "Attempted to update an entry with an existing path (already exists with id: "TUID_HEX_FORMAT"):\n  TUID: "TUID_HEX_FORMAT"\n  path: %s\n",
        foundFile->m_Id, id, filePath.c_str() ); 

      // add it to the lookup table and update the existing look up pairs (if there are any)
      if ( ( m_DBManager->ExecSQLVMPrintF( s_ReplaceLookupFileIDSQL, ( i64 ) id, ( i64 ) foundFile->m_Id ) != SQLITE_OK )
        || ( m_DBManager->ExecSQLVMPrintF( s_UpdateLookupFileIDSQL, ( i64 ) foundFile->m_Id, ( i64 ) id ) != SQLITE_OK ) )
      {
        throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
      }

      // remove the old entry so the user must look up the file in the fix-up table
      MarkFileDeleted( id, modifiedTime, userID );

      // either way early out and return the existing TUID
      return foundFile->m_Id;
    }
  }


  //////////////////////////
  // otherwise, update the existing entry
  if ( m_DBManager->ExecSQLVMPrintF(
    s_UpdateFileSQL,
    ( i64 ) modifiedTime,
    filePath.c_str(),
    ( i32 ) userID,
    ( i64 ) id ) != SQLITE_OK )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // update the history
  InsertFileHistory( id, PatchOperations::Update, modifiedTime, userID, oldFilePath );

  return id;
}


/////////////////////////////////////////////////////////////////////////////
// Marks a single file as delete in the Cache DB
void CacheDB::MarkFileDeleted( const tuid id, const u64 modifiedTime, const u32 userID )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  // early out if the file doesn't exist
  ManagedFilePtr file = SelectFileByID( id );
  if ( !file )
    return;

  if ( m_DBManager->ExecStatement( m_SqlMarkDeletedStatementHandle, ( i32 ) userID, ( i64 ) id ) != SQLITE_DONE )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // update the history
  InsertFileHistory( id, PatchOperations::Delete, modifiedTime, userID );
}

/////////////////////////////////////////////////////////////////////////////
// Actually deletes a single file from the Cache DB
void CacheDB::ObliterateFile( const tuid id, const u64 modifiedTime, const u32 userID )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  // early out if the file doesn't exist
  ManagedFilePtr foundFile = SelectFileByID( id, true );
  if ( !foundFile )
  {
    return;
  }

  if ( m_DBManager->ExecStatement( m_SqlObliterateStatementHandle, ( i64 ) id ) != SQLITE_DONE )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // update the history
  InsertFileHistory( id, PatchOperations::Obliterate, modifiedTime, userID );
}

/////////////////////////////////////////////////////////////////////////////
// Restores a single file into the Cache DB, if it exists
bool CacheDB::RestoreFile( const tuid id, const u64 modifiedTime, const u32 userID )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  // early out if the file doesn't exist, or it is already restored
  ManagedFilePtr foundFile = SelectFileByID( id, true );
  if ( !foundFile )
  {
    return false;
  }

  if ( !foundFile->m_WasDeleted )
  {
    return true;
  }

  if ( m_DBManager->ExecStatement( m_SqlRestoreStatementHandle, userID, ( i64 ) id ) != SQLITE_DONE )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // update the history
  InsertFileHistory( id, PatchOperations::Restore, modifiedTime, userID );

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// Populate the ManagedFilePtr with row returned from the query
// Returns true if a file was successfully read from the DB
ManagedFilePtr CacheDB::StepSelectFile( int sqlResult, const SQL::StmtHandle stmt, bool resetStmt )
{
  FILE_SCOPE_TIMER((""));

  if ( sqlResult == SQLITE_DONE )
  {
    return NULL;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
  else if ( sqlResult == SQLITE_ROW )
  {
    ManagedFilePtr file = new ManagedFile();

    int index = -1;
    m_DBManager->GetColumnI64(  stmt,  ++index, ( i64& ) file->m_Created );
    m_DBManager->GetColumnI64(  stmt,  ++index, ( i64& ) file->m_Modified );
    m_DBManager->GetColumnI64(  stmt,  ++index, ( i64& ) file->m_Id );
    m_DBManager->GetColumnText( stmt,  ++index,          file->m_Path );
    m_DBManager->GetColumnInt(  stmt,  ++index, ( i32 &) file->m_UserId );

    int wasDeleted;
    m_DBManager->GetColumnInt(  stmt,  ++index, ( int &) wasDeleted );
    file->m_WasDeleted = ( ( wasDeleted == 1 ) ? true : false ) ;

    if ( resetStmt )
    {
      m_DBManager->ResetStatement( stmt );
    }

    return file;
  }

  return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Populate the ManagedFilePtr with entry data from the Cache DB,
// returns true if a file was found
//
ManagedFilePtr CacheDB::SelectFileByID( const tuid id, bool getDeletedFiles )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  SQL::StmtHandle stmt = m_SqlSelectIdStatementHandle;
  int sqlResult = m_DBManager->ExecStatement( stmt, ( i64 ) id, ( getDeletedFiles ? 1 : 0 ) );

  // search the lookup table next
  if ( sqlResult == SQLITE_DONE )
  {
    stmt = m_SqlLookupFileByIDStatementHandle;
    sqlResult = m_DBManager->ExecStatement( stmt, ( i64 ) id, ( getDeletedFiles ? 1 : 0 ) );
  }

  return StepSelectFile( sqlResult, stmt );
}


/////////////////////////////////////////////////////////////////////////////
// Get's the file for the given the file path
//
ManagedFilePtr CacheDB::SelectFileByPath( const std::string& filePath, bool getDeletedFiles )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  SQL::StmtHandle stmt = m_SqlSelectOnePathStatementHandle;
  int sqlResult = m_DBManager->ExecStatement( stmt, filePath.c_str(), ( getDeletedFiles ? 1 : 0 ) );

  return StepSelectFile( sqlResult, stmt, true );
}


/////////////////////////////////////////////////////////////////////////////
// Get's a list of filePath/tuid pairs, given the file path; populating
// the listOfFiles argument.
bool CacheDB::SelectListOfFilesByPath( const std::string& filePath, V_ManagedFilePtr &listOfFiles, bool getOneRow, bool getDeletedFiles )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  SQL::StmtHandle stmt;
  int sqlResult = SQLITE_DONE;
  if ( getOneRow )
  {
    // "SELECT DISTINCT * FROM file WHERE path=? LIMIT 1;"

    stmt = m_SqlSelectOnePathStatementHandle;
    sqlResult = m_DBManager->ExecStatement( stmt, filePath.c_str(), ( getDeletedFiles ? 1 : 0 ) );
  }
  else
  {
    // "SELECT DISTINCT * FROM file WHERE path LIKE ? ESCAPE '@';"

    // Convert all asterisks to percent signs for the SQL LIKE expression 
    std::string cleanFilePath = "*" + filePath + "*";

    CleanExpressionForSQL(cleanFilePath); 

    stmt = m_SqlSelectPathStatementHandle;
    sqlResult = m_DBManager->ExecStatement( stmt, cleanFilePath.c_str(), ( getDeletedFiles ? 1 : 0 ) );
  }    

  // if no files were found return false
  if (  sqlResult == SQLITE_DONE )
  {
    return false;
  }

  // if an unexpected error occurred throw an exception
  if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }


  // Step through one or more entries, and pull the data from the columns
  while ( sqlResult == SQLITE_ROW )
  {
    listOfFiles.push_back( StepSelectFile( sqlResult, stmt, false ) );

    if ( getOneRow )
    {
      break;
    }

    sqlResult = m_DBManager->StepStatement( stmt );
  }

  m_DBManager->ResetStatement( stmt );

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// Just checks if the resolver contains the given id
bool CacheDB::Contains( const tuid id, const bool getDeletedFiles )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  int stepResult = m_DBManager->ExecStatement( m_SqlContainsIdStatementHandle, ( i64 ) id, ( getDeletedFiles ? 1 : 0 ) );

  if ( stepResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__, "Could not execute count on database to locate id." );
  }

  int count = 0;
  m_DBManager->GetColumnInt( m_SqlContainsIdStatementHandle, 0, ( i32& ) count );
  m_DBManager->ResetStatement( m_SqlContainsIdStatementHandle );

  return ( ( count > 0 ) ? true : false );
}




//*************************************************************************//
//
//  HISTORY FUNCTIONS
//
//*************************************************************************//



/////////////////////////////////////////////////////////////////////////////
// Insert a single file table row into the Cache DB
void CacheDB::InsertFileHistory
( 
 const tuid fileID, 
 const PatchOperation operation,
 const u64 modifiedTime, 
 const u32 userID,
 const std::string& data
 )
{
  FILE_SCOPE_TIMER((""));

  if ( modifiedTime == 0 || userID == 0 )
  {
    return;
  }

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  i64 dataRowId = SQL::InvalidRowID;

  // insert the data
  if ( !data.empty() )
  {
    if ( m_DBManager->ExecSQLVMPrintF( s_InsertFileHistoryDataSQL, data.c_str() ) != SQLITE_OK )
    {
      throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
    }

    dataRowId = m_DBManager->GetLastInsertRowId();
  }

  if ( m_DBManager->ExecSQLVMPrintF(
    s_InsertFileHistorySQL,
    ( i64 ) fileID,
    GetPatchOperationString( operation ).c_str(),
    ( i64 ) modifiedTime,
    ( i32 ) userID,
    dataRowId ) != SQLITE_OK )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Gets the file's modified history from the history table
void CacheDB::SelectFileHistory( const ManagedFilePtr& file, S_FileHistory& history )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectFileHistoryHandle, (i64) file->m_Id );

  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // Step through one or more entries, and pull the data from the columns
  while ( sqlResult == SQLITE_ROW )
  {
    std::string operation;
    u64 modifiedTime;
    std::string username;
    std::string data;

    int index = -1;
    m_DBManager->GetColumnText( m_SqlSelectFileHistoryHandle,  ++index,          operation );
    m_DBManager->GetColumnI64( m_SqlSelectFileHistoryHandle,   ++index, ( i64& ) modifiedTime );
    m_DBManager->GetColumnText( m_SqlSelectFileHistoryHandle,  ++index,          username );
    m_DBManager->GetColumnText( m_SqlSelectFileHistoryHandle,  ++index,          data );

    history.insert( FileHistory( modifiedTime, GetPatchOperation( operation ), username, data ) );

    sqlResult = m_DBManager->StepStatement( m_SqlSelectFileHistoryHandle );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Gets files that have been modified by the given user
void CacheDB::SelectFilesByHistory
( 
 const std::string& searchQuery,
 const std::string& modifiedBy,
 V_ManagedFilePtr &listOfFiles,
 const std::string& operation,
 bool getDeletedFiles,
 bool searchHistoryData
 )
{
  FILE_SCOPE_TIMER((""));

  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  // Convert all asterisks to percent signs for the SQL LIKE expression 
  // filepath search query
  std::string cleanSearchQuery = "*" + searchQuery + "*";
  CleanExpressionForSQL(cleanSearchQuery); 

  // modified/createdby
  std::string cleanModifiedBy = "*" + modifiedBy + "*";
  CleanExpressionForSQL(cleanModifiedBy); 

  std::string searchOp = ( !searchHistoryData ) ? operation : GetPatchOperationString( PatchOperations::Update ) ;
  const SQL::StmtHandle stmt = ( !searchHistoryData ) ? m_SqlSelectFileByHistoryHandle : m_SqlSelectFileByHistoryDataHandle;

  int sqlResult = m_DBManager->ExecStatement( stmt, 
    cleanSearchQuery.c_str(), 
    cleanModifiedBy.c_str(),
    searchOp.c_str(),
    ( getDeletedFiles ? 1 : 0 ) );

  // if no files were found return false
  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }

  // if an unexpected error occurred throw an exception
  else if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // Step through one or more entries, and pull the data from the columns
  while ( sqlResult == SQLITE_ROW )
  {
    listOfFiles.push_back( StepSelectFile( sqlResult, stmt, false ) );
    sqlResult = m_DBManager->StepStatement( stmt );
  }
}

/////////////////////////////////////////////////////////////////////////////
// Dumps the SQL inserts for the given db
void CacheDB::DumpDBData( const std::string& outputFile )
{    
  FILE_SCOPE_TIMER((""));

  V_string dumpTables;
  dumpTables.push_back( "file_id_lookup" );
  dumpTables.push_back( "file" );
  dumpTables.push_back( "history" );
  dumpTables.push_back( "history_data" );

  std::ofstream outFile( outputFile.c_str(), std::ios::app );
  if ( !outFile.is_open() )
  {
    throw Exception( "Could not open file for writing: %s", outputFile.c_str() );
  }

  try
  {
    for each ( const std::string& tableName in dumpTables )
    {
      m_DBManager->DumpTable( outFile, tableName, SQL::DumpConfigs::Data | SQL::DumpConfigs::CompleteInserts );

      if ( outFile.fail() )
        break;
    }
  }
  catch( ... )
  {
    outFile.close();
    throw;
  }

  if ( outFile.fail() )
  {
    outFile.close();
    throw Exception( "Could not write to sql file: %s", outputFile.c_str() );
  }

  outFile.close();
}
