#include "stdafx.h"

#include "Windows/Console.h"

#include "AssetDB.h"
#include "AssetTracker.h"

#include "Asset/AssetClass.h"
#include "Asset/AssetInit.h"
#include "Asset/EngineTypeInfo.h"
#include "Debug/Exception.h"
#include "Console/Console.h"
#include "Common/Boost/Regex.h"
#include "Common/String/Utilities.h"
#include "Common/Environment.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Finder/ProjectSpecs.h"
#include "FileSystem/FileSystem.h"

#include "SQL/SQLite.h"
#include "Windows/Thread.h"

#include "sqlite/src/sqlite3.h"
#include <sstream>

using namespace Asset;


//
// MySQL 
//

//SQL::SQLite* g_DBHandle;

// max storage size for a query string
#define MAX_QUERY_LENGTH  2048
#define MAX_INSERT_LENGTH 2048

//const u64 s_InvalidRowID = 0;

#define ASSETS_DB_NAME  "Assets"
#define TOOLS_DB_NAME   "Tools"
#define USERS_DB_NAME   "Users"

const char* s_DBName  = ASSETS_DB_NAME;

// Users DB
const char* s_SelectUsersComputerIDSQL = "SELECT id FROM computers WHERE name=?;";
const char* s_InsertUsersComputerSQL = "INSERT INTO computers (name) VALUES ('%s');";

const char* s_SelectP4UserIDSQL = "SELECT id FROM p4_users WHERE username=?;";
const char* s_InsertP4UserSQL = "INSERT INTO p4_users (username) VALUES ('%s');";

// Tools DB
static const char* s_SelectToolsProjectIDSQL = "SELECT id FROM projects WHERE name=?;";
static const char* s_InsertToolsProjectSQL = "INSERT INTO projects (name, long_name) VALUES ('%s','%s');";

static const char* s_SelectToolsBranchIDSQL = "SELECT id FROM branches WHERE name=?;";
static const char* s_InsertToolsBranchSQL = "INSERT INTO branches (name) VALUES ('%s');";

// Assets DB
static const char* s_SelectAssetFileRowIDSQL = "SELECT id FROM assets WHERE file_id=? AND project_id=? AND asset_branch_id=?;";
static const char* s_InsertAssetFileSQL = "INSERT INTO assets (file_id,project_id,asset_branch_id,path,name,file_type_id,p4_user_id,engine_type_id,size) VALUES(%lld,%lld,%lld,'%q','%q',%lld,%lld,%lld,%lld);";
static const char* s_UpdateAssetFileSQL =
"UPDATE assets \
SET path='%q', \
name='%q', \
file_type_id=%lld, \
p4_user_id=%lld, \
engine_type_id=%lld, \
size=%lld, \
last_updated=CURRENT_TIMESTAMP \
WHERE file_id='%lld' AND project_id='%lld' AND asset_branch_id='%lld';";
//static const char* s_SelectAssetLastUpdatedSQL = "SELECT last_updated FROM assets where file_id=?;";
static const char* s_SelectAssetLastUpdatedSQL = "SELECT strftime(\"%s\", last_updated) FROM assets where file_id=?;";

static const char* s_ReplaceUsagesSQL = "REPLACE INTO asset_usages (asset_id,dependency_id) VALUES(%lld,%lld);";
//static const char* s_SelectUsagesQL = "SELECT dependency_id FROM "ASSETS_DB_NAME".asset_usages WHERE asset_id=%I64u;";
static const char* s_DeleteUnrenewedUsagesSQL = "DELETE FROM asset_usages WHERE asset_id=%lld AND dependency_id NOT IN (%s);";
static const char* s_DeleteUsagesSQL = "DELETE FROM asset_usages WHERE asset_id=%lld;";

static const char* s_SelectFileTypeIDSQL = "SELECT id FROM file_types WHERE type=?;";
static const char* s_InsertFileTypeSQL = "INSERT INTO file_types (type) VALUES ('%s');";

static const char* s_SelectEngineTypeIDSQL = "SELECT id FROM engine_types WHERE name=?;";
static const char* s_InsertEngineTypeSQL = "INSERT INTO engine_types (name) VALUES ('%s');";

