#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Debug
  {
  public:
    const static FileSpec TRACE_FILE;
    const static FileSpec WARNING_FILE;
    const static FileSpec ERROR_FILE;
  };
}
