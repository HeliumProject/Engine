#include "Precompile.h"
#include "ThumbnailTile.h"

#include "Pipeline/Asset/AssetFile.h"
#include "Pipeline/Asset/AssetFolder.h"
#include "Platform/Exception.h"

using namespace Luna;

ThumbnailTile::ThumbnailTile( Asset::AssetFile* file )
: m_Type( ThumbnailTileTypes::File )
, m_File( file )
{
    Init();
}

ThumbnailTile::ThumbnailTile( Asset::AssetFolder* folder )
: m_Type( ThumbnailTileTypes::Folder )
, m_Folder( folder )
{
    Init();
}

ThumbnailTile::~ThumbnailTile()
{
}

void ThumbnailTile::Init()
{
    m_Row = 0;
    m_Column = 0;
    m_IsSelected = false;
    m_IsHighlighted = false;
}

ThumbnailTileType ThumbnailTile::GetType() const
{
    return m_Type;
}

bool ThumbnailTile::IsFile() const
{
    return ( m_Type == ThumbnailTileTypes::File );
}

Asset::AssetFile* ThumbnailTile::GetFile()
{
    if ( !IsFile() )
    {
        throw Nocturnal::Exception( TXT( "ThumbnailTile is not a file!" ) );
    }
    return m_File;
}

bool ThumbnailTile::IsFolder() const
{
    return ( m_Type == ThumbnailTileTypes::Folder );
}

Asset::AssetFolder* ThumbnailTile::GetFolder()
{
    if ( !IsFolder() )
    {
        throw Nocturnal::Exception( TXT( "ThumbnailTile is not a folder!" ) );
    }
    return m_Folder;
}

void ThumbnailTile::GetRowColumn( u32& row, u32& col ) const
{
    row = m_Row;
    col = m_Column;
}

void ThumbnailTile::SetRowColumn( u32 row, u32 col )
{
    m_Row = row;
    m_Column = col;
}

tstring ThumbnailTile::GetLabel() const
{
#pragma TODO ("ThumbnailTile::GetLabel - it would be cool not to incur a string copy right here and return a const tstring& instead." )

    if ( IsFolder() )
    {
        return m_Folder->GetShortName();
    }
    else
    {
        return m_File->GetShortName() + m_File->GetExtension();
    }
}

#pragma TODO( "get rid of these string copies!" )
tstring ThumbnailTile::GetEditableName() const
{
    if ( IsFolder() )
    {
        return m_Folder->GetShortName();
    }
    else
    {
        return m_File->GetShortName();
    }
}

tstring ThumbnailTile::GetFullPath() const
{
    if ( IsFolder() )
    {
        return m_Folder->GetFullPath();
    }
    else
    {
        return m_File->GetFilePath();
    }
}

tstring ThumbnailTile::GetTypeLabel() const
{
    tstring label;
    if ( IsFile() )
    {
    }
    return label;
}

bool ThumbnailTile::GetTypeColor( DWORD& color ) const
{
    if ( IsFile() )
    {
    }
    return false;
}


const Thumbnail* ThumbnailTile::GetThumbnail() const
{
    return m_Thumbnail;
}

void ThumbnailTile::SetThumbnail( ThumbnailPtr thumbnail )
{
    m_Thumbnail = thumbnail;
}
