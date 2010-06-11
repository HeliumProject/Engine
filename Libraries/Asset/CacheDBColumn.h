#pragma once

#include "API.h"

#include "SQL/SQLiteDB.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"

namespace Asset
{
  class CacheDB;

  /////////////////////////////////////////////////////////////////////////////
  ASSET_API typedef u32 CacheDBColumnID;
  ASSET_API typedef std::set< CacheDBColumnID > S_CacheDBColumnID;
  namespace CacheDBColumnIDs
  {
    enum CacheDBColumnID
    {
      Null = 0,

      FileID,
      Name,
      Path,
      FileType,
      AssetType,
      P4User,
      Shader,
      Level,

      //Last
      Count,
    };
    //static void CacheDBColumnIDsEnumerateEnumeration( Reflect::Enumeration* info )
    //{
    //  info->AddElement(Null, "Null");
    //
    //  info->AddElement(FileID, "FileID");
    //  info->AddElement(Name, "Name");
    //  info->AddElement(Path, "Path");
    //  info->AddElement(FileType, "FileType");
    //  info->AddElement(AssetType, "AssetType");
    //  info->AddElement(P4User, "P4User");  
    //  info->AddElement(Shader, "Shader");
    //  info->AddElement(Level, "Level");
    //}

    // Display Label
    static const std::string s_Labels[Count] = 
    {
      "INVALID", // placeholder for Null

      "File ID",
      "Name",
      "Path",
      "FileType",
      "AssetType",
      "Created By", //P4 User
      "Shader",
      "Level",
    };

    inline const std::string& Label( i32 id )
    {
      NOC_ASSERT( id > Null && id < Count );
      return s_Labels[id];
    }

    // CacheDB Column
    // Should be same as available tables in CacheDB::m_CacheDBColumns
    static const std::string s_Column[Count] = 
    {
      "INVALID", // placeholder for Null
      
      "file_id",
      "name",
      "path",
      "file_type",
      "engine_type",
      "p4_user",
      "shader",
      "level",
    };

    inline const std::string& Column( i32 id )
    {
      NOC_ASSERT( id > Null && id < Count );
      return s_Column[id];
    }
  }

  ///////////////////////////////////////////////////////////////////////////// 
  class ASSET_API CacheDBColumn : public Nocturnal::RefCountBase< CacheDBColumn >
  {
  public:
    CacheDBColumn( Asset::CacheDB* assetCacheDB,
      const std::string& name,
      const std::string& table,
      const std::string& column,
      const std::string& columnAliases = std::string("") );

    const std::string& GetTableColumn() const { return m_TableColumn; }
    const std::string& GetSelect() const { return m_TableColumn; }

    void SetJoin( const std::string& primaryKey, const std::string& foreignTable, const std::string& foreignKey );
    const std::string& GetJoin() const { return m_JoinExpr; }

    void GetWhere( const std::string& phrase, std::string& whereExpr, const std::string& op = std::string( "LIKE" ), bool escape = true ) const;

    void SetPopulateSQL( const std::string& sql );
    const std::string& GetPopulateSQL() const { return m_PopulateSQL; }
    void SetPopulateStmtHandle( SQL::StmtHandle stmtHandle );
    SQL::StmtHandle GetPopulateStmtHandle() const { return m_PopulateStmtHandle; }

    bool UseInGenericSearch() const { return m_UseInGenericSearch; }
    void UseInGenericSearch( const bool useInGenericSearch ) { m_UseInGenericSearch = useInGenericSearch; }

  public:
    CacheDB*    m_AssetCacheDB; // Example: 
    std::string m_Name;         //  - engine_type
    std::string m_Table;        //  - engine_types
    std::string m_Column;       //  - name
    std::string m_PrimaryKey;   //  - id
    std::string m_ForeignTable; //  - assets
    std::string m_ForeignKey;   //  - engine_type_id

    bool        m_UseInGenericSearch;

    S_string    m_ColumnAliases;//  - engine_type,engine,eng,type

    std::string m_TableColumn;  //  - engine_types.type
    std::string m_JoinExpr;     //  - LEFT JOIN file_types ON file_types.id = assets.file_type_id
    std::string m_SelectExpr;   //  - engine_types.type AS engine_type

    std::string         m_PopulateSQL;
    SQL::StmtHandle     m_PopulateStmtHandle;
    
  };
  ASSET_API typedef Nocturnal::SmartPtr< CacheDBColumn > CacheDBColumnPtr;
  ASSET_API typedef std::map< CacheDBColumnID, CacheDBColumnPtr > M_CacheDBColumns;

}
