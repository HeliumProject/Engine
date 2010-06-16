#pragma once

#include "API.h"
#include "Exceptions.h"
#include "FileInfo.h"

#include <vector>

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Application/SQL/SQLite/SQLiteDB.h"

namespace Dependencies
{ 
  class GraphDB : public SQL::SQLiteDB
  {
  public:
    GraphDB( const char* friendlyName );
    virtual ~GraphDB();

    virtual bool Open( const std::string& dbFilename, const std::string& configFolder, const std::string& version = std::string( "" ) );

    i64   InsertVersion( const Finder::FileSpec& fileSpec, const Finder::FormatVersion formatVersion = "" );
    i64   SelectVersionId( const Finder::FileSpec& fileSpec, const Finder::FormatVersion formatVersion = "" );
    void  DeleteInvalidVersion( const Finder::FileSpec& fileSpec );

    i64   ReplaceDependency( const DependencyInfoPtr& file, const i64 versionId );
    i64   SelectDependency( const std::string& filePath, const DependencyInfoPtr& file );
    i64   SelectDependency( const DependencyInfoPtr& file );
    i64   SelectDependencyBySig( const std::string& signature, const DependencyInfoPtr& file );
    
    void  InsertGraph( const i64 outFileId, const i64 inFileId, const i64 inFileLastModified, const i64 inFileOrderIndex = 0, const bool inFileIsOptional = false, const bool inFileExistedLastBuild = true );
    void  SelectGraph( const DependencyInfoPtr& outFile, OS_DependencyInfo &listOfFiles );
    void  DeleteGraphPairs( const i64 outFileId, const std::string& inFileIdIds );

    friend class Dependencies;    

  protected:

    //
    // SQLiteDB overrides functions
    //

    virtual void PrepareStatements() NOC_OVERRIDE;

  public:
    static const char* s_GraphDBVersion;

  private:

    //
    // Members
    //

    SQL::StmtHandle   m_SqlSelectVersionIdStmtHandle;
    SQL::StmtHandle   m_SqlSelectDependencyRowIdByPathStmtHandle;
    SQL::StmtHandle   m_SqlSelectDependencyBySigStmtHandle;
    SQL::StmtHandle   m_SqlSelectDependencyStmtHandle;
    SQL::StmtHandle   m_SqlSelectGraphRowIdStmtHandle;
    SQL::StmtHandle   m_SqlSelectGraphStmtHandle;   
    
    //
    // Helper functions
    //

    i64   InsertDependency( const DependencyInfoPtr& file, const i64 versionId );

    i64   StepSelectRowId( int sqlResult, SQL::StmtHandle stmt );
    bool  StepSelectDependency( int sqlResult, const SQL::StmtHandle stmt, const DependencyInfoPtr& file, bool resetStmt = true, bool getGraphInfo = false );

    i64   SelectDependencyRowIdByPath( const DependencyInfoPtr& file );
    i64   SelectGraphRowId( const i64 outFileId, const i64 inFileId, const i64 inFileLastModified = 0 );

  };

  typedef Nocturnal::SmartPtr< GraphDB > GraphDBPtr;

}