#pragma once

#include "Thumbnail.h"
#include "ThumbnailLoader.h"
#include "Platform/Mutex.h"

namespace Render
{
    class D3DManager;
}

namespace Luna
{
    struct ThumbnailResultArgs
    {
        const V_ThumbnailPtr& m_Thumbnails;
        Asset::AssetFilePtr m_File;

        ThumbnailResultArgs( const V_ThumbnailPtr& thumbnails, Asset::AssetFilePtr file )
            : m_Thumbnails( thumbnails )
            , m_File( file )
        {
        }
    };
    typedef Nocturnal::Signature< void, const ThumbnailResultArgs& > ThumbnailResultSignature;

    class ThumbnailManager
    {
    public:
        ThumbnailManager( wxWindow* window, Render::D3DManager* d3dmanager, const std::string& thumbnailDirectory );
        virtual ~ThumbnailManager();

        void Reset();
        void Request( const Asset::V_AssetFiles& files );
        void Cancel();
        void DetachFromWindow();

    private:
        void OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args );

    private:
        wxWindow* m_Window;
        ThumbnailLoader m_Loader;
        Platform::Locker< Asset::M_AssetFiles > m_AllRequests;
        Platform::Mutex m_WindowMutex;
    };
}
