#pragma once

#include "Platform/Mutex.h"

#include "SceneGraph/DeviceManager.h"

#include "Editor/Vault/Thumbnail.h"
#include "Editor/Vault/ThumbnailLoader.h"

namespace Helium
{
    namespace Editor
    {
        struct ThumbnailResultArgs
        {
            const V_ThumbnailPtr& m_Thumbnails;
            Helium::FilePath m_Path;

            ThumbnailResultArgs( const V_ThumbnailPtr& thumbnails, const Helium::FilePath& path )
                : m_Thumbnails( thumbnails )
                , m_Path( path )
            {
            }
        };
        typedef Helium::Signature< const ThumbnailResultArgs& > ThumbnailResultSignature;

        class ThumbnailManager
        {
        public:
            ThumbnailManager( wxWindow* window, DeviceManager* d3dmanager );
            virtual ~ThumbnailManager();

            void Reset();
            void Request( const std::set< Helium::FilePath >& paths );
            void Cancel();
            void DetachFromWindow();

        private:
            void OnThumbnailLoaded( const ThumbnailLoader::ResultArgs& args );

        private:
            wxWindow* m_Window;
            ThumbnailLoader m_Loader;
            Helium::Locker< std::map< uint32_t, Helium::FilePath > > m_AllRequests;
            Helium::Mutex m_WindowMutex;
        };
    }
}
