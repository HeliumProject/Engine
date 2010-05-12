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
  const FileSpec Asset::CONDUIT_CACHE                     ( "Asset::CONDUIT_CACHE",                "cache",   Extension::REFLECT_BINARY, "0_jfaust" );
  const FileSpec Asset::ASSETCLASS_CACHE                  ( "Asset::ASSETCLASS_CACHE",             "assetclasscache",   Extension::REFLECT_BINARY, "0_jfaust" );

  //
  // folders
  //

  const FolderSpec Asset::MAYA_FOLDER                     ( "Asset::MAYA_FOLDER",                   "maya" );
  const FolderSpec Asset::TEXTURES_FOLDER                 ( "Asset::TEXTURES_FOLDER",               "textures" );
  const FolderSpec Asset::ENTITY_FOLDER                   ( "Asset::ENTITY_FOLDER",                 "entities" );
  const FolderSpec Asset::EXPORT_FOLDER                   ( "Asset::EXPORT_FOLDER",                 "export" );
  const FolderSpec Asset::METADATA_FOLDER                 ( "Asset::METADATA_FOLDER",               "metadata" );
  const FolderSpec Asset::LEVEL_FOLDER                    ( "Asset::LEVEL_FOLDER",                  "levels" );
  const FolderSpec Asset::REQUIREDLIST_FOLDER             ( "Asset::REQUIREDLIST_FOLDER",           "reqlists" );
  const FolderSpec Asset::FONT_FOLDER                     ( "Asset::FONT_FOLDER",                   "fonts" );
  const FolderSpec Asset::LOCALIZATION_FOLDER             ( "Asset::LOCALIZATION_FOLDER",           "localization" );
  const FolderSpec Asset::LOCALIZATION_PROCESSED_FOLDER   ( "Asset::LOCALIZATION_PROCESSED_FOLDER", "processed" );
  const FolderSpec Asset::EFFECT_FOLDER                   ( "Asset::EFFECT_FOLDER",                 "effects");
  const FolderSpec Asset::TEXTUREPACK_FOLDER              ( "Asset::TEXTUREPACK_FOLDER",            "texturepacks");
  const FolderSpec Asset::SHADER_FOLDER                   ( "Asset::SHADER_FOLDER",                 "shaders" );
  const FolderSpec Asset::GRAPH_SHADER_FOLDER             ( "Asset::GRAPH_SHADER_FOLDER",           "graphshaders" );
  const FolderSpec Asset::LOOSE_GRAPH_SHADER_FOLDER       ( "Asset::LOOSE_GRAPH_SHADER_FOLDER",     "loosegraphshaders" );
  const FolderSpec Asset::SKY_FOLDER                      ( "Asset::SKY_FOLDER",                    "skies" );
  const FolderSpec Asset::CONDUIT_FOLDER                  ( "Asset::CONDUIT_FOLDER",                "conduit" );
  const FolderSpec Asset::CINEMATIC_FOLDER                ( "Asset::CINEMATIC_FOLDER",              "cinematics" );
  const FolderSpec Asset::SCRIPT_FOLDER                   ( "Asset::SCRIPT_FOLDER",                 "scripts" );
  const FolderSpec Asset::CUBEMAP_FOLDER                  ( "Asset::CUBEMAP_FOLDER",                "cubemaps" );
  const FolderSpec Asset::MOVIE_FOLDER                    ( "Asset::MOVIE_FOLDER",                  "movies" );
  const FolderSpec Asset::TUIDS_FOLDER                    ( "Asset::TUIDS_FOLDER",                  "tuids" );
  const FolderSpec Asset::THUMBNAILS_FOLDER               ( "Asset::THUMBNAILS_FOLDER",             "thumbnails" );

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
  const FamilySpec Asset::FOLIAGE_FAMILY                  ( "Asset::FOLIAGE_FAMILY",             "Foliage",                             "foliage" );
  const FamilySpec Asset::GAME_FAMILY                     ( "Asset::GAME_FAMILY",                "Game",                                "game" );
  const FamilySpec Asset::LEVEL_FAMILY                    ( "Asset::LEVEL_FAMILY",               "Level",                               "level" );
  const FamilySpec Asset::MATERIAL_FAMILY                 ( "Asset::MATERIAL_FAMILY",            "Material",                            "material" );
  const FamilySpec Asset::REQUIREDLIST_FAMILY             ( "Asset::REQUIREDLIST_FAMILY",        "Required List",                       "req" );
  const FamilySpec Asset::FONT_FAMILY                     ( "Asset::FONT_FAMILY",                "Font",                                "font" );
  const FamilySpec Asset::SHADER_FAMILY                   ( "Asset::SHADER_FAMILY",              "Shader",                              "shader" );
  const FamilySpec Asset::LOOSE_GRAPH_SHADER_FAMILY       ( "Asset::LOOSE_GRAPH_SHADER_FAMILY",  "Loose Graph Shader",                  "loosegraphshader" );
  const FamilySpec Asset::POSTEFFECTS_GRAPH_SHADER_FAMILY       ( "Asset::POSTEFFECTS_GRAPH_SHADER_FAMILY",  "PostEffects Graph Shader",                  "posteffectsgraphshader" ); 
  const FamilySpec Asset::SKY_FAMILY                      ( "Asset::SKY_FAMILY",                 "Sky",                                 "sky" );
  const FamilySpec Asset::CINEMATIC_FAMILY                ( "Asset::CINEMATIC_FAMILY",           "Cinematic",                           "cinematic" );
  const FamilySpec Asset::EFFECT_FAMILY                   ( "Asset::EFFECT_FAMILY",              "Effect",                              "effect" );
  const FamilySpec Asset::TEXTUREPACK_FAMILY              ( "Asset::TEXTUREPACK_FAMILY",         "Texture Pack",                        "texturepack" );
  const FamilySpec Asset::CONDENSED_FAMILY                ( "Asset::CONDENSED_FAMILY",           "Condensed",                           "condensed" );
  const FamilySpec Asset::WORLD_FAMILY                    ( "Asset::WORLD_FAMILY",               "World",                               "world" );
  const FamilySpec Asset::ZONE_FAMILY                     ( "Asset::ZONE_FAMILY",                "Zone",                                "zone" );
  const FamilySpec Asset::CUBEMAP_FAMILY                  ( "Asset::CUBEMAP_FAMILY",             "Cube Map",                            "cubemap" );
  const FamilySpec Asset::MOVIE_FAMILY                    ( "Asset::MOVIE_FAMILY",               "Movie",                               "movie" );
  const FamilySpec Asset::SHARED_ANIMGROUP_FAMILY         ( "Asset::SHARED_ANIMGROUP_FAMILY",    "Shared Animation Group",              "sharedanimgroup" );
  const FamilySpec Asset::ATTRIBUTE_FAMILY                ( "Asset::ATTRIBUTE_FAMILY",           "Attributes",                          "attribute" ); 
  const FamilySpec Asset::SKELETON_FAMILY                 ( "Asset::SKELETON_FAMILY",            "Skeleton",                            "skeleton"  ); 
  const FamilySpec Asset::SHADOWCASTERS_FAMILY            ( "Asset::SHADOWCASTERS_FAMILY",       "Shadowcasters",                       "shadowcasters"  ); 
  const FamilySpec Asset::WRINKLEMAP_FAMILY               ( "Asset::WRINKLEMAP_FAMILY",          "Wrinkle Map",                         "wrinklemap"  ); 

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
  const DecorationSpec Asset::FOLIAGE_DECORATION          ( "Asset::FOLIAGE_DECORATION",            "Foliage",            FOLIAGE_FAMILY,       Extension::REFLECT_BINARY );
  const DecorationSpec Asset::GAME_DECORATION             ( "Asset::GAME_DECORATION",               "Game",               GAME_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::LEVEL_DECORATION            ( "Asset::LEVEL_DECORATION",              "Level",              LEVEL_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::MATERIAL_DECORATION         ( "Asset::MATERIAL_DECORATION",           "Material",           MATERIAL_FAMILY,      Extension::REFLECT_BINARY );
  const DecorationSpec Asset::REQUIREDLIST_DECORATION     ( "Asset::REQUIREDLIST_DECORATION",       "Required List",      REQUIREDLIST_FAMILY,  Extension::REFLECT_BINARY );
  const DecorationSpec Asset::FONT_DECORATION             ( "Asset::FONT_DECORATION",               "Font",               FONT_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::SHADER_DECORATION           ( "Asset::SHADER_DECORATION",             "Shader",             SHADER_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::LOOSE_GRAPH_SHADER_DECORATION( "Asset::LOOSE_GRAPH_SHADER_DECORATION",             "Loose Graph Shader",             LOOSE_GRAPH_SHADER_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::POSTEFFECTS_GRAPH_SHADER_DECORATION( "Asset::POSTEFFECTS_GRAPH_SHADER_DECORATION",             "PostEffects Graph Shader",             POSTEFFECTS_GRAPH_SHADER_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::SKY_DECORATION              ( "Asset::SKY_DECORATION",                "Sky",                SKY_FAMILY,           Extension::REFLECT_BINARY );
  const DecorationSpec Asset::CINEMATIC_DECORATION        ( "Asset::CINEMATIC_DECORATION",          "Cinematic",          CINEMATIC_FAMILY,     Extension::REFLECT_BINARY );
  const DecorationSpec Asset::EFFECT_DECORATION           ( "Asset::EFFECT_DECORATION",             "Effect",             EFFECT_FAMILY,        Extension::REFLECT_BINARY );
  const DecorationSpec Asset::TEXTUREPACK_DECORATION      ( "Asset::TEXTUREPACK_DECORATION",        "Texture Pack",       TEXTUREPACK_FAMILY,   Extension::REFLECT_BINARY );
  const DecorationSpec Asset::CONDENSED_DECORATION        ( "Asset::CONDENSED_DECORATION",          "Condensed",          CONDENSED_FAMILY,     Extension::REFLECT_BINARY );
  const DecorationSpec Asset::WORLD_DECORATION            ( "Asset::WORLD_DECORATION",              "World",              WORLD_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ZONE_DECORATION             ( "Asset::ZONE_DECORATION",               "Zone",               ZONE_FAMILY,          Extension::REFLECT_BINARY );
  const DecorationSpec Asset::CUBEMAP_DECORATION          ( "Asset::CUBEMAP_DECORATION",            "Cube Map",           CUBEMAP_FAMILY,       Extension::REFLECT_BINARY );
  const DecorationSpec Asset::MOVIE_DECORATION            ( "Asset::MOVIE_DECORATION",              "Movie",              MOVIE_FAMILY,         Extension::REFLECT_BINARY );
  const DecorationSpec Asset::SHARED_ANIMGROUP_DECORATION ( "Asset::SHARED_ANIMGROUP_DECORATION",   "Shared Animation Group",SHARED_ANIMGROUP_FAMILY,Extension::REFLECT_BINARY );
  const DecorationSpec Asset::ATTRIBUTE_DECORATION        ( "Asset::ATTRIBUTE_DECORATION",          "Attribute",          ATTRIBUTE_FAMILY,     Extension::REFLECT_BINARY ); 
  const DecorationSpec Asset::SKELETON_DECORATION         ( "Asset::SKELETON_DECORATION",           "Skeleton" ,          SKELETON_FAMILY,      Extension::REFLECT_BINARY ); 
  const DecorationSpec Asset::SHADOWCASTERS_DECORATION    ( "Asset::SHADOWCASTERS_DECORATION",      "Shadowcasters" ,     SHADOWCASTERS_FAMILY, Extension::TXT ); 
  const DecorationSpec Asset::WRINKLEMAP_DECORATION       ( "Asset::WRINKLEMAP_DECORATION",         "Wrinkle Map" ,       WRINKLEMAP_FAMILY,    Extension::REFLECT_BINARY ); 

  FilterSpec Asset::ASSET_EDITOR_FILTER ( "Asset::ASSET_EDITOR_FILTER", "All asset files" );
  FilterSpec Asset::ANIMATION_FILTER    ( "Asset::ANIMATION_FILTER",    "All valid files" );
  FilterSpec Asset::DEPENDENCIES_FILTER ( "Asset::DEPENDENCIES_FILTER", "All valid files" );

  void Asset::Init( )
  {
    // These are the file types that can be opened by the Asset Editor
    ASSET_EDITOR_FILTER.AddSpec( ANIMCONFIG_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( ANIMSET_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( CINEMATIC_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( CUBEMAP_DECORATION ); 
    ASSET_EDITOR_FILTER.AddSpec( ENTITY_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( FOLIAGE_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( FONT_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( LEVEL_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( MATERIAL_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( MOVIE_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( REQUIREDLIST_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SHADER_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( LOOSE_GRAPH_SHADER_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( POSTEFFECTS_GRAPH_SHADER_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SHARED_ANIMGROUP_DECORATION ); 
    ASSET_EDITOR_FILTER.AddSpec( SKY_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( SKELETON_DECORATION ); 
    ASSET_EDITOR_FILTER.AddSpec( TEXTUREPACK_DECORATION );
    ASSET_EDITOR_FILTER.AddSpec( WRINKLEMAP_DECORATION );


    // These are the file types used by an AnimationGroupAttribute
    ANIMATION_FILTER.AddSpec( ANIM_DECORATION );
    ANIMATION_FILTER.AddSpec( ANIMCHAIN_DECORATION );

    // These are the file types used by the DependenciesAttribute/RequiredListAsset
    DEPENDENCIES_FILTER.AddSpec( ENTITY_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( FONT_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( MOVIE_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( REQUIREDLIST_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( SHADER_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( TEXTUREPACK_DECORATION );
    DEPENDENCIES_FILTER.AddSpec( ZONE_DECORATION );
  }
}
