#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Animation
  {
  public:
    const static FolderSpec CONFIG_FOLDER;
    const static FolderSpec QUERY_FOLDER;

    const static FileSpec ANIMSET_FILE;
    const static FileSpec ANIMCLIPS_FILE;
    const static FileSpec ENUM_MANIFEST_FILE;
    const static FileSpec SHARED_GROUP_FILE;
		const static FileSpec ENUM_FILE;
    const static FileSpec ENUM_COMPARISON_FILE;
    const static FileSpec ENUM_DEPENDENCY_CHECK_FILE;
    const static FileSpec UNCATEGORIZED_GROUP;
    const static FileSpec CATEGORIES_FILE;
    const static FileSpec SUBCATEGORIES_FILE;
    const static FileSpec MODIFIERS_FILE;

    const static FileSpec EVENTS_FILE;
    const static SuffixSpec EVENTS_SUFFIX;
    const static DecorationSpec EVENTS_DECORATION;

    static void Init();
  };

}
