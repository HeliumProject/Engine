#include "LevelSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "AssetSpecs.h"

namespace FinderSpecs
{
  const FileSpec   Level::REGION_LIST_FILE            ( "Level::REGION_LIST_FILE",          "region_list",        Extension::TXT );
  const FileSpec   Level::ASSET_STATS_FILE            ( "Level::ASSET_STATS_FILE",          "assetstats",         Extension::DATA );
  const FileSpec   Level::ASSET_LOOKUP_FILE           ( "Level::ASSET_LOOKUP_FILE",         "assetlookup",        Extension::DATA );
  const FileSpec   Level::GAMEPLAY_FILE               ( "Level::GAMEPLAY_FILE",             "gameplay",           Extension::DATA, "18 - rsmith_counts" ); 
}
