#include "Platform/Windows/Windows.h"
#undef ReplaceDependency

#include "GraphDB.h"

#include "Foundation/Container/OrderedSet.h"
#include "Foundation/Log.h"
#include "Finder/ProjectSpecs.h"
#include "SQL/SQLite.h"
#include "Foundation/Exception.h"

#include <sqlite3.h>

using namespace Finder;
using namespace Dependencies;

const char* GraphDB::s_GraphDBVersion = "6.0";

///////////////////////////////////////////////
// Version Table SQL
static const char* s_SelectVersionIdSQL = "SELECT id FROM version WHERE spec_name=? AND version=?;";
static const char* s_InsertVersionSQL   = "INSERT INTO version (spec_name,version) VALUES('%q','%q');";

static const char* s_DeleteInvalidVersionSQL =
  "BEGIN TRANSACTION; \n \
  DROP TABLE IF EXISTS temp.del_file; \n \
  CREATE TEMP TABLE temp.del_file AS \n \
  SELECT DISTINCT fc.id \n \
    FROM file     AS fc \n \
    JOIN version  AS ver ON ver.id = fc.version_id \n \
    WHERE ver.spec_name='%q'  \n \
      AND ver.version='%q';   \n \
  DROP TABLE IF EXISTS temp.del_graph; \n \
  CREATE TEMP TABLE temp.del_graph AS \n \
  SELECT DISTINCT gc.id \n \
    FROM graph    AS gc \n \
    JOIN del_file AS dfc ON dfc.id = gc.out_file_id OR dfc.id = gc.in_file_id; \n \
  DELETE FROM file  \n \
   WHERE file->id  IN ( SELECT id FROM del_file ); \n \
  DELETE FROM graph \n \
   WHERE graph.id IN ( SELECT id FROM del_graph ); \n \
  COMMIT TRANSACTION;";


///////////////////////////////////////////////
// File Table SQL

static const char* s_SelectDependencyRowIdByPathSQL = "SELECT id FROM file WHERE path=?;";

static const char* s_SelectDependencyBySigSQL =
  "SELECT fc.id,fc.path,ver.id,ver.spec_name,ver.version,fc.is_leaf,fc.input_order_matters,fc.last_modified,fc.size,fc.md5,fc.signature \n \
    FROM file    AS fc \n \
    JOIN version AS ver ON ver.id = fc.version_id \n \
   WHERE fc.signature=?;";

static const char* s_SelectDependencySQL   = 
  "SELECT fc.id,fc.path,ver.id,ver.spec_name,ver.version,fc.is_leaf,fc.input_order_matters,fc.last_modified,fc.size,fc.md5,fc.signature \n \
    FROM file    AS fc \n \
    JOIN version AS ver ON ver.id = fc.version_id \n \
   WHERE fc.path=?;";

static const char* s_InsertFileSQL   = "INSERT INTO file (path,version_id,is_leaf,input_order_matters,last_modified,size,md5,signature) VALUES('%q','%lld','%d','%d','%lld','%lld','%q','%q');";
static const char* s_UpdateFileSQL =
  "UPDATE file \n \
      SET version_id='%lld', \n \
          is_leaf='%d', \n \
          input_order_matters='%d', \n \
          last_modified='%lld', \n \
          size='%lld', \n \
          md5='%q', \n \
          signature='%q' \n \
    WHERE path='%q';";


///////////////////////////////////////////////
// FileGraph Table SQL

static const char* s_SelectGraphRowIdSQL = "SELECT id FROM graph WHERE out_file_id=? AND in_file_id=? AND in_last_modified=?;";
static const char* s_RelaceGraphSQL   = "REPLACE INTO graph (out_file_id,in_file_id,in_last_modified,in_file_order_index, in_can_be_missing, in_file_existed) VALUES('%lld','%lld','%lld','%lld','%d','%d');";

