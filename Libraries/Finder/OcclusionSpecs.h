#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Occlusion
  {
  public:
    const static FileSpec PROCESSED_MAIN_FILE;
    const static FileSpec PROCESSED_GRID_FILE;
    const static FileSpec DATABASE_FILE;
    const static FileSpec CLUSTERS_FILE;
    const static FolderSpec GRIDS_FOLDER;
    const static FolderSpec BUILT_FOLDER;
  };
}
