#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Content
  {
  public:
    const static FileSpec MANIFEST_FILE;
    const static FileSpec RIGGED_FILE;
    const static FileSpec STATIC_FILE;
    const static FileSpec COLLISION_FILE;
    const static FileSpec DESTRUCTION_GLUE_FILE;
    const static FileSpec PATHFINDING_FILE;
    const static FileSpec INSTANCES_FILE;
    const static FileSpec LIGHTS_FILE;
    const static FileSpec MENTALRAY_FILE;
    const static FileSpec ANIMATION_FILE;
    const static FileSpec CINESCENE_FILE;
    const static FileSpec SHADOWCASTERS_FILE;
    const static FileSpec OBJECTS_FILE;

    const static SuffixSpec MANIFEST_SUFFIX;
    const static SuffixSpec RIGGED_SUFFIX;
    const static SuffixSpec STATIC_SUFFIX;
    const static SuffixSpec COLLISION_SUFFIX;
    const static SuffixSpec DESTRUCTION_GLUE_SUFFIX;
    const static SuffixSpec PATHFINDING_SUFFIX;
    const static SuffixSpec INSTANCES_SUFFIX;
    //const static SuffixSpec LIGHTS_SUFFIX;
    const static SuffixSpec MENTALRAY_SUFFIX;
    const static SuffixSpec ANIMATION_SUFFIX;
    const static SuffixSpec CINESCENE_SUFFIX;
    const static SuffixSpec OBJECTS_SUFFIX;

    const static DecorationSpec MANIFEST_DECORATION;
    const static DecorationSpec RIGGED_DECORATION;
    const static DecorationSpec STATIC_DECORATION;
    const static DecorationSpec COLLISION_DECORATION;
    const static DecorationSpec DESTRUCTION_GLUE_DECORATION;
    const static DecorationSpec PATHFINDING_DECORATION;
    //const static DecorationSpec LIGHTS_DECORATION;
    const static DecorationSpec MENTALRAY_DECORATION;
    const static DecorationSpec ANIMATION_DECORATION;
    const static DecorationSpec CINESCENE_DECORATION;
    const static DecorationSpec OBJECTS_DECORATION;
  };
}
