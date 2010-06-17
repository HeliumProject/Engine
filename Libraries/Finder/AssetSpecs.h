#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Asset
  {
  public:
    const static FileSpec CONTENT_FILE;
    const static FileSpec SKELETON_FILE; 
    const static FileSpec GLOBAL_REQUIRED_LIST;
    const static FileSpec CONDUIT_CACHE;
    const static FileSpec ASSETCLASS_CACHE;

    const static FamilySpec ANIM_FAMILY;
    const static FamilySpec ANIMSET_FAMILY;
    const static FamilySpec ANIMGROUP_FAMILY;
    const static FamilySpec ANIMCHAIN_FAMILY;
    const static FamilySpec ANIMCONFIG_FAMILY;
    const static FamilySpec CONTENT_FAMILY;
    const static FamilySpec ENTITY_FAMILY;
    const static FamilySpec FOLIAGE_FAMILY;
    const static FamilySpec GAME_FAMILY;
    const static FamilySpec LEVEL_FAMILY;
    const static FamilySpec MATERIAL_FAMILY;
    const static FamilySpec REQUIREDLIST_FAMILY;
    const static FamilySpec FONT_FAMILY;
    const static FamilySpec SHADER_FAMILY;
    const static FamilySpec SKY_FAMILY;
    const static FamilySpec CINEMATIC_FAMILY;
    const static FamilySpec EFFECT_FAMILY;
    const static FamilySpec TEXTUREPACK_FAMILY;
    const static FamilySpec CONDENSED_FAMILY;
    const static FamilySpec WORLD_FAMILY;
    const static FamilySpec ZONE_FAMILY;
    const static FamilySpec CUBEMAP_FAMILY;
    const static FamilySpec MOVIE_FAMILY;
    const static FamilySpec SHARED_ANIMGROUP_FAMILY;
    const static FamilySpec COMPONENT_FAMILY; 
    const static FamilySpec SKELETON_FAMILY;
    const static FamilySpec SHADOWCASTERS_FAMILY;
    const static FamilySpec WRINKLEMAP_FAMILY; 

    const static DecorationSpec ANIM_DECORATION;
    const static DecorationSpec ANIMSET_DECORATION;
    const static DecorationSpec ANIMGROUP_DECORATION;
    const static DecorationSpec ANIMCHAIN_DECORATION;
    const static DecorationSpec ANIMCONFIG_DECORATION;
    const static DecorationSpec CONTENT_DECORATION;
    const static DecorationSpec ENTITY_DECORATION;
    const static DecorationSpec GAME_DECORATION;
    const static DecorationSpec LEVEL_DECORATION;
    const static DecorationSpec MATERIAL_DECORATION;
    const static DecorationSpec REQUIREDLIST_DECORATION;
    const static DecorationSpec FONT_DECORATION;
    const static DecorationSpec SHADER_DECORATION;
    const static DecorationSpec SKY_DECORATION;
    const static DecorationSpec CINEMATIC_DECORATION;
    const static DecorationSpec EFFECT_DECORATION;
    const static DecorationSpec CONDENSED_DECORATION;
    const static DecorationSpec WORLD_DECORATION;
    const static DecorationSpec ZONE_DECORATION;
    const static DecorationSpec MOVIE_DECORATION;
    const static DecorationSpec COMPONENT_DECORATION; 
    const static DecorationSpec SKELETON_DECORATION; 
    const static DecorationSpec SHADOWCASTERS_DECORATION; 

    static FilterSpec ASSET_EDITOR_FILTER;
    static FilterSpec ANIMATION_FILTER;
    static FilterSpec DEPENDENCIES_FILTER;

    static void Init( );
  };
}
