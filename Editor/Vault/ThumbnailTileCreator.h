#pragma once

#include "VaultSearchResults.h"
#include "ThumbnailSorter.h"
#include "Editor/ThreadMechanism.h"

namespace Helium
{
    namespace Editor
    {
        class ThumbnailView;

        class ThumbnailTileCreator : public ThreadMechanism
        {
        public:
            ThumbnailTileCreator( ThumbnailView* view );
            virtual ~ThumbnailTileCreator();

            void SetDefaultThumbnails( Thumbnail* error, Thumbnail* loading, Thumbnail* folder );

        protected:
            void Reset();
            virtual void InitData() HELIUM_OVERRIDE;
            virtual void ThreadProc( int32_t threadID ) HELIUM_OVERRIDE;

            void OnEndThread( const ThreadProcArgs& args );

        private:
            ThumbnailView* m_View;
            ThumbnailPtr m_TextureError;
            ThumbnailPtr m_TextureLoading;
            ThumbnailPtr m_TextureFolder;
            VaultSearchResultsPtr m_Results;
            M_PathToTilePtr m_Tiles;
            ThumbnailSorter m_Sorter;
            std::set< Helium::Path > m_TextureRequests;
        };
    }
}