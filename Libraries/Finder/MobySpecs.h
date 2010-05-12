#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Moby
  {
  public:
    const static FileSpec BUILT_FILE;
    const static FileSpec BUILT_ENUM_MANIFEST_FILE;
    const static FileSpec INTERMEDIATE_BSPHERE_FILE;
    const static FileSpec SHADER_LIST_FILE;
    const static FileSpec SKELETON_DBG_FILE; 
  };
}
