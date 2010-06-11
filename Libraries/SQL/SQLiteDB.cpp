#include "Platform/Windows/Windows.h"
#include "SQLiteDB.h"
#include "SQLite.h"

#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"
#include "FileSystem/FileSystem.h"
#include "Foundation/Exception.h"
#include "Platform/Process.h"

#include <sstream>

#include "sqlite3.h"

using namespace SQL;

/////////////////////////////////////////////////////////////////////////////
// *.db File Versioning
static const char* s_CreateFileVersionTableSQL =
"DROP TABLE IF EXISTS db_file_version; \n \
CREATE TABLE db_file_version ( \n \
file_name       VARCHAR(50) NOT NULL, \n \
created         TIMESTAMP NOT NULL, \n \
version         VARCHAR(20) NOT NULL DEFAULT '0', \n \
UNIQUE(file_name) \n \
);";

static const char* s_InsertDBFileVersionSQL = "INSERT INTO db_file_version (file_name,created,version)  VALUES('%q',datetime('now','localtime'),'%q');";
static const char* s_SelectDBFileVersionSQL = "SELECT version FROM db_file_version LIMIT 1;";


/////////////////////////////////////////////////////////////////////////////
// Ctor - Initialiaze the CacheDb
SQLiteDB::SQLiteDB( const char* friendlyName )
: m_DBManager( NULL )
{
  m_DBManager = new SQLite( friendlyName );
}

/////////////////////////////////////////////////////////////////////////////
// Dtor - Closes the SQLiteDB
SQLiteDB::~SQLiteDB()
{
  Close();

  delete m_DBManager;
}

#pragma TODO("These should be passed into Open someday")
static const char* s_SQLiteDBFolder = "sqlitedb";
static const char* s_SQLiteDataFile = "data.sql";

/////////////////////////////////////////////////////////////////////////////
// Opens and Load the  DB; creating the DB if it does not exist.
bool SQLiteDB::Open( const std::string& dbFilename, const std::string& configFolder, const std::string& version, int flags )
{   
  Platform::TakeMutex mutex ( m_Mutex );

  //////////////////////////////////
  // Setup the class members

  m_DBVersion = version;
  m_OpenFlags = flags;

  // Initialize the SQLite DB and ensure it's not read-only
  NOC_ASSERT( !dbFilename.empty() );
  m_DBFilename = dbFilename;

  if ( m_DBFilename.empty() )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__, "No database file path was specified." );
  }

  FileSystem::MakePath( m_DBFilename, true );

  m_ConfigFolder = configFolder;
  FileSystem::AppendPath( m_ConfigFolder, s_SQLiteDBFolder );

  m_DataFilename = m_DBFilename;
  FileSystem::StripLeaf( m_DataFilename );
  FileSystem::AppendPath( m_DataFilename, s_SQLiteDataFile );


  //////////////////////////////////
  // Check the DB version

  // db does exists and we are NOT read-only - check that the file is writable
  if ( FileSystem::Exists( m_DBFilename ) && !( m_OpenFlags & SQLITE_OPEN_READONLY ) )
  {
    if ( FileSystem::HasAttribute( m_DBFilename, FILE_ATTRIBUTE_READONLY ) )
    {
      //throw SQL::DBManagerException( this, __FUNCTION__, "[%s] is read-only!", m_DBFilename.c_str() );
      Log::Error( "SQLite DB [%s] is read-only.\n", m_DBFilename.c_str() );
      return false;
    }
  }
  // db does NOT exist and we can NOT create
  else if ( !FileSystem::Exists( m_DBFilename ) && !( m_OpenFlags & SQLITE_OPEN_CREATE ) )
  {
    Log::Error( "SQLite DB [%s] does not exist.\n", m_DBFilename.c_str() );
    return false;
  }


    
  if ( !Load() )
  {
    return false;
  }
  
  // open the DB if we still need to
  if ( !m_DBManager->IsConnected() )
  {
    m_DBManager->Open( m_DBFilename, m_OpenFlags );
  }

  PrepareStatements();

  return true;
}

/////////////////////////////////////////////////////////////////////////////
// Closes the DB and clean up all statement handles
//
void SQLiteDB::Close()
{
  m_DBManager->Close();
}

/////////////////////////////////////////////////////////////////////////////
// Create the DB, either load it from the auth file, or from the versioned sql file
bool SQLiteDB::Load()
{
  Platform::TakeMutex mutex ( m_Mutex );

  // IsOutOfDate may attempt to Update the db
  if ( !IsOutOfDate() )
  {
    return true;
  }
  else if ( !( m_OpenFlags & SQLITE_OPEN_CREATE ) )
  {
    Log::Error( "SQLite DB [%s] is out of date and cannot be created.\n", m_DBFilename.c_str() );
    return false;
  }
  else if ( m_OpenFlags & SQLITE_OPEN_READONLY )
  {
    Log::Error( "SQLite DB [%s] is out of date and the DB is read-only.\n", m_DBFilename.c_str() );
    return false;
  }

  // call this so that subclasses can do additional work if the DB is being created
  m_DBManager->Close();

  std::string error;

  // create the cache DB from scratch
  std::string dbCreateFilename = m_ConfigFolder;
  FileSystem::AppendPath( dbCreateFilename, m_DBVersion );
  dbCreateFilename += ".sql";
  if ( !m_DBManager->FromFile( dbCreateFilename, m_DBFilename, error ) )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__, "%s", error.c_str() );
  }

  // If the data SQL file exists, read and execute the SQL into the DB
  if ( FileSystem::Exists( m_DataFilename )
    && !m_DBManager->ApplyFile( m_DataFilename, m_DBFilename, error ) )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__, "%s", error.c_str() );
  }
  
  UpdateDBFileVersionTable();

  return true;
}