///////////////
// select a dependency
static const char* s_SelectGraphSQL =
"SELECT in_fc.id, \
       in_fc.path, \
       in_v.id, \
       in_v.spec_name, \
       in_v.version, \
       in_fc.is_leaf, \
       in_fc.input_order_matters, \
       in_fc.last_modified, \
       in_fc.size, \
       in_fc.md5, \
       in_fc.signature, \
       gc.in_last_modified, \
       gc.in_can_be_missing, \
       gc.in_file_existed, \
       out_fc.path \
  FROM graph   as gc \
  JOIN file    as in_fc  ON in_fc.id  = gc.in_file_id \
  JOIN file    as out_fc ON out_fc.id = gc.out_file_id \
  JOIN version as in_v   ON in_v.id   = in_fc.version_id \
  JOIN version as out_v  ON out_v.id  = out_fc.version_id \
 WHERE out_fc.path=? \
   AND out_v.spec_name=? \
   AND out_v.version=? \
ORDER BY gc.in_file_order_index;";

static const char* s_DeleteGraphPairsSQL = "DELETE FROM graph WHERE out_file_id='%lld' AND in_file_id NOT IN (%s);";




/////////////////////////////////////////////////////////////////////////////
// Ctor
GraphDB::GraphDB( const char* friendlyName )
: SQL::SQLiteDB( friendlyName )
{
}


/////////////////////////////////////////////////////////////////////////////
// Dtor
GraphDB::~GraphDB()
{
}


/////////////////////////////////////////////////////////////////////////////
// Opens and Load the  DB; creating the DB if it does not exist.
bool GraphDB::Open( const std::string& dbFilename, const std::string& configFolder, const std::string& version )
{
  Platform::TakeMutex mutex ( m_Mutex );

  Nocturnal::Path path( dbFilename );
  path.Create();

  return __super::Open( dbFilename, configFolder, version );
}

/////////////////////////////////////////////////////////////////////////////
// Prepares all of the statements.
void GraphDB::PrepareStatements()
{
  DEPENDENCIES_SCOPE_TIMER((""));

  m_SqlSelectVersionIdStmtHandle          = m_DBManager->CreateStatement( s_SelectVersionIdSQL, "tt" );
  m_SqlSelectDependencyRowIdByPathStmtHandle    = m_DBManager->CreateStatement( s_SelectDependencyRowIdByPathSQL, "t" );
  m_SqlSelectDependencyBySigStmtHandle          = m_DBManager->CreateStatement( s_SelectDependencyBySigSQL, "t" );
  m_SqlSelectDependencyStmtHandle               = m_DBManager->CreateStatement( s_SelectDependencySQL, "t" );
  m_SqlSelectGraphRowIdStmtHandle         = m_DBManager->CreateStatement( s_SelectGraphRowIdSQL, "lll" );
  m_SqlSelectGraphStmtHandle              = m_DBManager->CreateStatement( s_SelectGraphSQL, "ttt" );
}



//*************************************************************************//
//
//  API FUNCTIONS
//
//*************************************************************************//



/////////////////////////////////////////////////////////////////////////////
// Insert a single version table row into the GraphDB
i64 GraphDB::InsertVersion( const Finder::FileSpec& fileSpec, const FormatVersion formatVersion )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  // return the current rowID if it is already in the graph
  i64 rowID = SelectVersionId( fileSpec, formatVersion );
  if ( rowID != SQL::InvalidRowID )
  {
    return rowID;
  }

  if ( ( m_DBManager->ExecSQLVMPrintF( s_InsertVersionSQL, fileSpec.GetName().c_str(), formatVersion.c_str() ) != SQLITE_OK )
    && !m_DBManager->IsNotUniqueErr() )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  rowID = SelectVersionId( fileSpec, formatVersion );
  return rowID;
}


/////////////////////////////////////////////////////////////////////////////
// Invalidate an old FormatVersion and it's graph DB, by removing these entries from the GraphDB
void GraphDB::DeleteInvalidVersion( const Finder::FileSpec& fileSpec )
{
  if ( m_DBManager->ExecSQLVMPrintF( s_DeleteInvalidVersionSQL, fileSpec.GetName() ) != SQLITE_OK )
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
}


