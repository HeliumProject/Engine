#pragma once

#include "Thumbnail.h"

#include "Pipeline/Asset/AssetType.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Container/OrderedSet.h"

namespace Asset
{
  class AssetFile;
  class AssetFolder;
}

namespace Luna
{
  namespace ThumbnailTileTypes
  {
    enum ThumbnailTileType
    {
      Folder,
      File
    };
  }
  typedef ThumbnailTileTypes::ThumbnailTileType ThumbnailTileType;

  class ThumbnailTile : public Nocturnal::RefCountBase< ThumbnailTile >
  {
  public:
    ThumbnailTile( Asset::AssetFile* file );
    ThumbnailTile( Asset::AssetFolder* folder );
    virtual ~ThumbnailTile();

  private:
    void Init();

  public:
    ThumbnailTileType GetType() const;

    bool IsFile() const;
    Asset::AssetFile* GetFile();

    bool IsFolder() const;
    Asset::AssetFolder* GetFolder();

    inline u32 GetRow() const
    {
      return m_Row;
    }

    inline u32 GetColumn() const
    {
      return m_Column;
    }

    void GetRowColumn( u32& row, u32& col ) const;
    void SetRowColumn( u32 row, u32 col );

    inline bool IsSelected() const 
    {
      return m_IsSelected;
    }

    inline void SetSelected( bool selected )
    {
      m_IsSelected = selected;
    }

    inline bool IsHighlighed() const
    {
      return m_IsHighlighted;
    }

    inline void SetHighlighted( bool highlight )
    {
      m_IsHighlighted = highlight;
    }

    tstring GetLabel() const;
    tstring GetEditableName() const;
    tstring GetFullPath() const;
    tstring GetTypeLabel() const;
    bool GetTypeColor( DWORD& color ) const;
    Asset::AssetType GetAssetType() const;

    const Thumbnail* GetThumbnail() const;
    void SetThumbnail( ThumbnailPtr thumbnail );

  private:
    ThumbnailTileType m_Type;

    union
    {
      Asset::AssetFile* m_File;
      Asset::AssetFolder* m_Folder;
    };

    u32 m_Row;
    u32 m_Column;

    bool m_IsSelected;
    bool m_IsHighlighted;

    ThumbnailPtr m_Thumbnail;
  };
  typedef Nocturnal::SmartPtr< ThumbnailTile > ThumbnailTilePtr;

  typedef std::vector< ThumbnailTilePtr > V_ThumbnailTilePtr;

  typedef std::set< ThumbnailTilePtr > S_ThumbnailTiles;
  typedef Nocturnal::OrderedSet< ThumbnailTilePtr > OS_ThumbnailTiles;

  typedef std::map< Asset::AssetFile*, ThumbnailTilePtr > M_FileToTilePtr;
  typedef std::map< Asset::AssetFolder*, ThumbnailTilePtr > M_FolderToTilePtr;
}
