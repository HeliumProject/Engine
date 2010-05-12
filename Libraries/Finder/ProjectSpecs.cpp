#include "ProjectSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

#include "Common/Environment.h"

namespace FinderSpecs
{
  const FolderSpec Project::ASSETS_FOLDER           ( "Project::ASSETS_FOLDER",         "assets"      , FolderRoots::ProjectRoot );
  const FolderSpec Project::BUILT_FOLDER            ( "Project::BUILT_FOLDER",          "built"       , FolderRoots::ProjectAssets );
  const FolderSpec Project::CACHE_FOLDER            ( "Project::CACHE_FOLDER",          "cache"       , FolderRoots::ProjectRoot );
  const FolderSpec Project::CODE_FOLDER             ( "Project::CODE_FOLDER",           "code"        , FolderRoots::ProjectRoot );
  const FolderSpec Project::CONFIG_FOLDER           ( "Project::CONFIG_FOLDER",         "config"      , FolderRoots::ProjectRoot );
  const FolderSpec Project::TEMP_FOLDER             ( "Project::TEMP_FOLDER",           "temp"        , FolderRoots::ProjectRoot );
  const FolderSpec Project::LOG_FOLDER              ( "Project::LOG_FOLDER",            "log"         , FolderRoots::ProjectRoot );
  const FolderSpec Project::PROCESSED_FOLDER        ( "Project::PROCESSED_FOLDER",      "processed"   , FolderRoots::ProjectAssets );
  const FolderSpec Project::DEFAULT_BRANCH_FOLDER   ( "Project::DEFAULT_BRANCH_FOLDER", "devel"       , FolderRoots::None );

  const FolderSpec Project::ASSETS_CONFIG_FOLDER    ( "Project::ASSETS_CONFIG_FOLDER",  "config"      , FolderRoots::ProjectAssets );

  const FolderSpec Project::EVENT_SYSTEM_FOLDER     ( "Project::EVENT_SYSTEM_FOLDER",   "eventsystem" , FolderRoots::None );
  const FolderSpec Project::EVENTS_FOLDER           ( "Project::EVENTS_FOLDER",         "events"      , FolderRoots::None );
  const FileSpec   Project::HANDLED_EVENTS_FILE     ( "Project::HANDLED_EVENTS_FILE",   "handled_events", Extension::TXT );

  const FamilySpec Project::EVENTS_FAMILY           ( "Project::EVENTS_FAMILY",         "Events",      "event" );
  const DecorationSpec Project::EVENTS_DAT_DECORATION( "Project::EVENTS_DAT_DECORATION", "Binary Events",                  EVENTS_FAMILY , Extension::DATA );
  const DecorationSpec Project::EVENTS_TXT_DECORATION( "Project::EVENTS_TXT_DECORATION", "Text Events",                    EVENTS_FAMILY , Extension::TXT );

  const FolderSpec Project::DYNAMIC_ENUM_FOLDER     ( "Project::DYNAMIC_ENUM_FOLDER",   "dynamicenum" , Project::ASSETS_CONFIG_FOLDER );
  const FileSpec   Project::DYNAMIC_ENUM_DB         ( "Project::DYNAMIC_ENUM_DB",       "cache"       , Extension::REFLECT_BINARY );

  const FolderSpec Project::ASSET_MANAGER_FOLDER    ( "Project::ASSET_MANAGER_FOLDER",  "assetmanager", Project::ASSETS_CONFIG_FOLDER );
  const FolderSpec Project::ASSET_MANAGER_CONFIGS   ( "Project::ASSET_MANAGER_CONFIGS", "data/assetmanager/", FolderRoots::ProjectTools );
  const FileSpec   Project::ASSET_MANAGER_DB        ( "Project::ASSET_MANAGER_DB",      "usages"      , Extension::DB, "1.0" );

  const FolderSpec Project::FILE_RESOLVER_FOLDER    ( "Project::FILE_RESOLVER_FOLDER",  "fileresolver", Project::ASSETS_CONFIG_FOLDER );
  const FolderSpec Project::FILE_RESOLVER_CONFIGS   ( "Project::FILE_RESOLVER_CONFIGS", "data/fileresolver/", FolderRoots::ProjectTools );
  const FileSpec   Project::FILE_RESOLVER_DB        ( "Project::FILE_RESOLVER_DB",      "cache"       , Extension::DB, "INVALID" );

  const FolderSpec Project::DEPENDENCY_GRAPH_FOLDER ( "Project::DEPENDENCY_GRAPH_FOLDER","dependencies", Project::ASSETS_CONFIG_FOLDER );
  const FolderSpec Project::DEPENDENCY_GRAPH_CONFIGS( "Project::DEPENDENCY_GRAPH_CONFIGS","data/dependencies/", FolderRoots::ProjectTools );
  const FileSpec   Project::DEPENDENCY_GRAPH_DB     ( "Project::DEPENDENCY_GRAPH_DB",   "graph"       , Extension::DB, "INVALID" );
  
  const FolderSpec Project::ASSET_CONVERSION_FOLDER ( "Project::ASSET_CONVERSION_FOLDER", "conversion"      , Project::CONFIG_FOLDER );
  const FileSpec   Project::ASSET_CONVERSION_TABLE  ( "Project::ASSET_CONVERSION_TABLE",  "conversion_table",  Extension::REFLECT_BINARY );

  const FolderSpec Project::PROJECT_ASSETS_FOLDER   ( "Project::PROJECT_ASSETS_FOLDER",   ""           , FolderRoots::ProjectAssets );

  const FolderSpec Project::ASSET_TRACKER_FOLDER    ( "Project::ASSET_TRACKER_FOLDER",  "assettracker", Project::ASSETS_CONFIG_FOLDER );
  const FolderSpec Project::ASSET_TRACKER_CONFIGS   ( "Project::ASSET_TRACKER_CONFIGS", "data/assettracker/", FolderRoots::ProjectTools );
  const FileSpec   Project::ASSET_TRACKER_DB        ( "Project::ASSET_TRACKER_DB",      "cache"       , Extension::DB, "INVALID" );

}
