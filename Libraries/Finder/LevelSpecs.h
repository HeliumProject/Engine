#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  ////////////////////////////////////////////
  // Level
  class FINDER_API Level
  {
  public:
    const static FileSpec FXCONDUIT_FILE;
    const static FileSpec FXCONDUIT_PACKED_FILE;
    const static FileSpec REGION_LIST_FILE;
    const static FileSpec LEVEL_ID_FILE; 
    const static FileSpec ASSET_STATS_FILE; 
    const static FileSpec ASSET_LOOKUP_FILE; 
    const static FileSpec GAMEPLAY_FILE; 
    
    const static FolderSpec SCRIPT_FOLDER;

    const static FileSpec   LIGHTING_JOB_FILE;
    const static FolderSpec LIGHTING_DATA_FOLDER;
    const static FolderSpec LIGHTING_BUILT_FOLDER;

    const static FolderSpec UBERVIEW_BUILT_FOLDER;
    const static FileSpec   UBERVIEW_COLLISION_FILE;

    const static FolderSpec PATCH_BUILT_FOLDER;
    const static FileSpec   PATCH_LOOKUP_FILE; 
    const static FileSpec   PATCH_REQ_SHADERS;
    const static FileSpec   PATCH_REQ_MOBYS;
    const static FileSpec   PATCH_REQ_ZONES;
    const static FileSpec   PATCH_REQ_LIGHTING;
  };
}
