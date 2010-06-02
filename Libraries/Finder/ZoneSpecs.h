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
        const static FileSpec GAMEPLAY_FILE; 
        const static FileSpec GP_SCENE_FILE;
        const static FileSpec PLACED_DECALS_FILE;
        const static FileSpec SHADER_LIST_FILE;
        const static FileSpec SERVERSIDE_FILE; 
    };
}
