#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Sky
  {
  public:
    const static FileSpec MAIN_FILE;
    const static FileSpec VRAM_FILE;
    
    const static FileSpec LF_CLOUD_TEXTURE_NAME;
    const static FileSpec HF_CLOUD_TEXTURE_NAME;
    const static FileSpec TURBULANCE_TEXTURE_NAME;
    const static FileSpec NORMAL_MAP_NAME;
    const static FileSpec SHELL_TEXTURE_NAME;
  };
}