static const char* s_SelectAttributeIDSQL = "SELECT id FROM attributes WHERE name=?;";
static const char* s_InsertAttributeSQL = "INSERT INTO attributes (name) VALUES ('%s');";

static const char* s_ReplaceAssetAttribSQL = "REPLACE INTO asset_x_attribute (asset_id,attribute_id,value) VALUES (%lld,%lld,'%s');";
//static const char* s_SelectAssetAttribRowIDSQL = "SELECT id FROM "ASSETS_DB_NAME".asset_x_attribute WHERE asset_id='%I64u' AND attribute_id='%I64u';";
static const char* s_InsertAssetAttribSQL = "INSERT INTO asset_x_attribute (asset_id,attribute_id,value) VALUES (%lld,%lld,'%s');";
static const char* s_UpdateAssetAttribSQL = "UPDATE asset_x_attribute SET value=%s WHERE asset_id='%lld' AND attribute_id='%lld';";
static const char* s_DeleteUnrenewedAssetAttribsSQL = "DELETE FROM asset_x_attribute WHERE asset_id=%lld AND attribute_id NOT IN (%s);";
static const char* s_DeleteAssetAttribsSQL = "DELETE FROM asset_x_attribute WHERE asset_id=%lld;";

//
// Init/Cleanup
//

/////////////////////////////////////////////////////////////////////////////
// Ctor - Initialiaze the AssetDB
AssetDB::AssetDB()
  : SQL::SQLiteDB()
{
  // Set m_RootDir
  std::string rootDir = Finder::ProjectAssets() + FinderSpecs::Project::ASSET_TRACKER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( rootDir );
  FileSystem::MakePath( rootDir );

  std::string assetTrackerConfig = FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder();
  FileSystem::StripPrefix( Finder::ProjectRoot(), assetTrackerConfig );

}


/////////////////////////////////////////////////////////////////////////////
// Dtor - Closes the AssetDB
AssetDB::~AssetDB()
{
}

/////////////////////////////////////////////////////////////////////////////
// Prepares all of the statements.
void AssetDB::PrepareStatements()
{
  Windows::TakeSection critSection( *m_GeneralCriticalSection );
  
  m_SqlSelectUsersComputerIdStatementHandle     = m_DBManager->CreateStatement( s_SelectUsersComputerIDSQL, "t" );
  m_SqlSelectP4IdStatementHandle                = m_DBManager->CreateStatement( s_SelectP4UserIDSQL, "t" );
  m_SqlSelectProjectIdStatementHandle           = m_DBManager->CreateStatement( s_SelectToolsProjectIDSQL, "t" );
  m_SqlSelectBranchIdStatementHandle            = m_DBManager->CreateStatement( s_SelectToolsBranchIDSQL, "t" );
  m_SqlSelectAssetRowIdStatementHandle          = m_DBManager->CreateStatement( s_SelectAssetFileRowIDSQL, "lll" );
  m_SqlSelectAssetLastUpdatedStatementHandle    = m_DBManager->CreateStatement( s_SelectAssetLastUpdatedSQL, "l" );

//  m_SqlDeleteUnrenewedUsagesStatementHandle     = m_DBManager->CreateStatement( s_DeleteUnrenewedUsagesSQL, "lt" );
//  m_SqlDeleteUsagesStatementHandle              = m_DBManager->CreateStatement( s_DeleteUsagesSQL, "l" );

  m_SqlSelectFileTypeIdStatementHandle          = m_DBManager->CreateStatement( s_SelectFileTypeIDSQL, "t" );
  m_SqlSelectEngineTypeIdStatementHandle        = m_DBManager->CreateStatement( s_SelectEngineTypeIDSQL, "t" );
  m_SqlSelectAttributeIdStatementHandle         = m_DBManager->CreateStatement( s_SelectAttributeIDSQL, "t" );

//  m_SqlReplaceAssetAttrStatementHandle          = m_DBManager->CreateStatement( s_ReplaceAssetAttribSQL, "llt" );
//  m_SqlDeleteUnrenewedAssetAttrStatementHandle  = m_DBManager->CreateStatement( s_DeleteUnrenewedAssetAttribsSQL, "lt" );
//  m_SqlDeleteAssetAttrStatementHandle           = m_DBManager->CreateStatement( s_DeleteAssetAttribsSQL, "l" );
  
}

