#pragma once

#include "SearchResults.h"
#include "ThumbnailSorter.h"
#include "Luna/UI/ThreadMechanism.h"

namespace Luna
{
  class ThumbnailView;

  class ThumbnailTileCreator : public Luna::ThreadMechanism
  {
  public:
    ThumbnailTileCreator( ThumbnailView* view );
    virtual ~ThumbnailTileCreator();

    void SetDefaultThumbnails( Thumbnail* error, Thumbnail* loading, Thumbnail* folder );

  protected:
    void Reset();
    virtual void InitData() NOC_OVERRIDE;
    virtual void ThreadProc( i32 threadID ) NOC_OVERRIDE;

    void OnEndThread( const Luna::ThreadProcArgs& args );

  private:
    ThumbnailView* m_View;
    ThumbnailPtr m_TextureError;
    ThumbnailPtr m_TextureLoading;
    ThumbnailPtr m_TextureFolder;
    SearchResultsPtr m_Results;
    M_FolderToTilePtr m_FolderTiles;
    M_FileToTilePtr m_FileTiles;
    ThumbnailSorter m_Sorter;
    Asset::V_AssetFiles m_TextureRequests;
  };
}
