#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Project
  {
  public:      
    const static FolderSpec ASSETS_FOLDER;
    const static FolderSpec BUILT_FOLDER;
    const static FolderSpec CACHE_FOLDER;
    const static FolderSpec CODE_FOLDER;
    const static FolderSpec CONFIG_FOLDER;
    const static FolderSpec TEMP_FOLDER;
    const static FolderSpec LOG_FOLDER;
    const static FolderSpec PROCESSED_FOLDER;
    const static FolderSpec DEFAULT_BRANCH_FOLDER;

    const static FolderSpec ASSETS_CONFIG_FOLDER;

    const static FolderSpec EVENT_SYSTEM_FOLDER;
    const static FolderSpec EVENTS_FOLDER;
    const static FileSpec   HANDLED_EVENTS_FILE;   

    const static FamilySpec EVENTS_FAMILY;
    const static DecorationSpec EVENTS_DAT_DECORATION;
    const static DecorationSpec EVENTS_TXT_DECORATION;

    const static FolderSpec DYNAMIC_ENUM_FOLDER;
    const static FileSpec   DYNAMIC_ENUM_DB;

    const static FolderSpec ASSET_MANAGER_FOLDER;
    const static FolderSpec ASSET_MANAGER_CONFIGS;
    const static FileSpec   ASSET_MANAGER_DB;

    const static FolderSpec FILE_RESOLVER_FOLDER;
    const static FolderSpec FILE_RESOLVER_CONFIGS;
    const static FileSpec   FILE_RESOLVER_DB;

    const static FolderSpec DEPENDENCY_GRAPH_FOLDER;
    const static FolderSpec DEPENDENCY_GRAPH_CONFIGS;
    const static FileSpec   DEPENDENCY_GRAPH_DB;

    const static FolderSpec ASSET_CONVERSION_FOLDER;
    const static FileSpec   ASSET_CONVERSION_TABLE;

    const static FolderSpec PROJECT_ASSETS_FOLDER;

    const static FolderSpec ASSET_TRACKER_FOLDER;
    const static FolderSpec ASSET_TRACKER_CONFIGS;
    const static FileSpec   ASSET_TRACKER_DB;

  };
}
