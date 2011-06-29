#include "EditorPch.h"
#include "ThumbnailManager.h"
#include "ThumbnailLoadedEvent.h"
#include "Pipeline/SceneGraph/DeviceManager.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// window - the window to receive ThumbnailLoadedEvents.
// 
ThumbnailManager::ThumbnailManager( wxWindow* window, DeviceManager* d3dmanager )
: m_Window( window )
, m_Loader( d3dmanager )
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
    Helium::Locker< std::map< uint64_t, Helium::Path > >::Handle list( m_AllRequests );
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
    Helium::MutexScopeLock mutex( m_WindowMutex );
    m_Window = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a texture is loaded.  This callback comes in from a 
// background thread.
// 
void ThumbnailManager::OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args )
{
    Helium::Locker< std::map< uint64_t, Helium::Path > >::Handle list( m_AllRequests );
    if ( args.m_Cancelled )
    {
        list->erase( args.m_Path.Hash() );
    }
    else
    {
        uint64_t hash = args.m_Path.Hash();
        Helium::StdInsert< std::map< uint64_t, Helium::Path > >::Result inserted = list->insert( std::make_pair( hash, args.m_Path ) );

        // only kick to foreground for new entries
        if ( inserted.second )
        {
            Helium::MutexScopeLock mutex( m_WindowMutex );
            if ( m_Window )
            {
                ThumbnailLoadedEvent evt;
                evt.SetThumbnails( args.m_Textures );
                evt.SetPath( args.m_Path );
                evt.SetCancelled( false );
                wxPostEvent( m_Window, evt );
            }
        }
    }
}
