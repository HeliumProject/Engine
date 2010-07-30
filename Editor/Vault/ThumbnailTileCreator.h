#pragma once

#include "SearchResults.h"
#include "ThumbnailSorter.h"
#include "Application/UI/ThreadMechanism.h"

namespace Helium
{
    namespace Editor
    {
        class ThumbnailView;

        class ThumbnailTileCreator : public Helium::ThreadMechanism
        {
        public:
            ThumbnailTileCreator( ThumbnailView* view );
            virtual ~ThumbnailTileCreator();

            void SetDefaultThumbnails( Thumbnail* error, Thumbnail* loading, Thumbnail* folder );

        protected:
            void Reset();
            virtual void InitData() HELIUM_OVERRIDE;
            virtual void ThreadProc( i32 threadID ) HELIUM_OVERRIDE;

            void OnEndThread( const Helium::ThreadProcArgs& args );

        private:
            ThumbnailView* m_View;
            ThumbnailPtr m_TextureError;
            ThumbnailPtr m_TextureLoading;
            ThumbnailPtr m_TextureFolder;
            SearchResultsPtr m_Results;
            M_PathToTilePtr m_Tiles;
            ThumbnailSorter m_Sorter;
            std::set< Helium::Path > m_TextureRequests;
        };
    }
}