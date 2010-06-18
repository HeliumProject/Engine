#include "Precompile.h"
#include "ThumbnailManager.h"
#include "ThumbnailLoadedEvent.h"
#include "Render/D3DManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// window - the window to receive ThumbnailLoadedEvents.
// 
ThumbnailManager::ThumbnailManager( wxWindow* window, Render::D3DManager* d3dmanager, const std::string& thumbnailDirectory )
: m_Window( window )
, m_Loader( d3dmanager, thumbnailDirectory )
{
  m_Loader.AddResultListener( ThumbnailLoader::ResultSignature::Delegate( this, &ThumbnailManager::OnThumbnailLoaded ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ThumbnailManager::~ThumbnailManager()
{
  m_Loader.RemoveResultListener( ThumbnailLoader::ResultSignature::Delegate( this, &ThumbnailManager::OnThumbnailLoaded ) );
}

///////////////////////////////////////////////////////////////////////////////
// Clear out the list of items that we have requested from the loader.
// 
void ThumbnailManager::Reset()
{
  Platform::Locker< Asset::M_AssetFiles >::Handle list = m_AllRequests.Lock();
  list->clear();
}

///////////////////////////////////////////////////////////////////////////////
// Request that some thumbnails be loaded.
// 
void ThumbnailManager::Request( const Asset::V_AssetFiles& files )
{
  m_Loader.Load( files );
}

///////////////////////////////////////////////////////////////////////////////
// Cancel any pending thumbnail loads.
// 
void ThumbnailManager::Cancel()
{
  m_Loader.Stop();
}

///////////////////////////////////////////////////////////////////////////////
// Should be called before the window is shut down.  Prevents any further events
// from being posted to the window.
// 
void ThumbnailManager::DetachFromWindow()
{
  Platform::TakeMutex mutex( m_WindowMutex );
  m_Window = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a texture is loaded.  This callback comes in from a 
// background thread.
// 
void ThumbnailManager::OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args )
{
  Platform::Locker< Asset::M_AssetFiles >::Handle list = m_AllRequests.Lock();
  if ( args.m_Cancelled )
  {
      list->erase( args.m_File->GetPath().Hash() );
  }
  else
  {
      Nocturnal::Insert< Asset::M_AssetFiles >::Result inserted = list->insert( std::make_pair( args.m_File->GetPath().Hash(), args.m_File ) );
    if ( inserted.second )
    {
      // Only post to the window if we still have a pointer
      Platform::TakeMutex mutex( m_WindowMutex );
      if ( m_Window )
      {
        ThumbnailLoadedEvent evt;
        evt.SetThumbnails( args.m_Textures );
        evt.SetAssetFile( args.m_File );
        evt.SetCancelled( false );
        wxPostEvent( m_Window, evt );
      }
    }
  }
}
