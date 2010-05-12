#include "RegionSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Region::REGION_FILE         ( "Region::REGION_FILE",          "region",             Extension::DATA,   "52 - abezrati" );
  const FileSpec Region::GP_PRIUS_FILE       ( "Region::GP_PRIUS_FILE",        "gp_prius",           Extension::DATA,   "21 - aburke" );
  const FileSpec Region::GP_PERSIST_FILE     ( "Region::GP_PERSIST_FILE",      "gp_persist",         Extension::DATA,   "0  - rsmith_split");
  const FileSpec Region::COLLISION_FILE      ( "Region::COLLISION_FILE",       "collision",          Extension::DATA );
  const FileSpec Region::COLLISION_PATCH     ( "Region::COLLISION_PATCH",      "collsionpatch",      Extension::DATA );
  const FileSpec Region::SKY_FILE            ( "Region::SKY_FILE",             "sky",                Extension::DATA,   "4 - abezrati" );
  const FileSpec Region::ASSET_STATS_FILE    ( "Region::ASSET_STATS_FILE",     "assetstats",         Extension::DATA,   "3 - gveltri" );
  const FileSpec Region::METADATA_FILE       ( "Region::METADATA_FILE",        "metadata",           Extension::DATA,   "2 - rsmith_userdata" );
  const FileSpec Region::GLOBAL_DEBUG_FILE   ( "Region::GLOBAL_DEBUG_FILE",    "debug",              Extension::DATA );
  const FileSpec Region::SERVERSIDE_FILE     ( "Region::SERVERSIDE_FILE",      "serverside",         Extension::DATA,   "2 - rsmith" );
}
