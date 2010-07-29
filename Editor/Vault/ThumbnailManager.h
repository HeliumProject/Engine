#pragma once

#include "Thumbnail.h"
#include "ThumbnailLoader.h"
#include "Platform/Mutex.h"

namespace Render
{
    class D3DManager;
}

namespace Editor
{
    struct ThumbnailResultArgs
    {
        const V_ThumbnailPtr& m_Thumbnails;
        Helium::Path m_Path;

        ThumbnailResultArgs( const V_ThumbnailPtr& thumbnails, const Helium::Path& path )
            : m_Thumbnails( thumbnails )
            , m_Path( path )
        {
        }
    };
    typedef Helium::Signature< void, const ThumbnailResultArgs& > ThumbnailResultSignature;

    class ThumbnailManager
    {
    public:
        ThumbnailManager( wxWindow* window, Render::D3DManager* d3dmanager, const tstring& thumbnailDirectory );
        virtual ~ThumbnailManager();

        void Reset();
        void Request( const std::set< Helium::Path >& paths );
        void Cancel();
        void DetachFromWindow();

    private:
        void OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args );

    private:
        wxWindow* m_Window;
        ThumbnailLoader m_Loader;
        Platform::Locker< std::map< u64, Helium::Path* > > m_AllRequests;
        Platform::Mutex m_WindowMutex;
    };
}
