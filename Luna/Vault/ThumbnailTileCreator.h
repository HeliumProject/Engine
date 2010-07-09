#pragma once

#include "SearchResults.h"
#include "ThumbnailSorter.h"
#include "Application/UI/ThreadMechanism.h"

namespace Luna
{
  class ThumbnailView;

  class ThumbnailTileCreator : public Nocturnal::ThreadMechanism
  {
  public:
    ThumbnailTileCreator( ThumbnailView* view );
    virtual ~ThumbnailTileCreator();

    void SetDefaultThumbnails( Thumbnail* error, Thumbnail* loading, Thumbnail* folder );

  protected:
    void Reset();
    virtual void InitData() NOC_OVERRIDE;
    virtual void ThreadProc( i32 threadID ) NOC_OVERRIDE;

    void OnEndThread( const Nocturnal::ThreadProcArgs& args );

  private:
    ThumbnailView* m_View;
    ThumbnailPtr m_TextureError;
    ThumbnailPtr m_TextureLoading;
    ThumbnailPtr m_TextureFolder;
    SearchResultsPtr m_Results;
    M_PathToTilePtr m_Tiles;
    ThumbnailSorter m_Sorter;
    std::set< Nocturnal::Path > m_TextureRequests;
  };
}