/////////////////////////////////////////////////////////////////////////////
// Insert a single file table row into the GraphDB
i64 GraphDB::InsertDependency( const DependencyInfoPtr& file, const i64 versionId )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  if ( versionId == SQL::InvalidRowID )
    throw Exception( "Invalid version row id passed to InsertFile, versionId: %I64d", versionId );

  if ( file->m_Path.empty() )
    throw Exception( "Cannot InsertFile a file with an empty path." );

  // return the current row Id if it is already in the graph
  i64 rowId = SelectDependencyRowIdByPath( file );
  if ( rowId != SQL::InvalidRowID )
  {
    return rowId;
  }

  // do not throw on IsNotUniqueErr, let the caller check the result
  if ( ( m_DBManager->ExecSQLVMPrintF(
    s_InsertFileSQL,
    file->m_Path.c_str(),
    versionId,
    (int) ( file->IsLeaf() ? 1 : 0 ),
    (int) ( file->InputOrderMatters() ? 1 : 0 ),
    (i64) file->m_LastModified,
    (i64) file->m_Size,
    file->m_MD5.c_str(),
    file->m_Signature.c_str() ) != SQLITE_OK )
    && !m_DBManager->IsNotUniqueErr() )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  rowId = SelectDependencyRowIdByPath( file );
  return rowId;
}


/////////////////////////////////////////////////////////////////////////////
// Insert a single file table row into the GraphDB
i64 GraphDB::ReplaceDependency( const ::Dependencies::DependencyInfoPtr& file, const i64 versionId )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  if ( versionId == SQL::InvalidRowID )
    throw Exception( "Invalid version row id passed to ReplaceDependency, versionId: %I64d", versionId );

  if ( file->m_Path.empty() )
    throw Exception( "Cannot ReplaceDependency a file with an empty path." );

  i64 rowID = SelectDependencyRowIdByPath( file );

  // INSERT a new record if it doesn't exist - return the NEW rowID
  // do not throw on IsNotUniqueErr, let the caller check the result
  if ( rowID == SQL::InvalidRowID )
  {
    if ( ( m_DBManager->ExecSQLVMPrintF(
      s_InsertFileSQL,
      file->m_Path.c_str(),
      versionId,
      (int) ( file->IsLeaf() ? 1 : 0 ),
      (int) ( file->InputOrderMatters() ? 1 : 0 ),
      (i64) file->m_LastModified,
      (i64) file->m_Size,
      file->m_MD5.c_str(),
      file->m_Signature.c_str() ) != SQLITE_OK )
      && !m_DBManager->IsNotUniqueErr() )
    {
      throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
    }

    rowID = SelectDependencyRowIdByPath( file );
  }
  // UPDATE the existing entry - return the current rodID
  else
  {     
    if ( m_DBManager->ExecSQLVMPrintF(
      s_UpdateFileSQL,
      versionId,
      (int) ( file->IsLeaf() ? 1 : 0 ),
      (int) ( file->InputOrderMatters() ? 1 : 0 ),
      (i64) file->m_LastModified,
      (i64) file->m_Size,
      file->m_MD5.c_str(),
      file->m_Signature.c_str(),
      file->m_Path.c_str() ) != SQLITE_OK )
    {
      throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
    }
  }

  return rowID;
}


/////////////////////////////////////////////////////////////////////////////
// Insert a single graph table row into the GraphDB
void GraphDB::InsertGraph
( 
 const i64 outFileId, 
 const i64 inFileId, 
 const i64 inFileLastModified, 
 const i64 inFileOrderIndex, 
 const bool inFileIsOptional, 
 const bool inFileExistedLastBuild 
 )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  if ( outFileId == SQL::InvalidRowID || inFileId == SQL::InvalidRowID )
  {
    throw Exception( "Invalid row id passed to InsertGraph, outFileId: %I64d  inFileId: %I64d ",
      outFileId, inFileId );
  }

  if ( m_DBManager->ExecSQLVMPrintF( s_RelaceGraphSQL, outFileId, inFileId, inFileLastModified, inFileOrderIndex, inFileIsOptional, inFileExistedLastBuild ) != SQLITE_OK )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}


