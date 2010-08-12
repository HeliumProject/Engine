#include "Precompile.h"
#include "ThumbnailManager.h"
#include "ThumbnailLoadedEvent.h"
#include "Core/Render/DeviceManager.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// window - the window to receive ThumbnailLoadedEvents.
// 
ThumbnailManager::ThumbnailManager( wxWindow* window, Render::DeviceManager* d3dmanager, const tstring& thumbnailDirectory )
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
    Helium::Locker< std::map< u64, Helium::Path* > >::Handle list = m_AllRequests.Lock();
    list->clear();
}

///////////////////////////////////////////////////////////////////////////////
// Request that some thumbnails be loaded.
// 
void ThumbnailManager::Request( const std::set< Helium::Path >& paths )
{
    m_Loader.Enqueue( paths );
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
    Helium::TakeMutex mutex( m_WindowMutex );
    m_Window = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a texture is loaded.  This callback comes in from a 
// background thread.
// 
void ThumbnailManager::OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args )
{
    Helium::Locker< std::map< u64, Helium::Path* > >::Handle list = m_AllRequests.Lock();
    if ( args.m_Cancelled )
    {
        list->erase( args.m_Path->Hash() );
    }
    else
    {
        Helium::Insert< std::map< u64, Helium::Path* > >::Result inserted = list->insert( std::make_pair( args.m_Path->Hash(), args.m_Path ) );
        if ( inserted.second )
        {
            // Only post to the window if we still have a pointer
            Helium::TakeMutex mutex( m_WindowMutex );
            if ( m_Window )
            {
                ThumbnailLoadedEvent evt;
                evt.SetThumbnails( args.m_Textures );
                evt.SetPath( *args.m_Path );
                evt.SetCancelled( false );
                wxPostEvent( m_Window, evt );
            }
        }
    }
}
