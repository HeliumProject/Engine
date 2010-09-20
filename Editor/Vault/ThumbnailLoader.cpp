#include "Precompile.h"
#include "ThumbnailLoader.h"

#include "Foundation/File/Directory.h"
#include "Core/Asset/AssetClass.h"
#include "Core/Asset/Classes/ShaderAsset.h"
#include "Core/Render/DeviceManager.h"
#include "Core/SceneGraph/Render.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Editor;

void* ThumbnailLoader::LoadThread::Entry()
{
#ifdef HELIUM_ASSERT_ENABLED
    bool emptyQueuePollingCheck = false;
#endif

    while ( true )
    {
        m_Loader.m_Signal.Decrement();

        if ( m_Loader.m_Quit )
        {
            break;
        }

        // the device is gone, our glorious benefactor is probably cleaning up
        IDirect3DDevice9* device = m_Loader.m_DeviceManager->GetD3DDevice();
        if ( !device )
        {
            // You should stop this thread before letting go of the window that
            // owns the device.
            HELIUM_BREAK();
            break;
        }

        // while the device is lost, just wait for it to come back
        while ( device->TestCooperativeLevel() != D3D_OK )
        {
            if ( m_Loader.m_Quit )
            {
                break;
            }

            wxThread::Sleep( 100 );
            continue;
        }

        if ( m_Loader.m_Quit )
        {
            break;
        }

        Helium::Path path;

        {
            Helium::Locker< Helium::OrderedSet< Helium::Path > >::Handle queue = m_Loader.m_FileQueue.Lock();
            if ( !queue->Empty() )
            {
#ifdef HELIUM_ASSERT_ENABLED
                emptyQueuePollingCheck = false;
#endif
                path = queue->Front();
            }
            else
            {
                // if you hit this then the bookkeeping of the counting semaphore is broken
                HELIUM_ASSERT( !emptyQueuePollingCheck );

#ifdef HELIUM_ASSERT_ENABLED
                emptyQueuePollingCheck = true;
#endif
                continue;
            }

            queue->Remove( path );
        }

        ResultArgs args;
        args.m_Path = &path;
        args.m_Cancelled = false;

        if ( Core::IsSupportedTexture( path.Get() ) )
        {
            IDirect3DTexture9* texture = NULL;
            if ( texture = LoadTexture( device, path.Get() ) )
            {
                ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_DeviceManager, texture );
                args.m_Textures.push_back( thumbnail );
            }
        }
        else
        {
            tstringstream str;
            str << path.Hash();
            Helium::Path thumbnailFolderPath( m_Loader.m_ThumbnailDirectory + wxT('/') + str.str() );
            Helium::Directory thumbnailFolder( thumbnailFolderPath.Get() );

            while( !thumbnailFolder.IsDone() )
            {
                IDirect3DTexture9* texture = NULL;
                if ( texture = LoadTexture( device, thumbnailFolder.GetItem().m_Path ) )
                {
                    ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_DeviceManager, texture );
                    args.m_Textures.push_back( thumbnail );
                }

                thumbnailFolder.Next();
            }

            // Include the color map of a shader as a possible thumbnail image
            if ( path.FullExtension() == TXT( "shader.nrb" ) )
            {
                Asset::ShaderAssetPtr shader = Asset::AssetClass::LoadAssetClass< Asset::ShaderAsset >( path );
                if ( shader )
                {
                    Asset::TexturePtr colorMap = Asset::AssetClass::LoadAssetClass< Asset::Texture >( shader->m_ColorMapPath );
                    if ( colorMap.ReferencesObject() )
                    {
                        if ( colorMap->GetPath().Exists() && Core::IsSupportedTexture( colorMap->GetPath().Get() ) )
                        {
                            IDirect3DTexture9* texture = NULL;
                            if ( texture = LoadTexture( device, colorMap->GetPath().Get() ) )
                            {
                                ThumbnailPtr thumbnail = new Thumbnail( m_Loader.m_DeviceManager, texture );
                                args.m_Textures.push_back( thumbnail );
                            }
                        }
                    }
                }
            }
        }

        m_Loader.m_Result.Raise( args );
    }

    return NULL;
}

ThumbnailLoader::ThumbnailLoader( DeviceManager* d3dManager, const tstring& thumbnailDirectory )
: m_LoadThread( *this )
, m_Quit( false )
, m_ThumbnailDirectory( thumbnailDirectory )
, m_DeviceManager( d3dManager )
{
    m_LoadThread.Create();
    m_LoadThread.Run();
}

ThumbnailLoader::~ThumbnailLoader()
{
    m_Quit = true;
    m_Signal.Increment();
    m_LoadThread.Wait();
}

void ThumbnailLoader::Enqueue( const std::set< Helium::Path >& files )
{
    Helium::Locker< Helium::OrderedSet< Helium::Path > >::Handle queue = m_FileQueue.Lock();

    for ( std::set< Helium::Path >::const_reverse_iterator itr = files.rbegin(), end = files.rend();
        itr != end;
        ++itr )
    {
        bool signal = !queue->Remove( *itr );
        queue->Prepend( *itr );
        if ( signal )
        {
            m_Signal.Increment();
        }
    }
}

void ThumbnailLoader::Stop()
{
    Helium::Locker< Helium::OrderedSet< Helium::Path > >::Handle queue = m_FileQueue.Lock();
    if ( queue->Empty() )
    {
        return;
    }

    while ( !queue->Empty() )
    {
        ResultArgs args;
        args.m_Path = &( queue->Front() );
        args.m_Cancelled = true;
        m_Result.Raise( args );
        
        queue->Remove( queue->Front() );
    }

    m_Signal.Reset();
}