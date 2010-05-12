#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Localization
  {
  public:
    const static FileSpec CHARACTER_SET_FILE;
    const static FileSpec TQD_FILE;
    const static FileSpec PACKAGE_FILE;
    const static FileSpec PACKAGE_LIST_FILE;
    const static FileSpec DIALOGUE_PACKAGE_FILE;

    const static FileSpec DATABASE;

    const static FileSpec CHARACTER_KEYS_FILE;

    const static FolderSpec ASSET_FOLDER;
    const static FolderSpec BASE_FOLDER;
    const static FolderSpec CACHE_FOLDER;
  };
}
