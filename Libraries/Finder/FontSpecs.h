#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Font
  {
  public:
    const static ExtensionSpec OPEN_TYPE_EXTENSION;
    const static ExtensionSpec TRUE_TYPE_EXTENSION;
    const static ExtensionSpec TRUE_TYPE_COLLECTION_EXTENSION;
    
    const static FileSpec OPEN_TYPE_FILE;
    const static FileSpec TRUE_TYPE_FILE;
    const static FileSpec TRUE_TYPE_COLLECTION_FILE;
    const static FileSpec CHARACTER_SET_FILE;
    const static FileSpec METRICS_FILE;
    const static FileSpec TEXTURE_DATA_FILE;
    const static FileSpec TEXTURE_HEADER_FILE;
    
    const static FileSpec TEMP_TEXTURE_FILE;

    static FilterSpec FONT_FILTER;

    static void Init();
  };
}