/////////////////////////////////////////////////////////////////////////////
// Deletes the Cache DB and handled events list
void AssetDB::Delete()
{
  __super::Delete();
}



bool AssetDB::Initialize()
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  // Set m_RootDir
  std::string rootDir = Finder::ProjectAssets() + FinderSpecs::Project::ASSET_TRACKER_FOLDER.GetRelativeFolder();
  FileSystem::GuaranteeSlash( rootDir );
  FileSystem::MakePath( rootDir );

  //std::string assetTrackerConfig = FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder();
  //FileSystem::StripPrefix( Finder::ProjectRoot(), assetTrackerConfig );

  //m_DBManager = new SQL::SQLite();

  try
  {
    this->Open( FinderSpecs::Project::ASSET_TRACKER_DB.GetFile( rootDir ),
      FinderSpecs::Project::ASSET_TRACKER_CONFIGS.GetFolder(),
      FinderSpecs::Project::ASSET_TRACKER_DB.GetFormatVersion() );

    // m_DBManager->Open( FinderSpecs::Project::ASSET_TRACKER_DB.GetFile( rootDir ) );
  }
  catch( const Nocturnal::Exception& ex )
  {
    Console::Error( "Failed to open SQLite database %s, reason: %s\n", s_DBName, ex.what() );
  }
  catch ( ... )
  {
    Console::Error( "Failed to open SQLite database %s\n", s_DBName );
  }
  
  if ( !m_DBManager->IsOpen() )
  {
    return false;
  }

  PrepareStatements();

  m_ProjectID = GetProjectId();
  m_AssetBranchID = GetAssetBranchId();

  return true;
}

void AssetDB::RegisterTracker( AssetTracker* tracker ) 
{ 
  m_Tracker = tracker;
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetDB::InsertIfNotFound( SQL::StmtHandle select, const char* value, const char* insert, const char* update )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  NOC_ASSERT( select );

  if ( !m_DBManager->IsOpen() )
  {
    throw Nocturnal::Exception( "Lost DB connection" );
  }

  bool haveID = false;
  u64 id = 0;
  int execResult = m_DBManager->ExecStatement( select, value );
  if ( execResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnInt( select, 0, ( i32& ) id );
    m_DBManager->ResetStatement( select );
    haveID = true;
  }
  else if ( execResult != SQLITE_DONE && execResult != SQLITE_OK )
  {
    throw Nocturnal::Exception( "Failed to execute SQL: %s", select );
  }

  if ( !haveID && insert )
  {
    execResult = m_DBManager->ExecSQLVMPrintF( insert, id );
    if ( execResult == SQLITE_OK )
    {
      id = m_DBManager->GetLastInsertRowId();
    }
    else
    {
      throw Nocturnal::Exception( "Failed to execute SQL: %s", insert );
    }
  }
  else if ( haveID && update )
  {
    execResult = m_DBManager->ExecSQLVMPrintF( update, id );
    if ( execResult != SQLITE_OK )
    {
      throw Nocturnal::Exception( "Failed to execute SQL: %s", update );
    }
  }
  return id;
}

