#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  ////////////////////////////////////////////
  // Level
  class FINDER_API Region
  {
  public:
    const static FileSpec REGION_FILE;
    const static FileSpec GP_PRIUS_FILE; 
    const static FileSpec GP_PERSIST_FILE;
    const static FileSpec COLLISION_FILE;
    const static FileSpec COLLISION_PATCH;
    const static FileSpec SKY_FILE;
    const static FileSpec ASSET_STATS_FILE;
    const static FileSpec METADATA_FILE;
    const static FileSpec GLOBAL_DEBUG_FILE;
    const static FileSpec SERVERSIDE_FILE; 
  };
}
