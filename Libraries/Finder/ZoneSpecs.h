#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Zone
  {
  public:
    const static FileSpec BUILT_FILE;
    const static FileSpec HELPER_FILE;
    const static FileSpec LIGHTMAPS_SET1_FILE;
    const static FileSpec LIGHTMAPS_SET2_FILE;
    const static FileSpec VERTLIGHTING_FILE;
    const static FileSpec LIGHTING_FILE;
    const static FileSpec LIGHTING_HIGH_FILE;
    const static FileSpec CONDUIT_DEPS_FILE;
    const static FileSpec GAMEPLAY_FILE; 
    const static FileSpec GP_SCENE_FILE;
    const static FileSpec PLACED_DECALS_FILE;
    const static FileSpec MOBY_LIST_FILE;
    const static FileSpec SHADER_LIST_FILE;
    const static FileSpec CUBEMAP_LIST_FILE;
		const static FileSpec INSTANCE_LIGHTING_FILE;
    const static FileSpec SERVERSIDE_FILE; 
    const static FileSpec LIGHTING_INPUT_DATA;
  };
}
