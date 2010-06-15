#include "Precompile.h"
#include "ThumbnailTileCreator.h"
#include "ThumbnailView.h"
#include "Asset/AssetFile.h"
#include "Asset/AssetFolder.h"

using namespace Luna;

ThumbnailTileCreator::ThumbnailTileCreator( ThumbnailView* view )
: m_View( view )
{
}

ThumbnailTileCreator::~ThumbnailTileCreator()
{
  StopThread();
}

void ThumbnailTileCreator::SetDefaultThumbnails( Thumbnail* error, Thumbnail* loading, Thumbnail* folder )
{
  m_TextureError = error;
  m_TextureLoading = loading;
  m_TextureFolder = folder;
}

void ThumbnailTileCreator::Reset()
{
  m_FolderTiles.clear();
  m_FileTiles.clear();
  m_Sorter.Clear();
  m_TextureRequests.clear();
  m_Results = NULL;
}

void ThumbnailTileCreator::InitData()
{
  Reset();
  m_Results = new SearchResults( m_View->GetResults() );
}

void ThumbnailTileCreator::ThreadProc( i32 threadID )
{
  ThreadEnter( threadID );

  if ( m_Results && m_Results->HasResults() )
  {
    // Build the folder list
    for ( Asset::V_AssetFolders::const_iterator folderItr = m_Results->GetFolders().begin(),
      folderEnd = m_Results->GetFolders().end(); folderItr != folderEnd; ++folderItr )
    {
      if ( CheckThreadLeave( threadID ) )
      {
        return;
      }
      ThumbnailTilePtr tile = new ThumbnailTile( *folderItr );
      m_FolderTiles.insert( std::make_pair( *folderItr, tile ) );
      m_Sorter.Add( tile );
      tile->SetThumbnail( m_TextureFolder );
    }

    // Build the file list
    for ( Asset::V_AssetFiles::const_iterator fileItr = m_Results->GetFiles().begin(),
      fileEnd = m_Results->GetFiles().end(); fileItr != fileEnd; ++fileItr )
    {
      if ( CheckThreadLeave( threadID ) )
      {
        return;
      }
      ThumbnailTilePtr tile = new ThumbnailTile( *fileItr );
      m_FileTiles.insert( std::make_pair( *fileItr, tile ) );
      m_Sorter.Add( tile );
      Nocturnal::Path path( ( *fileItr )->GetFilePath() );
      if ( path.Exists() )
      {
        // Default to the loading texture
        tile->SetThumbnail( m_TextureLoading );
      }
      else
      {
        // File is in the resolver, or in the dependency cache, but not on disk?
        tile->SetThumbnail( m_TextureError );
      }
    }
  }

  // Build the ordered list of files that we need thumbnails for
  for ( ThumbnailIteratorPtr sortedItr = m_Sorter.GetIterator();
    !sortedItr->IsDone(); sortedItr->Next() )
  {
    if ( CheckThreadLeave( threadID ) )
    {
      return;
    }

    ThumbnailTile* tile = sortedItr->GetCurrentTile();
    if ( tile->IsFile() && tile->GetThumbnail() == m_TextureLoading.Ptr() )
    {
      m_TextureRequests.push_back( tile->GetFile() );
    }
  }

  ThreadLeave( threadID );
}

void ThumbnailTileCreator::OnEndThread( const UIToolKit::ThreadProcArgs& args )
{
  if ( !IsCurrentThread( args.m_ThreadID ) )
    return;

  m_View->OnTilesCreated( m_FolderTiles, m_FileTiles, m_Sorter, m_TextureRequests );
  Reset();
}
