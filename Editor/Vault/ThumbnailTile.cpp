#include "EditorPch.h"
#include "ThumbnailTile.h"

#include "Platform/Exception.h"

using namespace Helium;
using namespace Helium::Editor;

ThumbnailTile::ThumbnailTile( const Helium::FilePath& path )
	: m_Path( path )
	, m_Row( 0 )
	, m_Column( 0 )
	, m_IsSelected( false )
	, m_IsHighlighted( false )
{
}

ThumbnailTile::~ThumbnailTile()
{
}

void ThumbnailTile::GetRowColumn( uint32_t& row, uint32_t& col ) const
{
	row = m_Row;
	col = m_Column;
}

void ThumbnailTile::SetRowColumn( uint32_t row, uint32_t col )
{
	m_Row = row;
	m_Column = col;
}

std::string ThumbnailTile::GetLabel() const
{
	return m_Path.IsDirectory() ? m_Path.Basename() : m_Path.Filename().Get();
}

// TODO: Get rid of these string copies

std::string ThumbnailTile::GetEditableName() const
{
	return m_Path.Basename();
}

const Helium::FilePath& ThumbnailTile::GetPath() const
{
	return m_Path;
}

const std::string& ThumbnailTile::GetFullPath() const
{
	return m_Path.Get();
}

std::string ThumbnailTile::GetTypeLabel() const
{
	return m_Path.IsDirectory() ? "Directory" : "File";
}

bool ThumbnailTile::GetTypeColor( uint32_t& color ) const
{
	color = m_Path.IsDirectory() ? 0x00FF00 : 0x0000FF;
	return true;
}

const Thumbnail* ThumbnailTile::GetThumbnail() const
{
	return m_Thumbnail;
}

void ThumbnailTile::SetThumbnail( ThumbnailPtr thumbnail )
{
	m_Thumbnail = thumbnail;
}
