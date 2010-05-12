#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Shader
  {
  public:

    //
    //  suffixes
    //
    const static SuffixSpec   LAMBERT_SUFFIX;         // _lam
    const static SuffixSpec   BLINN_SUFFIX;           // _bli

    const static SuffixSpec   TEXTURE_NODE_SUFFIX;    // _tex
    const static SuffixSpec   NORMAL_MAP_NODE_SUFFIX; // _nrml

    const static SuffixSpec   NORMAL_MAP_SUFFIX;      // _n
    const static SuffixSpec   COLOR_MAP_SUFFIX;       // _c
    const static SuffixSpec   EXPENSIVE_MAP_SUFFIX;   // _e


    //
    //  files
    //

    const static FileSpec     CACHE_FILE;

    // we have different file specs for the various types of
    // custom shaders.  See ShaderAsset.h for a more detailed
    // description of why this is
    const static FileSpec     BUILT_FILE;
    const static FileSpec     WATER_BUILT_FILE;
    const static FileSpec     FUR_BUILT_FILE;
    const static FileSpec     REFRACTION_BUILT_FILE;
    const static FileSpec     GROUNDFOG_BUILT_FILE;
    const static FileSpec     FURFRAGMENT_BUILT_FILE;
    const static FileSpec     ANISOTROPIC_BUILT_FILE;
    const static FileSpec     AUDIOVIS_BUILT_FILE;
    const static FileSpec     OFF_SCREEN_BUILT_FILE;
    const static FileSpec     BRDF_BUILT_FILE;
    const static FileSpec     FOLIAGE_SHADER_BUILT_FILE;
    const static FileSpec     GRAPH_SHADER_BUILT_FILE;
    const static FileSpec     GRAPH_SHADER_GRAPH_BUILT_FILE; 

    const static FileSpec     TEX_RAW_FILE;
    const static FileSpec     MENTALRAY_FILE;
    const static FileSpec     MENTALRAY_COLOR_TEX;

    const static FileSpec     TEXTURE_FILE;

    const static FolderSpec   DATA_FOLDER;
  };
}