/////////////////////////////////////////////////////////////////////////////
// Determine if the current DB is out-of-date:
//  - the auth file exists and is newer than the DB
// - we couldn't select the version from the DB
// - the version was empty
// - the found version is not equal to the current version
//   AND we couldn't Update  
bool SQLiteDB::IsOutOfDate()
{
  //  - the auth file exists and is newer than the DB
  struct _stati64 dbFileStats;
  if ( FileSystem::GetStats64( m_DBFilename, dbFileStats ) )
  {
    if ( ( FileSystem::Exists( m_DataFilename ) )
      && ( FileSystem::UpdatedSince( m_DataFilename, dbFileStats.st_mtime ) ) )
    {
      return true;
    }
  }
  else
  {
    // the db doesn't exist
    return true;
  }    

  // - we couldn't select the version from the DB
  // - the version was empty
  std::string dbVersion;
  if ( !SelectDBVersion( dbVersion ) || dbVersion.empty() )
  {
    return true;
  }
  // - the found version is not equal to the current version
  //   AND we couldn't Update
  else if ( m_DBVersion.compare( dbVersion ) != 0 )
  {
    // If it's not read-only - try to update the DB
    if ( m_OpenFlags & SQLITE_OPEN_READONLY )
    {
      return true;
    }
    else
    {
      // otherwise try to update first
      return !Update( dbVersion );
    }
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
// Apply updates to the DB; returns true of the DB was updated
//
bool SQLiteDB::Update( const std::string& dbVersion )
{
  Platform::TakeMutex mutex ( m_Mutex );

  m_DBManager->Close();

  std::stringstream updateFileName;
  updateFileName << dbVersion;
  updateFileName << "-";
  updateFileName << m_DBVersion;
  updateFileName << ".sql";

  std::string updateFilePath = m_ConfigFolder;
  FileSystem::AppendPath( updateFilePath, updateFileName.str() );

  if ( !FileSystem::Exists( updateFilePath ) )
  {
    return false;
  }

  std::string error;
  if ( !m_DBManager->ApplyFile( updateFilePath, m_DBFilename, error ) )
  {    
    Log::Warning( error.c_str() );
    return false;
  }

  UpdateDBFileVersionTable();

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// Create and update the db_file_version table
void SQLiteDB::UpdateDBFileVersionTable()
{
  if ( !m_DBManager->IsConnected() )
  {
    m_DBManager->Open( m_DBFilename, m_OpenFlags );
  }

  if ( ( ( m_DBManager->ExecSQL( s_CreateFileVersionTableSQL ) )!= SQLITE_OK )
    || ( ( m_DBManager->ExecSQLVMPrintF( s_InsertDBFileVersionSQL,
    FileSystem::GetLeaf( m_DBFilename ).c_str(), 
    m_DBVersion.c_str() ) ) != SQLITE_OK ) )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the  DB and handled events list
void SQLiteDB::Delete()
{
  Platform::TakeMutex mutex ( m_Mutex );

  m_DBManager->Delete();
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the old graph (if it exits) and creates a new one
// make sure all cache related files are cleared out before re-creating the db
//
void SQLiteDB::Recreate()
{
  m_DBManager->Close();

  Delete();

  Load();

  if ( !m_DBManager->IsConnected() )
  {
    m_DBManager->Open( m_DBFilename, m_OpenFlags );
  }

  PrepareStatements();
}


/////////////////////////////////////////////////////////////////////////////
// Selects the current graph version from the DB
bool SQLiteDB::SelectDBVersion( std::string& version )
{
  return SelectDBVersion( s_SelectDBFileVersionSQL, version );
}


/////////////////////////////////////////////////////////////////////////////
// Selects the current graph version from the DB
bool SQLiteDB::SelectDBVersion( const std::string& sql, std::string& version )
{
  Platform::TakeMutex mutex ( m_Mutex );

  bool result = false;

  if ( !m_DBManager->IsConnected() )
  {
    m_DBManager->Open( m_DBFilename, m_OpenFlags );
  }

  try
  {
    // prepare the statement
    m_StmtHandleSelectDBVersion = m_DBManager->CreateStatement( sql.c_str() );

    int sqlResult = m_DBManager->ExecStatement( m_StmtHandleSelectDBVersion );

    if ( sqlResult == SQLITE_ROW )
    {
      m_DBManager->GetColumnText( m_StmtHandleSelectDBVersion, 0, version );

      m_DBManager->ResetStatement( m_StmtHandleSelectDBVersion );
      result = true;
    }
  }
  catch( Exception & )
  {
    // SQLiteDB does not have correct version info
    result = false;
  }

  return result;
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::BeginTrans()
{
  Platform::TakeMutex mutex ( m_Mutex );

  m_DBManager->BeginTrans();
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::CommitTrans()
{
  Platform::TakeMutex mutex ( m_Mutex );

  m_DBManager->CommitTrans();
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::RollbackTrans()
{
  Platform::TakeMutex mutex ( m_Mutex );

  m_DBManager->RollbackTrans();
}

/////////////////////////////////////////////////////////////////////////////
bool SQLiteDB::IsTransOpen()
{
  Platform::TakeMutex mutex ( m_Mutex );

  return m_DBManager->IsTransOpen();
}