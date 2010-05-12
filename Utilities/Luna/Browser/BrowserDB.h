#pragma once

#include "API.h"
#include "AssetFile.h"

#include "SQL/SQLiteDB.h"
#include "TUID/TUID.h"


namespace Asset
{
  class AssetTracker;

  class FILE_API AssetDB : public SQL::SQLiteDB
  {
  public:
    AssetDB();
    virtual ~AssetDB();

    bool Initialize();
    void InsertAssetFile( AssetFile* assetFile, M_AssetFiles* assetFiles );
    void InsertAssetAttributes( AssetFile* assetFile );
    void InsertAssetUsages( AssetFile* assetFile, M_AssetFiles* assetFiles );

    void RegisterTracker( AssetTracker* tracker );

    u64 InsertIfNotFound( SQL::StmtHandle select, const char* value, const char* insert, const char* update = NULL );
    u64 SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert = NULL );
    u64 SelectAssetFileID( SQL::StmtHandle select, u64 fileId, u64 branchId, u64 projectId, const char* insert = NULL, const char* update = NULL );

    bool AssetHasChangedOnDisc( const File::ManagedFilePtr& file );

  protected:
    // SQLiteDB overrides functions
    virtual void PrepareStatements() NOC_OVERRIDE;
    virtual void Delete();

  private:
    //SQL::StmtHandle   m_SqlSelectIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectUsersComputerIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectP4IdStatementHandle;
    SQL::StmtHandle   m_SqlSelectProjectIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectBranchIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectAssetRowIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectAssetLastUpdatedStatementHandle;

    SQL::StmtHandle   m_SqlDeleteUnrenewedUsagesStatementHandle;
    SQL::StmtHandle   m_SqlDeleteUsagesStatementHandle;

    SQL::StmtHandle   m_SqlSelectFileTypeIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectEngineTypeIdStatementHandle;
    SQL::StmtHandle   m_SqlSelectAttributeIdStatementHandle;

    SQL::StmtHandle   m_SqlReplaceAssetAttrStatementHandle;
    SQL::StmtHandle   m_SqlDeleteUnrenewedAssetAttrStatementHandle;
    SQL::StmtHandle   m_SqlDeleteAssetAttrStatementHandle;

    u64 GetProjectId();
    u64 GetAssetBranchId();

    AssetTracker* m_Tracker;

    u64 m_AssetBranchID;
    u64 m_ProjectID;
  };

  typedef FILE_API Nocturnal::SmartPtr< AssetDB > AssetDBPtr;
}
