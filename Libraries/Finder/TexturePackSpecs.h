#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API TexturePack
  {
  public:
    const static FileSpec     TEXEL_FILE;
    const static FileSpec     HEADER_FILE;
    const static FileSpec     DEBUG_FILE;
    const static FileSpec     COMBINED_FILE;

    const static FolderSpec   DATA_FOLDER;
  };
}