/////////////////////////////////////////////////////////////////////////////
// Removes graph entries that are not in the list of graphIds
void GraphDB::DeleteGraphPairs( const i64 outFileId, const std::string& inFileIdIds )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  if ( m_DBManager->ExecSQLVMPrintF( s_DeleteGraphPairsSQL, outFileId, inFileIdIds.c_str() ) != SQLITE_OK )
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
}




/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::StepSelectRowId( int sqlResult, SQL::StmtHandle stmt )
{
  DEPENDENCIES_SCOPE_TIMER((""));

  int rowID = SQL::InvalidRowID;

  if ( sqlResult == SQLITE_ROW )
  {
    m_DBManager->GetColumnInt( stmt, 0, ( int ) rowID );
    m_DBManager->ResetStatement( stmt );
  }
  else if ( sqlResult != SQLITE_DONE )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  return rowID;
}


/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::SelectVersionId( const Finder::FileSpec& fileSpec, const FormatVersion formatVersion )
{
  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectVersionIdStmtHandle, fileSpec.GetName().c_str(), formatVersion.c_str() );
  return StepSelectRowId( sqlResult, m_SqlSelectVersionIdStmtHandle );
}



/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::SelectDependencyRowIdByPath( const DependencyInfoPtr& file )
{
  if ( file->m_Path.empty() )
    throw Exception( "Invalid path passed to SelectDependencyRowIdByPath" );

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectDependencyRowIdByPathStmtHandle, file->m_Path.c_str() );
  return StepSelectRowId( sqlResult, m_SqlSelectDependencyRowIdByPathStmtHandle );
}


/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::SelectGraphRowId( const i64 outFileId, const i64 inFileId, const i64 inFileLastModified )
{
  if ( outFileId == SQL::InvalidRowID || inFileId == SQL::InvalidRowID )
    throw Exception( "Invalid row id passed to SelectGraphRowId, outFileId: %I64d  inFileId: %I64d  inFileLastModified: %I64d",
    outFileId, inFileId, inFileLastModified );

  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectGraphRowIdStmtHandle, outFileId, inFileId, inFileLastModified );
  return StepSelectRowId( sqlResult, m_SqlSelectGraphRowIdStmtHandle );
}


/////////////////////////////////////////////////////////////////////////////
// Returns true if a file was selected
//
bool GraphDB::StepSelectDependency( int sqlResult, const SQL::StmtHandle stmt, const DependencyInfoPtr& file, bool resetStmt, bool getGraphInfo )
{    
  DEPENDENCIES_SCOPE_TIMER((""))

    int rowID = SQL::InvalidRowID;

  if ( sqlResult == SQLITE_DONE )
  {
    return false;
  }
  else if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
  else if ( sqlResult == SQLITE_ROW )
  {
    int index = 0;
    m_DBManager->GetColumnI64(  stmt,   0,       ( i64 &)     file->m_RowID );

    std::string path;
    m_DBManager->GetColumnText( stmt,   ++index,              path );
    file->m_Path.Set( path );

    m_DBManager->GetColumnI64(  stmt,   ++index, ( i64 &)     file->m_VersionRowID );
    m_DBManager->GetColumnText( stmt,   ++index,              file->m_SpecName );

    try
    {
      file->m_Spec = Finder::GetFileSpec( file->m_SpecName );
    }
    catch( const Finder::Exception & )
    {
      // this file's FileSpec has been removed from the system since being inserted into the DB
      Log::Print( Log::Levels::Verbose, "Finder::GetFileSpec failed for FinderSpec \"%s\", file %s\n", file->m_SpecName.c_str(), file->m_Path.c_str() );
      file->m_Spec = NULL;
    }

    m_DBManager->GetColumnText( stmt,   ++index,              file->m_FormatVersion );

    int isLeaf;
    m_DBManager->GetColumnInt( stmt,    ++index, ( int &)     isLeaf );
    file->IsLeaf( ( isLeaf == 1 ) ? true : false ) ;

    int InputOrderMatters;
    m_DBManager->GetColumnInt( stmt,    ++index, ( int &)     InputOrderMatters );
    file->InputOrderMatters( ( InputOrderMatters == 1 ) ? true : false ) ;

    m_DBManager->GetColumnI64( stmt,    ++index, ( i64 &)     file->m_LastModified );
    m_DBManager->GetColumnI64( stmt,    ++index, ( i64 &)     file->m_Size );
    m_DBManager->GetColumnText( stmt,   ++index,              file->m_MD5 );
    m_DBManager->GetColumnText( stmt,   ++index,              file->m_Signature );

    // get graph.in_last_modified time
    if ( getGraphInfo )//&& m_DBManager->GetColumnCount( stmt ) == 13 )
    {
      i64 inFileLastModified; 
      m_DBManager->GetColumnI64( stmt,  ++index,              inFileLastModified );

      int inFileIsOptionalInt;
      m_DBManager->GetColumnInt( stmt,  ++index, ( int &)     inFileIsOptionalInt );
      bool inFileIsOptional = ( ( inFileIsOptionalInt == 1 ) ? true : false ) ;

      int inFileExistedInt;
      m_DBManager->GetColumnInt( stmt,  ++index, ( int &)     inFileExistedInt );
      bool inFileExisted = ( ( inFileExistedInt == 1 ) ? true : false ) ;

      std::string outFilePath;
      m_DBManager->GetColumnText( stmt, ++index,              outFilePath );

      file->m_GraphInfo.insert( M_GraphInfo::value_type( outFilePath, GraphInfo( inFileLastModified, inFileIsOptional, inFileExisted )  ) );
    }

    if ( resetStmt )
    {
      m_DBManager->ResetStatement( stmt );
    }

    return true;
  }

  return false;
}


