#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Cinematic
  {
  public:
    const static FileSpec BUILT_FILE;
    const static FileSpec DEBUG_FILE;
    const static FileSpec CONFIG_FILE;
    const static FileSpec EVENTS_FILE;
    const static SuffixSpec EVENTS_SUFFIX;
    const static DecorationSpec EVENTS_DECORATION;
    const static FileSpec GLOBAL_CONFIG_FILE;
    const static FileSpec RTLIGHT_FILE;
  };
}