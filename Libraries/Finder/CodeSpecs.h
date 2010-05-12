#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Code
  {
  public:
    const static FolderSpec PACKAGES;
    const static FolderSpec CONFIG;
    const static FolderSpec OUTPUT;

    const static ExtensionSpec HEADER_FILE_EXTENSION;

    static void Init();
  };
}