/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::SelectDependency( const std::string& filePath, const DependencyInfoPtr& file )
{
  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectDependencyStmtHandle, filePath.c_str() );
  if ( StepSelectDependency( sqlResult, m_SqlSelectDependencyStmtHandle, file ) )
  {
    return file->m_RowID;
  }

  return SQL::InvalidRowID;
}

/////////////////////////////////////////////////////////////////////////////
i64 GraphDB::SelectDependency( const DependencyInfoPtr& file )
{
  return SelectDependency( file->m_Path, file );
}

/////////////////////////////////////////////////////////////////////////////
// Should we determine if the file has changed on disk
i64 GraphDB::SelectDependencyBySig( const std::string& signature, const DependencyInfoPtr& file )
{
  int sqlResult = m_DBManager->ExecStatement( m_SqlSelectDependencyBySigStmtHandle, signature.c_str() );

  if ( StepSelectDependency( sqlResult, m_SqlSelectDependencyBySigStmtHandle, file ) )
  {
    return file->m_RowID;
  }

  return SQL::InvalidRowID;
}



/////////////////////////////////////////////////////////////////////////////
void GraphDB::SelectGraph( const DependencyInfoPtr& outFile, OS_DependencyInfo &listOfFiles )
{
  DEPENDENCIES_SCOPE_TIMER((""))

    int sqlResult;
  {
    DEPENDENCIES_SCOPE_TIMER(("SQLite Statement Execution"));
    sqlResult = m_DBManager->ExecStatement( m_SqlSelectGraphStmtHandle, outFile->m_Path.c_str(), outFile->m_SpecName.c_str(), outFile->m_FormatVersion.c_str() );
  }

  // if no files were found return
  if ( sqlResult == SQLITE_DONE )
  {
    return;
  }

  // if an unexpected error occurred throw an exception
  if ( sqlResult != SQLITE_ROW )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }

  // Step through one or more entries, and pull the data from the columns
  while ( sqlResult == SQLITE_ROW )
  {
    DependencyInfoPtr inFile = new FileInfo;

    // get the inFile
    StepSelectDependency( sqlResult, m_SqlSelectGraphStmtHandle, inFile, false, true );

    listOfFiles.Append( inFile );

    sqlResult = m_DBManager->StepStatement( m_SqlSelectGraphStmtHandle );
  }

  // if an unexpected error occurred throw an exception
  if ( sqlResult != SQLITE_DONE )
  {
    throw SQL::DBManagerException( m_DBManager, __FUNCTION__ );
  }
}
