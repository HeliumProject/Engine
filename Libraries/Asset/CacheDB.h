#pragma once

#include "API.h"

#include "CacheDBColumn.h"
#include "CacheDBQuery.h"

#include "Common/File/Path.h"
#include "SQL/SQLiteDB.h"
#include "UID/TUID.h"


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

    /////////////////////////////////////////////////////////////////////////////
    /// class CacheDB
    /////////////////////////////////////////////////////////////////////////////
    class ASSET_API CacheDB : public SQL::SQLiteDB
    {
    public:
        CacheDB( const char* friendlyName, const std::string& dbFilename, const std::string& configFolder, int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, const std::string& version = "" );
        virtual ~CacheDB();

        virtual bool Load() NOC_OVERRIDE;

        const M_CacheDBColumns& GetDBColumns() const;

        u32 GetPopulateTableData( const CacheDBColumnID columnID, V_string& tableData, bool* cancel = NULL ); 

        u32 Search( const CacheDBQuery* search, Nocturnal::S_Path& assetFiles, bool* cancel = NULL );

        bool HasAssetChangedOnDisk( Nocturnal::Path& filePath, bool* cancel = NULL );
        void InsertAssetFile( AssetFile* assetFile, M_AssetFiles* assetFiles, Nocturnal::S_Path& visited, bool* cancel = NULL );
        void DeleteAssetFile( AssetFile* assetFile );

        void SelectAssetPathByHash( const u64 pathHash, std::string& path );
        void SelectAssetByHash( const u64 pathHash, AssetFile* assetFile );

        u64 FindAttributeRowID( const std::string& value );
        u32 GetAttributesTableData( V_string& tableData, bool* cancel = NULL );

        void GetAssetDependencies( const Nocturnal::Path& path, Nocturnal::S_Path& dependencies, bool reverse = false, u32 maxDepth = 0, u32 currDepth = 0, bool* cancel = NULL );
        void GetDependencyGraph( const Nocturnal::Path& path, M_AssetFiles* assetFiles, bool reverse = false, u32 maxDepth = 0, u32 currDepth = 0, bool* cancel = NULL );

        static void CleanExpressionForSQL( std::string& argument, bool wrapEscape = true );

    protected:
        // SQLiteDB overrides functions
        virtual void PrepareStatements() NOC_OVERRIDE;

    private:
        bool Open( const std::string& dbFilename, const std::string& configFolder, const std::string& version = std::string( "" ), int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE );

        // Used by Tracker
        u64 SelectAssetRowID( u64 fileId, const char* insert = NULL, const char* update = NULL, bool* cancel = NULL );
        u64 SelectIDByName( SQL::StmtHandle select, const char* value, const char* insert = NULL, bool* cancel = NULL );

        void InsertAssetAttributes( AssetFile* assetFile, bool* cancel = NULL );
        void InsertAssetUsages( AssetFile* assetFile, M_AssetFiles* assetFiles, Nocturnal::S_Path& visited, bool* cancel = NULL );
        void InsertAssetShaders( AssetFile* assetFile, M_AssetFiles* assetFiles, Nocturnal::S_Path& visited, bool* cancel = NULL );
        void InsertLevelEntities( AssetFile* assetFile, M_AssetFiles* assetFiles, Nocturnal::S_Path& visited, bool* cancel = NULL );
        void InsertDependencies( const Nocturnal::S_Path& dependencies, u64 assetRowID, M_AssetFiles* assetFiles, Nocturnal::S_Path& visited, const char* replaceSQL, const char* deleteSQL, const char* deleteUnrenewedSQL, bool* cancel = NULL );

        // Used by Search
        std::string StepSelectPath( int sqlResult, const SQL::StmtHandle stmt, bool resetStmt = false );
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
        SQL::StmtHandle   m_SelectAssetTypeIDHandle;
        SQL::StmtHandle   m_SelectAttributeIDHandle;
        SQL::StmtHandle   m_FindAttributeIDHandle;
        SQL::StmtHandle   m_SelectAttributesHandle;

    };
    ASSET_API typedef Nocturnal::SmartPtr< CacheDB > CacheDBPtr;
}