/////////////////////////////////////////////////////////////////////////////
// Queries the DB using a select statement that has a single text value
// Note: If it is not in the DB it and an insert statement has been passed in
//        this function will insert the value into the DB
//
u64 AssetDB::SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert )
{
  ASSETTRACKER_SCOPE_TIMER((""));

  NOC_ASSERT( select );

  if ( !m_DBManager->IsOpen() )
  {
    throw Nocturnal::Exception( "Lost DB connection" );
  }

  u64 id = 0;
  int execResult = m_DBManager->ExecStatement( select, value );
  if ( execResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnInt( select, 0, ( i32& ) id );
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
    sprintf_s( insertBuff, sizeof( insertBuff ), insert, value, value /*TODO: [ckosan] this needs to be a long name*/ );
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

/////////////////////////////////////////////////////////////////////////////
// Queries the DB using a select statement that has a 3 u64 ids
// Note: If it is not in the DB it and an insert statement has been passed in
//        this function will insert the value into the DB, also note that the 
//        insert statement must be completely filled out before being passed
//        into this function since it requires more params than we have at this
//        point.
//
u64 AssetDB::SelectAssetFileID( SQL::StmtHandle select, u64 fileId, u64 branchId, u64 projectId, const char* insert, const char* update )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  NOC_ASSERT( select );

  if ( !m_DBManager->IsOpen() )
  {
    throw Nocturnal::Exception( "Lost DB connection" );
  }

  u64 id = 0;
  int execResult = m_DBManager->ExecStatement( select, ( i64 ) fileId, ( i64 ) branchId, ( i64 ) projectId );
  if ( execResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnI64( select, 0, ( i64& ) id );
    m_DBManager->ResetStatement( select );
    if( update )
    {
      execResult = m_DBManager->ExecSQLVMPrintF( update, ( i64 ) id );
      if ( execResult != SQLITE_OK )
      {
        throw Nocturnal::Exception( "Failed to execute SQL: %s", update );
      }
    } 
  }
  else if ( execResult != SQLITE_DONE && execResult != SQLITE_OK )
  {
    throw Nocturnal::Exception( "Failed to execute SQL: %s", select );
  }
  else if ( insert )
  {
    execResult = m_DBManager->ExecSQLVMPrintF( insert, ( i64 ) id );
    if ( execResult == SQLITE_OK )
    {
      id = m_DBManager->GetLastInsertRowId();
    }
    else
    {
      throw Nocturnal::Exception( m_DBManager->GetLastErrMsg().c_str() );
    }
  }

  return id;
}

/////////////////////////////////////////////////////////////////////////////
// Insert attributes
//
void AssetDB::InsertAssetAttributes( AssetFile* assetFile )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  int execResult = SQLITE_OK;
  std::string validAttributeIDsStr;

  M_string::const_iterator attrItr = assetFile->GetAttributes().begin();
  M_string::const_iterator attrEnd = assetFile->GetAttributes().end();
  for ( ; attrItr != attrEnd; ++attrItr )
  {
    const std::string& attrName = attrItr->first;
    u64 attributeID = SelectIDByName( m_SqlSelectAttributeIdStatementHandle, attrName.c_str(), s_InsertAttributeSQL  );

    std::string attrValue = attrItr->second;    

    if ( attrValue.empty() )
    {
      attrValue = "NULL";
    }

//    if ( sizeof( queryAttrStr ) > attrValue.length() * 2 )
//    {
//      memset( attrValueBuff, '\0', sizeof( queryAttrStr ) );
////      mysql_real_escape_string( m_DBManager->GetDBHandle(), attrValueBuff, attrValue.c_str(), (unsigned long) attrValue.length() );
//      //sprintf_s( queryAttrStr, sizeof(queryAttrStr), s_InsertReportsEnvSQL, attrValueBuff );
//      attrValue = "'";
//      attrValue += attrValueBuff;
//      attrValue += "'";
//    }
//    else
//    {
//      //sprintf_s( queryAttrStr, sizeof(queryAttrStr), s_InsertReportsEnvSQL, "Environment too large" );
//      attrValue = "NULL";
//    }

    execResult = m_DBManager->ExecSQLVMPrintF( s_ReplaceAssetAttribSQL, ( i64 ) assetFile->GetRowID(), ( i64 ) attributeID, attrValue.c_str() );  
    if ( execResult != SQLITE_OK )
    {
      throw Nocturnal::Exception( "Failed to insert asset attribute, reason: %s\n", m_DBManager->GetLastErrMsg().c_str() );
    }

    // append to the list of valid IDs
    if ( !validAttributeIDsStr.empty() )
    {
      validAttributeIDsStr += ",";
    }

    std::stringstream idStr;
    idStr << "'" << attributeID << "'";  
    validAttributeIDsStr += idStr.str();
  }

  // Delete asset usages that are not in the given set of attributeIDs,
  if ( validAttributeIDsStr.empty() )
  {
    execResult = m_DBManager->ExecSQLVMPrintF( s_DeleteAssetAttribsSQL, assetFile->GetRowID() );  
  }
  else
  {
    execResult = m_DBManager->ExecSQLVMPrintF( s_DeleteUnrenewedAssetAttribsSQL, assetFile->GetRowID(), validAttributeIDsStr.c_str() );  
  }

  if ( execResult != SQLITE_OK )
  {
    throw Nocturnal::Exception( "Failed to delete legacy asset attributes, reason: %s\n", m_DBManager->GetLastErrMsg().c_str() );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Insert a single shader usage rowrow into the AssetDependenciesDB
void AssetDB::InsertAssetUsages( AssetFile* assetFile, M_AssetFiles* assetFiles )
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  int execResult = SQLITE_OK;

  // build up the string for checking which ids should be in the given graph
  std::string validDependencyIDsStr;

  const u64& assetRowID = assetFile->GetRowID();

  validDependencyIDsStr.clear();
  if ( !assetFile->GetDependencyIDs().empty() )
  {
    S_tuid::const_iterator itrDependency = assetFile->GetDependencyIDs().begin();
    S_tuid::const_iterator endDependency = assetFile->GetDependencyIDs().end();
    for( ; itrDependency != endDependency; ++itrDependency )
    {
      const tuid& dependencyID = (*itrDependency);

      if ( assetFiles->find( dependencyID ) == assetFiles->end() )
      {
        // We haven't yet loaded the dependency so track it first   
        m_Tracker->TrackFile( dependencyID );

        File::ManagedFilePtr file = File::GlobalManager().GetManagedFile( dependencyID );
        AssetFilePtr dependency = new AssetFile( file );
        dependency->SetRowID( SelectAssetFileID( m_SqlSelectAssetRowIdStatementHandle, dependency->GetFileID(), m_ProjectID, m_AssetBranchID ) );
        assetFiles->insert( M_AssetFiles::value_type( dependencyID, dependency ) );
      }

      u64 dependencyRowID = (*assetFiles)[dependencyID]->GetRowID();

      if ( dependencyRowID == 0 )
      {
        m_Tracker->TrackFile( dependencyID );
        (*assetFiles)[dependencyID]->SetRowID( SelectAssetFileID( m_SqlSelectAssetRowIdStatementHandle, (*assetFiles)[dependencyID]->GetFileID(), m_ProjectID, m_AssetBranchID ) );

        dependencyRowID = (*assetFiles)[dependencyID]->GetRowID();
        //InsertAssetFile( (*assetFiles)[dependencyID], assetFiles );
        //NOC_BREAK();
      }

      // insert into the db
      execResult = m_DBManager->ExecSQLVMPrintF( s_ReplaceUsagesSQL, assetRowID, dependencyRowID );  
      if ( execResult != SQLITE_OK )
      {
        throw Nocturnal::Exception( "Failed to insert file usage, reason: %s\n", m_DBManager->GetLastErrMsg().c_str() );
      }

      // append to the list of valid IDs
      if ( !validDependencyIDsStr.empty() )
      {
        validDependencyIDsStr += ",";
      }

      std::stringstream idStr;
      idStr << "'" << dependencyRowID << "'";  
      validDependencyIDsStr += idStr.str();
    }
  }

  // Delete asset usages that are not in the given set of dependencyIDs,
  if ( validDependencyIDsStr.empty() )
  {
    execResult = m_DBManager->ExecSQLVMPrintF( s_DeleteUsagesSQL, assetRowID );  
  }
  else
  {
    execResult = m_DBManager->ExecSQLVMPrintF( s_DeleteUnrenewedUsagesSQL, assetRowID, validDependencyIDsStr.c_str() );  
  }

  if ( execResult != SQLITE_OK )
  {
    throw Nocturnal::Exception( "Failed to delete legacy file usage, reason: %s\n", m_DBManager->GetLastErrMsg().c_str() );
  }
}

/////////////////////////////////////////////////////////////////////////////
void AssetDB::InsertAssetFile( AssetFile* assetFile, M_AssetFiles* assetFiles )
{
  ASSETTRACKER_SCOPE_TIMER((""));

  u64 p4UserID      = SelectIDByName( m_SqlSelectP4IdStatementHandle, assetFile->GetP4User().c_str(), s_InsertP4UserSQL );
  u64 engineTypeID  = SelectIDByName( m_SqlSelectEngineTypeIdStatementHandle, assetFile->GetEngineTypeName().c_str(), s_InsertEngineTypeSQL );
  u64 fileTypeID    = SelectIDByName( m_SqlSelectFileTypeIdStatementHandle, assetFile->GetFileType().c_str(), s_InsertFileTypeSQL );

  std::string relativeFilePath = assetFile->GetRelativePath();

  //static char queryBuff[MAX_QUERY_LENGTH];
  //memset( queryBuff, '\0', MAX_QUERY_LENGTH );
//  mysql_real_escape_string( m_DBManager->GetDBHandle(), queryBuff, relativeFilePath.c_str(), (unsigned long) relativeFilePath.length() );
  //relativeFilePath = queryBuff;

  char* insertBuff = sqlite3_mprintf( 
    s_InsertAssetFileSQL,
    (i64) assetFile->GetFileID(),
    (i64) m_ProjectID,
    (i64) m_AssetBranchID,
    relativeFilePath.c_str(),
    assetFile->GetShortName().c_str(),
    (i64) fileTypeID,
    (i64) p4UserID,
    (i64) engineTypeID,
    (i64) assetFile->GetSize() );

  char* updateBuff = sqlite3_mprintf( 
    s_UpdateAssetFileSQL,
    relativeFilePath.c_str(),
    assetFile->GetShortName().c_str(),
    (i64) fileTypeID,
    (i64) p4UserID,
    (i64) engineTypeID,
    (i64) assetFile->GetSize(),
    (i64) assetFile->GetFileID(),
    (i64) m_ProjectID,
    (i64) m_AssetBranchID );

  // insert the asset
  u64 rowID = SelectAssetFileID( m_SqlSelectAssetRowIdStatementHandle, assetFile->GetFileID(), m_ProjectID, m_AssetBranchID, insertBuff, updateBuff );
  assetFile->SetRowID( rowID );

  InsertAssetAttributes( assetFile );
  InsertAssetUsages( assetFile, assetFiles );

  sqlite3_free( updateBuff );
  sqlite3_free( insertBuff );
}


/////////////////////////////////////////////////////////////////////////////
u64 AssetDB::GetProjectId()
{
  ASSETTRACKER_SCOPE_TIMER((""));
  
  std::string projectName;
  char        buf[MAX_PATH];

  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( NOCTURNAL_STUDIO_PREFIX"PROJECT_NAME", buf, MAX_PATH );
  projectName = buf;

  return SelectIDByName( m_SqlSelectProjectIdStatementHandle, projectName.c_str(), s_InsertToolsProjectSQL );
}

/////////////////////////////////////////////////////////////////////////////
u64 AssetDB::GetAssetBranchId()
{
  ASSETTRACKER_SCOPE_TIMER((""));

  std::string assetBranch;
  char        buf[MAX_PATH];

  ZeroMemory( buf, MAX_PATH );
  GetEnvironmentVariable( NOCTURNAL_STUDIO_PREFIX"ASSETS_BRANCH_NAME", buf, MAX_PATH );
  assetBranch = buf;
  FileSystem::CleanName( assetBranch );

  return SelectIDByName( m_SqlSelectBranchIdStatementHandle, assetBranch.c_str(), s_InsertToolsBranchSQL );
}

/////////////////////////////////////////////////////////////////////////////
bool AssetDB::AssetHasChangedOnDisc( const File::ManagedFilePtr& file )
{
  FILE_SCOPE_TIMER((""));

  bool ret = false;

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectAssetLastUpdatedStatementHandle, (i64) file->m_Id );

  if ( sqlResult == SQLITE_DONE )
  {
    ret = true;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    ret = false;
  }

  // Get the timestamp from the column and check to see if the last modified time on disc is more recent than in the DB
  if ( sqlResult == SQLITE_ROW )
  {
    __time64_t dbModifiedTime;

    m_DBManager->GetColumnI64( m_SqlSelectAssetLastUpdatedStatementHandle, 0, dbModifiedTime );

    if ( FileSystem::Exists( file->m_Path )
      && FileSystem::UpdatedSince( file->m_Path, dbModifiedTime ) )
    {
      ret = true;
    }
  
    m_DBManager->ResetStatement( m_SqlSelectAssetLastUpdatedStatementHandle );
  }

  return ret;
}