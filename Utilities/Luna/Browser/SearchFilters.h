#pragma once

#include "AssetTracker/AssetDB.h"
#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"


namespace Luna
{

  /////////////////////////////////////////////////////////////////////////////
  typedef std::map< u32, S_string > M_SearchFilters;
  namespace SearchFilters
  {
    enum SearchFilter
    {
      Null = 0,

      FileID,
      Path,
      FileType,
      EngineType,
      P4User,
      Shader,
      Level,

      //Last
      Count,
    };

    static void SearchFiltersEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Null, "Null");
      info->AddElement(FileID, "FileID");
      info->AddElement(Path, "Path");
      info->AddElement(FileType, "FileType");
      info->AddElement(EngineType, "EngineType");
      info->AddElement(P4User, "P4User");  
      info->AddElement(Shader, "Shader");
      info->AddElement(Level, "Level");
    }

    // Display Label
    static const std::string s_Labels[Count] = 
    {
      "INVALID", // placeholder for Null
      "File ID",
      "Path",
      "FileType",
      "EngineType",
      "Created By", //P4 User
      "Shader",
      "Level",
    };

    inline const std::string& Label( i32 id )
    {
      NOC_ASSERT( id > Null && id < Count );
      return s_Labels[id];
    }

    // AssetDB Column
    // Should be same as available tables in AssetDB::m_AssetDBColumns
    static const std::string s_Column[Count] = 
    {
      "file_id",
      "path",
      "file_type",
      "engine_type",
      "p4_user", //P4 User
      "shader",
      "level",
    };

    inline const std::string& Column( i32 id )
    {
      NOC_ASSERT( id > Null && id < Count );
      return s_Column[id];
    }

    static void SetupFilters( M_SearchFilters& searchFilters );
    static void ClearFilters( M_SearchFilters& searchFilters, u32 filter = SearchFilters::Null );
    static void AddFilter( M_SearchFilters& searchFilters, u32 filter, const std::string& value );
    static void RemoveFilter( M_SearchFilters& searchFilters, u32 filter, const std::string& value );
    static void AppendAssetDBSearchFilters( const M_SearchFilters& searchFilters, Asset::AssetDBSearch* search );
  }
  
}
