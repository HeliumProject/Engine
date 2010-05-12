#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API VisualEffect
  {
  public:
    const static FileSpec TYPE_CONFIG_FILE;
    const static FolderSpec CONFIG_FOLDER;

    const static FileSpec BUILT_EFFECT_FILE;
    const static FileSpec BUILT_MANIFEST_FILE;

    const static FileSpec SYSTEM_TEXTURE_PACK;
    const static FileSpec SYSTEM_TEXTURE_PACK_HEADER;

    const static FileSpec EFFECT_TEXTURE_PACK;
  };
}