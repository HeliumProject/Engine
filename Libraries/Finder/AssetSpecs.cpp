#include "AssetSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  //
  // files
  //

  const FileSpec Asset::CONTENT_FILE                      ( "Asset::CONTENT_FILE",                 "" );
  const FileSpec Asset::SKELETON_FILE                     ( "Asset::SKELETON_FILE",                "" ); 
  const FileSpec Asset::GLOBAL_REQUIRED_LIST              ( "Asset::GLOBAL_REQUIRED_LIST",         "global",   REQUIREDLIST_DECORATION );

  //
  // families
  //

  const FamilySpec Asset::ANIM_FAMILY                     ( "Asset::ANIM_FAMILY",                "Animation",                           "anim" );
  const FamilySpec Asset::ANIMSET_FAMILY                  ( "Asset::ANIMSET_FAMILY",             "Animation Set",                       "animset" );
  const FamilySpec Asset::ANIMGROUP_FAMILY                ( "Asset::ANIMGROUP_FAMILY",           "Animation Group",                     "animgroup" );
  const FamilySpec Asset::ANIMCHAIN_FAMILY                ( "Asset::ANIMCHAIN_FAMILY",           "Animation Chain",                     "animchain" );
  const FamilySpec Asset::ANIMCONFIG_FAMILY               ( "Asset::ANIMCONFIG_FAMILY",          "Animation Config",                    "animconfig" );
  const FamilySpec Asset::CONTENT_FAMILY                  ( "Asset::CONTENT_FAMILY",             "Content",                             "content" );
  const FamilySpec Asset::ENTITY_FAMILY                   ( "Asset::ENTITY_FAMILY",              "Entity",                              "entity" );
  const FamilySpec Asset::GAME_FAMILY                     ( "Asset::GAME_FAMILY",                "Game",                                "game" );
  const FamilySpec Asset::LEVEL_FAMILY                    ( "Asset::LEVEL_FAMILY",               "Level",                               "level" );
  const FamilySpec Asset::MATERIAL_FAMILY                 ( "Asset::MATERIAL_FAMILY",            "Material",                            "material" );
  const FamilySpec Asset::REQUIREDLIST_FAMILY             ( "Asset::REQUIREDLIST_FAMILY",        "Required List",                       "reqlist" );
  const FamilySpec Asset::FONT_FAMILY                     ( "Asset::FONT_FAMILY",                "Font",                                "font" );
  const FamilySpec Asset::SHADER_FAMILY                   ( "Asset::SHADER_FAMILY",              "Shader",                              "shader" );
  const FamilySpec Asset::SKY_FAMILY                      ( "Asset::SKY_FAMILY",                 "Sky",                                 "sky" );
  const FamilySpec Asset::CINEMATIC_FAMILY                ( "Asset::CINEMATIC_FAMILY",           "Cinematic",                           "cinematic" );
  const FamilySpec Asset::EFFECT_FAMILY                   ( "Asset::EFFECT_FAMILY",              "Effect",                              "effect" );
  const FamilySpec Asset::WORLD_FAMILY                    ( "Asset::WORLD_FAMILY",               "World",                               "world" );
  const FamilySpec Asset::ZONE_FAMILY                     ( "Asset::ZONE_FAMILY",                "Zone",                                "zone" );
  const FamilySpec Asset::MOVIE_FAMILY                    ( "Asset::MOVIE_FAMILY",               "Movie",                               "movie" );
  const FamilySpec Asset::COMPONENT_FAMILY                ( "Asset::COMPONENT_FAMILY",           "Components",                          "component" ); 
  const FamilySpec Asset::SKELETON_FAMILY                 ( "Asset::SKELETON_FAMILY",            "Skeleton",                            "skeleton"  ); 

  //
  // decorations
  //

  const DecorationSpec Asset::ANIM_DECORATION             ( "Asset::ANIM_DECORATION",               "Animation",          ANIM_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ANIMSET_DECORATION          ( "Asset::ANIMSET_DECORATION",            "Animation Set",      ANIMSET_FAMILY,       Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ANIMGROUP_DECORATION        ( "Asset::ANIMGROUP_DECORATION",          "Animation Group",    ANIMGROUP_FAMILY,     Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ANIMCHAIN_DECORATION        ( "Asset::ANIMCHAIN_DECORATION",          "Animation Chain",    ANIMCHAIN_FAMILY,     Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ANIMCONFIG_DECORATION       ( "Asset::ANIMCONFIG_DECORATION",         "Animation Config",   ANIMCONFIG_FAMILY,    Extension::REFLECT_BINARY );
  const DecorationSpec Asset::CONTENT_DECORATION          ( "Asset::CONTENT_DECORATION",            "Content",            CONTENT_FAMILY,       Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ENTITY_DECORATION           ( "Asset::ENTITY_DECORATION",             "Entity",             ENTITY_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::GAME_DECORATION             ( "Asset::GAME_DECORATION",               "Game",               GAME_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::LEVEL_DECORATION            ( "Asset::LEVEL_DECORATION",              "Level",              LEVEL_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::MATERIAL_DECORATION         ( "Asset::MATERIAL_DECORATION",           "Material",           MATERIAL_FAMILY,      Extension::REFLECT_BINARY );
  const DecorationSpec Asset::REQUIREDLIST_DECORATION     ( "Asset::REQUIREDLIST_DECORATION",       "Required List",      REQUIREDLIST_FAMILY,  Extension::REFLECT_BINARY );
  const DecorationSpec Asset::FONT_DECORATION             ( "Asset::FONT_DECORATION",               "Font",               FONT_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::SHADER_DECORATION           ( "Asset::SHADER_DECORATION",             "Shader",             SHADER_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::SKY_DECORATION              ( "Asset::SKY_DECORATION",                "Sky",                SKY_FAMILY,           Extension::REFLECT_BINARY );
  const DecorationSpec Asset::CINEMATIC_DECORATION        ( "Asset::CINEMATIC_DECORATION",          "Cinematic",          CINEMATIC_FAMILY,     Extension::REFLECT_BINARY );
  const DecorationSpec Asset::EFFECT_DECORATION           ( "Asset::EFFECT_DECORATION",             "Effect",             EFFECT_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::WORLD_DECORATION            ( "Asset::WORLD_DECORATION",              "World",              WORLD_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ZONE_DECORATION             ( "Asset::ZONE_DECORATION",               "Zone",               ZONE_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::MOVIE_DECORATION            ( "Asset::MOVIE_DECORATION",              "Movie",              MOVIE_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::COMPONENT_DECORATION        ( "Asset::COMPONENT_DECORATION",          "Component",          COMPONENT_FAMILY,     Extension::REFLECT_BINARY ); 
  const DecorationSpec Asset::SKELETON_DECORATION         ( "Asset::SKELETON_DECORATION",           "Skeleton" ,          SKELETON_FAMILY,      Extension::REFLECT_BINARY ); 

  FilterSpec Asset::ASSET_EDITOR_FILTER ( "Asset::ASSET_EDITOR_FILTER", "All asset files" );
  FilterSpec Asset::ANIMATION_FILTER    ( "Asset::ANIMATION_FILTER",    "All valid files" );
  FilterSpec Asset::DEPENDENCIES_FILTER ( "Asset::DEPENDENCIES_FILTER", "All valid files" );

  void Asset::Init( )
  {
    // These are the file types that can be opened by the Asset Editor
    ASSET_EDITOR_FILTER.AddSpec( ANIMCONFIG_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( ANIMSET_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( CINEMATIC_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( ENTITY_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( FONT_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( LEVEL_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( MATERIAL_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( MOVIE_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( REQUIREDLIST_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SHADER_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SKY_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SKELETON_DECORATION ); 


    // These are the file types used by an AnimationGroupComponent
    ANIMATION_FILTER.AddSpec( ANIM_DECORATION );
    ANIMATION_FILTER.AddSpec( ANIMCHAIN_DECORATION );

    // These are the file types used by the DependenciesComponent/RequiredListAsset
    DEPENDENCIES_FILTER.AddSpec( ENTITY_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( FONT_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( MOVIE_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( REQUIREDLIST_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( SHADER_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( ZONE_DECORATION );
  }
}
