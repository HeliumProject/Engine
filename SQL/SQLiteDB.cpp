#include "stdafx.h"

#include "SQLiteDB.h"
#include "SQLite.h"

#include "Common/String/Tokenize.h"
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "Windows/Error.h"
#include "Windows/Process.h"
#include "Windows/Thread.h"

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
SQLiteDB::SQLiteDB()
: m_DBManager( NULL )
{
  m_GeneralCriticalSection = new ::CRITICAL_SECTION;
  ::InitializeCriticalSection( m_GeneralCriticalSection );

  m_DBManager = new SQLite();
}

/////////////////////////////////////////////////////////////////////////////
// Dtor - Closes the SQLiteDB
SQLiteDB::~SQLiteDB()
{
  Close();

  ::DeleteCriticalSection( m_GeneralCriticalSection );

  delete m_GeneralCriticalSection;
  delete m_DBManager;
}

#pragma TODO("These should be passed into Open someday")
static const char* s_SQLiteDBFolder = "sqlitedb";
static const char* s_SQLiteDataFile = "data.sql";

/////////////////////////////////////////////////////////////////////////////
// Opens and Load the DB; creating the DB if it does not exist.
bool SQLiteDB::Open( const std::string& dbFilename, const std::string& configFolder, const std::string& version, int flags )
{   
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

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

  if ( FileSystem::Exists( m_DBFilename ) && !( m_OpenFlags & SQLITE_OPEN_READONLY ) )
  {
    if ( FileSystem::HasAttribute( m_DBFilename, FILE_ATTRIBUTE_READONLY ) )
    {
      //throw SQL::DBManagerException( this, __FUNCTION__, "[%s] is read-only!", m_DBFilename.c_str() );
      Console::Error( "SQLite DB [%s] is read-only.\n", m_DBFilename.c_str() );
      return false;
    }
  }
  else if ( !FileSystem::Exists( m_DBFilename ) && !( m_OpenFlags & SQLITE_OPEN_CREATE ) )
  {
    Console::Error( "SQLite DB [%s] does not exist.\n", m_DBFilename.c_str() );
    return false;
  }

  m_ConfigFolder = configFolder;
  FileSystem::AppendPath( m_ConfigFolder, s_SQLiteDBFolder );

  m_DataFilename = m_DBFilename;
  FileSystem::StripLeaf( m_DataFilename );
  FileSystem::AppendPath( m_DataFilename, s_SQLiteDataFile );

  if ( !Load() )
  {
    return false;
  }
  
  // open the DB
  m_DBManager->Open( m_DBFilename, m_OpenFlags );


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
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  if ( !IsOutOfDate() )
  {
    return true;
  }
  else if ( !( m_OpenFlags & SQLITE_OPEN_CREATE ) )
  {
    Console::Error( "SQLite DB [%s] is out of date and cannot be created.\n", m_DBFilename.c_str() );
    return false;
  }
  else if ( m_OpenFlags & SQLITE_OPEN_READONLY )
  {
    Console::Error( "SQLite DB [%s] is out of date and the DB is read-only.\n", m_DBFilename.c_str() );
    return false;
  }

  // call this so that subclasses can do additional work if the DB is being created
  Delete();

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
//  - the DB major version is NOT up to date
bool SQLiteDB::IsOutOfDate()
{
  // is the data file newer than the db?
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

  // is the version up-to-date
  std::string dbVersion;
  if ( !SelectDBVersion( dbVersion ) || dbVersion.empty() )
  {
    return true;
  }

  // if DB version is up to date there is no need to recreate the DB
  NOC_ASSERT( !m_DBVersion.empty() );
  if ( m_DBVersion.compare( dbVersion ) != 0 )
  {
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
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

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
    Console::Warning( error.c_str() );
    return false;
  }

  UpdateDBFileVersionTable();

  return true;
}


/////////////////////////////////////////////////////////////////////////////
// Create and update the db_file_version table
void SQLiteDB::UpdateDBFileVersionTable()
{
  m_DBManager->Open( m_DBFilename );

  if ( ( ( m_DBManager->ExecSQL( s_CreateFileVersionTableSQL ) )!= SQLITE_OK )
    || ( ( m_DBManager->ExecSQLVMPrintF( s_InsertDBFileVersionSQL,
    FileSystem::GetLeaf( m_DBFilename ).c_str(), 
    m_DBVersion.c_str() ) ) != SQLITE_OK ) )
  {
    m_DBManager->Close();
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  m_DBManager->Close();
}


/////////////////////////////////////////////////////////////////////////////
// Deletes the  DB and handled events list
void SQLiteDB::Delete()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

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
  m_DBManager->Open( m_DBFilename );
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
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  bool result = false;

  m_DBManager->Open( m_DBFilename );

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

  m_DBManager->Close();

  return result;
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::BeginTrans()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  m_DBManager->BeginTrans();
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::CommitTrans()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  m_DBManager->CommitTrans();
}

/////////////////////////////////////////////////////////////////////////////
void SQLiteDB::RollbackTrans()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  m_DBManager->RollbackTrans();
}

/////////////////////////////////////////////////////////////////////////////
bool SQLiteDB::IsTransOpen()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );

  return m_DBManager->IsTransOpen();
}
