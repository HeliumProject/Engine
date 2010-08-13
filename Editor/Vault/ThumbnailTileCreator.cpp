#include "Precompile.h"
#include "ThumbnailTileCreator.h"
#include "ThumbnailView.h"

using namespace Helium;
using namespace Helium::Editor;

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
    m_Tiles.clear();
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
        for ( std::map< u64, Helium::Path >::const_iterator itr = m_Results->GetPathsMap().begin(), end = m_Results->GetPathsMap().end(); itr != end; ++itr )
        {
            if ( CheckThreadLeave( threadID ) )
            {
                return;
            }
            const Helium::Path& path = (*itr).second;
            ThumbnailTilePtr tile = new ThumbnailTile( path );
            m_Tiles.insert( std::make_pair( path, tile ) );
            m_Sorter.Add( tile );
            if ( path.IsDirectory() )
            {
                tile->SetThumbnail( m_TextureFolder );
            }
            else if ( path.Exists() )
            {
                tile->SetThumbnail( m_TextureLoading );
            }
            else
            {
                tile->SetThumbnail( m_TextureError );
            }
        }
    }

    // Build the ordered list of files that we need thumbnails for
    for ( ThumbnailIteratorPtr sortedItr = m_Sorter.GetIterator(); !sortedItr->IsDone(); sortedItr->Next() )
    {
        if ( CheckThreadLeave( threadID ) )
        {
            return;
        }

        ThumbnailTile* tile = sortedItr->GetCurrentTile();
        if ( tile->GetPath().IsFile() && tile->GetThumbnail() == m_TextureLoading.Ptr() )
        {
            m_TextureRequests.insert( tile->GetPath() );
        }
    }

    ThreadLeave( threadID );
}

void ThumbnailTileCreator::OnEndThread( const ThreadProcArgs& args )
{
    if ( !IsCurrentThread( args.m_ThreadID ) )
        return;

    m_View->OnTilesCreated( m_Tiles, m_Sorter, m_TextureRequests );
    Reset();
}
