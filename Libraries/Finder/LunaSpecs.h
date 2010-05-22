#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API Luna
  {
  public:      
    const static FolderSpec GLOBAL_CONFIG_FOLDER;
    const static FolderSpec CONFIG_FOLDER;
    const static FolderSpec THEMES_FOLDER;
    const static FolderSpec DEFAULT_THEME_FOLDER;
    const static FolderSpec GAME_THEME_FOLDER;
    const static FolderSpec SPLASH_SCREEN_FOLDER;
    const static FolderSpec PREFERENCES_FOLDER;

    const static FileSpec EMPTY_MAYA_FILE;
    const static FileSpec TYPE_CONFIGURATIONS;
    const static FileSpec MISC_SETTINGS;
    const static FileSpec GLOBAL_PREFS;
    const static FileSpec ASSET_EDITOR_PREFS;
    const static FileSpec SCENE_EDITOR_PREFS;
    const static FileSpec BROWSER_PREFS;
    const static FileSpec ANIMATION_EVENTS_EDITOR_PREFS;
    const static FileSpec CINEMATIC_EVENTS_EDITOR_PREFS;
    const static FileSpec CHARACTER_EDITOR_PREFS;

    const static FamilySpec ASSET_COLLECTION_FAMILY;
    const static DecorationSpec ASSET_COLLECTION_RB_DECORATION;
    const static DecorationSpec ASSET_COLLECTION_XML_DECORATION;

    // Session Manager files
    const static FamilySpec SESSION_FAMILY;
    const static DecorationSpec SESSION_DECORATION;
    
    static void Init();
  };
}