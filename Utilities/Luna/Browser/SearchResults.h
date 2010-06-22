#pragma once

#include <vector>

#include "Luna/API.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Asset
{
  class AssetFile;
  typedef Nocturnal::SmartPtr< AssetFile > AssetFilePtr;
  typedef std::vector< AssetFilePtr > V_AssetFiles;
  typedef std::map< u64, AssetFilePtr > M_AssetFiles;

  class AssetFolder;
  typedef Nocturnal::SmartPtr< AssetFolder > AssetFolderPtr;
  typedef std::vector< AssetFolderPtr > V_AssetFolders;
}

namespace Luna
{
  class SearchResults : public Nocturnal::RefCountBase< SearchResults >
  {
  public:
    SearchResults( u32 browserSearchID = 0 );
    SearchResults( const SearchResults* results );
    virtual ~SearchResults();

    void Clear();
    bool HasResults() const;

    const Asset::V_AssetFiles& GetFiles() const;
    bool AddFile( Asset::AssetFilePtr assetFile );
    bool RemoveFile( Asset::AssetFilePtr assetFile );

    const Asset::V_AssetFolders& GetFolders() const;
    bool AddFolder( Asset::AssetFolderPtr assetFolder );
    bool RemoveFolder( const std::string& fullPath );

    i32 GetSearchID() { return m_BrowserSearchID; }
  private:
    i32 m_BrowserSearchID; // This is the ID of the BrowserSearch that created these results, for easy of debugging
    Asset::V_AssetFiles  m_AssetFiles;
    Asset::M_AssetFiles  m_LookupByHash; // To speed up checks of duplicate entries

    Asset::V_AssetFolders m_Folders;


    Asset::AssetFile* FindFileByHash( const u64& hash ) const;
  };
  typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;
}
