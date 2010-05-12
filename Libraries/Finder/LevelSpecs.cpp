#include "LevelSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{
  const FileSpec   Level::FXCONDUIT_FILE              ( "Level::FXCONDUIT_FILE",            "fxconduit",          Extension::DATA, "5 - johnnyb_cinedialogue" );
  const FileSpec   Level::FXCONDUIT_PACKED_FILE       ( "Level::FXCONDUIT_PACKED_FILE",     "fxconduit_packed",   Extension::DATA, "5 - johnnyb_cinedialogue" );
  const FileSpec   Level::REGION_LIST_FILE            ( "Level::REGION_LIST_FILE",          "region_list",        Extension::TXT );
  const FileSpec   Level::LEVEL_ID_FILE               ( "Level::LEVEL_ID_FILE",             "id",                 Extension::TXT );
  const FileSpec   Level::ASSET_STATS_FILE            ( "Level::ASSET_STATS_FILE",          "assetstats",         Extension::DATA );
  const FileSpec   Level::ASSET_LOOKUP_FILE           ( "Level::ASSET_LOOKUP_FILE",         "assetlookup",        Extension::DATA );
  const FileSpec   Level::GAMEPLAY_FILE               ( "Level::GAMEPLAY_FILE",             "gameplay",           Extension::DATA, "18 - rsmith_counts" ); 
  const FolderSpec Level::SCRIPT_FOLDER               ( "Level::SCRIPT_FOLDER",             "scripts",            FolderRoots::None );

  const FileSpec   Level::LIGHTING_JOB_FILE           ( "Level::LIGHTING_JOB_FILE",         "",                  Extension::DATA );
  const FolderSpec Level::LIGHTING_BUILT_FOLDER       ( "Level::LIGHTING_BUILT_FOLDER",     "built/lighting jobs", FolderRoots::ProjectAssets );
  const FolderSpec Level::LIGHTING_DATA_FOLDER        ( "Level::LIGHTING_DATA_FOLDER",      "lighting_jobs",       FolderRoots::ProjectAssets );

  const FolderSpec Level::UBERVIEW_BUILT_FOLDER       ( "Level::UBERVIEW_BUILT_FOLDER",     "uberview",           FolderRoots::ProjectBuilt );
  const FileSpec   Level::UBERVIEW_COLLISION_FILE     ( "Level::UBERVIEW_COLLISION_FILE",   "collision",          Extension::DATA );

  const FolderSpec Level::PATCH_BUILT_FOLDER          ( "Level::PATCH_BUILT_FOLDER",        "patch",               FolderRoots::ProjectBuilt );
  const FileSpec   Level::PATCH_LOOKUP_FILE           ( "Level::PATCH_LOOKUP_FILE",         "assetlookup",         Extension::DATA );
  const FileSpec   Level::PATCH_REQ_SHADERS           ( "Level::PATCH_REQ_SHADERS",         "patchshaders",        Asset::REQUIREDLIST_DECORATION );
  const FileSpec   Level::PATCH_REQ_MOBYS             ( "Level::PATCH_REQ_MOBYS",           "patchmobys",          Asset::REQUIREDLIST_DECORATION );
  const FileSpec   Level::PATCH_REQ_ZONES             ( "Level::PATCH_REQ_ZONES",           "patchzones",          Asset::REQUIREDLIST_DECORATION );
  const FileSpec   Level::PATCH_REQ_LIGHTING          ( "Level::PATCH_REQ_LIGHTING",        "patchlighting",       Asset::REQUIREDLIST_DECORATION );
}
