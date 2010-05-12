#pragma once

#include "API.h"

#include "CacheDBColumn.h"
#include "CacheDBQuery.h"

#include "File/ManagedFile.h"
#include "SQL/SQLiteDB.h"
#include "TUID/TUID.h"


namespace Asset
{
  //
  // Forwards
  //
  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::vector< AssetFilePtr > V_AssetFiles;
  typedef std::map< tuid, AssetFilePtr > M_AssetFiles;

  /////////////////////////////////////////////////////////////////////////////
  ASSET_API typedef std::map< u64, std::string > M_CacheDBTableData;

  /////////////////////////////////////////////////////////////////////////////
  class CacheDB;
  ASSET_API typedef Nocturnal::SmartPtr< CacheDB > CacheDBPtr;

  ASSET_API class CacheDB* GlobalCacheDB();

  /////////////////////////////////////////////////////////////////////////////
  /// class CacheDB
  /////////////////////////////////////////////////////////////////////////////
  class ASSET_API CacheDB : public SQL::SQLiteDB
  {
  public:
    CacheDB( const char* friendlyName );
    virtual ~CacheDB();

    // Used to initialize the Global
    static void Initialize();
    static void Cleanup();

    bool Open( std::string& dbFilename, const std::string& configFolder, const std::string& version = std::string( "" ), int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE );
    virtual bool Load() NOC_OVERRIDE;

    const M_CacheDBColumns& GetDBColumns() const;

    u32 GetPopulateTableData( const CacheDBColumnID columnID, V_string& tableData, bool* cancel = NULL ); 

    u32 Search( const CacheDBQuery* search, V_AssetFiles& assetFiles, bool* cancel = NULL );
    u32 Search( const CacheDBQuery* search, S_tuid& assetFileIDs, bool* cancel = NULL );

    bool HasAssetChangedOnDisc( const File::ManagedFilePtr& file, bool* cancel = NULL );
    void InsertAssetFile( AssetFile* assetFile, M_AssetFiles* assetFiles, S_tuid& visitedIDs, bool* cancel = NULL );
    void DeleteAssetFile( AssetFile* assetFile );

    void SelectAssetPathByID( const tuid id, std::string& path );
    void SelectAssetByID( const tuid id, AssetFile* assetFile );

    u64 FindAttributeRowID( const std::string& value );
    u32 GetAttributesTableData( V_string& tableData, bool* cancel = NULL );

    void GetAssetDependencies( const tuid id, S_tuid& dependencies, bool reverse = false, u32 maxDepth = 0, u32 currDepth = 0, bool* cancel = NULL );
    void GetDependencyGraph( const tuid id, M_AssetFiles* assetFiles, bool reverse = false, u32 maxDepth = 0, u32 currDepth = 0, bool* cancel = NULL );

    static void CleanExpressionForSQL( std::string& argument, bool wrapEscape = true );

  protected:
    // SQLiteDB overrides functions
    virtual void PrepareStatements() NOC_OVERRIDE;

  private:
    // Used by Tracker
    u64 SelectAssetRowID( u64 fileId, const char* insert = NULL, const char* update = NULL, bool* cancel = NULL );
    u64 SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert = NULL, bool* cancel = NULL );

    void InsertAssetAttributes( AssetFile* assetFile, bool* cancel = NULL );
    void InsertAssetUsages( AssetFile* assetFile, M_AssetFiles* assetFiles, S_tuid& visitedIDs, bool* cancel = NULL );
    void InsertAssetShaders( AssetFile* assetFile, M_AssetFiles* assetFiles, S_tuid& visitedIDs, bool* cancel = NULL );
    void InsertLevelEntities( AssetFile* assetFile, M_AssetFiles* assetFiles, S_tuid& visitedIDs, bool* cancel = NULL );
    void InsertDependencyIds( S_tuid tuids, u64 assetRowID, M_AssetFiles* assetFiles, S_tuid& visitedIDs, const char* replaceSQL, const char* deleteSQL, const char* deleteUnrenewedSQL, bool* cancel = NULL );

    // Used by Search
    tuid StepSelectFileID( int sqlResult, const SQL::StmtHandle stmt, bool resetStmt = false );
    void GetSelectStmt( const CacheDBQuery* search, std::string& selectStmt );

    // Used by UI to populate Choice boxes
    u32 GetPopulateTableData( const SQL::StmtHandle select, M_CacheDBTableData& tableData, bool* cancel = NULL );
    u32 GetPopulateTableData( const SQL::StmtHandle select, V_string& tableData, bool* cancel = NULL );

  public:
    static const char* s_TrackerDBVersion;

  private:    
    M_CacheDBColumns m_CacheDBColumns;

    SQL::StmtHandle   m_SelectUsersComputerIDHandle;
    SQL::StmtHandle   m_SelectP4IDHandle;
    SQL::StmtHandle   m_SelectAssetPathByIDHandle;
    SQL::StmtHandle   m_SelectAssetByIDHandle;
    SQL::StmtHandle   m_SelectAssetRowIDHandle;
    SQL::StmtHandle   m_SelectAssetLastUpdatedHandle;
    SQL::StmtHandle   m_SelectAssetP4RevisionHandle;
    SQL::StmtHandle   m_SelectDependenciesByIDHandle;
    SQL::StmtHandle   m_SelectUsagesByIDHandle;
    SQL::StmtHandle   m_SelectFileTypeIDHandle;
    SQL::StmtHandle   m_SelectEngineTypeIDHandle;
    SQL::StmtHandle   m_SelectAttributeIDHandle;
    SQL::StmtHandle   m_FindAttributeIDHandle;
    SQL::StmtHandle   m_SelectAttributesHandle;

  };
  ASSET_API typedef Nocturnal::SmartPtr< CacheDB > CacheDBPtr;
}
